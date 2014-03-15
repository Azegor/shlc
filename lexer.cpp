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

#include "lexer.h"

std::unordered_map<std::string, Token::TokenType> Lexer::identifierTokens{
    {"if", Token::id_if},
    {"elif", Token::id_elif},
    {"el", Token::id_el},
    {"for", Token::id_for},
    {"whl", Token::id_whl},
    {"brk", Token::id_brk},
    {"cnt", Token::id_cnt},
    {"var", Token::id_var},
    {"int", Token::id_int},
    {"flt", Token::id_flt},
    {"boo", Token::id_boo},
    {"str", Token::id_str},
    {"vac", Token::id_vac},
    {"chr", Token::id_chr},
    {"T", Token::id_T},
    {"F", Token::id_F},
    {"nil", Token::id_nil},
    {"use", Token::id_use},
    {"fn", Token::id_fn},
    {"main", Token::id_main},
    {"ret", Token::id_ret},
    {"native", Token::id_native}};

std::unordered_map<char, char> Lexer::escapeCharacters{
    {'\'', '\''}, // --------------
    {'"', '\"'},
    {'?', '\?'},
    {'\\', '\\'},
    {'0', '\0'},
    {'a', '\a'},
    {'b', '\b'},
    {'f', '\f'},
    {'n', '\n'},
    {'r', '\r'},
    {'t', '\t'},
    {'v', '\v'}};

std::unordered_map<int, std::string> Lexer::tokenNames{
    {Token::none, "none"},
    {Token::eof, "eof"},
    {Token::identifier, "identifier"},
    {Token::dq_string, "dq_string"},
    {Token::sq_string, "sq_string"},
    {Token::dec_number, "dec_number"},
    {Token::hex_number, "hex_number"},
    {Token::bin_number, "bin_number"},
    {Token::oct_number, "oct_number"},

    // arithmetic operators:
    {Token::add_assign, "add_assign"},
    {Token::sub_assign, "sub_assign"},
    {Token::mul_assign, "mul_assign"},
    {Token::div_assign, "div_assign"},
    {Token::mod_assign, "mod_assign"},
    {Token::pow_assign, "pow_assign"},
    {Token::increment, "increment"},
    {Token::decrement, "decrement"},
    {Token::power, "power"},

    // bool operators
    {Token::lte, "lte"},
    {Token::gte, "gte"},
    {Token::log_and, "log_and"},
    {Token::log_or, "log_or"},

    // bit operations
    {Token::lshift, "lshift"},
    {Token::rshift, "rshift"},
    {Token::lshift_assign, "lshift_assign"},
    {Token::rshift_assign, "rshift_assign"},
    {Token::bit_and_assign, "bit_and_assign"},
    {Token::bit_or_assign, "bit_or_assign"},
    {Token::bit_complement_assign, "bit_complement_assign"},
    {Token::bit_xor_assign, "bit_xor_assign"},

    //  identifiers:
    {Token::id_if, "id_if"},
    {Token::id_elif, "id_elif"},
    {Token::id_el, "id_el"},
    {Token::id_for, "id_for"},
    {Token::id_whl, "id_whl"},
    {Token::id_brk, "id_brk"},
    {Token::id_cnt, "id_cnt"},
    {Token::id_T, "id_T"},
    {Token::id_F, "id_F"},
    {Token::id_nil, "id_nil"},
    {Token::id_use, "id_use"},
    {Token::id_fn, "id_fn"},
    {Token::id_main, "id_main"},
    {Token::id_ret, "id_ret"},
    {Token::id_native, "id_native"},
    {Token::id_op, "id_op"},

    // type IDs
    {Token::id_var, "id_var"},
    {Token::id_vac, "id_vac"},
    // valid var types:
    {Token::id_int, "id_int"},
    {Token::id_flt, "id_flt"},
    {Token::id_boo, "id_boo"},
    {Token::id_str, "id_str"},
    {Token::id_chr, "id_chr"}, };

int Lexer::readNext()
{
  static enum { normal, cr, lf, crlf } lineState = normal;

  lastChar = input->get();

  switch (lineState) {
  case normal: {
    switch (lastChar) {
    case '\r':
      lineState = cr;
      break;
    case '\n':
      lineState = lf;
      break;
    default:
      // lineState = normal;
      break;
    }
    break;
  }
  case cr: {
    switch (lastChar) {
    case '\r':
      // lineState = cr;
      goto newline;
    case '\n':
      lineState = crlf;
      break;
    default:
      lineState = normal;
      goto newline;
    }
    break;
  }
  case lf: {
    switch (lastChar) {
    case '\r':
      lineState = cr;
      goto newline;
    case '\n':
      // lineState = lf;
      goto newline;
    default:
      lineState = normal;
      goto newline;
    }
    break;
  }
  case crlf: {
    switch (lastChar) {
    case '\r':
      lineState = cr;
      goto newline;
    case '\n':
      lineState = lf;
      goto newline;
    default:
      lineState = normal;
      goto newline;
    }
    break;
  }
  newline:
    currentLine.clear();
    if (lastChar != '\r' && lastChar != '\n' && !input->eof())
      currentLine += lastChar;
    column = 1;
    ++line;
    return lastChar;
  }

  if (lastChar != '\r' && lastChar != '\n' && !input->eof())
    currentLine += lastChar;
  ++column;
  return lastChar;
}

static inline bool isoctal(int c) { return '0' <= c && c <= '7'; }

Token Lexer::nextToken()
{
  // skip all whitespaces
  while (std::isspace(lastChar))
    readNext();

  if (lastChar == '"') {
    return readString('"', Token::dq_string);
  }
  if (lastChar == '\'')
    return readString('\'', Token::sq_string);

  // identifiers [_a-zA-Z][_a-zA-Z0-9]*
  if (std::isalpha(lastChar) || lastChar == '_') {
    tokenString = lastChar;
    while (std::isalnum(readNext()) || lastChar == '_')
      tokenString += lastChar;

    auto res = identifierTokens.find(tokenString);
    if (res != identifierTokens.end())
      return makeToken(res->second);
    return makeToken(Token::identifier);
  }

  // leading 0:
  if (lastChar == '0')
    return readLeadingZeroNumber();

  // leading .:
  if (lastChar == '.')
    return readDotOrNumberDotPart();

  // leading 1-9:
  if (std::isdigit(lastChar))
    return readDecNumber();

  //-----------
  // operators
  if (lastChar == '/')
    return readDivideOperatorOrComment();

  if (lastChar == '*')
    return readMultiplyOperator();

  if (lastChar == '+')
    return readPlusOperator();

  if (lastChar == '-')
    return readMinusOperator();

  if (lastChar == '<')
    return readLTOperator();

  if (lastChar == '>')
    return readGTOperator();

  if (lastChar == '&')
    return readAndOperator();

  if (lastChar == '|')
    return readOrOperator();

  if (lastChar == '~')
    return readBitComplOperator();

  if (lastChar == '^')
    return readBitXorOperator();

  // -----------
  // end of file
  if (input->eof())
    return makeToken(Token::eof);

  // remaining single characters as tokens (i.e. operator symbols)
  tokenString = lastChar;
  int thisChar = lastChar;
  readNext();
  return makeToken(thisChar);
}

inline Token Lexer::readString(char sep, int tokenType)
{
  tokenString = ""; // skip opening '"'
  while (readNext() != sep && !input->eof()) {
    if (lastChar == '\\') { // escape character
      if (readNext() == 'x') {
        std::string nr;
        for (int i = 0; i < 2; ++i) {
          nr += readNext();
          if (!std::isxdigit(lastChar))
            error(std::string("unexpected '") + ((char)lastChar) +
                  "' in hex escape sequence");
        }
        tokenString += (char)std::stoi(nr, nullptr, 16);
      } else if ('0' <= lastChar && lastChar <= '9') {
        std::string nr;
        nr += lastChar;
        for (int i = 0; i < 2; ++i) {
          nr += readNext();
          if (lastChar < '0' || '9' < lastChar)
            error(std::string("unexpected '") + ((char)lastChar) +
                  "' in octal escape sequence");
        }
        tokenString += (char)std::stoi(nr, nullptr, 8);
      } else {
        auto pos = escapeCharacters.find(lastChar);
        if (pos == escapeCharacters.end())
          error(std::string("Unknown Escape sequence '\\") + ((char)lastChar) +
                '\'');
        tokenString += pos->second;
      }
    } else {
      tokenString += lastChar;
    }
  }
  if (input->eof())
    error("unterminated string at end of file");
  readNext(); // skip closing '"'
  return makeToken(tokenType);
}

inline Token Lexer::readLeadingZeroNumber()
{
  // decimal numbers (0|[1-9]\.[0-9]*|0?\.[0-9]+)([eE][0-9]+)?
  // hex numbers 0[xX][0-9]+
  // octal numbers 0[0-9]+

  tokenString = '0';
  if (readNext() == '.')
    return readDotOrNumberDotPart();
  if (lastChar == 'e' || lastChar == 'E')
    return readNumberExponentPart();

  // hex numbers:
  if (lastChar == 'x' || lastChar == 'X') {
    do
      tokenString += lastChar;
    while (std::isxdigit(readNext()));
    if (std::isalpha(lastChar))
      error(std::string("unexpected '") + ((char)lastChar) +
            "' in hexadecimal number literal");
    return makeToken(Token::hex_number);
  }
  if (lastChar == 'b' || lastChar == 'B') {
    do
      tokenString += lastChar;
    while (readNext() == '0' || lastChar == '1');
    if (std::isalnum(lastChar))
      error(std::string("unexpected '") + ((char)lastChar) +
            "' in binary number literal");
    return makeToken(Token::bin_number);
  }
  // octal numbers:
  if (isoctal(lastChar)) {
    do
      tokenString += lastChar;
    while (isoctal(readNext()));
    if (std::isalpha(lastChar))
      error(std::string("unexpected '") + ((char)lastChar) +
            "' in octal number literal");
    return makeToken(Token::oct_number);
  }
  // plain '0':
  return makeToken(Token::dec_number);
}
inline Token Lexer::readDecNumber()
{
  tokenString = lastChar;
  while (std::isdigit(readNext()))
    tokenString += lastChar;
  if (lastChar == '.') {
    return readDotOrNumberDotPart();
  }
  if (lastChar == 'e' || lastChar == 'E') {
    return readNumberExponentPart();
  }
  if (std::isalpha(lastChar))
    error(std::string("unexpected '") + ((char)lastChar) +
          "' in number literal");
  return makeToken(Token::dec_number);
}

inline Token Lexer::readDotOrNumberDotPart()
{
  // assert(lastChar == '.');
  tokenString += '.';
  if (std::isdigit(readNext())) {
    do
      tokenString += lastChar;
    while (std::isdigit(readNext()));
    if (lastChar == 'e' || lastChar == 'E') {
      return readNumberExponentPart();
    }
    if (std::isalpha(lastChar))
      error(std::string("unexpected '") + ((char)lastChar) +
            "' in number literal");
    return makeToken(Token::dec_number);
  }
  // dot operator
  return makeToken('.');
}

inline Token Lexer::readNumberExponentPart()
{
  // assert(lastChar == 'e' || lastChar == 'E')
  tokenString += lastChar;
  while (std::isdigit(readNext()))
    tokenString += lastChar;
  if (std::isalnum(lastChar))
    error(std::string("unexpected '") + ((char)lastChar) +
          "' in number literal");
  return makeToken(Token::dec_number);
}
//----
inline Token Lexer::readDivideOperatorOrComment()
{
  // read '/' '/=' '//' '/*'
  tokenString = '/';
  if (readNext() == '/') {
    do
      readNext();
    while (lastChar != '\r' && lastChar != '\n' && !input->eof());
    if (!input->eof()) {
      if (lastChar == '\r' && readNext() == '\n') // crlf
        readNext();
      if (!input->eof())
        return nextToken(); // recursive call TODO replace with goto top?
    }
    return makeToken(Token::eof);
  } else if (lastChar == '*') {
    readNext();
    while (!input->eof()) {
      if (lastChar == '*') {
        if (readNext() == '/') {
          readNext();         // eat '/'
          return nextToken(); // recursive call
        } else
          continue; // skip readNext() for cases like '**/'
      }
      readNext();
    }
    error("unexpected end of file in multi line comment");
  } else if (lastChar == '=') {
    appendAndNext();
    return makeToken(Token::div_assign);
  } else {
    return makeToken('/');
  }
}

inline Token Lexer::readMultiplyOperator()
{
  tokenString = '*';
  if (readNext() == '=') {
    appendAndNext();
    return makeToken(Token::mul_assign);
  }
  if (lastChar == '*') {
    appendAndNext();
    if (lastChar == '=') {
      appendAndNext();
      return makeToken(Token::pow_assign);
    }
    return makeToken(Token::power);
  }
  return makeToken('*');
}

inline Token Lexer::readModuloOperator()
{
  tokenString = '%';
  if (readNext() == '=') {
    appendAndNext();
    return makeToken(Token::mod_assign);
  }
  return makeToken('%');
}

inline Token Lexer::readPlusOperator()
{
  tokenString = '+';
  if (readNext() == '=') {
    appendAndNext();
    return makeToken(Token::add_assign);
  }
  if (lastChar == '+') {
    appendAndNext();
    return makeToken(Token::increment);
  }
  return makeToken('+');
}

inline Token Lexer::readMinusOperator()
{
  tokenString = '-';
  if (readNext() == '=') {
    appendAndNext();
    return makeToken(Token::sub_assign);
  }
  if (lastChar == '-') {
    appendAndNext();
    return makeToken(Token::decrement);
  }
  return makeToken('-');
}

inline Token Lexer::readLTOperator()
{
  tokenString = '<';
  if (readNext() == '=') {
    appendAndNext();
    return makeToken(Token::lte);
  }
  if (lastChar == '<') {
    appendAndNext();
    if (lastChar == '=') {
      appendAndNext();
      return makeToken(Token::lshift_assign);
    }
    return makeToken(Token::lshift);
  }
  return makeToken('<');
}
inline Token Lexer::readGTOperator()
{
  tokenString = '>';
  if (readNext() == '=') {
    appendAndNext();
    return makeToken(Token::gte);
  }
  if (lastChar == '>') {
    appendAndNext();
    if (lastChar == '=') {
      appendAndNext();
      return makeToken(Token::rshift_assign);
    }
    return makeToken(Token::rshift);
  }
  return makeToken('>');
}

inline Token Lexer::readAndOperator()
{
  tokenString = '&';
  if (readNext() == '&') {
    appendAndNext();
    return makeToken(Token::log_and);
  }
  if (lastChar == '=') {
    appendAndNext();
    return makeToken(Token::bit_and_assign);
  }
  return makeToken('&');
}
inline Token Lexer::readOrOperator()
{
  tokenString = '|';
  if (readNext() == '|') {
    appendAndNext();
    return makeToken(Token::log_or);
  }
  if (lastChar == '=') {
    appendAndNext();
    return makeToken(Token::bit_or_assign);
  }
  return makeToken('|');
}

inline Token Lexer::readBitComplOperator()
{
  tokenString = '~';
  if (readNext() == '=') {
    appendAndNext();
    return makeToken(Token::bit_complement_assign);
  }
  return makeToken('~');
}

inline Token Lexer::readBitXorOperator()
{
  tokenString = '^';
  if (readNext() == '=') {
    appendAndNext();
    return makeToken(Token::bit_xor_assign);
  }
  return makeToken('^');
}
