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

// #include <llvm/ExecutionEngine/JIT.h> // needed because of error "JIT has not been linked in"
#include <llvm/Transforms/IPO.h>

// std::string GlobalContext::errorString;

GlobalContext::GlobalContext()
//     : llvm_context(llvm::getGlobalContext()),
    : _llvm_context(std::make_unique<llvm::LLVMContext>()),
      llvm_context(*_llvm_context),
      module(new llvm::Module("shl_global_module", llvm_context)),
      builder(llvm_context),
      diBuilder(*module),
      llvmTypeRegistry(*this),
      pm_builder(),
      fpm(),
      mpm(),
      errorString(),
                  execEngine(llvm::EngineBuilder(std::unique_ptr<llvm::Module>(module)).setErrorStr(&errorString)
//       execEngine(
//         llvm::EngineBuilder(module)
//           .setErrorStr(&errorString)
//           //           .setMCJITMemoryManager(std::make_unique<llvm::SectionMemoryManager>())
//           //           .setMCJITMemoryManager(new
//           //           llvm::SectionMemoryManager)
          .create())
{
  if (!execEngine)
    throw std::runtime_error("Could not create Execution Engine: " +
                             errorString);

  // Set up the optimizer pipeline.  Start with registering info about how the
  // target lays out data structures.
  module->setDataLayout(execEngine->getDataLayout());

#if 0
  // Provide basic AliasAnalysis support for GVN.
  fpm.add(llvm::createBasicAliasAnalysisPass());
  // Promote allocas to registers.
  fpm.add(llvm::createPromoteMemoryToRegisterPass());
  // Do simple "peephole" optimizations and bit-twiddling optzns.
  fpm.add(llvm::createInstructionCombiningPass());
  // Reassociate expressions.
  fpm.add(llvm::createReassociatePass());
  // Eliminate Common SubExpressions.
  fpm.add(llvm::createEarlyCSEPass());
  fpm.add(llvm::createGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  //   fpm.add(llvm::createFlattenCFGPass());
  fpm.add(llvm::createCFGSimplificationPass()); // broken for if statements

  // some other passes:
  fpm.add(llvm::createLoopSimplifyPass());
//   fpm.add(llvm::createTailCallEliminationPass());

//   fpm.add(llvm::createSimpleLoopUnrollPass());
//   fpm.add(llvm::createDeadInstEliminationPass());
//   fpm.add(llvm::createDeadCodeEliminationPass());
//   fpm.add(llvm::createAggressiveDCEPass());
//   fpm.add(llvm::createLoopDeletionPass());
#endif

  //   fpm.add(llvm::createPromoteMemoryToRegisterPass());
  //   fpm.add(llvm::createInstructionCombiningPass());
  //   fpm.add(llvm::createCFGSimplificationPass());
  //   fpm.add(llvm::createTailCallEliminationPass());
  //   fpm.add(llvm::createCFGSimplificationPass());
  //   fpm.add(llvm::createReassociatePass());
  //   fpm.add(llvm::createLoopRotatePass());
  //   fpm.add(llvm::createInstructionCombiningPass());

  //   fpm.doInitialization();

  // Module passes
  //   mpm.addPass(llvm::createFunctionAttrsPass());
  //   mpm.addPass(llvm::createFunctionInliningPass());
  //   mpm.addPass(llvm::createGlobalOptimizerPass());
  //   mpm.addPass(llvm::createDeadArgEliminationPass());

  // others
  mallocFunction = llvm::Function::Create(
    llvm::FunctionType::get(
      llvmTypeRegistry.getVoidPointerType(),
      {llvmTypeRegistry.getBuiltinType(BuiltinTypeKind::int_t)},
      false),
    llvm::Function::ExternalLinkage,
    "malloc",
    module
  );
  //mallocFunction->addFnAttr(llvm::Attribute::NoAlias);
}

GlobalContext::~GlobalContext()
{
    delete execEngine;
}

void GlobalContext::initCompilationUnit(llvm::StringRef filePath, bool isOptimized)
{
    auto idx = filePath.find_last_of('/');
    idx = idx == llvm::StringRef::npos ? 0 : idx + 1;
    llvm::StringRef dirName = filePath.take_front(idx);
    llvm::StringRef fileName = filePath.drop_front(idx);

    diCompUnit = diBuilder.createCompileUnit(llvm::dwarf::DW_LANG_C_plus_plus, diBuilder.createFile(fileName, "."), "SHLC", isOptimized, "", 0);
    // TODO set different for every included file!
    diFile = diBuilder.createFile(fileName, dirName);
}

void GlobalContext::finalizeDIBuilder()
{
  diBuilder.finalize();

  // TODO: this is a fix from "https://groups.google.com/forum/#!topic/llvm-dev/1O955wQjmaQ"
  module->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 3);
  module->addModuleFlag(llvm::Module::Error, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);
}

void GlobalContext::initPMB()
{
  if (optimizeLevel > 0) {
    pm_builder.Inliner = llvm::createFunctionInliningPass(optimizeLevel, 275, false);
  }
  pm_builder.OptLevel = optimizeLevel;
}

void GlobalContext::initFPM()
{
  fpm = std::make_unique<llvm::legacy::FunctionPassManager>(module);
  pm_builder.populateFunctionPassManager(*fpm);
}
void GlobalContext::finalizeFPM() { fpm->doFinalization(); }

void GlobalContext::initMPM()
{
  mpm = std::make_unique<llvm::legacy::PassManager>();
  pm_builder.populateModulePassManager(*mpm);
}
void GlobalContext::finalizeMPM() {}

FunctionHead *GlobalContext::getFunction(const std::string &name) const
{
  auto range = declaredFunctions.equal_range(name);
  if (range.first == range.second) return nullptr;
  if (std::distance(range.first, range.second) > 1) // multiple overloads
    throw CodeGenError("Function overloading not supported yet");
  return range.first->second.fnHead;
}

FunctionHead *GlobalContext::getFunctionOverload(
  const std::string &name, const std::vector<Type*> &args) const
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

llvm::DISubroutineType *GlobalContext::createDIFunctionType(FunctionHead *fnHead)
{
  llvm::SmallVector<llvm::Metadata *, 8> EltTys;

  // Add the result type.
  if (fnHead->getName() == "main") {
    EltTys.push_back(llvmTypeRegistry.getDIType(TypeRegistry::getBuiltinType(BuiltinTypeKind::int_t)));
  } else {
    EltTys.push_back(llvmTypeRegistry.getDIType(fnHead->getReturnType()));
  }

  for (auto *argType :fnHead->getArgTypes()) {
    llvm::DIType *argDbgType = llvmTypeRegistry.getDIType(argType);
    EltTys.push_back(argDbgType);
  }

  return diBuilder.createSubroutineType(diBuilder.getOrCreateTypeArray(EltTys));
}
