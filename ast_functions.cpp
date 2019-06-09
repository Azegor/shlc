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

#include <cassert>

#include <llvm/IR/IRBuilder.h>

#include "codegen.h"
#include "context.h"
#include "ast_statements.h"

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
  llvm::Function *f = createLLVMFunction(ctx.global);

  // Set names for all arguments.
  size_t idx = 0;
  for (llvm::Function::arg_iterator ai = f->arg_begin(); idx != args.size();
       ++ai, ++idx)
  {
    ai->setName(args[idx].second);
  }
  return f;
}

llvm::Function *FunctionHead::createLLVMFunction(GlobalContext &gl_ctx)
{
  auto range = gl_ctx.declaredFunctions.equal_range(name);
  if (range.first == range.second) {
    throw CodeGenError("can't find function in function table", this);
  }
  FunctionHead *correctOverloadFH = nullptr;
  for (auto i = range.first; i != range.second; ++i)
  {
    if (hasSameArgsAs(*i->second.fnHead)) {
      correctOverloadFH = i->second.fnHead;
      break;
    }
  }
  assert(correctOverloadFH != nullptr);
  //   if (std::distance(range.first, range.second) > 1) {
  //     throw CodeGenError("too many functions (overloading not supported
  //     yet)",
  //                        this);
  //   }
  if (correctOverloadFH->llvm_fn != 0) {
    llvm_fn = correctOverloadFH->llvm_fn;
    return llvm_fn;
  }
  else
  {
    std::vector<llvm::Type *> argumentTypes;
    argumentTypes.reserve(args.size());
    for (auto &arg : args)
    {
      argumentTypes.push_back(getLLVMTypeFromType(gl_ctx, arg.first));
    }

    llvm::FunctionType *ft = llvm::FunctionType::get(
      getLLVMTypeFromType(gl_ctx, retType), argumentTypes, false);

    // TODO: maybe change linkage for internal functions to something fast?
    auto linkage = llvm::Function::ExternalLinkage;
    //     if (binding == Binding::Intern)
    //     {
    //         linkage = llvm::Function::PrivateLinkage;
    //     }
    llvm::Function *f =
      llvm::Function::Create(ft, linkage, getMangledName(), gl_ctx.module);
    llvm_fn = f;

    return f;
  }
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
    ctx.global.builder.CreateStore(&*ai, alloca);

    // Add arguments to variable symbol table.
    ctx.putVar(args[idx].second, args[idx].first, alloca);
  }
}

std::vector<BuiltinTypeKind> FunctionHead::getArgTypes() const
{
  std::vector<BuiltinTypeKind> res;
  res.reserve(args.size());
  for (auto &arg : args)
    res.push_back(arg.first);
  return res;
}

bool FunctionHead::hasSameArgsAs(const FunctionHead &o)
{
  if (args.size() != o.args.size()) return false;
  for (int i = 0, s = args.size(); i < s; ++i)
  {
    if (args[i].first != o.args[i].first) return false;
  }
  return true;
}

bool FunctionHead::canCallWithArgs(const std::vector<BuiltinTypeKind> &types) const
{
  auto fit = getOverloadFit(types);
  return fit == OverloadFit::Perfect || fit == OverloadFit::Cast;
}

FunctionHead::OverloadFit FunctionHead::getOverloadFit(
  const std::vector<BuiltinTypeKind> &types) const
{
  if (types.size() != args.size()) return OverloadFit::None;
  OverloadFit fit = OverloadFit::Perfect;
  for (int i = 0, s = args.size(); i < s; ++i)
  {
    if (types[i] == args[i].first) continue;
    if (!canImplicitlyCast(types[i], args[i].first)) return OverloadFit::None;
    fit = OverloadFit::Cast;
  }
  return fit;
}

std::string FunctionHead::sigString() const
{
  auto res = name + '(';
  bool first = true;
  for (auto &arg : args)
  {
    if (first) {
      res += getTypeName(arg.first);
      first = false;
    }
    else
    {
      res += ", " + getTypeName(arg.first);
    }
  }
  res += ')';
  return res;
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
  auto &builder = gl_ctx.builder;
  head->addToFunctionTable(gl_ctx, FunctionHead::FnReg::Define);
  auto fn = head->codegen(ctx);

  ctx.currentFn = fn;
  ctx.ret.type = head->getReturnType();

  // Create a new basic block to start insertion into.
  llvm::BasicBlock *entryBB =
    llvm::BasicBlock::Create(gl_ctx.llvm_context, "entry", fn);
  builder.SetInsertPoint(entryBB);

  head->createArgumentAllocas(ctx, fn);

  if (ctx.ret.type != BuiltinTypeKind::vac_t) {
    ctx.ret.val = builder.CreateAlloca(
      getLLVMTypeFromType(gl_ctx, head->getReturnType()), 0, "retval");
  }
  ctx.ret.BB = llvm::BasicBlock::Create(gl_ctx.llvm_context, "ret");

  // make main block, leave entry for allocas only
  auto mainBB = llvm::BasicBlock::Create(gl_ctx.llvm_context, "fnbody", fn);
  builder.SetInsertPoint(mainBB);

  ctx.pushFrame();

  body->codegen(ctx);

  ctx.popFrame();

  auto currentInsBlock = builder.GetInsertBlock();
  builder.SetInsertPoint(entryBB);
  builder.CreateBr(mainBB);
  builder.SetInsertPoint(currentInsBlock);

  if (ctx.frameCount() != 1)
    throw CodeGenError("variable frame count inconsistent (" +
                         std::to_string(ctx.frameCount()) + " != 1)",
                       this);

  auto CFR = body->codeFlowReturn();
  if (CFR != Statement::CodeFlowReturn::Never) {
    if (head->getReturnType() == BuiltinTypeKind::vac_t) // missing return statement
    {
      ReturnStmt({}).codegen(ctx);
    }
    else
    {
      throw CodeGenError("not all control flow branches return a value", this);
    }
  }

  fn->getBasicBlockList().push_back(ctx.ret.BB); // add return block last!
  builder.SetInsertPoint(ctx.ret.BB);
  if (ctx.ret.type == BuiltinTypeKind::vac_t)
    builder.CreateRetVoid();
  else
    builder.CreateRet(builder.CreateLoad(ctx.ret.val));

  gl_ctx.fpm->run(*fn);

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
  head->addToFunctionTable(
    gl_ctx, FunctionHead::FnReg::Native); // register before codegen -> allows
                                          // self-recursion
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
  head->addToFunctionTable(gl_ctx, FunctionHead::FnReg::Declare);
  return head->codegen(ctx);
}

void FunctionHead::addToFunctionTable(GlobalContext &ctx, FnReg regType)
{
  auto range = ctx.declaredFunctions.equal_range(name);
  if (range.first == range.second) // no other fns
  {
    ctx.declaredFunctions.insert({name, {regType, this}});
    return;
  }
  // already functions with same name
  for (auto fn = range.first; fn != range.second;
       ++fn) // should only be one element for now
  {
    // cannot have function with same args again
    if (hasSameArgsAs(*fn->second.fnHead)) {
      // not yet declared or this is a declaration
      if (regType == FnReg::Declare || fn->second.regType == FnReg::Declare) {
        if (retType !=
            fn->second.fnHead->retType) // same signature, other return type
          throw CodeGenError(
            "invalid overload of function (with different return type)", this);
        return; // already exist -> exit
      }
      else
      {
        throw CodeGenError("invalid redeclaration of function", this);
      }
    }
  }
  // no conflicting declarations -> is (valid) overload
  //   throw CodeGenError("function overloading not supported yet", this);
  ctx.declaredFunctions.insert({name, {regType, this}});
}

std::string FunctionHead::getMangledName() const
{
  if (binding == Binding::Intern) {
    std::string res = "_Z" + std::to_string(name.length()) + name;
    if (args.empty())
      res += getMangleName(BuiltinTypeKind::vac_t);
    else
      for (auto &arg : args)
      {
        res += getMangleName(arg.first);
      }
    //     res += '_';
    //     res += getMangleName(retType);
    return res;
  }
  if (binding == Binding::Extern_C) {
    return bindName;
  }
  throw CodeGenError("unknown function binding type", this);
}
