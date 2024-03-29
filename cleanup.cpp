#include "cleanup.h"

#include <string>
#include "codegen.h"
#include "context.h"

using namespace std::string_literals;

CleanupManager::CleanupManager(GlobalContext &ctx)
    : ctx(ctx), cleanupTargetVarType(llvm::Type::getInt32Ty(ctx.llvm_context))
{
}

CleanupManager::~CleanupManager() {
  assert(blockScopeInfos.empty());
  assert(cleanupBlocks.empty());
  assert(cleanupScopes.empty());
}

void CleanupManager::enterFunction(Context *fn_ctx) {
  currentFnCtx = fn_ctx;
  assert(blockScopeInfos.empty());
  assert(cleanupBlocks.empty());
  cleanupBlocks.emplace();
  llvm::BasicBlock *returnLabel = fn_ctx->ret.BB;
  addJumpTargetInCurrentScope(returnLabel);
  jumpTargetHighestId = JumpTargetId(0);
  auto returnId = getJumpTargetId(returnLabel); // this makes returns always ID 1
  assert(returnId == JumpTargetId(1));
}
void CleanupManager::leaveFunction() {
  assert(blockScopeInfos.empty());
  assert(cleanupBlocks.size() == 1);
  cleanupBlocks.pop();
  jumpTargetIDs.clear();
  cleanupTargetAlloca = nullptr;
  currentFnCtx = nullptr;
}

void CleanupManager::addCleanupTargetAlloca() {
  cleanupTargetAlloca = createEntryBlockAlloca(currentFnCtx->currentFn, "cleanup_target"s, cleanupTargetVarType);
}

void CleanupManager::createJumpViaCleanupTarget(llvm::BasicBlock *target) {
  currentCleanupScope().addExternalTarget(target);
  assignJumpTargetId(getJumpTargetId(target));
  auto cleanupPadTarget = currentCleanupScope().getCleanupTarget(ctx.llvm_context);
  ctx.builder.CreateBr(cleanupPadTarget);
}

void CleanupManager::assignJumpTargetId(JumpTargetId id) {
  auto jumpTargetIdConst = llvm::ConstantInt::get(ctx.llvm_context, llvm::APInt(32, id, false));
  ctx.builder.CreateStore(jumpTargetIdConst, getCleanupTargetAlloca());
}

JumpTargetSet CleanupManager::createCleanup(const CleanupScope& cs, const CleanupBlockInfo &outerCleanupBlock) {
  // NOTE: jumpTargets.pop() was called before this function, meaning jumpTargets.top() is now the outer scope's targets
  llvm::Function *fn = currentFnCtx->currentFn;
  bool hasExtTargets = cs.hasExternalTargets();
  bool hasRegularControlFlow;
  if (hasExtTargets) {
    auto *cleanupBB = cs.cleanupBlockLabel;
    assert(cleanupBB);
    hasRegularControlFlow = blockScopeInfos.top().block->codeFlowReturn() != Statement::CodeFlowReturn::Never;
    if (hasRegularControlFlow) {
      assignJumpTargetId(0);
      ctx.builder.CreateBr(cleanupBB);
    }
    fn->getBasicBlockList().push_back(cleanupBB);
    ctx.builder.SetInsertPoint(cleanupBB);
    std::cout << "placing cleanup block " << cleanupBB << "\n";
  } else {
    assert(cs.cleanupBlockLabel == nullptr);
    std::cout << "unused cleanup block " << cs.cleanupBlockLabel << "\n";
  }

  makeXDecRefCall(*currentFnCtx, ctx.builder.CreateLoad(cs.varType, cs.varAlloca, "obj"), cs.destructor);
  ctx.builder.CreateStore(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(cs.varAlloca->getAllocatedType())), cs.varAlloca);

  if (hasExtTargets) {
    JumpTargetSet newOuterJumpTargets;
    llvm::BasicBlock *defaultTarget = nullptr;
    if (cleanupScopes.empty()) { // is the outmost scope
      defaultTarget = currentFnCtx->ret.BB;
    } else {
      defaultTarget = currentCleanupScope().getCleanupTarget(ctx.llvm_context); // NOTE: creates the block if necessary
    }
    auto targetVal = ctx.builder.CreateLoad(getCleanupTargetType(), getCleanupTargetAlloca(), "cleanup_target");
    auto switchStmt = ctx.builder.CreateSwitch(targetVal, defaultTarget, cs.externalTargets.size());
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
    llvm::BasicBlock *afterBB;
    if (hasRegularControlFlow) {
      // all remaining entries go to the outer cleanup label
      afterBB = llvm::BasicBlock::Create(ctx.llvm_context, "after_cleanup", currentFnCtx->currentFn);
    } else {
      afterBB = defaultTarget;
    }
    if (newOuterJumpTargets.empty()) {
      // we need a default label -> use for the 0 case too
      switchStmt->setDefaultDest(afterBB); // change default target
    } else if (hasRegularControlFlow) {
      // 0 is always the "continue" target
      switchStmt->addCase(llvm::ConstantInt::get(ctx.llvm_context, llvm::APInt(32, 0, false)), afterBB);
    }
    ctx.builder.SetInsertPoint(afterBB);
    return newOuterJumpTargets;
  } else {
    return {};
  }
}
