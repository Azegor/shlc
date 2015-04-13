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

#include "codegen.h"

#include <llvm/IR/IRBuilder.h>

#include "ast.h"
#include "context.h"
#include "ast_expressions.h"

llvm::Type *getLLVMTypeFromType(GlobalContext &ctx, Type type)
{
  switch (type)
  {
    // int_t, flt_t, chr_t, boo_t, str_t, vac_t
    case Type::vac_t:
      return llvm::Type::getVoidTy(ctx.llvm_context);
    case Type::int_t:
      return llvm::Type::getInt64Ty(ctx.llvm_context);
    case Type::flt_t:
      return llvm::Type::getDoubleTy(ctx.llvm_context);
    case Type::boo_t:
      return llvm::Type::getInt1Ty(ctx.llvm_context);
    case Type::chr_t:
      return llvm::Type::getInt8Ty(ctx.llvm_context); // no unicode
    case Type::str_t:
      return llvm::Type::getInt8PtrTy(ctx.llvm_context);
    default:
      throw CodeGenError("Unknown type id " + getTypeName(type));
  }
}

llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *fn,
                                         const std::string &varName,
                                         llvm::Type *varType)
{
  llvm::IRBuilder<> TmpB(&fn->getEntryBlock(), fn->getEntryBlock().end());
  return TmpB.CreateAlloca(varType, 0, (varName + "_alloca").c_str());
}

#define SWITCH_CANNOT_CAST                                                     \
  case Type::vac_t:                                                            \
  case Type::none:                                                             \
  case Type::inferred:                                                         \
  default:                                                                     \
    return CastMode::None

CastMode castMode(Type from, Type to)
{
  switch (from)
  {
    case Type::int_t:
      switch (to)
      {
        case Type::int_t:
          return CastMode::Same;
        case Type::flt_t:
          return CastMode::Implicit;
        case Type::chr_t:
          return CastMode::Explicit;
        case Type::boo_t:
          return CastMode::Explicit;
        case Type::str_t:
          return CastMode::None;
          SWITCH_CANNOT_CAST;
      }
    case Type::flt_t:
      switch (to)
      {
        case Type::int_t:
          return CastMode::Explicit;
        case Type::flt_t:
          return CastMode::Same;
        case Type::chr_t:
          return CastMode::Explicit;
        case Type::boo_t:
          return CastMode::Explicit;
        case Type::str_t:
          return CastMode::None;
          SWITCH_CANNOT_CAST;
      }
    case Type::chr_t:
      switch (to)
      {
        case Type::int_t:
          return CastMode::Implicit;
        case Type::flt_t:
          return CastMode::Implicit;
        case Type::chr_t:
          return CastMode::Same;
        case Type::boo_t:
          return CastMode::Explicit;
        case Type::str_t:
          return CastMode::None;
          SWITCH_CANNOT_CAST;
      }
    case Type::boo_t:
      switch (to)
      {
        case Type::int_t:
          return CastMode::Implicit;
        case Type::flt_t:
          return CastMode::Implicit;
        case Type::chr_t:
          return CastMode::Explicit;
        case Type::boo_t:
          return CastMode::Same;
        case Type::str_t:
          return CastMode::None;
          SWITCH_CANNOT_CAST;
      }
    case Type::str_t:
      switch (to)
      {
        case Type::int_t:
          return CastMode::None;
        case Type::flt_t:
          return CastMode::None;
        case Type::chr_t:
          return CastMode::None;
        case Type::boo_t:
          return CastMode::None;
        case Type::str_t:
          return CastMode::Same;
          SWITCH_CANNOT_CAST;
      }
      SWITCH_CANNOT_CAST;
  }
}

#undef SWITCH_CANNOT_CAST

#define NO_CAST                                                                \
  case Type::vac_t:                                                            \
  case Type::none:                                                             \
  case Type::inferred:                                                         \
  default:                                                                     \
    return nullptr

llvm::Value *generateCast(Context &ctx, llvm::Value *val, Type from, Type to,
                          const llvm::Twine &valName)
{
  auto &builder = ctx.global.builder;
  auto targetType = getLLVMTypeFromType(ctx.global, to);
  switch (from)
  {
    case Type::int_t:
      switch (to)
      {
        case Type::int_t:
          return val;
        case Type::flt_t:
          return builder.CreateSIToFP(val, targetType, valName);
        case Type::chr_t:
          return builder.CreateIntCast(val, targetType, true, valName);
        case Type::boo_t:
          return builder.CreateIsNotNull(val, valName);
        case Type::str_t:
          NO_CAST;
      }
    case Type::flt_t:
      switch (to)
      {
        case Type::int_t:
        case Type::chr_t:
          return builder.CreateFPToSI(val, targetType, valName);
        case Type::flt_t:
          return val;
        case Type::boo_t:
          return builder.CreateFCmpONE(val, FltNumberExpr(0.0).codegen(ctx),
                                       valName);
        case Type::str_t:
          NO_CAST;
      }
    case Type::chr_t:
      switch (to)
      {
        case Type::int_t:
          return builder.CreateIntCast(val, targetType, true, valName);
        case Type::flt_t:
          return builder.CreateSIToFP(val, targetType, valName);
        case Type::chr_t:
          return val;
        case Type::boo_t:
          return builder.CreateIsNotNull(val, valName);
        case Type::str_t:
          NO_CAST;
      }
    case Type::boo_t:
      switch (to)
      {
        case Type::int_t:
        case Type::chr_t:
          return builder.CreateIntCast(val, targetType, true, valName);
        case Type::flt_t:
          return builder.CreateSIToFP(val, targetType, valName);
        case Type::boo_t:
          return val;
        case Type::str_t:
          NO_CAST;
      }
    case Type::str_t:
      switch (to)
      {
        case Type::str_t:
          return val;
        case Type::int_t:
        case Type::flt_t:
        case Type::chr_t:
        case Type::boo_t:
          NO_CAST;
      }
      NO_CAST;
  }
}

#undef NO_CAST

#define NO_COMMON                                                              \
  case Type::str_t:                                                            \
  case Type::vac_t:                                                            \
  case Type::none:                                                             \
  case Type::inferred:                                                         \
  default:                                                                     \
    return Type::none

Type commonType(Type t1, Type t2)
{
  if (t1 == t2) return t1;
  switch (t1)
  {
    case Type::int_t:
      switch (t2)
      {
        case Type::flt_t:
          return Type::flt_t;
        case Type::chr_t:
          return Type::int_t;
        case Type::boo_t:
          return Type::int_t;
          NO_COMMON;
      }
    case Type::flt_t:
      switch (t2)
      {
        case Type::int_t:
          return Type::flt_t;
        case Type::chr_t:
          return Type::flt_t;
        case Type::boo_t:
          return Type::flt_t;
          NO_COMMON;
      }
    case Type::chr_t:
      switch (t2)
      {
        case Type::int_t:
          return Type::int_t;
        case Type::flt_t:
          return Type::flt_t;
        case Type::boo_t:
          return Type::chr_t;
          NO_COMMON;
      }
    case Type::boo_t:
      switch (t2)
      {
        case Type::int_t:
          return Type::int_t;
        case Type::flt_t:
          return Type::flt_t;
        case Type::chr_t:
          return Type::chr_t;
          NO_COMMON;
      }
      NO_COMMON;
  }
}

#undef NO_COMMON

llvm::Constant *getIntConst(Context &ctx, Type intType, int val)
{
  auto bits = 0;
  auto is_signed = false;
  switch (intType)
  {
    case Type::int_t:
      bits = 64;
      is_signed = true;
      break;
    case Type::chr_t:
      bits = 8;
      is_signed = true;
      break;
    case Type::boo_t:
      bits = 1;
      is_signed = false;
      break;
    default:
      throw CodeGenError("invalid int type " + getTypeName(intType));
  }
  return llvm::ConstantInt::get(ctx.global.llvm_context,
                                llvm::APInt(bits, val, is_signed));
}

llvm::Constant *createDefaultValueConst(Context &ctx, Type type)
{
  switch (type)
  {
    case Type::int_t:
      return IntNumberExpr(0).codegen(ctx);
    case Type::flt_t:
      return FltNumberExpr(0.0).codegen(ctx);
    case Type::chr_t:
      return CharConstExpr('\0').codegen(ctx);
    case Type::boo_t:
      return BoolConstExpr(false).codegen(ctx);
    case Type::str_t:
    default:
      throw CodeGenError("no default value for type " + getTypeName(type));
  }
}

int getCompAssigOpBaseOp(int op)
{
  using Tok = Token::TokenType;
  switch (op)
  {
    case Tok::add_assign:
      return '+';
    case Tok::sub_assign:
      return '-';
    case Tok::mul_assign:
      return '*';
    case Tok::div_assign:
      return '/';
    case Tok::mod_assign:
      return '%';
    case Tok::pow_assign:
      return Tok::power;
    // bit operation
    case Tok::lshift_assign:
      return Tok::lshift;
    case Tok::rshift_assign:
      return Tok::rshift;
    case Tok::bit_and_assign:
      return '&';
    case Tok::bit_or_assign:
      return '|';
    //     case Tok::bit_complement_assign:
    //       return '~';
    case Tok::bit_xor_assign:
      return '^';
    default:
      throw CodeGenError("invalid compound assigment operator " +
                         Lexer::getTokenName(op));
  }
}

#define OP_NOT_SUPPORTED(op, ty)                                               \
  default:                                                                     \
    throw CodeGenError("operation '" + Lexer::getTokenName(op) +               \
                       "' not supported for type '" + getTypeName(ty) + "'");

llvm::Value *createUnOp(Context &ctx, int op, Type type, llvm::Value *rhs)
{
  //   using Tok = Token::TokenType;
  // valid unops:
  /* !
   * ~
   */

  auto &builder = ctx.global.builder;

  switch (op)
  {
    case '!':
      switch (type)
      {
        case Type::boo_t:
          return builder.CreateNot(rhs, "not");
          OP_NOT_SUPPORTED(op, type);
      }
    case '~':
      switch (type)
      {
        case Type::int_t:
        case Type::chr_t:
        case Type::boo_t:
          return builder.CreateXor(rhs, getIntConst(ctx, type, -1), "bitcmpl");
          OP_NOT_SUPPORTED(op, type);
      }
    default:
      CodeGenError("type '" + getTypeName(type) +
                   "' doesnt support unary operations");
  }
  return nullptr;
}

llvm::Value *createBinOp(Context &ctx, int op, Type commonType,
                         llvm::Value *lhs, llvm::Value *rhs)
{
  using Tok = Token::TokenType;
  // valid binops:
  /* +
   * -
   * *
   * /
   * %
   * increment,
   * decrement,
   * power,
   * // bool operators
   * lte,
   * gte,
   * eq,
   * neq,
   * log_and,
   * log_or,
   * // bit operations
   * lshift,
   * rshift,
   */
  if (!isBinOp(op)) // no binop
    throw CodeGenError("invalid binop " + Lexer::getTokenName(op));

  auto &builder = ctx.global.builder;

  switch (commonType)
  {
    case Type::int_t:
    case Type::chr_t:
    case Type::boo_t:
      switch (op)
      {
        // arithmetic operations
        case '+':
          return builder.CreateAdd(lhs, rhs, "sum");
        case '-':
          return builder.CreateSub(lhs, rhs, "diff");
        case '*':
          return builder.CreateMul(lhs, rhs, "prod");
        case '/':
          if (commonType == Type::boo_t)
            // return builder.CreateUDiv(lhs, rhs, "quot");
            throw CodeGenError("cannot divide two booleans");
          else
            return builder.CreateSDiv(lhs, rhs, "quot");
        case '%':
          if (commonType == Type::boo_t)
            // return builder.CreateURem(lhs, rhs, "rem");
            throw CodeGenError("cannot calculate rem of two booleans");
          else
            return builder.CreateSRem(lhs, rhs, "rem");
        case Tok::increment:
          return builder.CreateAdd(lhs, getIntConst(ctx, commonType, 1), "inc");
        case Tok::decrement:
          return builder.CreateSub(lhs, getIntConst(ctx, commonType, 1), "dec");
        case Tok::power:
          throw CodeGenError("power not implemented yet");
        //           return builder.CreateCall()

        // boolean operations
        case '<':
          if (commonType == Type::boo_t)
            return builder.CreateICmpULT(lhs, rhs, "cmp_lt");
          else
            return builder.CreateICmpSLT(lhs, rhs, "cmp_lt");
        case '>':
          if (commonType == Type::boo_t)
            return builder.CreateICmpUGT(lhs, rhs, "cmp_gt");
          else
            return builder.CreateICmpSGT(lhs, rhs, "cmp_gt");
        case Tok::lte:
          if (commonType == Type::boo_t)
            return builder.CreateICmpULE(lhs, rhs, "cmp_lte");
          else
            return builder.CreateICmpSLE(lhs, rhs, "cmp_lte");
        case Tok::gte:
          if (commonType == Type::boo_t)
            return builder.CreateICmpUGE(lhs, rhs, "cmp_gte");
          else
            return builder.CreateICmpSGE(lhs, rhs, "cmp_gte");
        case Tok::eq:
          return builder.CreateICmpEQ(lhs, rhs, "cmp_eq");
        case Tok::neq:
          return builder.CreateICmpNE(lhs, rhs, "cmp_neq");

        // TODO for bool
        case Tok::log_and:
        case Tok::log_or:

        // bit operations
        case '|':
          return builder.CreateOr(lhs, rhs, "bitor");
        case '&':
          return builder.CreateAnd(lhs, rhs, "bitand");
        case '^':
          return builder.CreateXor(lhs, rhs, "bitxor");
        case Tok::lshift:
          if (commonType == Type::boo_t)
            throw CodeGenError("cannot bitshift boolean type");
          else
            return builder.CreateShl(lhs, rhs, "shl");
        case Tok::rshift:
          if (commonType == Type::boo_t)
            throw CodeGenError("cannot bitshift boolean type");
          else
            return builder.CreateAShr(lhs, rhs, "shr");

          // unsupported
          OP_NOT_SUPPORTED(op, commonType);
      }
    case Type::flt_t:
      switch (op)
      {
        // arithmetic operations
        case '+':
          return builder.CreateFAdd(lhs, rhs, "sum");
        case '-':
          return builder.CreateFSub(lhs, rhs, "diff");
        case '*':
          return builder.CreateFMul(lhs, rhs, "prod");
        case '/':
          return builder.CreateFDiv(lhs, rhs, "quot");
        case '%':
          return builder.CreateFRem(lhs, rhs, "rem");
        case Tok::power:
          throw CodeGenError("power not implemented yet");
        //           return builder.CreateCall()

        // boolean operations
        case '<':
          return builder.CreateFCmpOLT(lhs, rhs, "cmp_lt");
        case '>':
          return builder.CreateFCmpOGT(lhs, rhs, "cmp_gt");
        case Tok::lte:
          return builder.CreateFCmpOLE(lhs, rhs, "cmp_lte");
        case Tok::gte:
          return builder.CreateFCmpOGE(lhs, rhs, "cmp_gte");
        case Tok::eq:
          return builder.CreateFCmpOEQ(lhs, rhs, "cmp_eq");
        case Tok::neq:
          return builder.CreateFCmpOEQ(lhs, rhs, "cmp_neq");

          // unsupported
          OP_NOT_SUPPORTED(op, commonType);
      }
    case Type::str_t:
      switch (op)
      {
        //         case '+':
        //         case '-':
        //         case '*':
        //         case '/':
        //         case '%':
        //         case Tok::increment:
        //         case Tok::decrement:
        //         case Tok::power:
        //         // bool operators
        //         case '<':
        //         case '>':
        //         case Tok::lte:
        //         case Tok::gte:
        //         case Tok::eq:
        //         case Tok::neq:
        //         case Tok::log_and:
        //         case Tok::log_or:
        //         // bit operations
        //         case Tok::lshift:
        //         case Tok::rshift:
        OP_NOT_SUPPORTED(op, commonType);
      }
    default:
      CodeGenError("type '" + getTypeName(commonType) +
                   "' doesnt support binary operations");
  }
  return nullptr;
}

#undef OP_NOT_SUPPORTED

llvm::Value *createAssignment(Context &ctx, llvm::Value *val, VariableExpr *var)
{
  auto alloca = var->getAlloca(ctx);
  return ctx.global.builder.CreateStore(val, alloca);
}