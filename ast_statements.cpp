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

#include "ast_statements.h"
#include "ast_expressions.h"

#include <iostream>

void ReturnStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "Return: [" << std::endl;
  expr->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

void CastExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "Cast: [" << std::endl;
  expr->print(indent + 1);
  printIndent(indent);
  std::cout << "] to " << getTypeName(newType) << std::endl;
}

void IfStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "If: (";
  cond->print();
  std::cout << ")" << std::endl;
  thenExpr->print(indent);
  if (elseExpr) {
    printIndent(indent);
    std::cout << "else" << std::endl;
    elseExpr->print(indent);
  }
  printIndent(indent);
  std::cout << std::endl;
}

void WhileStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "While: (";
  cond->print();
  std::cout << ")" << std::endl;
  body->print(indent);
  printIndent(indent);
  std::cout << std::endl;
}

void DoWhileStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "Do:";
  body->print(indent);
  printIndent(indent);
  std::cout << "While (";
  cond->print();
  std::cout << ')' << std::endl;
}

void ForStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "For: (";
  init->print();
  std::cout << ',';
  cond->print();
  std::cout << ',';
  incr->print();
  std::cout << ")" << std::endl;
  body->print(indent);
  printIndent(indent);
  std::cout << std::endl;
}

void BreakStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "[Break]" << std::endl;
}

void ContinueStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "[Continue]" << std::endl;
}

llvm::Value *ReturnStmt::codegen(Context &ctx)
{
    if (expr->getType(ctx) == Type::vac_t)
    {
        ctx.global.builder.CreateRetVoid();
    }
    else
    {
        ctx.global.builder.CreateRet(expr->codegen(ctx));
    }
    return nullptr;
}
