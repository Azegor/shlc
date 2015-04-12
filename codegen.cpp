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
    //         case Type::str_t: return
    //         llvm::Type::getInt8PtrTy(ctx.llvm_context); // TODO: do the right
    //         thing here!!!
    case Type::chr_t:
      return llvm::Type::getInt8Ty(ctx.llvm_context); // no unicode
    default:
      throw CodeGenError(nullptr, "Unknown type id" + getTypeName(type));
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
          return builder.CreateIsNotNull(val);
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
          return builder.CreateIsNotNull(val);
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
    // TODO
    case Type::str_t:
      switch (to)
      {
        case Type::int_t:
          return nullptr;
        case Type::flt_t:
          return nullptr;
        case Type::chr_t:
          return nullptr;
        case Type::boo_t:
          return nullptr;
        case Type::str_t:
          return nullptr;
          NO_CAST;
      }
      NO_CAST;
  }
}