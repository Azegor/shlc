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
class FieldAccessExpr;

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

CastMode castMode(BuiltinTypeKind from, BuiltinTypeKind to);
inline bool canImplicitlyCast(Type *from, Type *to)
{
  if (from == to) {
    return true;
  }
  auto mode = castMode(from->getKind(), to->getKind());
  return mode == CastMode::Implicit || mode == CastMode::Same;
}

inline bool canCast(Type *from, Type *to)
{
  if (from == to) {
    return true;
  }
  auto mode = castMode(from->getKind(), to->getKind());
  return mode == CastMode::Explicit || mode == CastMode::Implicit ||
         mode == CastMode::Same;
}

llvm::Value *generateCast(Context &ctx, llvm::Value *val, Type *from, Type *to,
                          const llvm::Twine &valName = "");

BuiltinTypeKind commonType(BuiltinTypeKind t1, BuiltinTypeKind t2);

llvm::Constant *getIntConst(Context &ctx, BuiltinTypeKind intType, int val);
ExprPtr getIntConstExpr(BuiltinTypeKind intType, int val);

llvm::Constant *createDefaultValueConst(Context &ctx, llvm::Type *type);

inline bool isBinOp(int op)
{
  static constexpr const char *sc_binops = "+-*/%|&^<>";
  return (op >= Token::TokenType::power && op <= Token::TokenType::rshift) ||
         std::strchr(sc_binops, op) != nullptr;
}

inline bool isUnOp(int op)
{
  static constexpr const char *sc_unops = "!~-";
  return std::strchr(sc_unops, op) != nullptr;
}

inline bool isCompAssign(int op)
{
  return op >= Token::TokenType::add_assign &&
         op <= Token::TokenType::bit_xor_assign;
}

int getCompAssigOpBaseOp(int op);

inline int getIncDecOpBaseOp(int op)
{
  if (op == Token::TokenType::increment) return Token::TokenType::add_assign;
  if (op == Token::TokenType::decrement) return Token::TokenType::sub_assign;
  throw CodeGenError("invalid inc/dec op " + Lexer::getTokenName(op));
}

llvm::Value *createUnOp(Context &ctx, int op, BuiltinTypeKind type, llvm::Value *rhs);

llvm::Value *createBinOp(Context &ctx, int op, BuiltinTypeKind commonType,
                         llvm::Value *lhs, llvm::Value *rhs);

BuiltinTypeKind getBinOpReturnType(int op, BuiltinTypeKind inType);

llvm::Value *createAssignment(Context &ctx, llvm::Value *val,
                              VariableExpr *var);
llvm::Value *createAssignment(Context &ctx, llvm::Value *val,
                              FieldAccessExpr *fa);
void handleAssignmentRefCounts(Context &ctx, llvm::Value *lhsAddress, llvm::Value *rhs);

void createMainFunctionReturn(Context &ctx);

#endif // CODEGEN_H
