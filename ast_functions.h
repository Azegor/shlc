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

#include "ast_base.h"
#include "ast_expressions.h"

class GlobalContext;

using ArgVector = std::vector<std::pair<Type, std::string>>;

class FunctionHead : public AstNode
{
public:
  enum class Binding
  {
    Intern,
    Extern_C
  };

private:
  std::string name;
  std::string bindName;
  ArgVector args;
  Type retType;
  Binding binding;
  llvm::Function *llvm_fn = nullptr;

public:
  FunctionHead(std::string fnName, ArgVector args, Type retType,
               Binding bind = Binding::Intern)
      : name(std::move(fnName)),
        args(std::move(args)),
        retType(retType),
        binding(bind),
        bindName(name)
  {
  }

  bool operator==(const FunctionHead &o) const
  {
    return name == o.name && args == o.args && retType == o.retType &&
           binding == o.binding && bindName == o.bindName;
  }
  // relative ordering for maps/sets
  bool operator<(const FunctionHead &o) const
  {
    if (name < o.name) return true;
    if (name > o.name) return false;
    if (args < o.args) return true;
    if (args > o.args) return false;
    if (retType < o.retType) return true;
    if (retType > o.retType) return false;
    if (binding < o.binding) return true;
    return false;
  }

  enum class FnReg
  {
    Declare,
    Define,
    Native
  };
  void addToFunctionTable(GlobalContext &ctx, FnReg regType);

  void print(int indent = 0) override;
  const std::string &getName() const { return name; }
  void setBindName(const std::string &name) { bindName = name; }
  std::string getMangledName() const;
  Type getReturnType() const { return retType; }
  llvm::Function *codegen(Context &ctx);
  void createArgumentAllocas(Context &ctx, llvm::Function *fn);
  void setBinding(Binding b) { binding = b; }
  llvm::Function *get_llvm_fn() { return llvm_fn; }

  bool canCallWithArgs(const std::vector<Type> &types) const;
  std::string sigString() const;

private:
  llvm::Function *createLLVMFunction(GlobalContext &gl_ctx);
};

using FunctionHeadPtr = std::unique_ptr<FunctionHead>;

// TODO: add to some function table, to look up function when called

class Function : public AstNode
{
protected:
  FunctionHeadPtr head;

public:
  Function(FunctionHeadPtr head) : head(std::move(head)) {}
  void print(int indent = 0) override;
  const std::string &getName() const { return head->getName(); }
  virtual llvm::Function *codegen(GlobalContext &gl_ctx) = 0;
};

class NativeFunction : public Function
{
public:
  NativeFunction(FunctionHeadPtr h) : Function(std::move(h))
  {
    head->setBinding(FunctionHead::Binding::Extern_C);
  }
  void print(int indent = 0) override;
  llvm::Function *codegen(GlobalContext &gl_ctx) override;
};

class NormalFunction : public Function
{
  BlockStmtPtr body;

public:
  NormalFunction(FunctionHeadPtr h, BlockStmtPtr body)
      : Function(std::move(h)), body(std::move(body))
  {
    if (head->getName() == "main")
      head->setBinding(FunctionHead::Binding::Extern_C); // no mangling!
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
