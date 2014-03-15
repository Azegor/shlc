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

#include <cerrno>
#include <cstring>

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
  enum TokenType : int {
    none = -0x100,
    eof,
    identifier,
    dq_string,
    sq_string,
    dec_number,
    hex_number,
    bin_number,
    oct_number,

    // arithmetic operators:
    add_assign,
    sub_assign,
    mul_assign,
    div_assign,
    mod_assign,
    pow_assign,
    increment,
    decrement,
    power,

    // bool operators
    lte,
    gte,
    log_and,
    log_or,

    // bit operations
    lshift,
    rshift,
    lshift_assign,
    rshift_assign,
    bit_and_assign,
    bit_or_assign,
    bit_complement_assign,
    bit_xor_assign,

    //  identifiers:
    id_if,
    id_elif,
    id_el,
    id_for,
    id_whl,
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
};

class Lexer
{
private:
  bool ownsStream = false;
  std::istream *input;
  int lastChar = ' '; // ' ' will be skipped immediately
  std::string tokenString;

  int line = 1, column = 1;
  std::string currentLine;
  void finishCurrentLine()
  {
    if (!*input) // if file open failed, don't try to read!
      return;
    while (lastChar = input->get(),
           lastChar != '\r' && lastChar != '\n' && !input->eof())
      currentLine += lastChar;
  }

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

  Token makeToken(int type) { return Token(type, line, column, tokenString); }

  [[noreturn]] void error(std::string msg)
  {
    finishCurrentLine();
    throw LexError(line, column, std::move(msg), std::move(currentLine));
  }

  void checkStream()
  {
    if (!*input) {
      std::cout << "throwing now" << std::endl;
      error(std::string("Broken input stream: ") + std::strerror(errno));
    }
  }

public:
  const std::string filename;

  Lexer(std::istream &input) : input(&input), filename("<unknown>")
  {
    checkStream();
  }

  Lexer(std::string filename)
      : input(new std::ifstream(filename)), filename(filename)
  {
    ownsStream = true;
    checkStream();
  }

  Lexer(const Lexer &) = delete;
  Lexer(Lexer &&o)
      : ownsStream(o.ownsStream),
        input(o.input),
        lastChar(o.lastChar),
        tokenString(o.tokenString),
        line(o.line),
        column(o.column),
        currentLine(o.currentLine)
  {
    // o.input = nullptr;
    o.ownsStream = false;
  }

  ~Lexer()
  {
    if (ownsStream)
      delete input;
  }

  Token nextToken();

  std::string abortAndGetCurrentLine()
  {
    finishCurrentLine();
    return currentLine;
  }

  static std::string getTokenName(int type) {
    auto pos = tokenNames.find(type);
    if (pos != tokenNames.end())
      return pos->second;
    return std::to_string((char)type);
  }
};

#endif // LEXER_H
