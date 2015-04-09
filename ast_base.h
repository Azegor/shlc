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

#include "lexer.h"
#include "context.h"

// TODO: might not be neccessary!
class AstNode
{
public:
  virtual ~AstNode() {}

  virtual void print(int indent = 0) = 0;
};

using AstNodePtr = std::unique_ptr<AstNode>;

enum class Type : int { none, inferred, int_t, flt_t, chr_t, boo_t, str_t, vac_t, };

namespace types {
  using int_t = long long;
  using flt_t = long double;
  using chr_t = signed char;
  using boo_t = bool;
  using str_t = std::string;
}

std::string getTypeName(Type t);
Type getTypeFromToken(int tok);

// TODO use different class as Expr-ancestor, so that Statement can have it's own codegen-function with other/no return type
class Statement : public AstNode
{
public:
  Statement() = default;
  virtual ~Statement() {}
  virtual llvm::Value *codegen(Context &ctx){return nullptr;} // TODO remove default make abstract
};

using StmtPtr = std::unique_ptr<Statement>;
using StmtList = std::vector<StmtPtr>;

class Expr : public Statement
{
protected:
  // Type type = Type::none;

public:
  Expr() = default; // TODO: remove when all derived classes use 2nd constructor
  // Expr(Type type) : type(type) {}
  virtual ~Expr() {}
  virtual Type getType(Context &cc) = 0;
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
  for (auto &&e : list) {
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
  const AstNode* node;
  const std::string reason, errorLine;
  CodeGenError(AstNode* node, std::string what)
      : node(std::move(node)), reason(std::move(what))
  {
  }
  const char *what() const noexcept override { return reason.c_str(); }
};


// // Functions: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// #include "ast_functions.h"
//
// // Expressions: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// #include "ast_expressions.h"
//
// // Statements:~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// #include "ast_statements.h"

#endif // AST_BASE_H
