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

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <string>
#include <stdexcept>

#include "parser.h"
#include "context.h"
#include "compilationunit.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Support/DynamicLibrary.h"
#include "KaleidoscopeJIT.h"

class CompileError : public std::runtime_error {
public:
    CompileError(std::string s) : std::runtime_error(std::move(s)) {}
};

class CodeGenerator
{
  CompilationUnit compUnit;
  Parser parser;
  std::unique_ptr<llvm::orc::KaleidoscopeJIT> kaleidoscopeJIT;
  GlobalContext gl_ctx;
  llvm::Function* mainFn = nullptr;


#include <llvm/Support/DynamicLibrary.h>
public:
  CodeGenerator(CompilationUnit input)
    : compUnit(std::move(input)),
      parser(),
      kaleidoscopeJIT(std::move(llvm::orc::KaleidoscopeJIT::Create().get())),
      gl_ctx(kaleidoscopeJIT->getDataLayout())
  {
  }

  void generateLLVM(int optLevel, bool emitDebugInfo);

  void writeCodeToFile(const std::string& fileName);

  void runFunction(std::string name);
};

#endif // CODEGENERATOR_H
