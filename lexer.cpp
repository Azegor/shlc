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

  /*
  if (lastChar == '*')
    return readTimesOperator();

  if (lastChar == '+')
    return readPlusOperator();

  if (lastChar == '-')
    return readMinusOperator();

  if (lastChar == '<')
    return readLTOperator();

  if (lastChar == '>')
    return readGTOperator();
  */
  // -----------
  // end of file
  if (input->eof())
    return makeToken(Token::eof);

  // remaining single characters as tokens (i.e. operator symbols)
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
Token Lexer::readDivideOperatorOrComment()
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
	  readNext();
          return nextToken(); // recursive call
	}
        else
          continue; // skip readNext() for cases like '**/'
      }
      readNext();
    }
    error("unexpected end of file in multi line comment");
  } else if (lastChar == '=') {
    tokenString += lastChar;
    readNext();
    return makeToken(Token::div_assign);
  } else {
    return makeToken(Token::divide);
  }
}
