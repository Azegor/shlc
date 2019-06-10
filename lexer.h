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

#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <fstream>
#include <deque>

#include <cerrno>
#include <cstring>

#include "compilationunit.h"

class LexError : public std::exception
{
public:
  const int line, col;
  const std::string reason, errorLine;
  LexError(int line, int col, std::string what, std::string errorLine)
      : line(line), col(col), reason(std::move(what)), errorLine(errorLine)
  {
  }
  const char *what() const noexcept override { return reason.c_str(); }

  std::string getErrorLineHighlight()
  {
    std::string error(errorLine);
    error += '\n';
    for (int i = 1; i < col; ++i)
      error += '~';
    error += '^';
    return error;
  }
};

struct Token
{
  enum TokenType : int
  {
    none = -0x100,
    eof,
    identifier,
    dq_string,
    sq_string,
    dec_number,
    dec_flt_number,
    hex_number,
    bin_number,
    oct_number,

    // --- assignment operations ---
    // arithmetic:
    add_assign,
    sub_assign,
    mul_assign,
    div_assign,
    mod_assign,
    pow_assign,
    // bit operation
    lshift_assign,
    rshift_assign,
    bit_and_assign,
    bit_or_assign,
    //     bit_complement_assign,
    bit_xor_assign,

    // --- binary operations ---
    // arithmetic operators:
    // +
    // -
    // *
    // /
    // %
    power,
    // bool operations
    lte,
    gte,
    eq,
    neq,
    log_and,
    log_or,
    // bit operations
    lshift,
    rshift,

    // -- unary operations --
    // arithmetic operations
    increment,
    decrement,
    // bool operations
    // !
    // bit operations
    // ~

    //  identifiers:
    id_if,
    id_elif,
    id_el,
    id_for,
    id_whl,
    id_do,
    id_brk,
    id_cnt,
    id_T,
    id_F,
    id_nil,
    id_use,
    id_fn,
    id_main,
    id_ret,
    id_native,
    id_op,
    id_cls,
    id_new,

    // type IDs
    id_var,
    id_vac,
    // valid var types:
    id_int,
    id_flt,
    id_boo,
    id_str,
    id_chr,
  };
  int type;
  int line, col;
  std::string str;

  Token() : Token(none, 0, 0, "") {}
  Token(int type) : Token(type, 0, 0, "") {}

  Token(int type, int line, int col, std::string tokString)
      : type(type), line(line), col(col), str(std::move(tokString))
  {
  }

  Token(const Token &o) : type(o.type), line(o.line), col(o.col), str(o.str) {}
  Token(Token &&o)
      : type(o.type), line(o.line), col(o.col), str(std::move(o.str))
  {
  }

  Token &operator=(const Token &o)
  {
    type = o.type;
    line = o.line;
    col = o.col;
    str = o.str;
    return *this;
  }

  Token &operator=(Token &&o)
  {
    type = o.type;
    line = o.line;
    col = o.col;
    str = std::move(o.str);
    return *this;
  }

  bool operator==(const Token &o) const
  {
    return type == o.type && line == o.line && col == o.col && str == o.str;
  }

  std::string toStr() const
  {
    return '<' + str + " at " + std::to_string(line) + ':' +
           std::to_string(col) + '>';
  }
};

class Lexer
{
private:
  int lexerNr;
    CompilationUnit compUnit;
  std::istream *input;
  int lastChar = ' '; // ' ' will be skipped immediately
  std::string tokenString;

  int line = 1, column = 0;
  int tokenLine, tokenCol;
  std::deque<std::string> lines;
  std::string *currentLine;

  int readNext();

  void appendAndNext()
  {
    tokenString += lastChar;
    readNext();
  }

  static std::unordered_map<std::string, Token::TokenType> identifierTokens;
  static std::unordered_map<char, char> escapeCharacters;
  static std::unordered_map<int, std::string> tokenNames;

  Token readString(char sep, int tokenType);
  Token readLeadingZeroNumber();
  Token readDecNumber();
  Token readDotOrNumberDotPart();
  Token readNumberExponentPart();

  Token readDivideOperatorOrComment();
  Token readMultiplyOperator();
  Token readModuloOperator();
  Token readPlusOperator();
  Token readMinusOperator();

  Token readLTOperator();
  Token readGTOperator();

  Token readAndOperator();
  Token readOrOperator();

  Token readBitComplOperator();
  Token readBitXorOperator();
  Token readEq();
  Token readBang();

  void initToken()
  {
    tokenString = "";
    tokenLine = line;
    tokenCol = column;
  }

  Token makeToken(int type)
  {
    return Token(type, tokenLine, tokenCol, tokenString);
  }

  [[noreturn]] void error(std::string msg)
  {
    finishCurrentLine();
    throw LexError(line, column, std::move(msg), *currentLine);
  }

  void checkStream()
  {
    if (!*input) {
      error(std::string("Broken input stream: ") + std::strerror(errno));
    }
  }

public:
  const std::string filename;

  Lexer(CompilationUnit inputCU)
      : lines(1), currentLine(&lines[0]), compUnit(std::move(inputCU)), input(compUnit.getStream())
  {
    checkStream();
  }

  Lexer(const Lexer &) = delete;
  Lexer(Lexer &&o)
      : compUnit(std::move(o.compUnit)),
        input(std::move(o.input)),
        lastChar(std::move(o.lastChar)),
        tokenString(std::move(o.tokenString)),
        line(std::move(o.line)),
        column(std::move(o.column)),
        lines(std::move(o.lines)),
        currentLine(std::move(o.currentLine))
  {
  }


  void setNr(int nr) { lexerNr = nr; }
  int getNr() const { return lexerNr; }

  const std::string &getFileName() const { return filename; }

  Token nextToken();

  void finishCurrentLine()
  {
    if (!*input) // if file open failed, don't try to read!
      return;
    while (lastChar = readNext(),
           lastChar != '\r' && lastChar != '\n' && !input->eof())
    {
    }
  }

  std::string abortAndGetCurrentLine()
  {
    finishCurrentLine();
    return *currentLine;
  }

  static std::string getTokenName(int type)
  {
    auto pos = tokenNames.find(type);
    if (pos != tokenNames.end()) return pos->second;
    return {(char)type};
  }

  const std::string &getLine(int i) const { return lines[i - 1]; }
};

struct TokenPos
{
  int line;
  int col;
  TokenPos() : line(-1), col(-1) {}
  TokenPos(int l, int c) : line(l), col(c) {}
  TokenPos(const Token &t, bool endToken) : line(t.line)
  {
    col = t.col + (endToken ? t.str.size() - 1 : 0);
  }

  bool operator==(const TokenPos &o) const
  {
    return line == o.line && col == o.col;
  }

  std::string toStr() const
  {
    return std::to_string(line) + ':' + std::to_string(col);
  }
};

struct SourceLocation
{
  int lexerNr;
  const TokenPos startToken, endToken;
  SourceLocation() = default;
  SourceLocation(int lexerNr, TokenPos start, TokenPos end)
      : lexerNr(lexerNr), startToken(start), endToken(end)
  {
  }
  SourceLocation(int lexerNr, const Token &single)
      : lexerNr(lexerNr), startToken(single, false), endToken(single, true)
  {
  }
  SourceLocation(const SourceLocation &o)
      : lexerNr(o.lexerNr), startToken(o.startToken), endToken(o.endToken)
  {
  }
  SourceLocation(SourceLocation &&o)
      : lexerNr(o.lexerNr),
        startToken(std::move(o.startToken)),
        endToken(std::move(o.endToken))
  {
  }

  std::string toStr() const
  {
    // TODO output source itself (get from lexer)
    if (startToken.col == -1) return "at unknown location";
    if (startToken == endToken)
      return "at token " + startToken.toStr() + "->" + endToken.toStr();
    return "between token " + startToken.toStr() + " and " + endToken.toStr();
  }

  std::string getErrorLineHighlight(const Lexer &lex) const
  {
    if (startToken.line == -1) return "at unknown location";
    std::string error("at \033[1;37m" + lex.getFileName() + "\033[00m:" +
                      std::to_string(startToken.line) + ':' +
                      std::to_string(startToken.col) + '\n');
    error += lex.getLine(startToken.line);
    error += '\n';
    for (int i = 1; i < startToken.col; ++i)
      error += '~';
    if (startToken.line == endToken.line) {
      for (int i = startToken.col; i <= endToken.col; ++i)
        error += '^';
    }
    else
    {
      error += '^';
    }
    return error;
  }
};

#endif // LEXER_H
