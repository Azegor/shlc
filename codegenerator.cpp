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

#include <llvm/Support/FileSystem.h>

void CodeGenerator::generateCode(int optLevel)
{
  try
  {
    auto parseRes = parser.parse(fileName);
    gl_ctx.optimizeLevel = optLevel;
    gl_ctx.initPMB();
    gl_ctx.initFPM();
    //     llvm::Function *mainFn = nullptr;
    for (auto &r : parseRes)
    {
      auto fn = r->codegen(gl_ctx);
      if (fn && fn->getName() == "main") mainFn = fn;
    }
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

void CodeGenerator::writeCodeToFile(const std::string &outFileName)
{
  //   gl_ctx.module->dump();

  std::error_code err;
  llvm::raw_fd_ostream outFile(outFileName, err,
                               llvm::sys::fs::F_RW | llvm::sys::fs::F_Text);
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
//   auto main_ptr = gl_ctx.execEngine->getPointerToFunction(mainFn);
  auto main_ptr = gl_ctx.execEngine->getFunctionAddress("main");
  if (main_ptr) {
    void (*_main)() = (void (*)())main_ptr;
    _main();
  }
  else
  {
    throw std::runtime_error("cannot call main() function");
  }
}
