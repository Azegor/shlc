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

#ifndef CODEGEN_H
#define CODEGEN_H

#include <cstring>

#include <llvm/IR/Type.h>
#include <llvm/IR/Instructions.h>

#include "ast.h"
#include "type.h"

class Context;
class GlobalContext;
class VariableExpr;

llvm::Type *getLLVMTypeFromType(GlobalContext &ctx, Type tokID);

llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *fn,
                                         const std::string &varName,
                                         llvm::Type *varType);

enum class CastMode
{
  None,
  Explicit,
  Implicit,
  Same
};

CastMode castMode(Type from, Type to);

inline bool canImplicitlyCast(Type from, Type to)
{
  auto mode = castMode(from, to);
  return mode == CastMode::Implicit || mode == CastMode::Same;
}

inline bool canCast(Type from, Type to)
{
  auto mode = castMode(from, to);
  return mode == CastMode::Explicit || mode == CastMode::Implicit ||
         mode == CastMode::Same;
}

llvm::Value *generateCast(Context &ctx, llvm::Value *val, Type from, Type to,
                          const llvm::Twine &valName = "");

Type commonType(Type t1, Type t2);

llvm::Constant *getIntConst(Context &ctx, Type intType, int val);

llvm::Constant *createDefaultValueConst(Context &ctx, Type type);

inline bool isBinOp(int op)
{
  static constexpr const char *sc_binops = "+-*/%";
  return (op >= Token::TokenType::increment &&
          op <= Token::TokenType::rshift) ||
         std::strchr(sc_binops, op) != nullptr;
}

inline bool isCompAssign(int op)
{
  return op >= Token::TokenType::add_assign &&
         op <= Token::TokenType::bit_xor_assign;
}

int getCompAssigOpBaseOp(int op);

llvm::Value *createBinOp(Context &ctx, int op, Type commonType,
                         llvm::Value *lhs, llvm::Value *rhs);

llvm::Value *createAssignment(Context &ctx, llvm::Value *val,
                              VariableExpr *var);

#endif // CODEGEN_H
