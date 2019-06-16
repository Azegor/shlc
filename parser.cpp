/*
 * Copyright 2014 Azegor
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "parser.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "lexer.h"

std::string ParseError::getErrorLineHighlight(const Parser &parser) const
{
  return srcLoc.getErrorLineHighlight(parser.getLexer(srcLoc.lexerNr));
}

static inline bool isVarTypeId(int tokenType)
{
  return Token::id_int <= tokenType && tokenType <= Token::id_chr;
}

std::unordered_set<int> unaryOperators{
  '!', '~', '-', Token::TokenType::increment, Token::TokenType::decrement,
};

std::unordered_map<int, int> binOpPrecedence{
  // Install standard binary operators.
  // 1 is lowest precedence.
  // the assignment operators might need a special treatment (right to left
  // associativity!)
  {'=', 2},
  {Token::add_assign, 2},
  {Token::sub_assign, 2},
  {Token::mul_assign, 2},
  {Token::div_assign, 2},
  {Token::mod_assign, 2},
  {Token::pow_assign, 2},
  {Token::bit_and_assign, 2},
  {Token::bit_or_assign, 2},
  {Token::bit_xor_assign, 2},
  {Token::lshift_assign, 2},
  {Token::rshift_assign, 2},

  // 20
  {Token::log_or, 20},
  // 30
  {Token::log_and, 30},
  // 40
  {'|', 40},
  // 50
  {'^', 50},
  // 60
  {'&', 60},
  // 70
  {Token::eq, 70},
  {Token::neq, 70},

  // 80
  {'<', 80},
  {'>', 80},
  {Token::lte, 80},
  {Token::gte, 80},

  // 90
  {Token::lshift, 90},
  {Token::rshift, 90},

  // 100
  {'+', 100},
  {'-', 100},

  // 110
  {'*', 110},
  {'/', 110},
  {'%', 110},

  // 120
  {Token::power, 120},

  // 130
  {'.', 130}
};

static inline bool isRightAssociative(int tokenType)
{
  return tokenType == '='
      || (tokenType >= Token::add_assign && tokenType <= Token::bit_xor_assign);
}

/* maybe needed later?
std::unordered_map<int, int> unOpPrecedence{
    // 150
    {Token::increment, 150},
    {Token::decrement, 150},
}
*/

bool Parser::isUnaryOperator(int type)
{
  if (unaryOperators.find(type) == unaryOperators.end()) return false;
  return true;
}

int Parser::getTokenPrecedence(int type)
{
  auto pos = binOpPrecedence.find(type);
  if (pos == binOpPrecedence.end()) return -1;
  return pos->second;
}

std::vector<FunctionPtr> Parser::parse(CompilationUnit compUnit)
{
  pushLexer(std::move(compUnit));
  // outer:
  std::vector<FunctionPtr> toplevelFunctions;
  while (!lexers.empty())
  {
    bool eof = false;
    readNextToken();
    while (!eof)
    {
      switch (curTok.type)
      {
        default:
          assertTokens({Token::id_use, Token::id_fn, Token::id_cls, Token::eof});
        //           error("unexpected token");
        //           std::cerr << "unexpected token '" << curTok.str << "' at "
        //                     << curTok.line << ':' << curTok.col << std::endl;
        //           readNextToken();
        //           break;
        case Token::id_use:
        {
          readNextToken();
          std::string filename;
          if (curTok.type == Token::dq_string) {
            filename = curTok.str;
          }
          else if (curTok.type == Token::identifier)
          {
            filename = curTok.str + ".shl";
          }
          else
          {
            assertTokens({Token::dq_string, Token::identifier});
          }
          readNextToken();
          assertToken(';');
          readNextToken();
          pushLexer(filename);
          readNextToken();
          break;
        }
        case Token::id_fn:
          toplevelFunctions.push_back(parseFunctionDef());
          break;
        case Token::id_cls:
          typeRegistry.registerClassType(parseClassDef());
          break;
        case Token::eof:
          //           std::cout << "Reached end of file in " <<
          //           currentLexer->filename
          //                     << std::endl;
          popLexer();
          if (lexers.empty()) eof = true;
          break;
      }
    }
  }
  return toplevelFunctions;
}

FunctionPtr Parser::parseFunctionDef()
{
  startSLContext(); // whole function
  startSLContext(); // function head
  readNextToken();  // eat 'fn'
  // handle main as normal Token::identifier
  if (curTok.type != Token::identifier && curTok.type != Token::id_main)
    error("Unexpected Token '" + curTok.str + "', expected identifier");
  std::string fnName = curTok.str;
  if (readNextToken().type != '(')
    error("Unexpected token '" + curTok.str + "', expected '('");
  ArgVector arguments;

  readNextToken(); // eat '('

  while (curTok.type != ')')
  {
    if (isVarTypeId(curTok.type)) {
      parseFunctionArguments(arguments);
      if (curTok.type == ';') readNextToken(); // eat ';'
    }
    else
    {
      error("Unexpected '" + curTok.str + '\'');
    }
  }

  markSLContextEnd();

  readNextToken(); // eat ')'

  Type *retType = TypeRegistry::getVoidType();
  if (curTok.type == ':') // with return type
  {
    readNextToken();
    retType = parseTypeName();
    markSLContextEndPrevToken();
  }

  auto head = FunctionHeadPtr{new FunctionHead(
    getSLContextMarkedEnd(), std::move(fnName), std::move(arguments), retType)};

  switch (curTok.type)
  {
    default:
      error("unexpected '" + curTok.str + "', expected 'native' '{' or ';'");
      break;
    case Token::id_native: // native function
      if (readNextToken().type == Token::dq_string) {
        head->setBindName(curTok.str);
        readNextToken();
      }
      if (curTok.type != ';')
        error("unexpected '" + curTok.str + "', expected ';'");
      readNextToken();
      return FunctionPtr{
        new NativeFunction{endSLContextPrevToken(), std::move(head)}};
      break;
    case '{': // function definition
    {
      auto body = parseStmtBlock();
      return FunctionPtr{new NormalFunction(endSLContextPrevToken(),
                                            std::move(head), std::move(body))};
      break;
    }
    case ';':          // function declaration
      readNextToken(); // eat ';'
      return FunctionPtr{
        new FunctionDecl(endSLContextPrevToken(), std::move(head))};
      break;
  }
}

// reads until ';' or ')'
void Parser::parseFunctionArguments(ArgVector &args)
{
  Type *argType = getTypeFromToken(curTok.type);
  readNextToken();
  do
  {
    if (curTok.type != Token::identifier) {
      error("unexpected '" + curTok.str + "', expected identifier");
    }
    args.emplace_back(getSLContextSingleCurToken(), argType, curTok.str);
    if (readNextToken().type == ',') {
      readNextToken(); // eat ','
    }
  } while (curTok.type != ';' && curTok.type != ')');
}

BlockStmtPtr Parser::parseStmtBlock()
{
  startSLContext();
  // assert(curTok.type == '{');
  readNextToken();
  StmtList exprL;
  while (curTok.type != '}')
  {
    bool blk = false;
    auto expr = parseTLExpr(blk);
    if (!blk) { // no block expr
      if (curTok.type != ';')
        error("unexpected '" + curTok.str + "', expected ';'");
      readNextToken();
    }
    exprL.push_back(std::move(expr));
  }
  readNextToken(); // eat '}'
  return BlockStmtPtr{new BlockStmt(endSLContextPrevToken(), std::move(exprL))};
}

StmtPtr Parser::parseTLExpr(bool &isBlock)
{
  isBlock = false;
  switch (curTok.type)
  {
    default:
    {
      auto expr = parseExpr();
      return make_SPtr<ExprStmt>(expr->srcLoc, std::move(expr));
    }
    case '{': // new block
      isBlock = true;
      return parseStmtBlock();
    case Token::id_if:
      isBlock = true;
      return parseIfStmt();
    case Token::id_for:
      isBlock = true;
      return parseForStmt();
    case Token::id_whl:
      isBlock = true;
      return parseWhlStmt();
    case Token::id_do:
      return parseDoStmt();
    case Token::id_ret:
      return parseRetStmt();
    case Token::id_var:
      return parseVarDeclStmt();
    case Token::id_brk:
      readNextToken();
      return make_SPtr<BreakStmt>(getSLContextSinglePrevToken());
      break;
    case Token::id_cnt:
      readNextToken();
      return make_SPtr<ContinueStmt>(getSLContextSinglePrevToken());
      break;
  }
}

StmtPtr Parser::parseVarDeclStmtOrExpr()
{
  if (curTok.type == Token::id_var) return parseVarDeclStmt();
  auto expr = parseExpr();
  return make_SPtr<ExprStmt>(expr->srcLoc, std::move(expr));
}

ExprPtr Parser::parseExpr()
{
  auto res = parseBinOpRHS(0, parseUnaryExpr());

  return res;
}

ExprPtr Parser::parseBinOpRHS(int exprPrec, ExprPtr lhs)
{
  startSLContext();
  while (true)
  {
    int tokPrec = getTokenPrecedence(curTok.type); // next operator

    // if new binOp (tokPrec) binds less tight, we're done
    // return current result as LHS for the next expr
    if (tokPrec < exprPrec) {
      popSLContext();
      return lhs; // this is the loop exit point!
    }

    auto binOp = curTok.type;
    readNextToken(); // eat binop

    if (binOp == '.') {
        std::string fieldName = std::move(curTok.str);
        dupSLContextTop();
        auto loc = endSLContextHere();
        readNextToken();
        lhs = make_EPtr<FieldAccessExpr>(loc, std::move(lhs), fieldName);
    } else {
        auto rhs = parseUnaryExpr();

        int nextPrec = getTokenPrecedence(curTok.type); // one operator after that
        // if new op binds less tightly take current op as its LHS
        // with right hand precedence let next OP be "stronger"
        int offset = isRightAssociative(binOp) ? -1 : 1;
        if (tokPrec < nextPrec || isRightAssociative(binOp)) rhs = parseBinOpRHS(tokPrec + offset, std::move(rhs));

        dupSLContextTop();
        // eat next OP and continue looping (this OP was stringer)

        lhs = make_EPtr<BinOpExpr>(endSLContextPrevToken(), binOp, std::move(lhs),
                                    std::move(rhs));
    }
  }
}

ExprPtr Parser::parseUnaryExpr()
{
  if (!isUnaryOperator(curTok.type)) return parsePrimaryExpr();

  startSLContext();
  int op = curTok.type;
  readNextToken();
  auto unary = parseUnaryExpr();
  return make_EPtr<UnOpExpr>(endSLContextPrevToken(), op, std::move(unary));
}

ExprPtr Parser::parsePrimaryExpr()
{
  ExprPtr res;
  switch (curTok.type)
  {
    default:
      error("unexpected '" + curTok.str + "', expected primary expression");
    case Token::identifier:
      res = parseIdentifierExpr();
      break;
    case Token::dec_number:
    case Token::hex_number:
    case Token::oct_number:
    case Token::bin_number:
    case Token::dec_flt_number:
      res = parseNumberExpr();
      break;
    case Token::sq_string: // char constant
      if (curTok.str.length() != 1)
        error("invalid char constant: '" + curTok.str + "' with length " +
              std::to_string(curTok.str.length()));
      res =
        make_EPtr<CharConstExpr>(getSLContextSingleCurToken(), curTok.str[0]);
      readNextToken();
      break;
    case Token::dq_string:
      res =
        make_EPtr<StringConstExpr>(getSLContextSingleCurToken(), curTok.str);
      readNextToken();
      break;
    case Token::id_T:
      res = make_EPtr<BoolConstExpr>(getSLContextSingleCurToken(), true);
      readNextToken();
      break;
    case Token::id_F:
      res = make_EPtr<BoolConstExpr>(getSLContextSingleCurToken(), false);
      readNextToken();
      break;
    case Token::id_new:
      res = parseNewExpr();
      break;
    case '(':
      res = parseParenExpr();
      break;
  }
  if (curTok.type == ':') // cast
  {
    dupSLContextTop(); // use same start as to-cast expression
    auto startTok = prevTok;
    readNextToken();
    auto type = parseTypeName();
    return make_EPtr<CastExpr>(endSLContextPrevToken(), std::move(res), type);
  }
  return res;
}

ExprPtr Parser::parseIdentifierExpr()
{
  startSLContext();
  auto idName = std::move(curTok.str);
  readNextToken(); // eat identifier
  if (curTok.type != '(')
    return make_EPtr<VariableExpr>(endSLContextPrevToken(), std::move(idName));

  readNextToken();
  ExprList args;
  if (curTok.type != ')') {
    while (true)
    {
      args.push_back(parseExpr());
      if (curTok.type == ')') break;
      if (curTok.type != ',')
        error("Unexpected '" + curTok.str + "', expected ')' or ','");

      readNextToken();
    }
  }
  readNextToken(); // eat ')'
  return make_EPtr<FunctionCallExpr>(endSLContextPrevToken(), std::move(idName),
                                     std::move(args));
}
ExprPtr Parser::parseNumberExpr()
{
  ExprPtr res;
  switch (curTok.type)
  {
    case Token::dec_number:
    case Token::hex_number:
    case Token::oct_number:
    case Token::bin_number:
      try {
        res = make_EPtr<IntNumberExpr>(getSLContextSingleCurToken(),
                                     std::stoll(curTok.str));
      }
      catch(std::out_of_range &oor)
      {
        error("Number '" + curTok.str + "' is too large to be represented as a 'int'");
      }
      break;
    case Token::dec_flt_number:
      try {
        res = make_EPtr<FltNumberExpr>(getSLContextSingleCurToken(),
                                     std::stold(curTok.str));
      }
      catch(std::out_of_range &oor)
      {
        error("Number '" + curTok.str + "' is too large to be represented as a 'flt'");
      }
      break;
    default:
      error("!!! unknown token in parseNumberExpr: " +
            Lexer::getTokenName(curTok.type));
  }
  readNextToken();
  return res;
}
ExprPtr Parser::parseParenExpr()
{
  readNextToken(); // eat '('
  auto expr = parseExpr();
  if (curTok.type != ')')
    error("unexpected '" + curTok.str + "', expected ')'");
  readNextToken(); // eat ')'
  return expr;
}
ExprPtr Parser::parseNewExpr()
{
  startSLContext();
  readNextToken();
  auto type = parseTypeName();
  auto classType = dynamic_cast<ClassType*>(type);
  if (!classType) {
    error("cannot use builtin types with new expressions");
  }
  assertToken('(');
  // TODO
  assertNextToken(')');
  auto res = make_EPtr<NewExpr>(endSLContextHere(), classType);
  readNextToken();
  return res;
}

StmtPtr Parser::parseIfStmt()
{
  startSLContext();
  readNextToken(); // eat 'if' or 'elif'
  auto cond = parseExpr();
  if (curTok.type != '{')
    error("unexpected '" + curTok.str + "', expected '{'");
  StmtPtr thenExpr = parseStmtBlock();
  StmtPtr elseExpr;
  if (curTok.type == Token::id_elif) {
    elseExpr = parseIfStmt();
  }
  else if (curTok.type == Token::id_el)
  {
    readNextToken();
    if (curTok.type != '{')
      error("unexpected '" + curTok.str + "', expected '{'");
    elseExpr = parseStmtBlock();
  }
  return make_SPtr<IfStmt>(endSLContextPrevToken(), std::move(cond),
                           std::move(thenExpr), std::move(elseExpr));
}
StmtPtr Parser::parseForStmt()
{
  startSLContext();
  readNextToken();
  StmtPtr init;
  if (curTok.type != ';') // empty init
    init = parseVarDeclStmtOrExpr();
  if (curTok.type != ';')
    error("unexpected '" + curTok.str + "', expected ';'");
  readNextToken();
  ExprPtr cond;
  if (curTok.type != ';') cond = parseExpr();
  if (curTok.type != ';')
    error("unexpected '" + curTok.str + "', expected ';'");
  readNextToken();
  ExprPtr incr;
  if (curTok.type != '{') incr = parseExpr();
  if (curTok.type != '{')
    error("unexpected '" + curTok.str + "', expected '{'");
  auto body = parseStmtBlock();
  return make_SPtr<ForStmt>(endSLContextPrevToken(), std::move(init),
                            std::move(cond), std::move(incr), std::move(body));
}
StmtPtr Parser::parseWhlStmt()
{
  startSLContext();
  readNextToken();
  auto cond = parseExpr();
  if (curTok.type != '{')
    error("unexpected '" + curTok.str + "', expected '{'");
  BlockStmtPtr body = parseStmtBlock();
  return make_SPtr<WhileStmt>(endSLContextPrevToken(), std::move(cond),
                              std::move(body));
}
StmtPtr Parser::parseDoStmt()
{
  startSLContext();
  readNextToken();
  assertToken('{');
  auto body = parseStmtBlock();
  assertToken(Token::id_whl);
  readNextToken(); // eat 'whl'
  auto cond = parseExpr();
  return make_SPtr<DoWhileStmt>(endSLContextPrevToken(), std::move(cond),
                                std::move(body));
}
StmtPtr Parser::parseRetStmt()
{
  startSLContext();
  readNextToken();
  if (curTok.type == ';') {
    return make_SPtr<ReturnStmt>(endSLContextPrevToken()); // void return
  }
  auto expr = parseExpr();
  return make_SPtr<ReturnStmt>(endSLContextPrevToken(), std::move(expr));
}

StmtPtr Parser::parseVarDeclStmt()
{
  startSLContext();
  readNextToken(); // eat 'var'
  VarDeclStmt::VarEnties vars;
  
  enum class Inferred {Unknown, NO, YES};

  Inferred inferred = Inferred::Unknown;
  while (true)
  {
    if (curTok.type != Token::identifier)
      error("unexpected '" + curTok.str + "', expected identifier");
    auto name = std::move(curTok.str);
    ExprPtr initializer;
    if (readNextToken().type == '=') {
      if (inferred == Inferred::Unknown)
      {
          inferred = Inferred::YES;
      }
      else if (inferred == Inferred::NO)
      {
          error("unexpected '=', expected ','  (cannot mix inferred with non-inferred mode in variable declaration)");
      }
      readNextToken();
      initializer = parseExpr();
      inferred = Inferred::YES;
    }
    else if (inferred == Inferred::YES)
    {
      error("unexpected '" + curTok.str + "', expected '=' (cannot mix inferred with non-inferred mode in variable declaration)");
    }
    else
    {
        inferred = Inferred::NO;
    }
    vars.push_back(std::make_pair(std::move(name), std::move(initializer)));

    if (inferred==Inferred::NO && curTok.type == ':') break;
    if (inferred==Inferred::YES && curTok.type == ';') break;
    if (curTok.type != ',')
      error("unexpected '" + curTok.str + "', expected ':', '=' or ','");
    readNextToken(); // eat ','
  }
  Type *type = nullptr;
  if (inferred==Inferred::NO) {
    readNextToken(); // eat ':'
    type = parseTypeName();
  } else {
    type = nullptr;
  }

  return make_SPtr<VarDeclStmt>(endSLContextPrevToken(), type, std::move(vars));
}

Type *Parser::parseTypeName()
{
  Type *type = nullptr;
  if (isVarTypeId(curTok.type)) {
    type = getTypeFromToken(curTok.type);
  } else {
    type = typeRegistry.findClassType(curTok.str);
    if (!type) {
      error("unknown type name '" + curTok.str + "'");
    }
  }
  readNextToken();
  return type;
}

ClassTypePtr Parser::parseClassDef()
{
  startSLContext();
  // assertToken(Token::id_cls);
  assertNextToken(Token::identifier);
  std::string className = std::move(curTok.str);
  assertNextToken('{');
  readNextToken();
  ClassFieldVec fieldL;
  unsigned fieldIdx = 0;
  while (curTok.type != '}') {
      assertToken(Token::identifier);
      std::string fieldName = std::move(curTok.str);
      assertNextToken(':');
      readNextToken();
      Type *fieldType = parseTypeName();
      assertToken(';');
      fieldL.emplace_back(std::move(fieldName), fieldType, fieldIdx++);
      readNextToken();
  }
  readNextToken(); // eat '}'
  return ClassTypePtr{new ClassType(endSLContextPrevToken(), std::move(className), std::move(fieldL))};
}
