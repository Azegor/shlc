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
namespace llvm
{
class BasicBlock;
}

struct SourceLocation
{
  const Token startToken, endToken;
  // reference file here
  SourceLocation() = default;
  SourceLocation(Token start, Token end) : startToken(start), endToken(end) {}
  SourceLocation(Token single) : startToken(single), endToken(single) {}
  SourceLocation(const SourceLocation &o)
      : startToken(o.startToken), endToken(o.endToken)
  {
  }
  SourceLocation(SourceLocation &&o)
      : startToken(std::move(o.startToken)), endToken(std::move(o.endToken))
  {
  }

  std::string toStr() const {
    // TODO output source itselfe (get from lexer)
    if (startToken.type == Token::TokenType::none)
      return "at unknown location";
    if (startToken == endToken)
      return "at token " + startToken.toStr();
    return "between token " + startToken.toStr() + " and " + endToken.toStr();
  }
};

// TODO: might not be neccessary!
class AstNode
{
public:
  AstNode(SourceLocation loc) : srcLoc(loc) {}
  virtual ~AstNode() {}

  virtual void print(int indent = 0) = 0;

  const SourceLocation srcLoc;
};

using AstNodePtr = std::unique_ptr<AstNode>;

// TODO use different class as Expr-ancestor, so that Statement can have it's
// own codegen-function with other/no return type
class Statement : public AstNode
{
public:
  Statement(SourceLocation loc) : AstNode(loc) {}
  virtual ~Statement() {}
  // TODO remove default make abstract
  virtual llvm::Value *codegen(Context &ctx) = 0;
  enum class CodeFlowReturn
  {
    Never,
    Sometimes,
    Always
  };
  static CodeFlowReturn combineCFR(CodeFlowReturn c1, CodeFlowReturn c2)
  {
    if (c1 == c2) return c1;
    return CodeFlowReturn::Sometimes;
  }
  virtual CodeFlowReturn codeFlowReturn() const
  {
    return Statement::CodeFlowReturn::Always; // default impl
  }
};

using StmtPtr = std::unique_ptr<Statement>;
using StmtList = std::vector<StmtPtr>;

class Expr : public AstNode
{
protected:
  // Type type = Type::none;

public:
  Expr(SourceLocation loc) : AstNode(loc) {}
  virtual ~Expr() {}
  virtual Type getType(Context &cc) = 0;
  // TODO remove default make abstract
  virtual llvm::Value *codegen(Context &ctx) = 0;
};

using ExprPtr = std::unique_ptr<Expr>;
using ExprList = std::vector<ExprPtr>;

class BlockStmt : public Statement
{
  StmtList block;

public:
  BlockStmt(SourceLocation loc, StmtList block)
      : Statement(loc), block(std::move(block))
  {
  }
  void print(int indent = 0) override;
  llvm::Value *codegen(Context &ctx) override;
  Statement::CodeFlowReturn codeFlowReturn() const override;

  // 'const' access to individual statements
  //  const Statement *front() const {return block.front().get(); }
  //  const Statement *back() const {return block.back().get(); }
  //  StmtList::const_iterator begin() const { return block.cbegin(); }
  //  StmtList::const_iterator end() const { return block.cend(); }
};

using BlockStmtPtr = std::unique_ptr<BlockStmt>;
using BlockStmtList = std::vector<BlockStmtPtr>;

class LoopStmt : public Statement
{
public:
  LoopStmt(SourceLocation loc) : Statement(loc) {}
  virtual llvm::BasicBlock *continueTarget() const = 0;
  virtual llvm::BasicBlock *breakTarget() const = 0;
};
class LoopCtrlStmt : public Statement
{
public:
  LoopCtrlStmt(SourceLocation loc) : Statement(loc) {}
};

template <typename St, typename... Args>
StmtPtr make_SPtr(SourceLocation loc, Args &&... args)
{
  return StmtPtr{new St(loc, std::forward<Args>(args)...)};
}
// for copy/move constructor
template <typename St> StmtPtr make_SPtr(const St &s)
{
  return StmtPtr{new St(s)};
}

template <typename Ex, typename... Args>
ExprPtr make_EPtr(SourceLocation loc, Args &&... args)
{
  return ExprPtr{new Ex(loc, std::forward<Args>(args)...)};
}
// for copy/move constructor
template <typename Ex> ExprPtr make_EPtr(const Ex &e)
{
  return ExprPtr{new Ex(e)};
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
  const std::string reason, errorLine;
  SourceLocation srcLoc;
  CodeGenError(std::string what, const AstNode *node = nullptr)
      : reason(std::move(what)), srcLoc(node ? node->srcLoc : SourceLocation())
  {
  }
  const char *what() const noexcept override { return reason.c_str(); }
  std::string errorLocation(){
    return srcLoc.toStr();
  }
};

#endif // AST_BASE_H
