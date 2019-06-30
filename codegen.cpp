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

llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *fn,
                                         const std::string &varName,
                                         llvm::Type *varType)
{
  llvm::IRBuilder<> TmpB(&fn->getEntryBlock(), fn->getEntryBlock().end());
  return TmpB.CreateAlloca(varType, 0, (varName + "_alloca").c_str());
}

#define SWITCH_CANNOT_CAST                                                     \
  case BuiltinTypeKind::vac_t:                                                            \
  case BuiltinTypeKind::none:                                                             \
  default:                                                                     \
    return CastMode::None

CastMode castMode(BuiltinTypeKind from, BuiltinTypeKind to)
{
  switch (from)
  {
    case BuiltinTypeKind::int_t:
      switch (to)
      {
        case BuiltinTypeKind::int_t:
          return CastMode::Same;
        case BuiltinTypeKind::flt_t:
          return CastMode::Implicit;
        case BuiltinTypeKind::chr_t:
          return CastMode::Explicit;
        case BuiltinTypeKind::boo_t:
          return CastMode::Explicit;
        case BuiltinTypeKind::str_t:
        case BuiltinTypeKind::cls_t:
          return CastMode::None;
          SWITCH_CANNOT_CAST;
      }
    case BuiltinTypeKind::flt_t:
      switch (to)
      {
        case BuiltinTypeKind::int_t:
          return CastMode::Explicit;
        case BuiltinTypeKind::flt_t:
          return CastMode::Same;
        case BuiltinTypeKind::chr_t:
          return CastMode::Explicit;
        case BuiltinTypeKind::boo_t:
          return CastMode::Explicit;
        case BuiltinTypeKind::str_t:
        case BuiltinTypeKind::cls_t:
          return CastMode::None;
          SWITCH_CANNOT_CAST;
      }
    case BuiltinTypeKind::chr_t:
      switch (to)
      {
        case BuiltinTypeKind::int_t:
          return CastMode::Implicit;
        case BuiltinTypeKind::flt_t:
          return CastMode::Implicit;
        case BuiltinTypeKind::chr_t:
          return CastMode::Same;
        case BuiltinTypeKind::boo_t:
          return CastMode::Explicit;
        case BuiltinTypeKind::str_t:
        case BuiltinTypeKind::cls_t:
          return CastMode::None;
          SWITCH_CANNOT_CAST;
      }
    case BuiltinTypeKind::boo_t:
      switch (to)
      {
        case BuiltinTypeKind::int_t:
          return CastMode::Implicit;
        case BuiltinTypeKind::flt_t:
          return CastMode::Implicit;
        case BuiltinTypeKind::chr_t:
          return CastMode::Implicit;
        case BuiltinTypeKind::boo_t:
          return CastMode::Same;
        case BuiltinTypeKind::str_t:
        case BuiltinTypeKind::cls_t:
          return CastMode::None;
          SWITCH_CANNOT_CAST;
      }
    case BuiltinTypeKind::str_t:
      switch (to)
      {
        case BuiltinTypeKind::int_t:
        case BuiltinTypeKind::flt_t:
        case BuiltinTypeKind::chr_t:
        case BuiltinTypeKind::boo_t:
        case BuiltinTypeKind::cls_t:
          return CastMode::None;
        case BuiltinTypeKind::str_t:
          return CastMode::Same;
          SWITCH_CANNOT_CAST;
      }
    case BuiltinTypeKind::cls_t:
      switch (to)
      {
        case BuiltinTypeKind::int_t:
        case BuiltinTypeKind::flt_t:
        case BuiltinTypeKind::chr_t:
        case BuiltinTypeKind::boo_t:
        case BuiltinTypeKind::str_t:
        case BuiltinTypeKind::cls_t:
          return CastMode::None;
          SWITCH_CANNOT_CAST;
      }
      SWITCH_CANNOT_CAST;
  }
}

#undef SWITCH_CANNOT_CAST

#define NO_CAST                                                                \
  case BuiltinTypeKind::vac_t:                                                 \
  case BuiltinTypeKind::none:                                                  \
  default:                                                                     \
    return nullptr

llvm::Value *generateCast(Context &ctx, llvm::Value *val, Type *from, Type *to,
                          const llvm::Twine &valName)
{
  auto &builder = ctx.global.builder;
  auto targetType = ctx.global.llvmTypeRegistry.getType(to);
  switch (from->getKind())
  {
    case BuiltinTypeKind::int_t:
      switch (to->getKind())
      {
        case BuiltinTypeKind::int_t:
          return val;
        case BuiltinTypeKind::flt_t:
          return builder.CreateSIToFP(val, targetType, valName);
        case BuiltinTypeKind::chr_t:
          return builder.CreateIntCast(val, targetType, true, valName);
        case BuiltinTypeKind::boo_t:
          return builder.CreateIsNotNull(val, valName);
        case BuiltinTypeKind::str_t:
        case BuiltinTypeKind::cls_t:
          NO_CAST;
      }
    case BuiltinTypeKind::flt_t:
      switch (to->getKind())
      {
        case BuiltinTypeKind::int_t:
        case BuiltinTypeKind::chr_t:
          return builder.CreateFPToSI(val, targetType, valName);
        case BuiltinTypeKind::flt_t:
          return val;
        case BuiltinTypeKind::boo_t:
          return builder.CreateFCmpONE(val, FltNumberExpr({}, 0.0).codegen(ctx),
                                       valName);
        case BuiltinTypeKind::str_t:
        case BuiltinTypeKind::cls_t:
          NO_CAST;
      }
    case BuiltinTypeKind::chr_t:
      switch (to->getKind())
      {
        case BuiltinTypeKind::int_t:
          return builder.CreateIntCast(val, targetType, true, valName);
        case BuiltinTypeKind::flt_t:
          return builder.CreateSIToFP(val, targetType, valName);
        case BuiltinTypeKind::chr_t:
          return val;
        case BuiltinTypeKind::boo_t:
          return builder.CreateIsNotNull(val, valName);
        case BuiltinTypeKind::str_t:
        case BuiltinTypeKind::cls_t:
          NO_CAST;
      }
    case BuiltinTypeKind::boo_t:
      switch (to->getKind())
      {
        case BuiltinTypeKind::int_t:
        case BuiltinTypeKind::chr_t:
          return builder.CreateIntCast(val, targetType, true, valName);
        case BuiltinTypeKind::flt_t:
          return builder.CreateSIToFP(val, targetType, valName);
        case BuiltinTypeKind::boo_t:
          return val;
        case BuiltinTypeKind::str_t:
        case BuiltinTypeKind::cls_t:
          NO_CAST;
      }
    case BuiltinTypeKind::str_t:
      switch (to->getKind())
      {
        case BuiltinTypeKind::str_t:
          return val;
        case BuiltinTypeKind::int_t:
        case BuiltinTypeKind::flt_t:
        case BuiltinTypeKind::chr_t:
        case BuiltinTypeKind::boo_t:
        case BuiltinTypeKind::cls_t:
          NO_CAST;
      }
    case BuiltinTypeKind::cls_t:
      switch (to->getKind())
      {
        case BuiltinTypeKind::cls_t:
          return val;
        case BuiltinTypeKind::int_t:
        case BuiltinTypeKind::flt_t:
        case BuiltinTypeKind::chr_t:
        case BuiltinTypeKind::boo_t:
        case BuiltinTypeKind::str_t:
          NO_CAST;
      }
      NO_CAST;
  }
}

#undef NO_CAST

#define NO_COMMON                                                              \
  case BuiltinTypeKind::str_t:                                                            \
  case BuiltinTypeKind::vac_t:                                                            \
  case BuiltinTypeKind::none:                                                             \
  default:                                                                     \
    return BuiltinTypeKind::none

BuiltinTypeKind commonType(BuiltinTypeKind t1, BuiltinTypeKind t2)
{
  if (t1 == t2) return t1;
  switch (t1)
  {
    case BuiltinTypeKind::int_t:
      switch (t2)
      {
        case BuiltinTypeKind::flt_t:
          return BuiltinTypeKind::flt_t;
        case BuiltinTypeKind::chr_t:
          return BuiltinTypeKind::int_t;
        case BuiltinTypeKind::boo_t:
          return BuiltinTypeKind::int_t;
          NO_COMMON;
      }
    case BuiltinTypeKind::flt_t:
      switch (t2)
      {
        case BuiltinTypeKind::int_t:
          return BuiltinTypeKind::flt_t;
        case BuiltinTypeKind::chr_t:
          return BuiltinTypeKind::flt_t;
        case BuiltinTypeKind::boo_t:
          return BuiltinTypeKind::flt_t;
          NO_COMMON;
      }
    case BuiltinTypeKind::chr_t:
      switch (t2)
      {
        case BuiltinTypeKind::int_t:
          return BuiltinTypeKind::int_t;
        case BuiltinTypeKind::flt_t:
          return BuiltinTypeKind::flt_t;
        case BuiltinTypeKind::boo_t:
          return BuiltinTypeKind::chr_t;
          NO_COMMON;
      }
    case BuiltinTypeKind::boo_t:
      switch (t2)
      {
        case BuiltinTypeKind::int_t:
          return BuiltinTypeKind::int_t;
        case BuiltinTypeKind::flt_t:
          return BuiltinTypeKind::flt_t;
        case BuiltinTypeKind::chr_t:
          return BuiltinTypeKind::chr_t;
          NO_COMMON;
      }
      NO_COMMON;
  }
}

#undef NO_COMMON

llvm::Constant *getIntConst(Context &ctx, BuiltinTypeKind intType, int val)
{
  auto bits = 0;
  auto is_signed = false;
  switch (intType)
  {
    case BuiltinTypeKind::int_t:
      bits = 64;
      is_signed = true;
      break;
    case BuiltinTypeKind::chr_t:
      bits = 8;
      is_signed = true;
      break;
    case BuiltinTypeKind::boo_t:
      bits = 1;
      is_signed = false;
      break;
    default:
      throw CodeGenError("invalid int type " + getTypeName(intType));
  }
  return llvm::ConstantInt::get(ctx.global.llvm_context,
                                llvm::APInt(bits, val, is_signed));
}

ExprPtr getIntConstExpr(BuiltinTypeKind intType, int val)
{
  switch (intType)
  {
    case BuiltinTypeKind::int_t:
      return make_EPtr<IntNumberExpr>({}, val);
    case BuiltinTypeKind::chr_t:
      return make_EPtr<CharConstExpr>({}, val);
    case BuiltinTypeKind::boo_t:
      return make_EPtr<BoolConstExpr>({}, val != 0);
    default:
      throw CodeGenError("invalid int type " + getTypeName(intType));
  }
}

llvm::Constant *createDefaultValueConst(Context &ctx, llvm::Type *type)
{
  switch (type->getTypeID())
  {
      case llvm::Type::IntegerTyID:
        switch(type->getIntegerBitWidth()) {
        case 1:
          return BoolConstExpr({}, false).codegen(ctx);
        case 8:
          return CharConstExpr({}, '\0').codegen(ctx);
        case 64:
          return IntNumberExpr({}, 0).codegen(ctx);
        default:
          throw CodeGenError("unknown integer size" + std::to_string(type->getIntegerBitWidth()));
        }
    case llvm::Type::DoubleTyID:
      return FltNumberExpr({}, 0.0).codegen(ctx);
    case llvm::Type::PointerTyID:
      return llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(type));
    default:
      throw CodeGenError("no default value for type!");
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

llvm::Value *createUnOp(Context &ctx, int op, BuiltinTypeKind type, llvm::Value *rhs)
{
  //   using Tok = Token::TokenType;
  // valid unops:
  /* !
   * ~
   */

  using Tok = Token::TokenType;

  auto &builder = ctx.global.builder;

  switch (op)
  {
    case '!':
      switch (type)
      {
        case BuiltinTypeKind::boo_t:
          return builder.CreateNot(rhs, "not");
          OP_NOT_SUPPORTED(op, type);
      }
    case '~':
      switch (type)
      {
        case BuiltinTypeKind::int_t:
        case BuiltinTypeKind::chr_t:
        case BuiltinTypeKind::boo_t:
          return builder.CreateXor(rhs, getIntConst(ctx, type, -1), "bitcmpl");
          OP_NOT_SUPPORTED(op, type);
      }
    case '-':
      switch (type)
      {
        case BuiltinTypeKind::int_t:
        case BuiltinTypeKind::chr_t:
          return builder.CreateNeg(rhs, "ineg");
        case BuiltinTypeKind::flt_t:
          return builder.CreateFNeg(rhs, "fneg");
          OP_NOT_SUPPORTED(op, type);
      }
    //     case Tok::increment:
    //       switch (type)
    //       {
    //         case Type::int_t:
    //         case Type::chr_t:
    //         case Type::boo_t:
    //           auto varexpr = dynamic_cast<VariableExpr *>(rhs.get());
    //           if (!varexpr)
    //             throw CodeGenError("increment must operate on a variable",
    //             this);
    //           auto inc = builder.CreateAdd(rhs, getIntConst(ctx, type, 1),
    //           "inc");
    //           return createAssignment(ctx, inc, varexpr);
    //           OP_NOT_SUPPORTED(op, type);
    //       }
    //     case Tok::decrement:
    //       switch (type)
    //       {
    //         case Type::int_t:
    //         case Type::chr_t:
    //         case Type::boo_t:
    //           auto varexpr = dynamic_cast<VariableExpr *>(rhs.get());
    //           if (!varexpr)
    //             throw CodeGenError("decrement must operate on a variable",
    //             this);
    //           auto inc = builder.CreateSub(rhs, getIntConst(ctx, type, 1),
    //           "inc");
    //           return createAssignment(ctx, inc, varexpr);
    //           OP_NOT_SUPPORTED(op, type);
    //       }
    default:
      throw CodeGenError("unary operation '" + Lexer::getTokenName(op) +
                         "' not implemented yet");
  }
  return nullptr;
}

llvm::Value *createBinOp(Context &ctx, int op, BuiltinTypeKind commonType,
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
    case BuiltinTypeKind::int_t:
    case BuiltinTypeKind::chr_t:
    case BuiltinTypeKind::boo_t:
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
          if (commonType == BuiltinTypeKind::boo_t)
            // return builder.CreateUDiv(lhs, rhs, "quot");
            throw CodeGenError("cannot divide two booleans");
          else
            return builder.CreateSDiv(lhs, rhs, "quot");
        case '%':
          if (commonType == BuiltinTypeKind::boo_t)
            // return builder.CreateURem(lhs, rhs, "rem");
            throw CodeGenError("cannot calculate rem of two booleans");
          else
            return builder.CreateSRem(lhs, rhs, "rem");
        case Tok::power:
          throw CodeGenError("power not implemented yet");
        //           return builder.CreateCall()

        // boolean operations
        case '<':
          if (commonType == BuiltinTypeKind::boo_t)
            return builder.CreateICmpULT(lhs, rhs, "cmp_lt");
          else
            return builder.CreateICmpSLT(lhs, rhs, "cmp_lt");
        case '>':
          if (commonType == BuiltinTypeKind::boo_t)
            return builder.CreateICmpUGT(lhs, rhs, "cmp_gt");
          else
            return builder.CreateICmpSGT(lhs, rhs, "cmp_gt");
        case Tok::lte:
          if (commonType == BuiltinTypeKind::boo_t)
            return builder.CreateICmpULE(lhs, rhs, "cmp_lte");
          else
            return builder.CreateICmpSLE(lhs, rhs, "cmp_lte");
        case Tok::gte:
          if (commonType == BuiltinTypeKind::boo_t)
            return builder.CreateICmpUGE(lhs, rhs, "cmp_gte");
          else
            return builder.CreateICmpSGE(lhs, rhs, "cmp_gte");
        case Tok::eq:
          return builder.CreateICmpEQ(lhs, rhs, "cmp_eq");
        case Tok::neq:
          return builder.CreateICmpNE(lhs, rhs, "cmp_neq");

        case Tok::log_and:
          if (commonType != BuiltinTypeKind::boo_t) {
            lhs = builder.CreateIsNotNull(lhs, "boo_cst");
            rhs = builder.CreateIsNotNull(rhs, "boo_cst");
            //             throw CodeGenError("cannot implicitly convert " +
            //                                getTypeName(commonType));
          }
          return builder.CreateAnd(lhs, rhs, "logand");
        case Tok::log_or:
          if (commonType != BuiltinTypeKind::boo_t) {
            lhs = builder.CreateIsNotNull(lhs, "boo_cst");
            rhs = builder.CreateIsNotNull(rhs, "boo_cst");
            //             throw CodeGenError("cannot implicitly convert " +
            //                                getTypeName(commonType));
          }
          return builder.CreateOr(lhs, rhs, "logor");

        // bit operations
        case '|':
          return builder.CreateOr(lhs, rhs, "bitor");
        case '&':
          return builder.CreateAnd(lhs, rhs, "bitand");
        case '^':
          return builder.CreateXor(lhs, rhs, "bitxor");
        case Tok::lshift:
          if (commonType == BuiltinTypeKind::boo_t)
            throw CodeGenError("cannot bitshift boolean type");
          else
            return builder.CreateShl(lhs, rhs, "shl");
        case Tok::rshift:
          if (commonType == BuiltinTypeKind::boo_t)
            throw CodeGenError("cannot bitshift boolean type");
          else
            return builder.CreateAShr(lhs, rhs, "shr");

          // unsupported
          OP_NOT_SUPPORTED(op, commonType);
      }
    case BuiltinTypeKind::flt_t:
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
    case BuiltinTypeKind::str_t:
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

BuiltinTypeKind getBinOpReturnType(int op, BuiltinTypeKind inType)
{
  static constexpr const char *bool_binops = "<>";
  if ((op >= Token::TokenType::lte && op <= Token::TokenType::log_or) ||
      std::strchr(bool_binops, op) != nullptr)
    return BuiltinTypeKind::boo_t;
  return inType;
}

llvm::Value *createAssignment(Context &ctx, llvm::Value *val, VariableExpr *var)
{
  auto alloca = var->getAlloca(ctx);
  ctx.global.builder.CreateStore(val, alloca);
  return val;
}


llvm::Value *createAssignment(Context &ctx, llvm::Value *val, FieldAccessExpr *fa)
{
  auto fieldAddr = fa->codegenFieldAddress(ctx);
  ctx.global.builder.CreateStore(val, fieldAddr);
  return val;
}

void handleAssignmentRefCounts(Context &ctx, llvm::Value *lhsAddress, llvm::Value *rhs)
{
  auto &gctx = ctx.global;
  // first rhs
  auto xincRefFn = gctx.getXIncRefFn();
  auto rhsCast = gctx.builder.CreateBitCast(rhs, gctx.llvmTypeRegistry.getVoidPointerType(), "obj_vcst");
  gctx.builder.CreateCall(xincRefFn, {rhsCast});
  // then lhs (to avoid leakage if both are the same object)
  if (lhsAddress) { // null in case of newly declared variable -> don't decrement
    auto xdecRefFn = gctx.getXDecRefFn();
    auto lhsCast = gctx.builder.CreateBitCast(gctx.builder.CreateLoad(lhsAddress, "lhs"),
                      gctx.llvmTypeRegistry.getVoidPointerType(), "obj_vcst");
    gctx.builder.CreateCall(xdecRefFn, {lhsCast});
  }
}

void createMainFunctionReturn(Context &ctx) {
  ctx.global.builder.CreateStore(llvm::ConstantInt::get(ctx.global.llvm_context, llvm::APInt(64, 0, false)), ctx.ret.val);
  ctx.global.createBrCheckCleanup(ctx.ret.BB);
}
