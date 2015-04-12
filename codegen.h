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

#include <llvm/IR/Type.h>

#include "context.h"
#include "ast.h"
#include "type.h"

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
  return castMode(from, to) == CastMode::Implicit;
}

inline bool canCast(Type from, Type to)
{
  auto mode = castMode(from, to);
  return mode == CastMode::Explicit || mode == CastMode::Implicit ||
         mode == CastMode::Same;
}

llvm::Value *generateCast(Context &ctx, llvm::Value *val, Type from, Type to,
                          const llvm::Twine &valName = "");

#endif // CODEGEN_H
