#ifndef CLEANUP_H
#define CLEANUP_H

#include <unordered_map>
#include <unordered_set>
#include <stack>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>

#include "type.h"

class Context;
class GlobalContext;

using JumpTargetSet = std::unordered_set<llvm::BasicBlock*>;

using JumpTargetId = int;

class CleanupBlockInfo {
    JumpTargetSet jumpTargets;

public:
    void addJumpTarget(llvm::BasicBlock *target) { jumpTargets.emplace(target); }
    bool isJumpTargetInScope(llvm::BasicBlock *target) const {
        return jumpTargets.find(target) != jumpTargets.end();
    }
};

class CleanupScope {
    friend class CleanupManager;

    // const llvm::Type *varType;
    llvm::AllocaInst *varAlloca;
    // clang::CXXDestructorDecl *destructor;
    JumpTargetSet externalTargets;
    llvm::BasicBlock *cleanupBlockLabel;

public:
    CleanupScope(llvm::LLVMContext &llvm_ctx, /*const llvm::Type *varType,*/ llvm::AllocaInst *alloca/*, clang::CXXDestructorDecl *destructor*/)
        : /*varType(varType),*/ varAlloca(alloca), /*destructor(destructor),*/ cleanupBlockLabel(llvm::BasicBlock::Create(llvm_ctx, "cleanup"))
    {
    }
    CleanupScope(const CleanupScope &) = delete;
    CleanupScope(CleanupScope &&) = default;
    CleanupScope& operator=(const CleanupScope &) = delete;
    CleanupScope& operator=(CleanupScope &&) = default;

    void addExternalTargets(JumpTargetSet &&jts) {
//         externalTargets.merge(jts);
        externalTargets.insert(jts.begin(), jts.end());
    }
    void addExternalTarget(llvm::BasicBlock *target) {
        externalTargets.emplace(target);
    }
    bool isExternalTarget(llvm::BasicBlock *target) const {
        return externalTargets.find(target) != externalTargets.end();
    }
    bool hasExternalTargets() const { return !externalTargets.empty(); }
    llvm::BasicBlock *getCleanupTarget() const { return cleanupBlockLabel; }
};

class CleanupManager {
    GlobalContext &ctx;
    Context *currentFnCtx = nullptr;

    llvm::Type *cleanupTargetVarType;

    std::stack<int> blockScopeSizes;
    std::stack<CleanupBlockInfo> cleanupBlocks;
    std::stack<CleanupScope> cleanupScopes;
    llvm::AllocaInst *cleanupTargetAlloca = nullptr;

    JumpTargetId jumpTargetHighestId;
    std::unordered_map<llvm::BasicBlock *, JumpTargetId> jumpTargetIDs;

public:
    CleanupManager(GlobalContext &ctx);
    ~CleanupManager();

    void enterFunction(Context *fn_ctx);
    void leaveFunction();

    void enterBlockScope() {
        blockScopeSizes.emplace(0);
    }
    void leaveBlockScope() {
        assert(!blockScopeSizes.empty());
        createCleanupsForCurrentBlockScope();
        blockScopeSizes.pop();
    }
    void createCleanupsForCurrentBlockScope() {
        while (blockScopeSizes.top() > 0) {
            cleanupBlocks.pop(); // pop inner targets, which are not needed anymore
            assert(!cleanupBlocks.empty());
            CleanupScope topScope = std::move(cleanupScopes.top());
            leaveCleanupScope();
            auto targetLabels = createCleanup(topScope, cleanupBlocks.top());
            if (!cleanupScopes.empty()) {
                cleanupScopes.top().addExternalTargets(std::move(targetLabels));
            } else {
                assert(targetLabels.empty());
            }
        }
    }

    void addCleanupTargetAlloca();
    llvm::AllocaInst *getCleanupTargetAlloca() {
        if (!cleanupTargetAlloca) {
            addCleanupTargetAlloca();
        }
        return cleanupTargetAlloca;
    }

    void enterCleanupScope(llvm::LLVMContext &llvmContext, llvm::AllocaInst *varAlloca/*, clang::CXXDestructorDecl *destructor*/) {
        assert(!blockScopeSizes.empty());
        cleanupScopes.emplace(llvmContext, /*varType,*/ varAlloca/*, destructor*/);
        cleanupBlocks.emplace();
        ++blockScopeSizes.top();
    }

    void addJumpTargetInCurrentScope(llvm::BasicBlock *target) {
        cleanupBlocks.top().addJumpTarget(target);
    }
    bool isJumpTargetInScope(llvm::BasicBlock *target) const {
        return cleanupBlocks.top().isJumpTargetInScope(target);
    }
    JumpTargetId getJumpTargetId(llvm::BasicBlock *target) {
        auto pos = jumpTargetIDs.find(target);
        if (pos != jumpTargetIDs.end()) {
            return pos->second;
        }
        JumpTargetId newId(++jumpTargetHighestId);
        jumpTargetIDs.emplace(target, newId);
        return newId;
    }
    void leaveCleanupScope() {
        assert(blockScopeSizes.top() > 0);
        --blockScopeSizes.top();
        assert(!cleanupScopes.empty());
        cleanupScopes.pop();
    }

    CleanupScope &currentCleanupScope() { assert(!cleanupScopes.empty()); return cleanupScopes.top(); }
    CleanupBlockInfo &currentCleanupBlock() { assert(!cleanupBlocks.empty()); return cleanupBlocks.top(); }

    void createJumpViaCleanupTarget(llvm::BasicBlock *target);

    void assignJumpTargetId(JumpTargetId id);

    JumpTargetSet createCleanup(const CleanupScope& cs, const CleanupBlockInfo &outerCleanupBlock);
};

#endif // CLEANUP_H
