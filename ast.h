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

enum Type : int { none, int_t, flt_t, chr_t, boo_t, str_t, vac_t, };

std::string getTypeName(Type t);
Type getTypeFromToken(int tok);

class Expr : public AstNode
{
protected:
  Type type = Type::none;

public:
  Expr() = default; // TODO: remove when all derived classes use 2nd constructor
  Expr(Type type) : type(type) {}
  virtual ~Expr() {}
  // virtual llvm::Value *Codegen() = 0;
};

using ExprPtr = std::unique_ptr<Expr>;
using ExprList = std::vector<ExprPtr>;

class BlockExpr : public Expr
{
  ExprList block;

public:
  BlockExpr(ExprList block) : block(std::move(block)) {}
  void print(int indent = 0) override;
  /*
  BlockExpr(const BlockExpr&) = delete;
  BlockExpr(BlockExpr&&) = delete;
  BlockExpr &operator=(const BlockExpr&) = delete;
  BlockExpr &operator=(BlockExpr&&) = delete;
  */
};

using BlockExprPtr = std::unique_ptr<BlockExpr>;

template <typename Ex, typename... Args> ExprPtr make_EPtr(Args &&... args)
{
  return ExprPtr{new Ex(std::forward<Args>(args)...)};
}

// Functions: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
using ArgVector = std::vector<std::pair<int, std::string>>;

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
};

using FunctionHeadPtr = std::unique_ptr<FunctionHead>;

class Function : public AstNode
{
protected:
  FunctionHeadPtr head;

public:
  Function(FunctionHeadPtr head) : head(std::move(head)) {}
  void print(int indent = 0) override;
};

class NativeFunction : public Function
{
public:
  NativeFunction(FunctionHeadPtr head) : Function(std::move(head)) {}
  void print(int indent = 0) override;
};

class NormalFunction : public Function
{
  BlockExprPtr body;

public:
  NormalFunction(FunctionHeadPtr head, BlockExprPtr body)
      : Function(std::move(head)), body(std::move(body))
  {
  }
  void print(int indent = 0) override;
};
class FunctionDecl : public Function
{
public:
  FunctionDecl(FunctionHeadPtr head) : Function(std::move(head)) {}
  void print(int indent = 0) override;
};

using FunctionPtr = std::unique_ptr<Function>;

// Expressions: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class CallExpr : public Expr
{
  std::string fnName;
  ArgVector args;

public:
  CallExpr(std::string fnName, ArgVector args)
      : fnName(std::move(fnName)), args(std::move(args))
  {
  }
};

class VariableExpr : public Expr
{
  std::string name;

public:
  VariableExpr(std::string name) : name(std::move(name)) {}
  void print(int indent = 0) override;
};

class FunctionCallExpr : public Expr
{
  std::string name;
  ExprList args;

public:
  FunctionCallExpr(std::string name, ExprList args)
      : name(std::move(name)), args(std::move(args))
  {
  }
  void print(int indent = 0) override;
};

class NumberExpr : public Expr
{
public:
  NumberExpr(Type type) : Expr(type) {}
  void print(int indent = 0) override;
};

class IntNumberExpr : public NumberExpr
{
  long long value;

public:
  IntNumberExpr(long long val) : NumberExpr(Type::int_t), value(val) {}
  void print(int indent = 0) override;
};

class FltNumberExpr : public NumberExpr
{
  long double value;

public:
  FltNumberExpr(long double val) : NumberExpr(Type::flt_t), value(val) {}
  void print(int indent = 0) override;
};

class VarDeclExpr : public Expr
{
  Type type;
  std::vector<std::string> names;

public:
  VarDeclExpr(Type type, std::vector<std::string> names)
      : type(type), names(std::move(names))
  {
  }
  void print(int indent = 0) override;
};

class BinOpExpr : public Expr
{
  int op;
  ExprPtr lhs, rhs;

public:
  BinOpExpr(int op, ExprPtr lhs, ExprPtr rhs)
      : op(op), lhs(std::move(lhs)), rhs(std::move(rhs))
  {
  }
  void print(int indent = 0) override;
};

class ReturnExpr : public Expr
{
  ExprPtr expr;

public:
  ReturnExpr(ExprPtr expr) : expr(std::move(expr)) {}
  void print(int indent = 0) override;
};

#endif // AST_H
