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

#include "lexer.h"

#include <vector>

static inline bool isVarTypeId(int tokenType)
{
  return Token::id_int <= tokenType && tokenType <= Token::id_chr;
}

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
    {Token::power, 120}, };

/* maybe needed later?
std::unordered_map<int, int> unOpPrecedence{
    // 150
    {Token::increment, 150},
    {Token::decrement, 150},
}
*/

int Parser::getTokenPrecedence(int type)
{
  auto pos = binOpPrecedence.find(type);
  if (pos == binOpPrecedence.end())
    return -1;
  return pos->second;
}

std::vector<FunctionPtr> Parser::parse(std::string filename)
{
  pushLexer(filename);
  // outer:
  std::vector<FunctionPtr> toplevelFunctions;
  while (!lexers.empty()) {
    bool eof = false;
    readNextToken();
    while (!eof) {
      switch (curTok.type) {
      default:
        std::cerr << "unexpected token '" << curTok.str << "' at "
                  << curTok.line << ':' << curTok.col << std::endl;
        readNextToken();
        break;
      case Token::id_fn:
        toplevelFunctions.push_back(parseFunctionDef());
        break;
      case Token::eof:
        std::cout << "Reached end of file in " << currentLexer->filename
                  << std::endl;
        popLexer();
        eof = true;
        break;
      }
    }
  }
  return toplevelFunctions;
}

FunctionPtr Parser::parseFunctionDef()
{
  readNextToken(); // eat 'fn'
  // handle main as normal Token::identifier
  if (curTok.type != Token::identifier && curTok.type != Token::id_main)
    error("Unexpected Token '" + curTok.str + "', expected identifier");
  std::string fnName = curTok.str;
  if (readNextToken().type != '(')
    error("Unexpected token '" + curTok.str + "', expected '('");
  ArgVector arguments;

  readNextToken(); // eat '('

  while (curTok.type != ')') {
    if (isVarTypeId(curTok.type)) {
      parseFunctionArguments(arguments);
      if (curTok.type == ';')
        readNextToken(); // eat ';'
    } else {
      error("Unexpected '" + curTok.str + '\'');
    }
  }

  readNextToken(); // eat ')'

  Type retType = Type::vac_t;
  if (curTok.type == ':') // with return type
  {
    readNextToken();
    if (!isVarTypeId(curTok.type))
      error("unexpected '" + curTok.str + "', expected type");
    retType = getTypeFromToken(curTok.type);
    readNextToken();
  }

  auto head = FunctionHeadPtr{
      new FunctionHead(std::move(fnName), std::move(arguments), retType)};

  switch (curTok.type) {
  default:
    error("unexpected '" + curTok.str + "', expected 'native' '{' or ';'");
    break;
  case Token::id_native: // native function
    if (readNextToken().type != ';')
      error("unexpected '" + curTok.str + "', expected ';'");
    readNextToken();
    return FunctionPtr{new NativeFunction{std::move(head)}};
    break;
  case '{': // function definition
  {
    auto body = parseStmtBlock();
    return FunctionPtr{new NormalFunction(std::move(head), std::move(body))};
    break;
  }
  case ';':          // function declaration
    readNextToken(); // eat ';'
    return FunctionPtr{new FunctionDecl(std::move(head))};
    break;
  }
}

// reads until ';' or ')'
void Parser::parseFunctionArguments(ArgVector &args)
{
  // assert (isVarTypeId(curTok));
  Type argType = getTypeFromToken(curTok.type);
  readNextToken();
  do {
    if (curTok.type != Token::identifier)
      error("unexpected '" + curTok.str + "', expected identifier");
    args.emplace_back(std::make_pair(argType, curTok.str));
    if (readNextToken().type == ',')
      readNextToken(); // eat ','
  } while (curTok.type != ';' && curTok.type != ')');
}

BlockStmtPtr Parser::parseStmtBlock()
{
  // assert(curTok.type == '{');
  readNextToken();
  StmtList exprL;
  while (curTok.type != '}') {
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
  return BlockStmtPtr{new BlockStmt(std::move(exprL))};
}

StmtPtr Parser::parseTLExpr(bool &isBlock)
{
  isBlock = false;
  switch (curTok.type) {
  default:
    return make_SPtr<ExprStmt>(parseExpr());
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
    return make_SPtr<BreakStmt>();
    break;
  case Token::id_cnt:
    readNextToken();
    return make_SPtr<ContinueStmt>();
    break;
  }
}

ExprPtr Parser::parseExpr()
{
  auto res =
      parseBinOpRHS(0, parsePrimaryExpr()); // use parseUnary() here later!

  return res;
}

ExprPtr Parser::parseBinOpRHS(int exprPrec, ExprPtr lhs)
{
  while (true) {
    int tokPrec = getTokenPrecedence(curTok.type);

    // if new binOp (tokPrec) binds less tight, we're done
    if (tokPrec < exprPrec)
      return lhs; // this is the loop exit point!

    auto binOp = curTok.type;
    readNextToken(); // eat binop

    auto rhs = parsePrimaryExpr();

    int nextPrec = getTokenPrecedence(curTok.type);
    // if new op binds less tightly take current op as its RHS
    if (tokPrec < nextPrec)
      rhs = parseBinOpRHS(tokPrec + 1, std::move(rhs));

    lhs = make_EPtr<BinOpExpr>(binOp, std::move(lhs), std::move(rhs));
  }
}

ExprPtr Parser::parsePrimaryExpr()
{
  ExprPtr res;
  switch (curTok.type) {
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
        error("invalid char constant: '" + curTok.str + "' with length " + std::to_string(curTok.str.length()));
    res = make_EPtr<CharConstExpr>(curTok.str[0]);
    readNextToken();
    break;
  case Token::dq_string:
    res = make_EPtr<StringConstExpr>(curTok.str);
    readNextToken();
    break;
  case Token::id_T:
    res = make_EPtr<BoolConstExpr>(true);
    readNextToken();
    break;
  case Token::id_F:
    res = make_EPtr<BoolConstExpr>(false);
    readNextToken();
    break;
  case '(':
    res = parseParenExpr();
    break;
  }
  if (curTok.type == ':') // cast
  {
    if (!isVarTypeId(readNextToken().type))
      error("unexpected '" + curTok.str + "', expected type");
    auto type = getTypeFromToken(curTok.type);
    readNextToken();
    return make_EPtr<CastExpr>(std::move(res), type);
  }
  return res;
}

ExprPtr Parser::parseIdentifierExpr()
{
  auto idName = curTok.str;
  readNextToken(); // eat identifier
  if (curTok.type != '(')
    return make_EPtr<VariableExpr>(std::move(idName));

  readNextToken();
  ExprList args;
  if (curTok.type != ')') {
    while (true) {
      args.push_back(parseExpr());
      if (curTok.type == ')')
        break;
      if (curTok.type != ',')
        error("Unexpected '" + curTok.str + "', expected ')' or ','");

      readNextToken();
    }
  }
  readNextToken(); // eat ')'
  return make_EPtr<FunctionCallExpr>(std::move(idName), std::move(args));
}
ExprPtr Parser::parseNumberExpr()
{
  ExprPtr res;
  switch (curTok.type) {
  case Token::dec_number:
  case Token::hex_number:
  case Token::oct_number:
  case Token::bin_number:
    res = make_EPtr<IntNumberExpr>(std::stoll(curTok.str));
    break;
  case Token::dec_flt_number:
    res = make_EPtr<FltNumberExpr>(std::stold(curTok.str));
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
StmtPtr Parser::parseIfStmt()
{
  readNextToken();
  auto cond = parseExpr();
  if (curTok.type != '{')
    error("unexpected '" + curTok.str + "', expected '{'");
  BlockStmtPtr thenExpr = parseStmtBlock();
  BlockStmtPtr elseExpr;
  if (curTok.type == Token::id_elif) {
    auto elsePart = parseIfStmt();
  } else if (curTok.type == Token::id_el) {
    readNextToken();
    if (curTok.type != '{')
      error("unexpected '" + curTok.str + "', expected '{'");
    elseExpr = parseStmtBlock();
  }
  return make_SPtr<IfStmt>(std::move(cond), std::move(thenExpr),
                           std::move(elseExpr));
}
StmtPtr Parser::parseForStmt()
{
  readNextToken();
  ExprPtr init;
  if (curTok.type != ';') // empty init
    init = parseExpr();
  if (curTok.type != ';')
    error("unexpected '" + curTok.str + "', expected ';'");
  readNextToken();
  ExprPtr cond;
  if (curTok.type != ';')
    cond = parseExpr();
  if (curTok.type != ';')
    error("unexpected '" + curTok.str + "', expected ';'");
  readNextToken();
  ExprPtr incr;
  if (curTok.type != '{')
    incr = parseExpr();
  if (curTok.type != '{')
    error("unexpected '" + curTok.str + "', expected '{'");
  auto body = parseStmtBlock();
  return make_SPtr<ForStmt>(std::move(init), std::move(cond), std::move(incr),
                            std::move(body));
}
StmtPtr Parser::parseWhlStmt()
{
  readNextToken();
  auto cond = parseExpr();
  if (curTok.type != '{')
    error("unexpected '" + curTok.str + "', expected '{'");
  BlockStmtPtr body = parseStmtBlock();
  return make_SPtr<WhileStmt>(std::move(cond), std::move(body));
}
StmtPtr Parser::parseDoStmt()
{
  readNextToken();
  assertToken('{');
  auto body = parseStmtBlock();
  assertToken(Token::id_whl);
  readNextToken(); // eat 'whl'
  auto cond = parseExpr();
  return make_SPtr<DoWhileStmt>(std::move(cond), std::move(body));
}
StmtPtr Parser::parseRetStmt()
{
  readNextToken();
  if (curTok.type == ';')
  {
      return make_SPtr<ReturnStmt>(); // void return
  }
  return make_SPtr<ReturnStmt>(parseExpr());
}

StmtPtr Parser::parseVarDeclStmt()
{
  readNextToken(); // eat 'var'
  VarDeclStmt::VarEnties vars;

  // TODO initializers missing so far
  bool inferred = false;
  while (true) {
    if (curTok.type != Token::identifier)
      error("unexpected '" + curTok.str + "', expected identifier");
    auto name = std::move(curTok.str);
    ExprPtr initializer;
    if (readNextToken().type == '=') {
      readNextToken();
      initializer = parseExpr();
      inferred = true;
    } else if (inferred) {
      error("unexpected '" + curTok.str + "', expected '='");
    }
    vars.push_back(std::make_pair(std::move(name), std::move(initializer)));

    if (!inferred && curTok.type == ':')
      break;
    if (inferred && curTok.type == ';')
      break;
    if (curTok.type != ',')
      error("unexpected '" + curTok.str + "', expected ':', '=' or ','");
    readNextToken(); // eat ','
  }
  Type type = Type::inferred;
  if (!inferred) {
    readNextToken(); // eat ':'
    if (!isVarTypeId(curTok.type))
      error("unexpected '" + curTok.str + "', expected type");
    type = getTypeFromToken(curTok.type);
    readNextToken(); // eat type
  }

  return make_SPtr<VarDeclStmt>(type, std::move(vars));
}
