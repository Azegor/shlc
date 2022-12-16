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
#include <llvm/IR/GlobalValue.h>
#include <llvm/ADT/APInt.h>
#include <llvm/Support/Alignment.h>

#include "ast.h"
#include "context.h"
#include "ast_functions.h"
#include "ast_types.h"
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
  std::cout << "[Number, type: " << type->getName() << "]";
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
  std::cout << " : " << type->getName() << ']' << std::endl;
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
  return llvm::ConstantFP::get(ctx.global.llvm_context, llvm::APFloat(value));
}

llvm::Constant *BoolConstExpr::codegen(Context &ctx)
{
  return llvm::ConstantInt::get(ctx.global.llvm_context, llvm::APInt(1, value));
}

llvm::Value *StringConstExpr::codegen(Context &ctx)
{
  auto pos = ctx.global.stringConstants.find(value);
  std::string globalVarName;
  if (pos == ctx.global.stringConstants.end()) {
    llvm::StringRef str(value.c_str(), value.size());

    llvm::Constant *str_const =
      llvm::ConstantDataArray::getString(ctx.global.llvm_context, str);

    auto *GV = new llvm::GlobalVariable(
      *ctx.global.module, str_const->getType(), true,
      llvm::GlobalValue::PrivateLinkage, str_const, "str_const");
//     GV->setUnnamedAddr(true);
    GV->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);

    globalVarName = GV->getName();
    ctx.global.putGlobalVar(globalVarName, TypeRegistry::getBuiltinType(BuiltinTypeKind::str_t), GV);
    ctx.global.stringConstants.insert({value, globalVarName});
  }
  else
  {
    globalVarName = pos->second;
  }
  return GlobalVarExpr(srcLoc, globalVarName).codegen(ctx);
}

void FunctionCallExpr::findFunction(Context &ctx)
{
  auto callArgs = getArgTypes(ctx);
  fnHead = ctx.global.getFunctionOverload(name, callArgs);
  if (!fnHead) throw CodeGenError("no viable function found for " + name, this);
}

Type *FunctionCallExpr::getType(Context &ctx)
{
  if (!fnHead) findFunction(ctx);
  return fnHead->getReturnType();
}

std::vector<Type*> FunctionCallExpr::getArgTypes(Context &ctx) const
{
  std::vector<Type*> res;
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
  std::vector<Type*> argTypes;
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
      args[i] = make_EPtr<CastExpr>(args[i]->srcLoc, std::move(args[i]),
                                    functionArgs[i]);
    params[i] = args[i]->codegen(ctx);
  }
  if (fnHead->getReturnType() == TypeRegistry::getVoidType()) {
    return ctx.global.builder.CreateCall(
      fnHead->get_llvm_fn(),
      llvm::ArrayRef<llvm::Value *>(params.get(), args.size()));
  }
  else
  {
    return ctx.global.builder.CreateCall(
      fnHead->get_llvm_fn(),
      llvm::ArrayRef<llvm::Value *>(params.get(), args.size()), "callres");
  }
}

Type *VariableExpr::getType(Context &ctx) { return ctx.getVariableType(name); }

llvm::Value *VariableExpr::codegen(Context &ctx)
{
  ContextFrame::VarInfo varInfo = ctx.getVar(name);
  llvm::Type* varType = ctx.global.llvmTypeRegistry.getType(varInfo.type);
  return ctx.global.builder.CreateLoad(varType, ctx.getVarAlloca(name), name);
}

Type *GlobalVarExpr::getType(Context &ctx)
{
  return ctx.global.getGlobalVarType(name);
}

llvm::Value *GlobalVarExpr::codegen(Context &ctx)
{
  auto var = ctx.global.getGlobalVar(name);
  llvm::Type* varType = ctx.global.llvmTypeRegistry.getType(var.type);
  if (var.type == TypeRegistry::getBuiltinType(BuiltinTypeKind::str_t))
    return ctx.global.builder.CreateConstGEP2_32(varType, var.var, 0, 0); // FIXME
  else
    return ctx.global.builder.CreateConstGEP1_32(varType, var.var, 0);
}

llvm::AllocaInst *VariableExpr::getAlloca(Context &ctx)
{
  return ctx.getVarAlloca(name);
}

llvm::Value *CastExpr::codegen(Context &ctx)
{
  auto from = expr->getType(ctx);
  if (!canCast(from, newType))
    throw CodeGenError("invalid cast from type '" + from->getName() +
                         "' to '" + newType->getName() + '\'',
                       this);
  auto val = expr->codegen(ctx);
  auto valName = val->getName();
  auto castName = valName + "_cst_" + newType->getName();
  return generateCast(ctx, val, from, newType, castName);
}

Type *BinOpExpr::getCommonType(Context &ctx)
{
  auto t1 = lhs->getType(ctx);
  auto t2 = rhs->getType(ctx);
  auto ct = TypeRegistry::getBuiltinType(commonType(t1->getKind(), t2->getKind()));
  if (!ct)
    throw CodeGenError("no common type for types " + t1->getName() + " and " +
                         t2->getName(),
                       this);
  if (!canImplicitlyCast(t1, ct))
    throw CodeGenError("cannot implicitly convert right argument of binop '" +
                         Lexer::getTokenName(op) + "' of type '" +
                         t1->getName() + "' to type '" + ct->getName() +
                         '\'',
                       this);
  if (!canImplicitlyCast(t2, ct))
    throw CodeGenError("cannot implicitly convert right argument of binop '" +
                         Lexer::getTokenName(op) + "' of type '" +
                         t2->getName() + "' to type '" + ct->getName() +
                         '\'',
                       this);
  return ct;
}

Type *BinOpExpr::getType(Context &ctx)
{
  auto ct = getCommonType(ctx);
  return TypeRegistry::getBuiltinType(getBinOpReturnType(op, ct->getKind()));
}

llvm::Value *BinOpExpr::codegen(Context &ctx)
{
  if (isBinOp(op)) {
    auto commonType = getCommonType(ctx);
    lhs = make_EPtr<CastExpr>(lhs->srcLoc, std::move(lhs), commonType);
    rhs = make_EPtr<CastExpr>(rhs->srcLoc, std::move(rhs), commonType);
    return createBinOp(ctx, op, commonType->getKind(), lhs->codegen(ctx),
                       rhs->codegen(ctx));
  }
  if (isCompAssign(op)) {
    int assign_op = getCompAssigOpBaseOp(op);
    auto rightType = rhs->getType(ctx);
    auto targetType = lhs->getType(ctx);
    if (!canImplicitlyCast(rightType, targetType))
      throw CodeGenError("cannot implicitly convert right argument of binop '" +
                           Lexer::getTokenName(op) + "' of type '" +
                           rightType->getName() + "' to type '" +
                           targetType->getName() + '\'',
                         this);
    rhs = make_EPtr<CastExpr>(rhs->srcLoc, std::move(rhs), targetType);
    auto lhsVal = lhs->codegen(ctx);
    auto rhsVal = rhs->codegen(ctx);
    auto assignVal = createBinOp(ctx, assign_op, targetType->getKind(), lhsVal, rhsVal);
    if (auto varexpr = dynamic_cast<VariableExpr *>(lhs.get())) {
      return createAssignment(ctx, assignVal, varexpr);
    } else if (auto fieldexpr = dynamic_cast<FieldAccessExpr *>(lhs.get())) {
      return createAssignment(ctx, assignVal, fieldexpr);
    } else {
      throw CodeGenError("left hand side of compound assignment must be a variable or field access", this);
    }
  }
  if (op == '=') // normal assignment
  {
    auto rightType = rhs->getType(ctx);
    auto targetType = lhs->getType(ctx);
    if (!canImplicitlyCast(rightType, targetType))
      throw CodeGenError(
        "cannot implicitly convert right argument of assignment '" +
          rightType->getName() + "' to type '" + targetType->getName() +
          '\'',
        this);
    rhs = make_EPtr<CastExpr>(rhs->srcLoc, std::move(rhs), targetType);
    auto rhsVal = rhs->codegen(ctx);
    llvm::Value *lhsAddress = nullptr;
    if (auto varexpr = dynamic_cast<VariableExpr *>(lhs.get())) {
      lhsAddress = varexpr->getAlloca(ctx);
    } else if (auto fieldexpr = dynamic_cast<FieldAccessExpr *>(lhs.get())) {
      lhsAddress = fieldexpr->codegenFieldAddress(ctx).first;
    } else {
      throw CodeGenError("left hand side of assignment must be a variable or field access", this);
    }
    if (targetType->getKind() == BuiltinTypeKind::cls_t) {
      handleAssignmentRefCounts(ctx, static_cast<ClassType*>(targetType), lhsAddress, rhsVal);
    }
    return ctx.global.builder.CreateStore(rhsVal, lhsAddress);
  }
  throw CodeGenError("invalid binary operation " + Lexer::getTokenName(op),
                     this);
}

Type *UnOpExpr::getType(Context &ctx)
{
  return rhs->getType(ctx); // for now, some might change type!?!
}

llvm::Value *UnOpExpr::codegen(Context &ctx)
{
  if (isUnOp(op)) {
    return createUnOp(ctx, op, getType(ctx)->getKind(), rhs->codegen(ctx));
  }
  else if (op == Token::TokenType::increment ||
           op == Token::TokenType::decrement)
  {
    auto varexpr = dynamic_cast<VariableExpr *>(rhs.get());
    if (!varexpr)
      throw CodeGenError("left hand side of assignment must be a variable",
                         this);
    // the following is a bit of a hack, can be improved
    return BinOpExpr(srcLoc, getIncDecOpBaseOp(op),
                     make_EPtr<VariableExpr>(*varexpr),
                     getIntConstExpr(varexpr->getType(ctx)->getKind(), 1)).codegen(ctx);
  }
  throw CodeGenError("invalid unary operation " + Lexer::getTokenName(op),
                     this);
}

void NewExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "New: [" << cls->getName() << ']' << std::endl;
}

llvm::Value *NewExpr::codegen(Context &ctx)
{
    auto &gctx = ctx.global;
    auto mallocFn = gctx.getMallocFn();
    auto classType = gctx.llvmTypeRegistry.getClassType(cls);
    auto classPtrType = gctx.llvmTypeRegistry.getClassPtrType(cls);
    size_t allocSize = gctx.module->getDataLayout().getTypeAllocSize(classType);
    auto allocSizeVal = llvm::ConstantInt::get(gctx.llvm_context,
                                llvm::APInt(64, allocSize, false));
    auto rawPointer = gctx.builder.CreateCall(mallocFn, {allocSizeVal}, "allocmemory");
    auto align = gctx.module->getDataLayout().getABITypeAlignment(classType); // TODO
    gctx.builder.CreateMemSet(rawPointer, llvm::ConstantInt::get(gctx.llvm_context, llvm::APInt(8, 0)), allocSize, llvm::MaybeAlign(align));
    auto classPtr = gctx.builder.CreateBitCast(rawPointer, classPtrType, "allocres");
    return classPtr;
}

void FieldAccessExpr::print(int indent)
{
    printIndent(indent);
    std::cout << "FieldAccess: [\n";
    printIndent(indent + 1);
    expr->print(indent + 1);
    std::cout << "->" << field << std::endl;
    printIndent(indent);
    std::cout << ']' << std::endl;
}

llvm::Value *FieldAccessExpr::codegen(Context &ctx)
{
    std::pair<llvm::Value*, llvm::Type*> fieldAccess = codegenFieldAddress(ctx);
    return ctx.global.builder.CreateLoad(fieldAccess.second, fieldAccess.first, field + "_field");
}

std::pair<llvm::Value *, llvm::Type*> FieldAccessExpr::codegenFieldAddress(Context &ctx)
{
    auto classField = getClassField(ctx);
    auto zeroIdx = llvm::ConstantInt::get(ctx.global.llvm_context, llvm::APInt(32, 0, true));
    auto fieldIdx = llvm::ConstantInt::get(ctx.global.llvm_context, llvm::APInt(32, classField->index, true));
    llvm::Type* fieldType = ctx.global.llvmTypeRegistry.getType(getType(ctx));
    llvm::Value* val = ctx.global.builder.CreateInBoundsGEP(fieldType, expr->codegen(ctx), {zeroIdx, fieldIdx}, field + "_fieldptr");
    return {val, fieldType};
}

const ClassField *FieldAccessExpr::getClassField(Context &ctx) const
{
    auto type = expr->getType(ctx);
    assert(type->getKind() == BuiltinTypeKind::cls_t);
    auto classType = dynamic_cast<ClassType*>(type);
    return classType->getField(field);
}
