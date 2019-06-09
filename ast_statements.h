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
#include "ast_types.h"

class VarDeclStmt : public Statement
{
public:
  using VarEnties = std::vector<std::pair<std::string, ExprPtr>>;

private:
  Type *type;
  VarEnties vars;

public:
  VarDeclStmt(SourceLocation loc, Type *type, VarEnties vars)
      : Statement(loc), type(type), vars(std::move(vars))
  {
  }
  void print(int indent = 0) override;
  Type *getType(Context &ctx);
  llvm::Value *codegen(Context &ctx) override;
};

class ReturnStmt : public Statement
{
  ExprPtr expr;

public:
  ReturnStmt(SourceLocation loc, ExprPtr expr = {})
      : Statement(loc), expr(std::move(expr))
  {
  }
  void print(int indent = 0) override;
  llvm::Value *codegen(Context &ctx) override;
  Statement::CodeFlowReturn codeFlowReturn() const override
  {
    return Statement::CodeFlowReturn::Never;
  }
};

class IfStmt : public Statement
{
  ExprPtr cond;
  StmtPtr thenExpr;
  StmtPtr elseExpr;

public:
  IfStmt(SourceLocation loc, ExprPtr cond, StmtPtr thenE, StmtPtr elseE)
      : Statement(loc),
        cond(std::move(cond)),
        thenExpr(std::move(thenE)),
        elseExpr(std::move(elseE))
  {
  }
  void print(int indent = 0) override;
  llvm::Value *codegen(Context &ctx) override;
  Statement::CodeFlowReturn codeFlowReturn() const override
  {
    return Statement::combineCFR(thenExpr->codeFlowReturn(),
                                 elseExpr ? elseExpr->codeFlowReturn()
                                          : Statement::CodeFlowReturn::Always);
  }
  Statement::BranchBehaviour branchBehaviour() const override
  {
    return elseExpr ? thenExpr->branchBehaviour() | elseExpr->branchBehaviour()
                    : thenExpr->branchBehaviour();
  }
};

class WhileStmt : public LoopStmt
{
  ExprPtr cond;
  StmtPtr body;

  llvm::BasicBlock *contBB = nullptr;
  llvm::BasicBlock *breakBB = nullptr;

public:
  WhileStmt(SourceLocation loc, ExprPtr cond, StmtPtr body)
      : LoopStmt(loc), cond(std::move(cond)), body(std::move(body))
  {
  }
  void print(int indent = 0) override;
  llvm::Value *codegen(Context &ctx) override;
  Statement::CodeFlowReturn codeFlowReturn() const override
  {
    return Statement::combineCFR(body->codeFlowReturn(),
                                 Statement::CodeFlowReturn::Always);
  }

  llvm::BasicBlock *continueTarget() const override { return contBB; }
  llvm::BasicBlock *breakTarget() const override { return breakBB; }
};

class DoWhileStmt : public LoopStmt
{
  ExprPtr cond;
  StmtPtr body;

  llvm::BasicBlock *contBB = nullptr;
  llvm::BasicBlock *breakBB = nullptr;

public:
  DoWhileStmt(SourceLocation loc, ExprPtr cond, StmtPtr body)
      : LoopStmt(loc), cond(std::move(cond)), body(std::move(body))
  {
  }
  void print(int indent = 0) override;
  llvm::Value *codegen(Context &ctx) override;
  Statement::CodeFlowReturn codeFlowReturn() const override
  {
    auto bodyCFR = body->codeFlowReturn();
    if (bodyCFR == Statement::CodeFlowReturn::Never &&
        (bool)(body->branchBehaviour() &
               (Statement::BranchBehaviour::Breaks |
                Statement::BranchBehaviour::Continues)))
      return Statement::CodeFlowReturn::Sometimes;
    return bodyCFR;
  }

  llvm::BasicBlock *continueTarget() const override { return contBB; }
  llvm::BasicBlock *breakTarget() const override { return breakBB; }
};

class ForStmt : public LoopStmt
{
  StmtPtr init;
  ExprPtr cond;
  ExprPtr incr;
  StmtPtr body;

  llvm::BasicBlock *contBB = nullptr;
  llvm::BasicBlock *breakBB = nullptr;

public:
  ForStmt(SourceLocation loc, StmtPtr init, ExprPtr cond, ExprPtr incr,
          StmtPtr body)
      : LoopStmt(loc),
        init(std::move(init)),
        cond(std::move(cond)),
        incr(std::move(incr)),
        body(std::move(body))
  {
  }
  void print(int indent = 0) override;
  llvm::Value *codegen(Context &ctx) override;
  Statement::CodeFlowReturn codeFlowReturn() const override
  {
    return Statement::combineCFR(body->codeFlowReturn(),
                                 Statement::CodeFlowReturn::Always);
  }

  llvm::BasicBlock *continueTarget() const override { return contBB; }
  llvm::BasicBlock *breakTarget() const override { return breakBB; }
};

class BreakStmt : public LoopCtrlStmt
{
public:
  BreakStmt(SourceLocation loc) : LoopCtrlStmt(loc) {}
  void print(int indent = 0) override;
  llvm::Value *codegen(Context &ctx) override;
  Statement::BranchBehaviour branchBehaviour() const override
  {
    return Statement::BranchBehaviour::Breaks;
  }
};

class ContinueStmt : public LoopCtrlStmt
{
public:
  ContinueStmt(SourceLocation loc) : LoopCtrlStmt(loc) {}
  void print(int indent = 0) override;
  llvm::Value *codegen(Context &ctx) override;
  Statement::BranchBehaviour branchBehaviour() const override
  {
    return Statement::BranchBehaviour::Continues;
  }
};

class ExprStmt : public Statement
{
  ExprPtr expr;

public:
  ExprStmt(SourceLocation loc, ExprPtr expr)
      : Statement(loc), expr(std::move(expr))
  {
  }
  void print(int indent = 0) override;
  llvm::Value *codegen(Context &ct) override;
};

#endif // ASTSTATEMENTS_H
