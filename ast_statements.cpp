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
  std::cout << "] to " << getTypeName(newType) << std::endl;
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
  Type type = Type::none;
  if (!expr) // void
  {
    ctx.global.builder.CreateRetVoid();
    type = Type::vac_t;
  }
  else
  {
    auto val = expr->codegen(ctx);
    if (!val) throw CodeGenError(this, "missing value for return statement");
    ctx.global.builder.CreateRet(val);
    type = expr->getType(ctx);
  }
  if (ctx.returnType == Type::none) {
    ctx.returnType = type;
  }
  else if (ctx.returnType != type)
  {
    throw CodeGenError(this, "incompatible return types '" + getTypeName(type) +
                               "' and '" + getTypeName(ctx.returnType) + '\'');
  }
  return nullptr;
}

llvm::Value *WhileStmt::codegen(Context &ctx)
{
  ctx.pushFrame();

  //     auto alloca = createEntryBlockAlloca(); // use, if variable definitions
  //     are allowed here

  auto condType = cond->getType(ctx);
  if (condType != Type::boo_t) {
    if (canCast(condType, Type::boo_t)) {
      cond = make_EPtr<CastExpr>(std::move(cond), Type::boo_t);
    }
    else
    {
      throw CodeGenError(this, "non-boolean or boolean-castable expression in "
                               "while statement condition");
    }
  }

  auto &builder = ctx.global.builder;

  auto headBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "whlhead", ctx.currentFn);
  auto loopBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "whlloop", ctx.currentFn);
  auto endBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "whlend", ctx.currentFn);

  builder.CreateBr(headBB);
  builder.SetInsertPoint(headBB);
  auto condVal = cond->codegen(ctx);
  builder.CreateCondBr(condVal, loopBB, endBB);
  builder.SetInsertPoint(loopBB);
  body->codegen(ctx);
  builder.CreateBr(headBB);
  builder.SetInsertPoint(endBB);

  ctx.popFrame();
  return nullptr;
}

llvm::Value *DoWhileStmt::codegen(Context &ctx)
{
  ctx.pushFrame();

  auto condType = cond->getType(ctx);
  if (condType != Type::boo_t) {
    if (canCast(condType, Type::boo_t)) {
      cond = make_EPtr<CastExpr>(std::move(cond), Type::boo_t);
    }
    else
    {
      throw CodeGenError(this, "non-boolean or boolean-castable expression in "
                               "do while statement condition");
    }
  }

  auto &builder = ctx.global.builder;

  auto loopBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "doloop", ctx.currentFn);
  auto endBB =
    llvm::BasicBlock::Create(ctx.global.llvm_context, "doend", ctx.currentFn);

  builder.CreateBr(loopBB);
  builder.SetInsertPoint(loopBB);
  body->codegen(ctx);
  auto condVal = cond->codegen(ctx);
  builder.CreateCondBr(condVal, loopBB, endBB);
  builder.SetInsertPoint(endBB);

  ctx.popFrame();
  return nullptr;
}

llvm::Value *ExprStmt::codegen(Context &ctx)
{
  expr->codegen(ctx);
  return nullptr;
}

Type VarDeclStmt::getType(Context &ctx)
{
  if (type == Type::inferred) {
    Type t = Type::none;
    for (auto &var : vars)
    {
      Type varType = var.second->getType(ctx);
      if (t == Type::none) {
        t = varType;
      }
      else if (t != varType)
      {
        throw CodeGenError(this, "mismatching types for variable declaration");
      }
    }
    type = t;
  }
  return type;
}

llvm::Value *VarDeclStmt::codegen(Context &ctx)
{
  auto llvm_type = getLLVMTypeFromType(ctx.global, getType(ctx));
  for (auto &var : vars)
  {
    auto alloca = createEntryBlockAlloca(ctx.currentFn, var.first, llvm_type);
    ctx.putVar(var.first, type, alloca);
    llvm::Value *init;
    if (var.second) // has initializer
    {
      init = var.second->codegen(ctx);
    }
    else
    {
      init = createDefaultValueConst(ctx, type);
    }
    VariableExpr tmpVarExp(var.first);
    createAssignment(ctx, init, &tmpVarExp);
  }
  return nullptr;
}