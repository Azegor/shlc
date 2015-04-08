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

#ifndef ASTFUNCTIONS_H
#define ASTFUNCTIONS_H

#include "ast.h"
#include "ast_expressions.h"

using ArgVector = std::vector<std::pair<Type, std::string>>;

class FunctionHead : public AstNode
{
  std::string name;
  ArgVector args;
  Type retType;

public:
  FunctionHead(std::string name, ArgVector args, Type retType)
      : name(std::move(name)), args(std::move(args)), retType(retType)
  {
  }

  void print(int indent = 0) override;
  const std::string &getName() const {return name; }
  llvm::Function *codegen(Context &ctx);
  void createArgumentAllocas(Context &ctx, llvm::Function* fn);
};

using FunctionHeadPtr = std::unique_ptr<FunctionHead>;

class Function : public AstNode
{
protected:
  FunctionHeadPtr head;

public:
  Function(FunctionHeadPtr head) : head(std::move(head)) {}
  void print(int indent = 0) override;
  const std::string &getName() const {return head->getName(); }
  virtual llvm::Function *codegen(GlobalContext &gl_ctx) = 0;

};

class NativeFunction : public Function
{
public:
  NativeFunction(FunctionHeadPtr head) : Function(std::move(head)) {}
  void print(int indent = 0) override;
  llvm::Function *codegen(GlobalContext &gl_ctx) override;
};

class NormalFunction : public Function
{
  BlockStmtPtr body;

public:
  NormalFunction(FunctionHeadPtr head, BlockStmtPtr body)
      : Function(std::move(head)), body(std::move(body))
  {
  }
  void print(int indent = 0) override;
  llvm::Function *codegen(GlobalContext &gl_ctx) override;
};
class FunctionDecl : public Function
{
public:
  FunctionDecl(FunctionHeadPtr head) : Function(std::move(head)) {}
  void print(int indent = 0) override;
  llvm::Function *codegen(GlobalContext &gl_ctx) override;
};

using FunctionPtr = std::unique_ptr<Function>;

#endif // ASTFUNCTIONS_H
