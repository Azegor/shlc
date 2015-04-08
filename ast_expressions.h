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

#ifndef ASTEXPRESSIONS_H
#define ASTEXPRESSIONS_H

#include "ast.h"

class VariableExpr : public Expr
{
  std::string name;

public:
  VariableExpr(std::string name) : name(std::move(name)) {}
  void print(int indent = 0) override;
  Type getType(Context &cc) override
  {
    return Type::none; // TODO consult variable table here
  }
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
  Type getType(Context &cc) override
  {
    return Type::none; // TODO consult function table here
  }
};

class ConstantExpr : public Expr
{
  Type type;

public:
  ConstantExpr(Type type) : type(type) {}
  void print(int indent = 0) override;
  Type getType(Context &cc) override { return type; }
};

class IntNumberExpr : public ConstantExpr
{
  long long value;

public:
  IntNumberExpr(long long val) : ConstantExpr(Type::int_t), value(val) {}
  void print(int indent = 0) override;
};

class FltNumberExpr : public ConstantExpr
{
  long double value;

public:
  FltNumberExpr(long double val) : ConstantExpr(Type::flt_t), value(val) {}
  void print(int indent = 0) override;
};

class BoolConstExpr : public ConstantExpr
{
  bool value;

public:
  BoolConstExpr(bool val) : ConstantExpr(Type::boo_t), value(val) {}
  void print(int indent = 0) override;
};

class StringConstExpr : public ConstantExpr
{
  std::string value;

public:
  StringConstExpr(std::string val)
      : ConstantExpr(Type::boo_t), value(std::move(val))
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
  Type getType(Context &cc) override
  {
    // TODO check implicit cast, throw errors, etc...
    return lhs->getType(cc);
  }
};

class CastExpr : public Expr
{
  ExprPtr expr;
  Type newType;

public:
  CastExpr(ExprPtr expr, Type newType) : expr(std::move(expr)), newType(newType)
  {
  }
  void print(int indent = 0) override;
  Type getType(Context &cc) override { return newType; }
};

#endif // ASTEXPRESSIONS_H
