#include <iostream>
#include <fstream>

#include <unordered_set>

#include "lexer.h"

std::unordered_set<std::string> includedFiles;

void printTokenInfo(std::string typeName, std::string tok,
                    std::string after = "")
{
  std::cout << typeName << "\033[1;29m" << tok << "\033[00m" << after
            << std::endl;
}

bool readFile(std::string input)
{
  if (includedFiles.find(input) != includedFiles.end()) {
    std::cout << "ignoring file " << input << " since it was already imported"
              << std::endl;
    return true;
  }
  includedFiles.insert(input);
  std::ifstream in(input);
  if (!in)
    return false;
  Lexer lex(in);
  try
  {
    Token token;
    while (token = lex.nextToken(), token.type != Token::eof) {
      switch (token.type) {
      case Token::dec_number:
        printTokenInfo("Dec Number ", token.tokenString);
        break;
      case Token::hex_number:
        printTokenInfo("Hex Number ", token.tokenString);
        break;
      case Token::bin_number:
        printTokenInfo("Bin Number ", token.tokenString);
        break;
      case Token::oct_number:
        printTokenInfo("Oct Number ", token.tokenString);
        break;
      case Token::identifier:
        printTokenInfo("Identifier ", token.tokenString);
        break;

      case Token::id_import: {
        std::cout << "Import statement " << std::endl;
        token = lex.nextToken();
        if (token.type != Token::dq_string)
          throw LexError(token.line, token.col,
                         "expected filename after import statement",
                         std::string("import \"") + token.tokenString + '"');
        printTokenInfo("Reading file ", token.tokenString);
        if (!readFile(token.tokenString))
          throw LexError(token.line, token.col,
                         "could not open file " + token.tokenString,
                         std::string("import \"") + token.tokenString + '"');
        printTokenInfo("Done reading ", token.tokenString);
        break;
      }

      case Token::dq_string:
        printTokenInfo("String \"", token.tokenString, "\"");
        break;
      case Token::sq_string:
        printTokenInfo("String '", token.tokenString, "'");
        break;
      default:
        if (token.type >= 0)
          printTokenInfo("Character '", std::string(1, (char)token.type),
                         "' (#" + std::to_string(token.type) + ')');
        else
          printTokenInfo("Unknown Token type: id = ",
                         std::to_string(token.type),
                         std::string(" on line ") + std::to_string(token.line) +
                             ':' + std::to_string(token.col));
        break;
      }
    }
  }
  catch (LexError &e)
  {
    std::cout << "Caught LexException on line " << e.line << ':' << e.col
              << " in file " << input << ": \033[1;31m" << e.what()
              << "\033[00m:" << std::endl;
    std::cout << e.getErrorLineHighlight() << std::endl;
  }
  return true;
}

int main(int argc, char **argv)
{
  auto filename = argc == 2 ? argv[1] : "../test.code";
  try { readFile(filename); }
  catch (LexError &e)
  {
    std::cout << "Caught LexException on line " << e.line << ':' << e.col
              << ": \033[1;31m" << e.what() << "\033[00m:" << std::endl;
    std::cout << e.getErrorLineHighlight() << std::endl;
  }
  return 0;
}
