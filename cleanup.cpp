#include "cleanup.h"

#include <string>
#include "codegen.h"

using namespace std::string_literals;

void CleanupManager::enterFunction(Context *fn_ctx) {
  currentFnCtx = fn_ctx;
  assert(blockScopeSizes.empty());
  assert(cleanupBlocks.empty());
  cleanupBlocks.emplace();
  llvm::BasicBlock *returnLabel = fn_ctx->ret.BB;
  addJumpTargetInCurrentScope(returnLabel);
  jumpTargetHighestId = JumpTargetId(0);
  auto returnId = getJumpTargetId(returnLabel); // this makes returns always ID 1
  assert(returnId == JumpTargetId(1));
}
void CleanupManager::leaveFunction() {
  assert(blockScopeSizes.empty());
  assert(cleanupBlocks.size() == 1);
  cleanupBlocks.pop();
  jumpTargetIDs.clear();
  cleanupTargetVar = nullptr;
  currentFnCtx = nullptr;
}

void CleanupManager::addCleanupTargetVar() {
  cleanupTargetVar = createEntryBlockAlloca(currentFnCtx->currentFn, "cleanup_target"s, cleanupTargetVarType);
}

void CleanupManager::createJumpViaCleanupTarget(llvm::BasicBlock *target) {
  currentCleanupScope().addExternalTarget(target);
  assignJumpTargetId(getJumpTargetId(target));
  auto cleanupPadTarget = currentCleanupScope().getCleanupTarget();
  ctx.builder.CreateBr(cleanupPadTarget);
}

void CleanupManager::assignJumpTargetId(JumpTargetId id) {
  auto jumpTargetIdConst = llvm::ConstantInt::get(ctx.llvm_context, llvm::APInt(32, id, false));
  ctx.builder.CreateStore(jumpTargetIdConst, getCleanupTargetVar());
}

JumpTargetSet CleanupManager::createCleanup(const CleanupScope& cs, const CleanupBlockInfo &outerCleanupBlock) {
  // NOTE: jumpTargets.pop() was called before this function, meaning jumpTargets.top() is now the outer scope's targets
  llvm::Function *fn = currentFnCtx->currentFn;
  bool hasExtTargets = cs.hasExternalTargets();
  if (hasExtTargets) {
    assignJumpTargetId(0);
    ctx.builder.SetInsertPoint(cs.cleanupBlockLabel);
  }

  ctx.builder.CreateCall(ctx.getXDecRefFn(), {cs.varAlloca});

  if (hasExtTargets) {
    JumpTargetSet newOuterJumpTargets;
    llvm::BasicBlock *defaultTarget = nullptr;
    if (cleanupScopes.empty()) { // is the outmost scope
      defaultTarget = currentFnCtx->ret.BB;
    } else {
      defaultTarget = currentCleanupScope().cleanupBlockLabel;
    }
    auto switchStmt = ctx.builder.CreateSwitch(getCleanupTargetVar(), defaultTarget, cs.externalTargets.size());
    for (auto extTarget : cs.externalTargets) {
      if (!outerCleanupBlock.isJumpTargetInScope(extTarget)) {
        newOuterJumpTargets.emplace(extTarget);
      } else if (extTarget != defaultTarget) {
        JumpTargetId id = getJumpTargetId(extTarget);
        auto jumpTargetIdConst = llvm::ConstantInt::get(ctx.llvm_context, llvm::APInt(32, id, false));
        switchStmt->addCase(jumpTargetIdConst, extTarget);
      } else {
        // target == default target -> no need for a case stmt
      }
    }
    // all remaining entries go to the outer cleanup label
    llvm::BasicBlock *continueBB = llvm::BasicBlock::Create(ctx.llvm_context, "cleanup.after", currentFnCtx->currentFn);
    if (newOuterJumpTargets.empty()) {
      // we need a default label -> use for the 0 case too
      switchStmt->setDefaultDest(continueBB); // change default target
    } else {
      // 0 is always the "continue" target
      switchStmt->addCase(llvm::ConstantInt::get(ctx.llvm_context, llvm::APInt(32, 0, false)), continueBB);
//       switchStmt->setDefaultDest(defaultTarget); // already set
    }
    fn->getBasicBlockList().push_back(continueBB); // add continue block
    return newOuterJumpTargets;
  } else {
    return {};
  }
}
