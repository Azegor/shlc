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
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Transforms/Scalar.h> // for createPromoteMemoryToRegisterPass()
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

#include "type.h"
#include "ast_functions.h"

class VariableAlreadyDefinedError : public CodeGenError
{
public:
  VariableAlreadyDefinedError(std::string name, const AstNode *node = nullptr)
      : CodeGenError("Variable '" + name + "' was already defined", node)
  {
  }
};

class VariableNotDefinedError : public CodeGenError
{
public:
  const std::string variableName;
  VariableNotDefinedError(const std::string &name,
                          const AstNode *node = nullptr)
      : CodeGenError("Variable '" + name + "' was not defined", node)
  {
  }
};

struct GlobalVars
{
  struct GlobVarInfo
  {
    Type *type;
    llvm::GlobalVariable *var;
  };
  std::map<std::string, GlobVarInfo> globVars;
};

class GlobalContext
{
  std::unique_ptr<llvm::LLVMContext> _llvm_context;
public:
  llvm::LLVMContext &llvm_context;
  llvm::Module *const module;
  llvm::IRBuilder<> builder;
  llvm::DIBuilder diBuilder;
  llvm::DICompileUnit *diCompUnit = nullptr;
  llvm::DIFile * diFile; // TODO: change to
  std::stack<llvm::DIScope*> diLexicalBlocks;
  LLVMTypeRegistry llvmTypeRegistry;
  llvm::PassManagerBuilder pm_builder;
  //   llvm::legacy::FunctionPassManager fpm;
  std::unique_ptr<llvm::legacy::FunctionPassManager> fpm;
  //   llvm::legacy::PassManager mpm;
  std::unique_ptr<llvm::legacy::PassManager> mpm;
  std::string errorString;
  llvm::ExecutionEngine *execEngine;
  int optimizeLevel = 0;
  bool emitDebugInfo = false;

  struct Fn
  {
    FunctionHead::FnReg regType;
    FunctionHead *fnHead;
    //         Fn(FunctionHead::FnReg reg, FunctionHead *fh) : regType(reg),
    //         fnHead(fh) {}
  };
  std::unordered_multimap<std::string, Fn> declaredFunctions;
  GlobalVars globalVars;
  std::unordered_map<std::string, std::string> stringConstants;

  llvm::Function *mallocFunction = nullptr;

  GlobalContext();
  ~GlobalContext();

  void initCompilationUnit(llvm::StringRef filePath, bool isOptimized);
  void finalizeDIBuilder();

  void initPMB();

  void initFPM();
  void finalizeFPM();

  void initMPM();
  void finalizeMPM();

  FunctionHead *getFunction(const std::string &name) const; // unused!
  FunctionHead *getFunctionOverload(const std::string &name,
                                    const std::vector<Type*> &args) const;

  void putGlobalVar(const std::string &name, Type *type,
                    llvm::GlobalVariable *globVar)
  {
    auto var = globalVars.globVars.find(name);
    if (var != globalVars.globVars.end())
      throw VariableAlreadyDefinedError(name);
    globalVars.globVars[name] = {type, globVar};
  }
  const GlobalVars::GlobVarInfo &getGlobalVar(const std::string &name) const
  {
    auto var = globalVars.globVars.find(name);
    if (var == globalVars.globVars.end()) throw VariableNotDefinedError(name);
    return var->second;
  }

  llvm::GlobalVariable *getGlobalVarInst(const std::string &name) const
  {
    return getGlobalVar(name).var;
  }

  Type *getGlobalVarType(const std::string &name) const
  {
    return getGlobalVar(name).type;
  }

  llvm::Function *getMallocFn() const { return mallocFunction; }

  // TODO: move into extra class
  llvm::DISubroutineType *createDIFunctionType(FunctionHead *fnHead);
  void enterDebugScope(llvm::DIScope *lexBlock);
  void leaveDebugScope();
  void emitDILocation(AstNode *astNode);
  void emitDILocation(size_t line, size_t col);
};

struct ContextFrame
{
  struct VarInfo
  {
    Type *type;
    llvm::AllocaInst *alloca;
  };
  //   std::map<std::pair<std::string, llvm::Type>, llvm::AllocaInst *>
  //   variables;
  std::map<std::string, VarInfo> variables;
};

struct ReturnData
{
  Type *type = nullptr;
  llvm::AllocaInst *val = nullptr;
  llvm::BasicBlock *BB = nullptr;
};

class Context
{
  // put parameters in lowest level frame
  std::vector<ContextFrame> frames;
  ContextFrame *top = nullptr;

public:
  GlobalContext &global;
  ReturnData ret;
  llvm::Function *currentFn;
  std::vector<LoopStmt *> currentLoops;

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
  void putVar(const std::string &name, Type *type, llvm::AllocaInst *aInst)
  {
    if (top->variables.find(name) != top->variables.end())
      throw VariableAlreadyDefinedError(name);
    top->variables[name] = {type, aInst};
  }
  const ContextFrame::VarInfo &getVar(const std::string &name) const
  {
    for (auto frame = frames.rbegin(); frame != frames.rend(); ++frame)
    {
      auto var = frame->variables.find(name);
      if (var != frame->variables.end()) return var->second;
    }
    throw VariableNotDefinedError(name);
  }

  llvm::AllocaInst *getVarAlloca(const std::string &name) const
  {
    return getVar(name).alloca;
  }
  Type *getVariableType(const std::string &name) const
  {
    return getVar(name).type;
  }

  int frameCount() const { return frames.size(); }

  void pushLoop(LoopStmt *loop) { currentLoops.push_back(loop); }
  bool popLoop()
  {
    if (currentLoops.empty()) return false;
    currentLoops.pop_back();
    return true;
  }
  LoopStmt *currentLoop() const
  {
    if (currentLoops.empty()) throw CodeGenError("currently not in a loop");
    return currentLoops.back();
  }
};

#endif // CONTEXT_H
