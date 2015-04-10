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

#ifndef CONTEXT_H
#define CONTEXT_H

#include <stack>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Transforms/Scalar.h> // for createPromoteMemoryToRegisterPass()
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

#include "type.h"
#include "ast_functions.h"

class VariableAlreadyDefinedError : public std::exception
{
public:
  const std::string variableName;
  VariableAlreadyDefinedError(std::string name)
      : variableName(std::move(name)) // implement row/col later
  {
  }
  const char *what() const noexcept override
  {
    return ("Variable '" + variableName + "' was already defined").c_str();
  }

  /*
  std::string getErrorLineHighlight()
  {
    std::string error(errorLine);
    error += '\n';
    for (int i = 1; i < col; ++i)
      error += '~';
    error += '^';
    return error;
  }
  */
};

class VariableNotDefinedError : public CodeGenError
{
public:
  const std::string variableName;
  VariableNotDefinedError(std::string name)
      : CodeGenError(nullptr, std::move(name)) // implement row/col later
  {
  }
  const char *what() const noexcept override
  {
    return ("Variable '" + variableName + "' was not defined").c_str();
  }

  /*
  std::string getErrorLineHighlight()
  {
    std::string error(errorLine);
    error += '\n';
    for (int i = 1; i < col; ++i)
      error += '~';
    error += '^';
    return error;
  }
  */
};

class GlobalContext {
public:
    llvm::LLVMContext &llvm_context;
    llvm::Module * const module;
    llvm::IRBuilder<> builder;
    llvm::legacy::FunctionPassManager fpm;
    std::string errorString;
    llvm::ExecutionEngine *execEngine;

    struct Fn {
        FunctionHead::FnReg regType;
        FunctionHead * fnHead;
//         Fn(FunctionHead::FnReg reg, FunctionHead *fh) : regType(reg), fnHead(fh) {}
    };
    std::unordered_multimap<std::string, Fn> declaredFunctions;

    GlobalContext();
};

struct ContextFrame
{
//   std::map<std::pair<std::string, llvm::Type>, llvm::AllocaInst *> variables;
  std::map<std::string, llvm::AllocaInst *> variables;
};

class Context
{
  // put parameters in lowest level frame
  std::stack<ContextFrame> frames;
  ContextFrame *top = nullptr;

public:
  GlobalContext &global;
  Type returnType = Type::none;

public:
  Context(GlobalContext &gl_ctx) : global(gl_ctx) { pushFrame(); }
  void pushFrame()
  {
    frames.emplace();
    top = &frames.top();
  }
  void popFrame()
  {
    frames.pop();
    if (frames.empty())
      top = nullptr;
    else
      top = &frames.top();
  }
  void putVar(const std::string &name, llvm::AllocaInst *aInst)
  {
    if (top->variables[name])
      throw VariableAlreadyDefinedError(name);
    top->variables[name] = aInst;
  }
  llvm::AllocaInst *getVar(const std::string &name) const
  {
    auto var = top->variables.find(name);
    if (var == top->variables.end())
      throw VariableNotDefinedError(name);
    return var->second;
  }

  int frameCount() const { return frames.size(); }
  // currentBlock() etc...
};

#endif // CONTEXT_H

