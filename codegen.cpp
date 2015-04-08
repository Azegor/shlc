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

llvm::Type *getTypeFromTokID(GlobalContext &ctx, Type type)
{
    switch(type) {
        // int_t, flt_t, chr_t, boo_t, str_t, vac_t
        case Type::vac_t: return llvm::Type::getVoidTy(ctx.llvm_context);
        case Type::int_t: return llvm::Type::getInt64Ty(ctx.llvm_context);
        case Type::flt_t: return llvm::Type::getDoubleTy(ctx.llvm_context);
        case Type::boo_t: return llvm::Type::getInt1Ty(ctx.llvm_context);
//         case Type::str_t: return llvm::Type::getInt8PtrTy(ctx.llvm_context); // TODO: do the right thing here!!!
        case Type::chr_t: return llvm::Type::getInt8Ty(ctx.llvm_context); // no unicode
        default: throw CodeGenError(nullptr, "Unknown type id" + getTypeName(type));
    }
}

llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *fn,
                        const std::string &varName, llvm::Type *varType)
{
  llvm::IRBuilder<> TmpB(&fn->getEntryBlock(),
                   fn->getEntryBlock().begin());
  return TmpB.CreateAlloca(varType, 0,
                           varName.c_str());
}