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

#ifndef ASTSTATEMENTS_H
#define ASTSTATEMENTS_H

#include "ast_base.h"

class VarDeclStmt : public Statement
{
public:
  using VarEnties = std::vector<std::pair<std::string, ExprPtr>>;

private:
  Type type;
  VarEnties vars;

public:
  VarDeclStmt(Type type, VarEnties vars) : type(type), vars(std::move(vars)) {}
  void print(int indent = 0) override;
};

class ReturnStmt : public Statement
{
  ExprPtr expr;

public:
  ReturnStmt(ExprPtr expr) : expr(std::move(expr)) {}
  void print(int indent = 0) override;
  llvm::Value *codegen(Context &ctx) override;
};

class IfStmt : public Statement
{
  ExprPtr cond;
  BlockStmtPtr thenExpr;
  BlockStmtPtr elseExpr;

public:
  IfStmt(ExprPtr cond, BlockStmtPtr thenE, BlockStmtPtr elseE)
      : cond(std::move(cond)),
        thenExpr(std::move(thenE)),
        elseExpr(std::move(elseE))
  {
  }
  void print(int indent = 0) override;
};

class WhileStmt : public Statement
{
  ExprPtr cond;
  BlockStmtPtr body;

public:
  WhileStmt(ExprPtr cond, BlockStmtPtr body)
      : cond(std::move(cond)), body(std::move(body))
  {
  }
  void print(int indent = 0) override;
};

class DoWhileStmt : public Statement
{
  ExprPtr cond;
  BlockStmtPtr body;

public:
  DoWhileStmt(ExprPtr cond, BlockStmtPtr body)
      : cond(std::move(cond)), body(std::move(body))
  {
  }
  void print(int indent = 0) override;
};

class ForStmt : public Statement
{
  ExprPtr init, cond, incr;
  BlockStmtPtr body;

public:
  ForStmt(ExprPtr init, ExprPtr cond, ExprPtr incr, BlockStmtPtr body)
      : init(std::move(init)),
        cond(std::move(cond)),
        incr(std::move(incr)),
        body(std::move(body))
  {
  }
  void print(int indent = 0) override;
};

class BreakStmt : public Statement
{
public:
  void print(int indent = 0) override;
};

class ContinueStmt : public Statement
{
public:
  void print(int indent = 0) override;
};

#endif // ASTSTATEMENTS_H
