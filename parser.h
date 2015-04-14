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

#ifndef PARSER_H
#define PARSER_H

#include <stack>
#include <stdexcept>

#include "lexer.h"
#include "ast.h"

class ParseError : public std::exception
{
public:
  const Token token;
  const std::string reason, errorLine;
  ParseError(Token token, std::string what, std::string errorLine)
      : token(std::move(token)), reason(std::move(what)), errorLine(errorLine)
  {
  }
  const char *what() const noexcept override { return reason.c_str(); }

  std::string getErrorLineHighlight()
  {
    std::string error(errorLine);
    error += '\n';
    for (int i = 1; i < token.col; ++i)
      error += '~';
    error += '^';
    return error;
  }
};

class Parser
{
  std::stack<Lexer> lexers;

  void pushLexer(std::string filename)
  {
    lexers.emplace(filename);
    currentLexer = &lexers.top();
  }
  void popLexer()
  {
    lexers.pop();
    if (!lexers.empty())
      currentLexer = &lexers.top();
    else
      currentLexer = nullptr;
  }

  Lexer *currentLexer;
  Token prevTok, curTok;

  Token &readNextToken()
  {
    prevTok = std::move(curTok);
    return curTok = currentLexer->nextToken();
  }

  static bool isUnaryOperator(int type);
  static int getTokenPrecedence(int type);

  // these functions expect the first token to already be in curTok
  FunctionPtr parseFunctionDef();
  BlockStmtPtr parseStmtBlock();
  StmtPtr parseTLExpr(bool &isBlock);
  ExprPtr parseExpr();
  ExprPtr parseBinOpRHS(int prec, ExprPtr rhs);
  ExprPtr parseUnaryExpr();
  ExprPtr parsePrimaryExpr();

  // Expressions:
  ExprPtr parseIdentifierExpr();
  ExprPtr parseNumberExpr();
  ExprPtr parseParenExpr();

  // Statements:
  StmtPtr parseVarDeclStmt();
  StmtPtr parseIfStmt();
  StmtPtr parseForStmt();
  StmtPtr parseWhlStmt();
  StmtPtr parseDoStmt();
  StmtPtr parseRetStmt();

  // helpers
  void parseFunctionArguments(ArgVector &args);

  [[noreturn]] void error(std::string msg)
  {
    throw ParseError(curTok, std::move(msg),
                     currentLexer->abortAndGetCurrentLine());
  }

  // TODO make variadic for more tokens?
  void assertToken(int token)
  {
    if (curTok.type != token)
      error("Unexpected '" + curTok.str + "', expected '" +
            Lexer::getTokenName(token) + '\'');
  }

public:
  Parser() = default;

  std::vector<FunctionPtr> parse(std::string filename);
};

#endif // PARSER_H
