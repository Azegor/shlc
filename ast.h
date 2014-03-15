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

#ifndef AST_H
#define AST_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "lexer.h"

// TODO: might not be neccessary!
class AstNode
{
public:
  virtual ~AstNode() {}

  virtual void print(int indent = 0) = 0;
};

using AstNodePtr = std::unique_ptr<AstNode>;

class Expr : public AstNode
{
public:
  virtual ~Expr() {}
  // virtual llvm::Value *Codegen() = 0;
};

using ExprPtr = std::unique_ptr<Expr>;
using ExprList = std::vector<ExprPtr>;

// Functions:

using ArgVector = std::vector<std::pair<int, std::string>>;

class FunctionHeadExpr : public AstNode
{
  std::string name;
  ArgVector args;

public:
  FunctionHeadExpr(std::string name, ArgVector args)
      : name(std::move(name)), args(std::move(args))
  {
  }

  void print(int indent = 0) override;
};

using FunctionHeadExprPtr = std::unique_ptr<FunctionHeadExpr>;

class FunctionExpr : public Expr
{
protected:
  FunctionHeadExprPtr head;

public:
  FunctionExpr(FunctionHeadExprPtr head) : head(std::move(head)) {}
  void print(int indent = 0) override;
};

class NativeFunctionExpr : public FunctionExpr
{
public:
  NativeFunctionExpr(FunctionHeadExprPtr head) : FunctionExpr(std::move(head))
  {
  }
  void print(int indent = 0) override;
};
class NormalFunctionExpr : public FunctionExpr
{
  ExprList body;

public:
  NormalFunctionExpr(FunctionHeadExprPtr head, ExprList body)
      : FunctionExpr(std::move(head)), body(std::move(body))
  {
  }
  void print(int indent = 0) override;
};
class FunctionDeclExpr : public FunctionExpr
{
public:
  FunctionDeclExpr(FunctionHeadExprPtr head) : FunctionExpr(std::move(head)) {}
  void print(int indent = 0) override;
};

using FunctionExprPtr = std::unique_ptr<FunctionExpr>;

// Expressions:

class CallExpr : public Expr
{
  std::string fnName;
  ArgVector args;

public:
};

/*

class TestToken : public Expr
{
  std::string name;
  std::vector<ExprPtr> children;

public:
  TestToken(std::string name) : name(std::move(name)) {}

  void addChild(ExprPtr child) { children.push_back(std::move(child)); }

  void print(int indent = 0) override
  {
    printIndent(indent);
    std::cout << name << '[' << std::endl;
    for (auto &c : children)
      c->print(indent + 1);
    std::cout << ']' << std::endl;
  }
};

*/

#endif // AST_H
