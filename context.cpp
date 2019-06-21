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
#include "parser.h"

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
          .create()),
      cleanupManager(*this)
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
}

GlobalContext::~GlobalContext()
{
    delete execEngine;
}
void GlobalContext::initCompilationUnit(const Parser &p, bool isOptimized)
{
  for (int i = 0, numLex = p.getNumLexers(); i < numLex; ++i) {
    auto &filePath = p.getLexer(i).getFilePath();
    allDIFiles.emplace_back(diBuilder.createFile(filePath.filename().c_str(), filePath.parent_path().c_str()));
  }

  diCompUnit = diBuilder.createCompileUnit(llvm::dwarf::DW_LANG_C_plus_plus, allDIFiles[0], "SHLC", isOptimized, "", 0);

  // TODO: this is a fix from "https://groups.google.com/forum/#!topic/llvm-dev/1O955wQjmaQ"
  module->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 3);
  module->addModuleFlag(llvm::Module::Error, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);
}

void GlobalContext::finalizeDIBuilder()
{
  diBuilder.finalize();
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

llvm::Function *GlobalContext::getMallocFn() const
{
  if (!mallocFunction) {
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
  return mallocFunction;
}

llvm::Function *GlobalContext::getFreeFn() const
{
  if (!freeFunction) {
    freeFunction = llvm::Function::Create(
        llvm::FunctionType::get(
            llvmTypeRegistry.getBuiltinType(BuiltinTypeKind::vac_t),
            {llvmTypeRegistry.getVoidPointerType()},
            false),
        llvm::Function::ExternalLinkage,
        "free",
        module
    );
  }
  return freeFunction;
}

llvm::Function *GlobalContext::getIncRefFn()
{
  if (!incRefFunction) {
    incRefFunction = createIncDecRefFn(false, "__shlcore_incref");
  }
  return incRefFunction;
}

llvm::Function *GlobalContext::getXIncRefFn()
{
  if (!xIncRefFunction) {
    auto incRefFn = getIncRefFn(); // generates it if necessary
    xIncRefFunction = createNullCheckDelegationFn(incRefFn, "__shlcore_xincref");
  }
  return xIncRefFunction;
}

llvm::Function *GlobalContext::getDecRefFn()
{
  if (!decRefFunction) {
    decRefFunction = createIncDecRefFn(true, "__shlcore_decref");
  }
  return decRefFunction;
}

llvm::Function *GlobalContext::getXDecRefFn()
{
  if (!xDecRefFunction) {
    auto incRefFn = getDecRefFn(); // generates it if necessary
    xDecRefFunction = createNullCheckDelegationFn(incRefFn, "__shlcore_xdecref");
  }
  return xDecRefFunction;
}

llvm::Function *GlobalContext::createIncDecRefFn(bool isDecrement, llvm::StringRef name)
{
  auto fn = llvm::Function::Create(
      llvm::FunctionType::get(
          llvmTypeRegistry.getBuiltinType(BuiltinTypeKind::vac_t),
          {llvmTypeRegistry.getVoidPointerType()}, // TODO
          false),
      llvm::Function::LinkOnceODRLinkage,
      name,
      module
  );
  auto oldInsertBlock = builder.GetInsertBlock();
  auto oldInsertPoint = builder.GetInsertPoint();

  // create body
  llvm::BasicBlock *entryBB =
      llvm::BasicBlock::Create(llvm_context, "entry", fn);
  builder.SetInsertPoint(entryBB);

  auto arg1 = &*fn->arg_begin();
  auto counterCast = builder.CreateBitCast(arg1, llvmTypeRegistry.getRefCounterPointerType(), "refcnt_ptr");
  auto oldC = builder.CreateLoad(counterCast, "cnt_val");
  auto newC = builder.CreateAdd(oldC, llvm::ConstantInt::get(llvm_context, llvm::APInt(64, isDecrement ? -1 : 1)));
  builder.CreateStore(newC, counterCast);
  if (isDecrement) {
    llvm::BasicBlock *freeBB =
        llvm::BasicBlock::Create(llvm_context, "free", fn);
    llvm::BasicBlock *retBB =
        llvm::BasicBlock::Create(llvm_context, "return", fn);
    auto isNullCond = builder.CreateICmpEQ(newC, llvm::ConstantInt::get(llvm_context, llvm::APInt(64, 0)), "is_zero");
    builder.CreateCondBr(isNullCond, freeBB, retBB);

    builder.SetInsertPoint(freeBB);
    builder.CreateCall(getFreeFn(), {arg1});
    builder.CreateBr(retBB);

    builder.SetInsertPoint(retBB);
  }
  builder.CreateRetVoid();

  // reset insert point to current function
  builder.SetInsertPoint(oldInsertBlock, oldInsertPoint);
  return fn;
}

llvm::Function *GlobalContext::createNullCheckDelegationFn(llvm::Function *callee, llvm::StringRef name)
{
  auto fn = llvm::Function::Create(
      llvm::FunctionType::get(
          llvmTypeRegistry.getBuiltinType(BuiltinTypeKind::vac_t),
          {llvmTypeRegistry.getVoidPointerType()}, // TODO
          false),
      llvm::Function::LinkOnceODRLinkage,
      name,
      module
  );
  auto oldInsertBlock = builder.GetInsertBlock();
  auto oldInsertPoint = builder.GetInsertPoint();

  // create body
  llvm::BasicBlock *entryBB =
      llvm::BasicBlock::Create(llvm_context, "entry", fn);
  llvm::BasicBlock *callBB =
      llvm::BasicBlock::Create(llvm_context, "nonnull", fn);
  llvm::BasicBlock *retBB =
      llvm::BasicBlock::Create(llvm_context, "return", fn);
  builder.SetInsertPoint(entryBB);
  auto arg1 = &*fn->arg_begin();
  auto cmp = builder.CreateICmpNE(arg1, llvm::ConstantPointerNull::get(llvmTypeRegistry.getVoidPointerType()));
  builder.CreateCondBr(cmp, callBB, retBB);

  builder.SetInsertPoint(callBB);
  builder.CreateCall(callee, {arg1});
  builder.CreateBr(retBB);

  builder.SetInsertPoint(retBB);
  builder.CreateRetVoid();

  // reset insert point to current function
  builder.SetInsertPoint(oldInsertBlock, oldInsertPoint);
  return fn;
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

void GlobalContext::enterDebugScope(llvm::DIScope *lexBlock)
{
    diLexicalBlocks.push(lexBlock);
}

void GlobalContext::leaveDebugScope()
{
    diLexicalBlocks.pop();
}

void GlobalContext::emitDILocation(AstNode *astNode)
{
  if (astNode) {
    emitDILocation(astNode->srcLoc.startToken.line, astNode->srcLoc.startToken.col);
  } else {
    builder.SetCurrentDebugLocation(llvm::DebugLoc());
  }
}

void GlobalContext::emitDILocation(size_t line, size_t col)
{
  if(!emitDebugInfo) { return; }
  llvm::DIScope *scope;
  if (diLexicalBlocks.empty()) {
      scope = diCompUnit;
  } else {
      scope = diLexicalBlocks.top();
  }
  builder.SetCurrentDebugLocation(llvm::DebugLoc::get(line, col, scope));
}
