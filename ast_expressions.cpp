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

#include "ast_expressions.h"

#include <iostream>
#include <memory>

#include <llvm/IR/Constants.h>
#include <llvm/ADT/APInt.h>

#include "ast.h"
#include "context.h"
#include "ast_functions.h"
#include "codegen.h"

void VariableExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Variable: " << name << ']';
}

void FunctionCallExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Function Call: " << name << '(';
  for (auto &e : args)
    e->print();
  std::cout << ")]";
}

void ConstantExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Number, type: " << getTypeName(type) << "]";
}
void IntNumberExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Int Number: " << value << "]";
}
void CharConstExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Char Const: '" << value << "']";
}
void FltNumberExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Flt Number: " << value << "]";
}

void BoolConstExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Bool constant: " << std::boolalpha << value << "]";
}

void StringConstExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[String constant: \"" << value << "\"]";
}

void VarDeclStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "[Variable Decl: ";
  printList(vars, [](VarEnties::value_type &s)
            {
    return s.first;
  });
  std::cout << " : " << getTypeName(type) << ']' << std::endl;
}

void BinOpExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "BinOp: [" << std::endl;
  printIndent(indent + 1);
  lhs->print(indent + 1);
  std::cout << ' ' << Lexer::getTokenName(op) << ' ';
  rhs->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

llvm::Constant *IntNumberExpr::codegen(Context &ctx)
{
  return llvm::ConstantInt::get(ctx.global.llvm_context,
                                llvm::APInt(64, value, true));
}

llvm::Constant *CharConstExpr::codegen(Context &ctx)
{
  return llvm::ConstantInt::get(ctx.global.llvm_context,
                                llvm::APInt(8, value, true));
}

llvm::Constant *FltNumberExpr::codegen(Context &ctx)
{
  // TODO do something about this (double) cast, or change back no short double
  return llvm::ConstantFP::get(ctx.global.llvm_context,
                               llvm::APFloat((double)value));
}

llvm::Constant *BoolConstExpr::codegen(Context &ctx)
{
  return llvm::ConstantInt::get(ctx.global.llvm_context, llvm::APInt(1, value));
}

llvm::Constant *StringConstExpr::codegen(Context &ctx)
{
  throw CodeGenError("strings not implemented yet");
}

Type FunctionCallExpr::getType(Context &)
{
  if (!fnHead) throw CodeGenError("Missing function for call", this);
  return fnHead->getReturnType();
}

llvm::Value *FunctionCallExpr::codegen(Context &ctx)
{
  fnHead = ctx.global.getFunction(name);
  if (!fnHead) throw CodeGenError("no viable function found for " + name, this);
  auto params = std::make_unique<llvm::Value *[]>(args.size());
  for (int i = 0; i < args.size(); ++i)
  {
    params[i] = args[i]->codegen(ctx);
  }
  ctx.global.builder.CreateCall(
    fnHead->get_llvm_fn(),
    llvm::ArrayRef<llvm::Value *>(params.get(), args.size()));
  return nullptr;
}

Type VariableExpr::getType(Context &ctx) { return ctx.getVariableType(name); }

llvm::Value *VariableExpr::codegen(Context &ctx)
{
  return ctx.global.builder.CreateLoad(ctx.getVarAlloca(name), name);
}

llvm::AllocaInst *VariableExpr::getAlloca(Context &ctx)
{
  return ctx.getVarAlloca(name);
}

llvm::Value *CastExpr::codegen(Context &ctx)
{
  auto from = expr->getType(ctx);
  if (!canCast(from, newType))
    throw CodeGenError("invalid cast from type '" + getTypeName(from) +
                         "' to '" + getTypeName(newType) + '\'',
                       this);
  auto val = expr->codegen(ctx);
  auto valName = val->getName();
  auto castName = valName + "_cst_" + getTypeName(newType);
  return generateCast(ctx, val, from, newType, castName);
}

Type BinOpExpr::getType(Context &ctx)
{
  auto t1 = lhs->getType(ctx);
  auto t2 = rhs->getType(ctx);
  auto ct = commonType(t1, t2);
  if (ct == Type::none)
    throw CodeGenError("no common type for types " + getTypeName(t1) + " and " +
                         getTypeName(t2),
                       this);
  if (!canImplicitlyCast(t1, ct))
    throw CodeGenError("cannot implicitly convert right argument of binop '" +
                         Lexer::getTokenName(op) + "' of type '" +
                         getTypeName(t1) + "' to type '" + getTypeName(ct) +
                         '\'',
                       this);
  if (!canImplicitlyCast(t2, ct))
    throw CodeGenError("cannot implicitly convert right argument of binop '" +
                         Lexer::getTokenName(op) + "' of type '" +
                         getTypeName(t2) + "' to type '" + getTypeName(ct) +
                         '\'',
                       this);
  return ct;
}

llvm::Value *BinOpExpr::codegen(Context &ctx)
{
  if (isBinOp(op)) {
    auto commonType = getType(ctx);
    lhs = make_EPtr<CastExpr>(std::move(lhs), commonType);
    rhs = make_EPtr<CastExpr>(std::move(rhs), commonType);
    return createBinOp(ctx, op, commonType, lhs->codegen(ctx),
                       rhs->codegen(ctx));
  }
  if (isCompAssign(op)) {
    auto varexpr = dynamic_cast<VariableExpr *>(lhs.get());
    if (!varexpr)
      throw CodeGenError("left hand side of assignment must be a variable",
                         this);

    int assign_op = getCompAssigOpBaseOp(op);
    auto rightType = rhs->getType(ctx);
    auto targetType = lhs->getType(ctx);
    if (!canImplicitlyCast(rhs->getType(ctx), targetType))
      throw CodeGenError("cannot implicitly convert right argument of binop '" +
                           Lexer::getTokenName(op) + "' of type '" +
                           getTypeName(rightType) + "' to type '" +
                           getTypeName(targetType) + '\'',
                         this);
    auto assignVal = createBinOp(ctx, assign_op, targetType, lhs->codegen(ctx),
                                 rhs->codegen(ctx));
    return createAssignment(ctx, assignVal, varexpr);
  }
  if (op == '=') // normal assignment
  {
    auto varexpr = dynamic_cast<VariableExpr *>(lhs.get());
    if (!varexpr)
      throw CodeGenError("left hand side of assignment must be a variable",
                         this);
    return createAssignment(ctx, rhs->codegen(ctx), varexpr);
  }
  throw CodeGenError("invalid operation " + Lexer::getTokenName(op), this);
}