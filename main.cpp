#include <iostream>
#include <fstream>

#include <cstdio>
// #include <csignal>

#include <llvm/IR/DataLayout.h>
#include <string>
#include <unordered_set>

#include <llvm/IR/Function.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/ExecutionEngine/MCJIT.h> // necessary to link MCJIT (with static linking)

#include "lexer.h"
#include "parser.h"
#include "context.h"
#include "compiler.h"
#include "compilationunit.h"

using namespace std::literals;

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
  Lexer lex((CompilationUnit(input)));
  try
  {
    Token token;
    while (token = lex.nextToken(), token.type != Token::eof)
    {
      switch (token.type)
      {
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

        case Token::id_use:
        {
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
                          << "\033[00m on line " << token.line << ':' <<
             token.col
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
    exit(1);
  }
  return true;
}

void testLexer(const char *filename)
{
  try
  {
    readFile(filename);
  }
  catch (LexError &e)
  {
    std::cout << "Caught LexError on line " << e.line << ':' << e.col
              << ": \033[1;31m" << e.what() << "\033[00m:" << std::endl;
    std::cout << e.getErrorLineHighlight() << std::endl;
    exit(1);
  }
  std::cout << std::endl;
}

void testParser(const char *filename)
{
  Parser parser;
  try
  {
    auto parseRes = parser.parse(CompilationUnit(filename));
    for (auto &r : parseRes)
      r->print();
  }
  catch (LexError &e)
  {
    std::cout << "Caught LexError on line " << e.line << ':' << e.col
              << ": \033[1;31m" << e.what() << "\033[00m:" << std::endl;
    std::cout << e.getErrorLineHighlight() << std::endl;
    exit(1);
  }
  catch (ParseError &e)
  {
    std::cout << "Caught ParseError: "
              << "\033[1;31m" << e.what() << "\033[00m " << std::endl;
    std::cout << e.getErrorLineHighlight(parser) << std::endl;
    exit(1);
  }
}

void testCodeGen(const char *filename, const char *outName, bool optimize)
{
  Parser parser;
  try
  {
    auto parseRes = parser.parse(CompilationUnit(filename));
    std::unique_ptr<llvm::TargetMachine> tm(llvm::EngineBuilder().selectTarget());
    llvm::DataLayout dl = tm->createDataLayout();
    GlobalContext gl_ctx(dl);
    gl_ctx.optimizeLevel = optimize ? 3 : 0;
    gl_ctx.initFPM();
    llvm::Function *mainFn = nullptr;
    for (auto &r : parseRes)
    {
      //       std::cout << "generating code for function " << r->getName() <<
      //       std::endl;
      auto fn = r->codegen(gl_ctx);
      if (fn && fn->getName() == "main") mainFn = fn;
      //         auto _ = gl_ctx.execEngine->getPointerToFunction(fn);
      //         if (fn)
      //         {
      //             std::cout << r->getName() << ":" << std::endl;
      //             std::cout << ">>>-----------------------------" <<
      //             std::endl;
      //             fn->dump();
      //             std::cout << "<<<-----------------------------" <<
      //             std::endl;
      //         }
      //         else
      //         {
      //             std::cout << r->getName() << ": nullptr" << std::endl;
      //         }
    }

    gl_ctx.finalizeFPM();
    gl_ctx.initMPM();
    gl_ctx.mpm->run(*gl_ctx.module);
    gl_ctx.finalizeMPM();

    std::cout << "===============================================" << std::endl;
    // gl_ctx.module->dump();

    std::error_code err;
    llvm::raw_fd_ostream outFile(outName, err, llvm::sys::fs::CD_CreateAlways,
                                 llvm::sys::fs::FA_Write, llvm::sys::fs::OF_Text);
    std::cout << "Wringing generated code to " << outName << std::endl;
    gl_ctx.module->print(outFile, nullptr);
    outFile.close();

    if (mainFn) {
//       auto main_ptr = gl_ctx.execEngine->getPointerToFunction(mainFn);
      //         auto main_ptr = gl_ctx.execEngine->getFunctionAddress("main");
//       void (*_main)() = (void (*)())main_ptr;
//       if (_main) {
//         std::cout << " --- calling main() ---" << std::endl;
//         _main();
//       }
    }
  }
  catch (LexError &e)
  {
    std::cout << "Caught LexError on line " << e.line << ':' << e.col
              << ": \033[1;31m" << e.what() << "\033[00m:" << std::endl;
    std::cout << e.getErrorLineHighlight() << std::endl;
    exit(1);
  }
  catch (ParseError &e)
  {
    std::cout << "Caught ParseError: "
              << "\033[1;31m" << e.what() << "\033[00m " << std::endl;
    std::cout << e.getErrorLineHighlight(parser) << std::endl;
    exit(1);
  }
  catch (CodeGenError &e)
  {
    std::cout << "Caught CodeGenError: "
              << "\033[1;31m" << e.what() << "\033[00m " << std::endl;
    std::cout << e.getErrorLineHighlight(parser) << std::endl;
    exit(1);
  }
}

void init_llvm()
{

  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  
//   llvm::InitializeAllTargetInfos();
//   llvm::InitializeAllTargets();
//   llvm::InitializeAllTargetMCs();
//   llvm::InitializeAllAsmParsers();
//   llvm::InitializeAllAsmPrinters();
}

// void FPE_ExceptionHandler(int nSig)
// {
//   std::cerr << "caught SIGFPE" << std::endl;
//   std::abort();
// }

int main(int argc, char **argv)
{
  //   signal(SIGFPE, FPE_ExceptionHandler);

  init_llvm();
//   auto filename = (argc >= 2) ? argv[1] : "../test.language";
//   auto outName = (argc >= 3) ? argv[2] : "out.ll";
//   auto optimize = (argc >= 4) ? argv[3] != ""s : false;
  //   testLexer(filename);
  //   testParser(filename);
  //   testCodeGen(filename, outName, optimize);
  Compiler compiler(argc, argv);
  int res = 0;
#ifdef __AFL_HAVE_MANUAL_CONTROL
  while (__AFL_LOOP(1000)) {
#endif

    res = compiler.run();

#ifdef __AFL_HAVE_MANUAL_CONTROL
  }
#endif
  return res;
}
