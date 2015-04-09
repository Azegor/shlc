#include <iostream>
#include <fstream>

#include <unordered_set>

#include <llvm/IR/Function.h>

#include "lexer.h"
#include "parser.h"
#include "context.h"

std::unordered_set<std::string> includedFiles;

void printTokenInfo(std::string typeName, std::string tok,
                    std::string after = "")
{
  /* std::cout << typeName << "\033[1;29m" << tok << "\033[00m" << after
             << std::endl; */
  std::cout << "\'\033[1;29m" << tok << "\033[00m\' ";
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
        printTokenInfo("Dec Number ", token.str);
        break;
      case Token::hex_number:
        printTokenInfo("Hex Number ", token.str);
        break;
      case Token::bin_number:
        printTokenInfo("Bin Number ", token.str);
        break;
      case Token::oct_number:
        printTokenInfo("Oct Number ", token.str);
        break;
      case Token::identifier:
        printTokenInfo("Identifier ", token.str);
        break;

      case Token::id_use: {
        std::cout << "Use statement " << std::endl;
        token = lex.nextToken();
        if (token.type != Token::dq_string)
          throw LexError(token.line, token.col,
                         "expected filename after import statement",
                         std::string("import \"") + token.str + '"');
        printTokenInfo("Reading file ", token.str);
        if (!readFile(token.str))
          throw LexError(token.line, token.col,
                         "could not open file " + token.str,
                         std::string("import \"") + token.str + '"');
        printTokenInfo("Done reading ", token.str);
        break;
      }

      case Token::dq_string:
        printTokenInfo("String \"", token.str, "\"");
        break;
      case Token::sq_string:
        printTokenInfo("String '", token.str, "'");
        break;
      default:
        if (token.type >= 0)
          printTokenInfo("Char '", std::string(1, (char)token.type),
                         "' (#" + std::to_string(token.type) + ')');
        else
          printTokenInfo("Token ", token.str);
        /*
              std::cout << "Unknown Token type: id = \033[1;31m" << token.type
                        << "\033[00m on line " << token.line << ':' << token.col
                        << std::endl; */
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

void testLexer(const char *filename)
{
  try { readFile(filename); }
  catch (LexError &e)
  {
    std::cout << "Caught LexError on line " << e.line << ':' << e.col
              << ": \033[1;31m" << e.what() << "\033[00m:" << std::endl;
    std::cout << e.getErrorLineHighlight() << std::endl;
  }
  std::cout << std::endl;
}

void testParser(const char *filename)
{
  try
  {
    Parser parser;
    auto parseRes = parser.parse(filename);
    for (auto &r : parseRes)
      r->print();
  }
  catch (LexError &e)
  {
    std::cout << "Caught LexError on line " << e.line << ':' << e.col
              << ": \033[1;31m" << e.what() << "\033[00m:" << std::endl;
    std::cout << e.getErrorLineHighlight() << std::endl;
  }
  catch (ParseError &e)
  {
    std::cout << "Caught ParseError on line " << e.token.line << ':'
              << e.token.col << ": \033[1;31m" << e.what()
              << "\033[00m:" << std::endl;
    std::cout << e.getErrorLineHighlight() << std::endl;
  }
}

void testCodeGen(const char* filename)
{
  try
  {
    Parser parser;
    auto parseRes = parser.parse(filename);
    GlobalContext gl_ctx;
    for (auto &r : parseRes)
    {
        auto fn = r->codegen(gl_ctx);
        if (fn)
        {
            std::cout << r->getName() << ":" << std::endl;
            std::cout << ">>>-----------------------------" << std::endl;
            fn->dump();
            std::cout << "<<<-----------------------------" << std::endl;
        }
        else
        {
            std::cout << r->getName() << ": nullptr" << std::endl;
        }
    }
    std::cout << "===============================================" << std::endl;
    gl_ctx.module->dump();
  }
  catch (LexError &e)
  {
    std::cout << "Caught LexError on line " << e.line << ':' << e.col
              << ": \033[1;31m" << e.what() << "\033[00m:" << std::endl;
    std::cout << e.getErrorLineHighlight() << std::endl;
  }
  catch (ParseError &e)
  {
    std::cout << "Caught ParseError on line " << e.token.line << ':'
              << e.token.col << ": \033[1;31m" << e.what()
              << "\033[00m:" << std::endl;
    std::cout << e.getErrorLineHighlight() << std::endl;
  }
  catch (CodeGenError &e)
  {
    std::cout << "Caught CodeGenError: node" << e.node
              << ": \033[1;31m" << e.what()
              << "\033[00m:" << std::endl;
  }
}

int main(int argc, char **argv)
{
  auto filename = argc == 2 ? argv[1] : "../test.language";
  testLexer(filename);
  testParser(filename);
  testCodeGen(filename);
  return 0;
}

