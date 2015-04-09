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

#include "ast_expressions.h"

#include <iostream>

#include <llvm/ADT/APInt.h>

#include "ast.h"

void VariableExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Variable: " << name << ']';
}

void FunctionCallExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Function Call: " << name << '(';
  for (auto &e : args)
    e->print();
  std::cout << ")]";
}

void ConstantExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Number, type: " << getTypeName(type) << "]";
}
void IntNumberExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Int Number: " << value << "]";
}
void FltNumberExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Flt Number: " << value << "]";
}

void BoolConstExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[Bool constant: " << std::boolalpha << value << "]";
}

void StringConstExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[String constant: \"" << value << "\"]";
}

void VarDeclStmt::print(int indent)
{
  printIndent(indent);
  std::cout << "[Variable Decl: ";
  printList(vars, [](VarEnties::value_type &s) { return s.first; });
  std::cout << " : " << getTypeName(type) << ']' << std::endl;
}

void BinOpExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "BinOp: [" << std::endl;
  printIndent(indent + 1);
  lhs->print(indent + 1);
  std::cout << ' ' << Lexer::getTokenName(op) << ' ';
  rhs->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

llvm::Value* IntNumberExpr::codegen(Context &ctx)
{
    return llvm::ConstantInt::get(ctx.global.llvm_context, llvm::APInt(64, value, true));
}

llvm::Value* FltNumberExpr::codegen(Context &ctx)
{
    // TODO do something about this (double) cast, or change back no short double
    return llvm::ConstantFP::get(ctx.global.llvm_context, llvm::APFloat((double)value));
}

llvm::Value* BoolConstExpr::codegen(Context &ctx)
{
    return llvm::ConstantInt::get(ctx.global.llvm_context, llvm::APInt(1, value));
}
