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

#include <vector>
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
  VariableNotDefinedError(const std::string &name)
      : CodeGenError(nullptr, "Variable '" + name + "' was not defined") // implement row/col later
  {
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

class GlobalContext
{
public:
  llvm::LLVMContext &llvm_context;
  llvm::Module *const module;
  llvm::IRBuilder<> builder;
  llvm::legacy::FunctionPassManager fpm;
  std::string errorString;
  llvm::ExecutionEngine *execEngine;

  struct Fn
  {
    FunctionHead::FnReg regType;
    FunctionHead *fnHead;
    //         Fn(FunctionHead::FnReg reg, FunctionHead *fh) : regType(reg),
    //         fnHead(fh) {}
  };
  std::unordered_multimap<std::string, Fn> declaredFunctions;

  FunctionHead *getFunction(const std::string &name);

  GlobalContext();
};

struct ContextFrame
{
  struct VarInfo {Type type; llvm::AllocaInst *alloca; };
  //   std::map<std::pair<std::string, llvm::Type>, llvm::AllocaInst *>
  //   variables;
  std::map<std::string, VarInfo> variables;
};

class Context
{
  // put parameters in lowest level frame
  std::vector<ContextFrame> frames;
  ContextFrame *top = nullptr;

public:
  GlobalContext &global;
  Type returnType = Type::none;
  llvm::Function *currentFn;

public:
  Context(GlobalContext &gl_ctx) : global(gl_ctx) { pushFrame(); }
  void pushFrame()
  {
    frames.emplace_back();
    top = &frames.back();
  }
  void popFrame()
  {
    frames.pop_back();
    if (frames.empty())
      top = nullptr;
    else
      top = &frames.back();
  }
  void putVar(const std::string &name, Type type, llvm::AllocaInst *aInst)
  {
    if (top->variables.find(name) != top->variables.end())
      throw VariableAlreadyDefinedError(name);
    top->variables[name] = {type, aInst};
  }
  const ContextFrame::VarInfo &getVar(const std::string &name) const
  {
    for(auto frame = frames.rbegin(); frame != frames.rend(); ++frame)
    {
      auto var = frame->variables.find(name);
      if (var != frame->variables.end())
        return var->second;
    }
    throw VariableNotDefinedError(name);
  }

  llvm::AllocaInst *getVarAlloca(const std::string &name) const { return getVar(name).alloca; }
  Type getVariableType(const std::string &name) const { return getVar(name).type; }

  int frameCount() const { return frames.size(); }
  // currentBlock() etc...
};

#endif // CONTEXT_H
