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

#ifndef AST_BASE_H
#define AST_BASE_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <llvm/IR/Value.h>

#include "lexer.h"
#include "type.h"

class Context;

// TODO: might not be neccessary!
class AstNode
{
public:
  virtual ~AstNode() {}

  virtual void print(int indent = 0) = 0;
};

using AstNodePtr = std::unique_ptr<AstNode>;

// TODO use different class as Expr-ancestor, so that Statement can have it's
// own codegen-function with other/no return type
class Statement : public AstNode
{
public:
  Statement() = default;
  virtual ~Statement() {}
  // TODO remove default make abstract
  virtual llvm::Value *codegen(Context &ctx) { return nullptr; }
};

using StmtPtr = std::unique_ptr<Statement>;
using StmtList = std::vector<StmtPtr>;

class Expr : public AstNode
{
protected:
  // Type type = Type::none;

public:
  Expr() = default; // TODO: remove when all derived classes use 2nd constructor
  // Expr(Type type) : type(type) {}
  virtual ~Expr() {}
  virtual Type getType(Context &cc) = 0;
  // TODO remove default make abstract
  virtual llvm::Value *codegen(Context &ctx) { return nullptr; }
};

using ExprPtr = std::unique_ptr<Expr>;
using ExprList = std::vector<ExprPtr>;

class BlockStmt : public Statement
{
  StmtList block;

public:
  BlockStmt(StmtList block) : block(std::move(block)) {}
  void print(int indent = 0) override;
  llvm::Value *codegen(Context &ctx) override;

  // 'const' access to individual statements
  //  const Statement *front() const {return block.front().get(); }
  //  const Statement *back() const {return block.back().get(); }
  //  StmtList::const_iterator begin() const { return block.cbegin(); }
  //  StmtList::const_iterator end() const { return block.cend(); }
};

using BlockStmtPtr = std::unique_ptr<BlockStmt>;
using BlockStmtList = std::vector<BlockStmtPtr>;

template <typename St, typename... Args> StmtPtr make_SPtr(Args &&... args)
{
  return StmtPtr{new St(std::forward<Args>(args)...)};
}

template <typename Ex, typename... Args> ExprPtr make_EPtr(Args &&... args)
{
  return ExprPtr{new Ex(std::forward<Args>(args)...)};
}

void printIndent(int indent);

template <typename L, typename Callback>
void printList(L &list, Callback callback)
{
  bool first = true;
  for (auto &&e : list)
  {
    if (first)
      first = false;
    else
      std::cout << ", ";
    std::cout << callback(std::forward<decltype(e)>(e));
  }
}

class CodeGenError : public std::exception
{
public:
  const AstNode *node;
  const std::string reason, errorLine;
  CodeGenError(const AstNode *node, std::string what)
      : node(std::move(node)), reason(std::move(what))
  {
  }
  const char *what() const noexcept override { return reason.c_str(); }
};

#endif // AST_BASE_H
