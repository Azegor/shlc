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

#include "ast.h"

static void printIndent(int indent)
{
  static constexpr int width = 2;
  for (int i = 0; i < indent * width; ++i)
    std::cout << ' ';
}

template <typename L, typename Callback>
static void printList(L list, Callback callback)
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

void FunctionHeadExpr::print(int indent)
{
  printIndent(indent);
  std::cout << name << '(';
  printList(args, [](ArgVector::value_type e) {
    return Lexer::getTokenName(e.first) + " " + e.second;
  });
  std::cout << ')' << std::endl;
}

void FunctionExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "* Function [" << std::endl;
  head->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

void NormalFunctionExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "Normal Function [" << std::endl;
  head->print(indent + 1);
  printIndent(indent + 1);
  std::cout << '{' << std::endl;
  for (auto &stmt : body)
    stmt->print(indent + 2);
  printIndent(indent + 1);
  std::cout << '}' << std::endl;
  printIndent(indent);
  std::cout << ']' << std::endl;
}

void NativeFunctionExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "Native Function [" << std::endl;
  head->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

void FunctionDeclExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "Function Declaration [" << std::endl;
  head->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}
