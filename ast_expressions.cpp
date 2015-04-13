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

void GlobalVarExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Global Variable: " << name << ']';
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

void UnOpExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "UnOp: [" << std::endl;
  printIndent(indent + 1);
  std::cout << Lexer::getTokenName(op) << ' ';
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

llvm::Value *StringConstExpr::codegen(Context &ctx)
{
  llvm::StringRef str(value.c_str(), value.size());

  llvm::Constant *str_const =
    llvm::ConstantDataArray::getString(ctx.global.llvm_context, str);

  auto *GV = new llvm::GlobalVariable(*ctx.global.module, str_const->getType(),
                                      true, llvm::GlobalValue::PrivateLinkage,
                                      str_const, "str_const");
  GV->setUnnamedAddr(true);

  auto name = GV->getName();
  ctx.putGlobalVar(name, Type::str_t, GV);
  return GlobalVarExpr(name).codegen(ctx);
}

void FunctionCallExpr::findFunction(Context &ctx)
{
  auto callArgs = getArgTypes(ctx);
  fnHead = ctx.global.getFunctionOverload(name, callArgs);
  if (!fnHead) throw CodeGenError("no viable function found for " + name, this);
}

Type FunctionCallExpr::getType(Context &ctx)
{
  if (!fnHead) findFunction(ctx);
  return fnHead->getReturnType();
}

std::vector<Type> FunctionCallExpr::getArgTypes(Context &ctx) const
{
  std::vector<Type> res;
  res.reserve(args.size());
  for (auto &arg : args)
  {
    res.push_back(arg->getType(ctx));
  }
  return res;
}

llvm::Value *FunctionCallExpr::codegen(Context &ctx)
{
  if (!fnHead) findFunction(ctx);
  auto callArgs = getArgTypes(ctx);
  std::vector<Type> argTypes;
  argTypes.reserve(args.size());
  for (auto &arg : args)
    argTypes.push_back(arg->getType(ctx));

  if (!fnHead->canCallWithArgs(argTypes))
    throw CodeGenError("cannot call function " + fnHead->sigString() +
                       " with given parameters");

  auto params = std::make_unique<llvm::Value *[]>(args.size());
  auto functionArgs = fnHead->getArgTypes();
  for (int i = 0; i < args.size(); ++i)
  {
    if (callArgs[i] != functionArgs[i])
      args[i] = make_EPtr<CastExpr>(std::move(args[i]), functionArgs[i]);
    params[i] = args[i]->codegen(ctx);
  }
  return ctx.global.builder.CreateCall(
    fnHead->get_llvm_fn(),
    llvm::ArrayRef<llvm::Value *>(params.get(), args.size()), "callres");
}

Type VariableExpr::getType(Context &ctx) { return ctx.getVariableType(name); }

llvm::Value *VariableExpr::codegen(Context &ctx)
{
  return ctx.global.builder.CreateLoad(ctx.getVarAlloca(name), name);
}

Type GlobalVarExpr::getType(Context &ctx) { return ctx.getGlobalVarType(name); }

llvm::Value *GlobalVarExpr::codegen(Context &ctx)
{
  return ctx.global.builder.CreateConstGEP2_32(ctx.getGlobalVarInst(name), 0,
                                               0);
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
  throw CodeGenError("invalid binary operation " + Lexer::getTokenName(op),
                     this);
}

Type UnOpExpr::getType(Context &ctx)
{
  return rhs->getType(ctx); // for now, some might change type!?!
}

llvm::Value *UnOpExpr::codegen(Context &ctx)
{
  if (isUnOp(op)) {
    return createUnOp(ctx, op, getType(ctx), rhs->codegen(ctx));
  }
  else if (op == Token::TokenType::increment ||
           op == Token::TokenType::decrement)
  {
    auto varexpr = dynamic_cast<VariableExpr *>(rhs.get());
    if (!varexpr)
      throw CodeGenError("left hand side of assignment must be a variable",
                         this);
    // the following is a bit of a hack, can be improved
    return BinOpExpr(getIncDecOpBaseOp(op), make_EPtr<VariableExpr>(*varexpr),
                     getIntConstExpr(varexpr->getType(ctx), 1)).codegen(ctx);
  }
  throw CodeGenError("invalid unary operation " + Lexer::getTokenName(op),
                     this);
}