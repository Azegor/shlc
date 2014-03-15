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

std::vector<FunctionExprPtr> Parser::parse(std::string filename)
{
  pushLexer(filename);
  // outer:
  std::vector<FunctionExprPtr> toplevelFunctions;
  while (!lexers.empty()) {
    bool eof = false;
    readNextToken();
    while (!eof) {
      switch (curTok.type) {
      default:
        std::cerr << "unexpected token: " << curTok.str << std::endl;
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

FunctionExprPtr Parser::parseFunctionDef()
{
  readNextToken(); // eat 'fn'
  // handle main as normal Token::identifier
  if (curTok.type != Token::identifier && curTok.type != Token::id_main)
    error("Unexpected Token " + curTok.str + ", expected identifier");
  std::string fnName = curTok.str;
  if (readNextToken().type != '(')
    error("Unexpected token " + curTok.str + ", expected '('");
  ArgVector arguments;

  readNextToken(); // eat '('

  while (curTok.type != ')') {
    if (isVarTypeId(curTok.type)) {
      readFunctionArguments(arguments);
      if (curTok.type == ';')
        readNextToken(); // eat ';'
    } else {
      error("Unexpected token " + curTok.str);
    }
  }

  readNextToken(); // eat ')'

  auto head = FunctionHeadExprPtr{
      new FunctionHeadExpr(std::move(fnName), std::move(arguments))};

  switch (curTok.type) {
  default:
    error("unexpected " + curTok.str + ", expected 'native' '{' or ';'");
    break;
  case Token::id_native: // native function
    if (readNextToken().type != ';')
      error("unexpected " + curTok.str + ", expected ';'");
    readNextToken();
    return FunctionExprPtr{new NativeFunctionExpr{std::move(head)}};
    break;
  case '{': // function definition
  {
    auto body = readFunctionBody();
    return FunctionExprPtr{
        new NormalFunctionExpr(std::move(head), std::move(body))};
    break;
  }
  case ';':          // function declaration
    readNextToken(); // eat ';'
    return FunctionExprPtr{new FunctionDeclExpr(std::move(head))};
    break;
  }
}

// reads until ';' or ')'
void Parser::readFunctionArguments(ArgVector &args)
{
  // assert (isVarTypeId(curTok));
  int argType = curTok.type;
  readNextToken();
  do {
    if (curTok.type != Token::identifier)
      error("unexpected token " + curTok.str + ", expected identifier");
    args.emplace_back(std::make_pair(argType, curTok.str));
    if (readNextToken().type == ',')
      readNextToken(); // eat ','
  } while (curTok.type != ';' && curTok.type != ')');
}

std::vector<ExprPtr> Parser::readFunctionBody()
{
  return {};
}
