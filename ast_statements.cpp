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

#include "ast_statements.h"

#include <iostream>

#include "ast_expressions.h"
#include "context.h"
#include "codegen.h"

void ReturnStmt::print(int indent)
{
  printIndent(indent);
  if (expr) {
    std::cout << "Return: [" << std::endl;
    expr->print(indent + 1);
    printIndent(indent);
    std::cout << ']' << std::endl;
  }
  else
  {
    std::cout << "Return void" << std::endl;
  }
}

void CastExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "Cast: [" << std::endl;
  expr->print(indent + 1);
  std::cout << std::endl;
  printIndent(indent);
  std::cout << "] to " << newType->getName() << std::endl;
}

void IfStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "If: (";
  cond->print();
  std::cout << ")" << std::endl;
  thenExpr->print(indent);
  if (elseExpr) {
    printIndent(indent);
    std::cout << "else" << std::endl;
    elseExpr->print(indent);
  }
  printIndent(indent);
  std::cout << std::endl;
}

void WhileStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "While: (";
  cond->print();
  std::cout << ")" << std::endl;
  body->print(indent);
  printIndent(indent);
  std::cout << std::endl;
}

void DoWhileStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "Do:";
  body->print(indent);
  printIndent(indent);
  std::cout << "While (";
  cond->print();
  std::cout << ')' << std::endl;
}

void ForStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "For: (";
  init->print();
  std::cout << ',';
  cond->print();
  std::cout << ',';
  incr->print();
  std::cout << ")" << std::endl;
  body->print(indent);
  printIndent(indent);
  std::cout << std::endl;
}

void BreakStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "[Break]" << std::endl;
}

void ContinueStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "[Continue]" << std::endl;
}

void ExprStmt::print(int indent)
{
  expr->print(indent);
  std::cout << std::endl;
}

llvm::Value *ReturnStmt::codegen(Context &ctx)
{
  ctx.global.emitDILocation(this);
  if (!expr) // void
  {
    if (ctx.ret.type != TypeRegistry::getVoidType())
      throw CodeGenError("cannot return void in non-void function", this);
    ctx.global.createBrCheckCleanup(ctx.ret.BB);
  }
  else
  {
    Type *type = expr->getType(ctx);
    if (ctx.ret.type != type) {
      if (canImplicitlyCast(type, ctx.ret.type)) {
        expr = make_EPtr<CastExpr>(expr->srcLoc, std::move(expr), ctx.ret.type);
      }
      else
      {
        throw CodeGenError("incompatible return types '" + type->getName() +
                             "' and '" + ctx.ret.type->getName() + '\'',
                           this);
      }
    }
    auto val = expr->codegen(ctx);
    assert(val);
    ctx.global.builder.CreateStore(val, ctx.ret.val);
    ctx.global.createBrCheckCleanup(ctx.ret.BB);
  }
  return nullptr;
}

llvm::Value *IfStmt::codegen(Context &ctx)
{
  ctx.pushFrame(this);

  auto condType = cond->getType(ctx);
  if (condType != TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t)) {
    if (canCast(condType, TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t))) {
      cond = make_EPtr<CastExpr>(cond->srcLoc, std::move(cond), TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t));
    }
    else
    {
      throw CodeGenError("non-boolean or non-boolean-castable expression in "
                         "if statement condition",
                         this);
    }
  }

  ctx.global.emitDILocation(this);
  auto &builder = ctx.global.builder;
  auto thenBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "then", ctx.currentFn);
  llvm::BasicBlock *elseBB;
  if (elseExpr)
    elseBB =
      llvm::BasicBlock::Create(ctx.global.llvm_context, "else");
  auto endBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "ifend");

  auto condVal = cond->codegen(ctx);
  if (elseExpr)
    builder.CreateCondBr(condVal, thenBB, elseBB);
  else
    builder.CreateCondBr(condVal, thenBB, endBB);

  builder.SetInsertPoint(thenBB);
  {
    ctx.pushFrame(thenExpr.get());
    thenExpr->codegen(ctx);
    ctx.popFrame();
  }
  if (thenExpr->codeFlowReturn() != Statement::CodeFlowReturn::Never)
    builder.CreateBr(endBB);

  if (elseExpr) {
    ctx.currentFn->getBasicBlockList().push_back(elseBB);
    builder.SetInsertPoint(elseBB);
    {
      ctx.pushFrame(elseExpr.get());
      elseExpr->codegen(ctx);
      ctx.popFrame();
    }
    if (elseExpr->codeFlowReturn() != Statement::CodeFlowReturn::Never)
      builder.CreateBr(endBB);
  }

  if (codeFlowReturn() != Statement::CodeFlowReturn::Never) {
    ctx.currentFn->getBasicBlockList().push_back(endBB);
    builder.SetInsertPoint(endBB);
  }

  ctx.popFrame();
  return nullptr;
}

llvm::Value *WhileStmt::codegen(Context &ctx)
{
  ctx.pushFrame(this);

  auto condType = cond->getType(ctx);
  if (condType != TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t)) {
    if (canCast(condType, TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t))) {
      cond = make_EPtr<CastExpr>(cond->srcLoc, std::move(cond), TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t));
    }
    else
    {
      throw CodeGenError("non-boolean or non-boolean-castable expression in "
                         "while statement condition",
                         this);
    }
  }

  ctx.global.emitDILocation(this);
  auto &builder = ctx.global.builder;

  auto headBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "whlhead", ctx.currentFn);
  auto loopBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "whlloop");
  auto endBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "whlend");

  ctx.global.cleanupManager.addJumpTargetInCurrentScope(headBB);
  ctx.global.cleanupManager.addJumpTargetInCurrentScope(endBB);

  contBB = headBB;
  breakBB = endBB;

  builder.CreateBr(headBB);

  builder.SetInsertPoint(headBB);
  auto condVal = cond->codegen(ctx);
  builder.CreateCondBr(condVal, loopBB, endBB);

  ctx.currentFn->getBasicBlockList().push_back(loopBB);
  builder.SetInsertPoint(loopBB);
  {
    ctx.pushLoop(this, body.get());
    body->codegen(ctx);
    ctx.popLoop();
  }
  if (body->codeFlowReturn() != Statement::CodeFlowReturn::Never)
    builder.CreateBr(headBB);

  ctx.currentFn->getBasicBlockList().push_back(endBB);
  builder.SetInsertPoint(endBB);

  ctx.popFrame();
  return nullptr;
}

llvm::Value *DoWhileStmt::codegen(Context &ctx)
{
  ctx.pushFrame(this);

  auto condType = cond->getType(ctx);
  if (condType != TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t)) {
    if (canCast(condType, TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t))) {
      cond = make_EPtr<CastExpr>(cond->srcLoc, std::move(cond), TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t));
    }
    else
    {
      throw CodeGenError("non-boolean or non-boolean-castable expression in "
                         "do while statement condition",
                         this);
    }
  }

  ctx.global.emitDILocation(this);
  auto &builder = ctx.global.builder;

  auto loopBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "doloop", ctx.currentFn);
  auto headBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "dohead");
  auto endBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "doend");

  ctx.global.cleanupManager.addJumpTargetInCurrentScope(headBB);
  ctx.global.cleanupManager.addJumpTargetInCurrentScope(endBB);

  contBB = headBB;
  breakBB = endBB;

  builder.CreateBr(loopBB);

  builder.SetInsertPoint(loopBB);
  {
    ctx.pushLoop(this, body.get());
    body->codegen(ctx);
    ctx.popLoop();
  }
  auto bodyReturnsCF =
    body->codeFlowReturn() != Statement::CodeFlowReturn::Never;
  if (bodyReturnsCF) {
    builder.CreateBr(headBB);
  }

  auto brb = body->branchBehaviour();
  // if cfr returns or it has a continue statement we need the function head!
  if (bodyReturnsCF || (bool)(brb & Statement::BranchBehaviour::Continues)) {
    // INFO: if body never return CF, this doesn't need to be generated
    ctx.currentFn->getBasicBlockList().push_back(headBB);
    builder.SetInsertPoint(headBB);
    auto condVal = cond->codegen(ctx);
    builder.CreateCondBr(condVal, loopBB, endBB);

    ctx.currentFn->getBasicBlockList().push_back(endBB);
    builder.SetInsertPoint(endBB);
  }
  else if ((bool)(brb & Statement::BranchBehaviour::Breaks)) {
    // no break, don't even need a block after the loop
    ctx.currentFn->getBasicBlockList().push_back(endBB);
    builder.SetInsertPoint(endBB);
  }

  ctx.popFrame();
  return nullptr;
}

llvm::Value *ForStmt::codegen(Context &ctx)
{

  ctx.pushFrame(this);

  // 1. init (only once)
  // -> loop:
  // 2. test
  // 3.1 body
  // 3.2 increment

  ctx.global.emitDILocation(this);
  auto &builder = ctx.global.builder;

  auto headBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "forhead", ctx.currentFn);
  auto loopBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "forloop");
  auto incBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "forinc");
  auto endBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "forend");

  ctx.global.cleanupManager.addJumpTargetInCurrentScope(incBB);
  ctx.global.cleanupManager.addJumpTargetInCurrentScope(endBB);

  contBB = incBB;
  breakBB = endBB;

  if (init) init->codegen(ctx);
  builder.CreateBr(headBB);

  builder.SetInsertPoint(headBB);
  if (cond) {
    auto condType = cond->getType(ctx);
    if (condType != TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t)) {
      if (canCast(condType, TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t))) {
        cond = make_EPtr<CastExpr>(cond->srcLoc, std::move(cond), TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t));
      }
      else
      {
        throw CodeGenError("non-boolean or non-boolean-castable expression in "
                           "while statement condition",
                           this);
      }
    }
    auto condVal = cond->codegen(ctx);
    builder.CreateCondBr(condVal, loopBB, endBB);
  }
  else
  {
    builder.CreateBr(loopBB); // assume true
  }

  ctx.currentFn->getBasicBlockList().push_back(loopBB);
  builder.SetInsertPoint(loopBB);
  {
    ctx.pushLoop(this, body.get());
    body->codegen(ctx);
    ctx.popLoop();
  }
  builder.CreateBr(incBB);
  ctx.currentFn->getBasicBlockList().push_back(incBB);
  builder.SetInsertPoint(incBB);
  if (incr) incr->codegen(ctx);
  if (body->codeFlowReturn() != Statement::CodeFlowReturn::Never)
    builder.CreateBr(headBB);

  ctx.currentFn->getBasicBlockList().push_back(endBB);
  builder.SetInsertPoint(endBB);

  ctx.popFrame();
  return nullptr;
}

llvm::Value *BreakStmt::codegen(Context &ctx)
{
  ctx.global.emitDILocation(this);
  ctx.global.createBrCheckCleanup(ctx.currentLoop()->breakTarget());
  return nullptr;
}

llvm::Value *ContinueStmt::codegen(Context &ctx)
{
  ctx.global.emitDILocation(this);
  ctx.global.createBrCheckCleanup(ctx.currentLoop()->continueTarget());
  return nullptr;
}

llvm::Value *ExprStmt::codegen(Context &ctx)
{
  ctx.global.emitDILocation(this);
  auto val = expr->codegen(ctx);
  if (dynamic_cast<NewExpr*>(expr.get())) {
    // unused new expr -> have to delete it
    auto voidPtr = ctx.global.builder.CreateBitCast(val, ctx.global.llvmTypeRegistry.getVoidPointerType(), "obj_vcst");
    ctx.global.builder.CreateCall(ctx.global.getFreeFn(), {voidPtr});
  }
  return nullptr;
}

Type *VarDeclStmt::getType(Context &ctx)
{
  if (!type) {
    Type *t = nullptr;
    for (auto &var : vars)
    {
      Type *varType = var.second->getType(ctx);
      if (!t) {
        t = varType;
      }
      else if (t != varType)
      {
        throw CodeGenError("mismatching types for variable declaration", this);
      }
    }
    type = t;
  }
  return type;
}

llvm::Value *VarDeclStmt::codegen(Context &ctx)
{
  auto &gctx = ctx.global;
  gctx.emitDILocation(this);

  auto deducedType = getType(ctx);
  auto llvm_type = ctx.global.llvmTypeRegistry.getType(deducedType);
  for (auto &var : vars)
  {
    auto alloca = createEntryBlockAlloca(ctx.currentFn, var.first, llvm_type);
    ctx.putVar(var.first, deducedType, alloca);

    if (gctx.emitDebugInfo) {
      int lineNr = srcLoc.startToken.line;
      int colNr = srcLoc.startToken.col;
      // NOTE: the passed parameter indices should start at 1 -> +1
      auto *scope = gctx.diLexicalBlocks.top();
      llvm::DILocalVariable *d = gctx.diBuilder.createAutoVariable(
          scope, var.first, gctx.currentDIFile, lineNr, gctx.llvmTypeRegistry.getDIType(deducedType)/*, true*/);
      gctx.diBuilder.insertDeclare(alloca, d, gctx.diBuilder.createExpression(),
          llvm::DebugLoc::get(lineNr, colNr, scope),
          gctx.builder.GetInsertBlock());
    }

    llvm::Value *init;
    if (var.second) // has initializer
    {
      init = var.second->codegen(ctx);
    }
    else
    {
      init = createDefaultValueConst(ctx, llvm_type);
    }
    VariableExpr tmpVarExp(srcLoc, var.first);
    if (deducedType->getKind() == BuiltinTypeKind::cls_t) {
      handleAssignmentRefCounts(ctx, nullptr, init);
      ctx.global.cleanupManager.enterCleanupScope(ctx.global.llvm_context, alloca);
    }
    createAssignment(ctx, init, &tmpVarExp);
  }
  return nullptr;
}
