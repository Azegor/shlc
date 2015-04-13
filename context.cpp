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

#include "context.h"

#include <llvm/ExecutionEngine/JIT.h> // needed because of error "JIT has not been linked in"

// std::string GlobalContext::errorString;

GlobalContext::GlobalContext()
    : llvm_context(llvm::getGlobalContext()),
      module(new llvm::Module("my jit module", llvm_context)),
      builder(llvm_context),
      fpm(module),
      errorString(),
      //             execEngine(llvm::EngineBuilder(std::unique_ptr<llvm::Module>(module)).setErrorStr(&errorString)
      execEngine(
        llvm::EngineBuilder(module)
          .setErrorStr(&errorString)
          //           .setMCJITMemoryManager(std::make_unique<llvm::SectionMemoryManager>())
          //           .setMCJITMemoryManager(new
          //           llvm::SectionMemoryManager)
          .create())
{
  if (!execEngine)
    throw std::runtime_error("Could not create Execution Engine: " +
                             errorString);

  // Set up the optimizer pipeline.  Start with registering info about how the
  // target lays out data structures.
  module->setDataLayout(execEngine->getDataLayout());
#if 1
  // Provide basic AliasAnalysis support for GVN.
  fpm.add(llvm::createBasicAliasAnalysisPass());
  // Promote allocas to registers.
  fpm.add(llvm::createPromoteMemoryToRegisterPass());
  // Do simple "peephole" optimizations and bit-twiddling optzns.
  fpm.add(llvm::createInstructionCombiningPass());
  // Reassociate expressions.
  fpm.add(llvm::createReassociatePass());
  // Eliminate Common SubExpressions.
  fpm.add(llvm::createGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  fpm.add(llvm::createCFGSimplificationPass());

  // some other passes:
  fpm.add(llvm::createLoopSimplifyPass());
//   fpm.add(llvm::createTailCallEliminationPass());

//   fpm.add(llvm::createSimpleLoopUnrollPass());
//   fpm.add(llvm::createDeadInstEliminationPass());
//   fpm.add(llvm::createDeadCodeEliminationPass());
//   fpm.add(llvm::createAggressiveDCEPass());
//   fpm.add(llvm::createLoopDeletionPass());
#endif

  fpm.doInitialization();
}

FunctionHead *GlobalContext::getFunction(const std::string &name) const
{
  auto range = declaredFunctions.equal_range(name);
  if (range.first == range.second) return nullptr;
  if (std::distance(range.first, range.second) > 1) // multiple overloads
    throw CodeGenError("Function overloading not supported yet");
  return range.first->second.fnHead;
}

FunctionHead *GlobalContext::getFunctionOverload(
  const std::string &name, const std::vector<Type> &args) const
{
  auto range = declaredFunctions.equal_range(name);
  if (range.first == range.second) return nullptr;
  std::vector<FunctionHead *> needCastOverloads;
  for (auto i = range.first; i != range.second; ++i)
  {
    auto overloadFit = i->second.fnHead->getOverloadFit(args);
    if (overloadFit ==
        FunctionHead::OverloadFit::Perfect) // can only occure once!
      return i->second.fnHead;
    if (overloadFit == FunctionHead::OverloadFit::Cast)
      needCastOverloads.push_back(i->second.fnHead);
  }
  if (needCastOverloads.size() == 1) return needCastOverloads[0];
  return nullptr;
}
