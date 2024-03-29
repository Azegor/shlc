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

#include <string>

#include <llvm/IR/Constant.h>

#include "ast_base.h"
#include "ast_types.h"
#include "codegen.h"

class FunctionHead;

class VariableExpr : public Expr
{
  std::string name;

public:
  VariableExpr(SourceLocation loc, std::string name)
      : Expr(loc), name(std::move(name))
  {
  }
  VariableExpr(const VariableExpr &o) : Expr(o.srcLoc), name(o.name) {}
  void print(int indent = 0) override;
  Type *getType(Context &ctx) override;
  llvm::AllocaInst *getAlloca(Context &ctx);
  llvm::Value *genLLVM(Context &ctx) override;
};

class GlobalVarExpr : public Expr
{
  std::string name;

public:
  GlobalVarExpr(SourceLocation loc, std::string name)
      : Expr(loc), name(std::move(name))
  {
  }
  void print(int indent = 0) override;
  Type *getType(Context &ctx) override;
  //   llvm::GlobalVariable *getGlobalVarInst(Context &ctx);
  llvm::Value *genLLVM(Context &ctx) override;
};

class FunctionCallExpr : public Expr
{
  std::string name;
  ExprList args;
  FunctionHead *fnHead = nullptr;

public:
  FunctionCallExpr(SourceLocation loc, std::string name, ExprList args)
      : Expr(loc), name(std::move(name)), args(std::move(args))
  {
  }
  void print(int indent = 0) override;
  Type *getType(Context &ctx) override;
  std::vector<Type*> getArgTypes(Context &ctx) const;
  llvm::Value *genLLVM(Context &ctx) override;

private:
  void findFunction(Context &ctx);
};

class ConstantExpr : public Expr
{
  Type *type;

public:
  ConstantExpr(SourceLocation loc, Type *type) : Expr(loc), type(type) {}
  void print(int indent = 0) override;
  Type *getType(Context &) override { return type; }
  llvm::Value *genLLVM(Context &ctx) override = 0;
};

class IntNumberExpr : public ConstantExpr
{
  long long value;

public:
  IntNumberExpr(SourceLocation loc, long long val)
      : ConstantExpr(loc, TypeRegistry::getBuiltinType(BuiltinTypeKind::int_t)), value(val)
  {
  }
  void print(int indent = 0) override;
  llvm::Constant *genLLVM(Context &ctx) override;
};

class CharConstExpr : public ConstantExpr
{
  char value;

public:
  CharConstExpr(SourceLocation loc, char val)
      : ConstantExpr(loc, TypeRegistry::getBuiltinType(BuiltinTypeKind::chr_t)), value(val)
  {
  }
  void print(int indent = 0) override;
  llvm::Constant *genLLVM(Context &ctx) override;
};

class FltNumberExpr : public ConstantExpr
{
  double value;

public:
  FltNumberExpr(SourceLocation loc, long double val)
      : ConstantExpr(loc, TypeRegistry::getBuiltinType(BuiltinTypeKind::flt_t)), value(val)
  {
  }
  void print(int indent = 0) override;
  llvm::Constant *genLLVM(Context &ctx) override;
};

class BoolConstExpr : public ConstantExpr
{
  bool value;

public:
  BoolConstExpr(SourceLocation loc, bool val)
      : ConstantExpr(loc, TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t)), value(val)
  {
  }
  void print(int indent = 0) override;
  llvm::Constant *genLLVM(Context &ctx) override;
};

class StringConstExpr : public ConstantExpr
{
  std::string value;

public:
  StringConstExpr(SourceLocation loc, std::string val)
      : ConstantExpr(loc, TypeRegistry::getBuiltinType(BuiltinTypeKind::str_t)), value(std::move(val))
  {
  }
  void print(int indent = 0) override;
  llvm::Value *genLLVM(Context &ctx) override;
};

class BinOpExpr : public Expr
{
  int op;
  ExprPtr lhs, rhs;

public:
  BinOpExpr(SourceLocation loc, int op, ExprPtr lhs, ExprPtr rhs)
      : Expr(loc), op(op), lhs(std::move(lhs)), rhs(std::move(rhs))
  {
  }
  void print(int indent = 0) override;
  Type *getCommonType(Context &ctx);
  Type *getType(Context &ctx) override;
  llvm::Value *genLLVM(Context &ctx) override;
};

class UnOpExpr : public Expr
{
  int op;
  ExprPtr rhs;

public:
  UnOpExpr(SourceLocation loc, int op, ExprPtr rhs)
      : Expr(loc), op(op), rhs(std::move(rhs))
  {
  }
  void print(int indent = 0) override;
  Type *getType(Context &ctx) override;
  llvm::Value *genLLVM(Context &ctx) override;
};

class CastExpr : public Expr
{
  ExprPtr expr;
  Type *newType;

public:
  CastExpr(ExprPtr expr, Type *newType)
      : Expr(expr->srcLoc), expr(std::move(expr)), newType(newType)
  {
  }
  CastExpr(SourceLocation loc, ExprPtr expr, Type *newType)
      : Expr(loc), expr(std::move(expr)), newType(newType)
  {
  }
  void print(int indent = 0) override;
  Type *getType(Context &) override { return newType; }
  llvm::Value *genLLVM(Context &ctx) override;
};

class NewExpr : public Expr
{
  ClassType *cls;

public:
  NewExpr(SourceLocation loc, ClassType *cls) : Expr(loc), cls(cls) {}

  void print(int indent = 0) override;
  Type *getType(Context &) override { return cls; }
  llvm::Value *genLLVM(Context &ctx) override;
};

class FieldAccessExpr : public Expr
{
    ExprPtr expr;
    std::string field;

public:
    FieldAccessExpr(SourceLocation loc, ExprPtr expr, std::string field)
        : Expr(loc), expr(std::move(expr)), field(std::move(field))
    {
    }

    void print(int indent = 0) override;
    Type *getType(Context &ctx) override { return getClassField(ctx)->type; }
    llvm::Value *genLLVM(Context &ctx) override;
    std::pair<llvm::Value *, llvm::Type*> codegenFieldAddress(Context &ctx);

private:
    const ClassField *getClassField(Context &ctx) const ;
};

#endif // ASTEXPRESSIONS_H
