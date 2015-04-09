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

#include "ast_functions.h"

#include <iostream>

#include <llvm/IR/IRBuilder.h>

#include "codegen.h"

void FunctionHead::print(int indent)
{
  printIndent(indent);
  std::cout << name << '(';
  printList(args, [](ArgVector::value_type &e)
            {
              return getTypeName(e.first) + " " + e.second;
            });
  std::cout << ") : " << getTypeName(retType) << std::endl;
}

llvm::Function *FunctionHead::codegen(Context &ctx)
{
  // Make the function type:  double(double,double) etc.
  std::vector<llvm::Type *> argumentTypes;
  argumentTypes.reserve(args.size());
  for (auto &arg : args)
  {
    argumentTypes.push_back(getLLVMTypeFromType(ctx.global, arg.first));
  }

  llvm::FunctionType *ft = llvm::FunctionType::get(
      getLLVMTypeFromType(ctx.global, retType), argumentTypes, false);

  llvm::Function *f = llvm::Function::Create(
      ft, llvm::Function::ExternalLinkage, name, ctx.global.module.get());

  // Set names for all arguments.
  int idx = 0;
  for (llvm::Function::arg_iterator ai = f->arg_begin(); idx != args.size();
       ++ai, ++idx)
  {
    ai->setName(args[idx].second);
  }
  return f;
}

void FunctionHead::createArgumentAllocas(Context &ctx, llvm::Function *fn)
{
  int idx = 0;
  for (llvm::Function::arg_iterator ai = fn->arg_begin(); idx != args.size();
       ++ai, ++idx)
  {
    // Create an alloca for this variable.
    llvm::AllocaInst *alloca = createEntryBlockAlloca(
        fn, args[idx].second, getLLVMTypeFromType(ctx.global, args[idx].first));

    // Store the initial value into the alloca.
    ctx.global.builder.CreateStore(ai, alloca);

    // Add arguments to variable symbol table.
    ctx.putVar(args[idx].second, alloca);
  }
}

void Function::print(int indent)
{
  printIndent(indent);
  std::cout << "* Function [" << std::endl;
  head->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

void NormalFunction::print(int indent)
{
  printIndent(indent);
  std::cout << "Normal Function [" << std::endl;
  head->print(indent + 1);
  body->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

llvm::Function *NormalFunction::codegen(GlobalContext &gl_ctx)
{
  Context ctx(gl_ctx);
  auto fn = head->codegen(ctx);

  // Create a new basic block to start insertion into.
  llvm::BasicBlock *bb =
      llvm::BasicBlock::Create(gl_ctx.llvm_context, "entry", fn);
  gl_ctx.builder.SetInsertPoint(bb);

  head->createArgumentAllocas(ctx, fn);

  return fn;
}

void NativeFunction::print(int indent)
{
  printIndent(indent);
  std::cout << "Native Function [" << std::endl;
  head->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

llvm::Function *NativeFunction::codegen(GlobalContext &gl_ctx)
{
  Context ctx(gl_ctx);
  return head->codegen(ctx);
}

void FunctionDecl::print(int indent)
{
  printIndent(indent);
  std::cout << "Function Declaration [" << std::endl;
  head->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

llvm::Function *FunctionDecl::codegen(GlobalContext &gl_ctx)
{
  Context ctx(gl_ctx);
  return head->codegen(ctx);
}
