/*
 * Copyright 2015 <copyright holder> <email>
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

#include "codegenerator.h"

#include <sstream>

#include <llvm/Support/FileSystem.h>
#include <llvm/IR/Mangler.h>
#include <llvm/Support/raw_ostream.h>

void CodeGenerator::generateCode(int optLevel, bool emitDebugInfo)
{
  std::stringstream errorMsg;
  try
  {
    parser.addIncludePath(std::filesystem::current_path());
#ifdef STD_LIBRARY_PATH
    parser.addIncludePath(STD_LIBRARY_PATH);
#endif
    std::string inputFileName = compUnit.getFileName();
    auto parseRes = parser.parse(std::move(compUnit));
    gl_ctx.optimizeLevel = optLevel;
    gl_ctx.emitDebugInfo = emitDebugInfo;
    if (emitDebugInfo) {
      gl_ctx.initCompilationUnit(parser, optLevel != 0);
    }
    gl_ctx.initPMB();
    gl_ctx.initFPM();
    //     llvm::Function *mainFn = nullptr;
    // --- codegen start ---
    for (auto &r : parseRes)
    {
      auto fn = r->codegen(gl_ctx);
      if (fn && fn->getName() == "main") mainFn = fn;
    }
    // --- codegen end ---
    gl_ctx.finalizeDIBuilder();
    gl_ctx.finalizeFPM();
    gl_ctx.initMPM();
    gl_ctx.mpm->run(*gl_ctx.module);
    gl_ctx.finalizeMPM();

    //     if (mainFn) {
    //       auto main_ptr = gl_ctx.execEngine->getPointerToFunction(mainFn);
    //       //         auto main_ptr =
    //       gl_ctx.execEngine->getFunctionAddress("main");
    //       void (*_main)() = (void (*)())main_ptr;
    //       if (_main) {
    //         std::cout << " --- calling main() ---" << std::endl;
    //         _main();
    //       }
    //     }
  }
  catch (LexError &e)
  {
    errorMsg << "Caught LexError on line " << e.line << ':' << e.col
              << ": \033[1;31m" << e.what() << "\033[00m:" << std::endl;
    errorMsg << e.getErrorLineHighlight() << std::endl;
    throw CompileError(errorMsg.str());
  }
  catch (ParseError &e)
  {
    errorMsg << "Caught ParseError: "
              << "\033[1;31m" << e.what() << "\033[00m " << std::endl;
    errorMsg << e.getErrorLineHighlight(parser) << std::endl;
    throw CompileError(errorMsg.str());
  }
  catch (CodeGenError &e)
  {
    errorMsg << "Caught CodeGenError: "
              << "\033[1;31m" << e.what() << "\033[00m " << std::endl;
    errorMsg << e.getErrorLineHighlight(parser) << std::endl;
    throw CompileError(errorMsg.str());
  }
}

void CodeGenerator::writeCodeToFile(const std::string &outFileName)
{
  //   gl_ctx.module->dump();

  std::error_code err;
  llvm::raw_fd_ostream outFile(outFileName, err, llvm::sys::fs::CD_CreateAlways,
                                 llvm::sys::fs::FA_Write, llvm::sys::fs::OF_Text);
  gl_ctx.module->print(outFile, nullptr, true);
//   outFile << *gl_ctx.module;
  outFile.close();
}

void CodeGenerator::runFunction(std::string name)
{
  if (!mainFn) {
    std::cerr << "no main function found, exiting" << std::endl;
    exit(1);
  }
  auto H = kaleidoscopeJIT.addModule(std::move(gl_ctx.module));

  auto ExprSymbol = kaleidoscopeJIT.findSymbol(name);
  assert(ExprSymbol && "Function not found");

  // Get the symbol's address and cast it to the right type (takes no
  // arguments, returns a double) so we can call it as a native function.
  long (*_main)() = (long (*)())(intptr_t)cantFail(ExprSymbol.getAddress());
  auto retVal = _main();
  fprintf(stderr, "Main function returned %ld\n", retVal);

  // Delete the anonymous expression module from the JIT.
//   kaleidoscopeJIT.removeModule(H);
}
