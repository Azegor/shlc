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
#include <vector>
#include <filesystem>

#include "lexer.h"
#include "ast.h"
#include "util.h"
#include "compilationunit.h"

class Parser;

class ParseError : public std::exception
{
public:
  const SourceLocation srcLoc;
  const std::string reason, errorLine;
  ParseError(SourceLocation srcLoc, std::string what, std::string errorLine)
      : srcLoc(std::move(srcLoc)), reason(std::move(what)), errorLine(errorLine)
  {
  }
  const char *what() const noexcept override { return reason.c_str(); }

  //   std::string getErrorLineHighlight()
  //   {
  //     std::string error(errorLine);
  //     error += '\n';
  //     for (int i = 1; i < srcLoc.startToken.col; ++i)
  //       error += '~';
  //     for (int i = srcLoc.startToken.col; i < srcLoc.endToken.col; ++i)
  //       error += '^';
  //     return error;
  //   }
  std::string getErrorLineHighlight(const Parser &parser) const;
};

class Parser
{
  TypeRegistry typeRegistry;
  std::deque<Lexer> allLexers;
  std::stack<Lexer *> lexers;
  std::stack<Token> lastTokens;
  std::vector<std::filesystem::path> includePaths;
  int currentLexerNr = 0;

  void pushLexer(CompilationUnit compUnit)
  {
    lastTokens.push(curTok);
    allLexers.emplace_back(std::move(compUnit));
    lexers.push(&allLexers.back());
    currentLexer = lexers.top();
    currentLexerNr = allLexers.size() - 1;
    currentLexer->setNr(currentLexerNr);
  }
  void popLexer()
  {
    lexers.pop();
    if (!lexers.empty()) {
      currentLexer = lexers.top();
      curTok = lastTokens.top();
      lastTokens.pop();
      currentLexerNr = currentLexer->getNr();
    }
    else
      currentLexer = nullptr;
  }

  std::filesystem::path findHeaderFile(const std::string &name);

  Lexer *currentLexer;
  Token prevTok, curTok;
  std::vector<TokenPos> startPositions;
  TokenPos endPos;

  void startSLContext() { startPositions.push_back({curTok, false}); }
  void dupSLContextTop() { startPositions.push_back(startPositions.back()); }
  void popSLContext() { startPositions.pop_back(); }
  void markSLContextEndPrevToken() { endPos = {prevTok, true}; }
  void markSLContextEnd() { endPos = {curTok, true}; }
  SourceLocation endSLContextHere()
  {
    assert(startPositions.size());
    auto start = startPositions.back();
    startPositions.pop_back();
    return {currentLexerNr, start, TokenPos(curTok, true)};
  }
  SourceLocation endSLContextPrevToken()
  {
    assert(startPositions.size());
    auto start = startPositions.back();
    startPositions.pop_back();
    return {currentLexerNr, start, TokenPos(prevTok, true)};
  }
  SourceLocation getSLContextMarkedEnd()
  {
    assert(startPositions.size());
    auto start = startPositions.back();
    startPositions.pop_back();
    return {currentLexerNr, start, endPos};
  }
  SourceLocation getSLContextSingleCurToken()
  {
    return {currentLexerNr, curTok};
  }
  SourceLocation getSLContextSinglePrevToken()
  {
    return {currentLexerNr, prevTok};
  }

  Token &readNextToken()
  {
    prevTok = std::move(curTok);
    return curTok = currentLexer->nextToken();
  }

  void assertNextToken(int token) {
      readNextToken();
      assertToken(token);
  }

  static bool isUnaryOperator(int type);
  static int getTokenPrecedence(int type);

  // these functions expect the first token to already be in curTok
  FunctionPtr parseFunctionDef();
  BlockStmtPtr parseStmtBlock();
  StmtPtr parseTLExpr(bool &isBlock);
  StmtPtr parseVarDeclStmtOrExpr();
  ExprPtr parseExpr();
  ExprPtr parseBinOpRHS(int prec, ExprPtr rhs);
  ExprPtr parseUnaryExpr();
  ExprPtr parsePrimaryExpr();

  // Expressions:
  ExprPtr parseIdentifierExpr();
  ExprPtr parseNumberExpr();
  ExprPtr parseParenExpr();
  ExprPtr parseNewExpr();

  // Statements:
  StmtPtr parseVarDeclStmt();
  StmtPtr parseIfStmt();
  StmtPtr parseForStmt();
  StmtPtr parseWhlStmt();
  StmtPtr parseDoStmt();
  StmtPtr parseRetStmt();

  // Types:
  Type *parseTypeName();
  ClassTypePtr parseClassDef();

  // helpers
  void parseFunctionArguments(ArgVector &args);


  [[noreturn]] void error(std::string msg)
  {
    //     currentLexer->finishCurrentLine();
    throw ParseError({currentLexerNr, curTok}, std::move(msg),
                     currentLexer->abortAndGetCurrentLine());
  }

  // TODO make variadic for more tokens?
  void assertToken(int token)
  {
    if (curTok.type != token)
      error("Unexpected '" + curTok.str + "', expected '" +
            Lexer::getTokenName(token) + '\'');
  }
  void assertTokens(std::initializer_list<int> tokens)
  {
    for (auto &&t : tokens)
      if (curTok.type == t) return;
    error("Unexpected '" + curTok.str + "', expected one of " +
          listToString(tokens, [](int t)
                       {
            return '\'' + Lexer::getTokenName(t) + '\'';
          }));
  }

public:
  Parser() = default;

  void addIncludePath(const char *path)
  {
    addIncludePath(std::filesystem::path(path));
  }
  void addIncludePath(const std::string &path)
  {
    addIncludePath(std::filesystem::path(path));
  }
  void addIncludePath(std::filesystem::path path);

  std::vector<FunctionPtr> parse(CompilationUnit compUnit);

  const Lexer &getLexer(int nr) const { return allLexers[nr]; }
  int getNumLexers() const { return allLexers.size(); }
};

#endif // PARSER_H
