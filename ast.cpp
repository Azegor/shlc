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

#include <unordered_map>

std::string getTypeName(Type t)
{
  static std::unordered_map<int, std::string> names = {
      {Type::none, "none"}, // *
      {Type::inferred, "inferred"},
      {Type::int_t, "int_t"},
      {Type::flt_t, "flt_t"},
      {Type::chr_t, "chr_t"},
      {Type::boo_t, "boo_t"},
      {Type::str_t, "str_t"},
      {Type::vac_t, "vac_t"}};
  return names[t];
}

Type getTypeFromToken(int tok)
{
  static std::unordered_map<int, Type> types = {
      {Token::id_int, Type::int_t}, // *
      {Token::id_flt, Type::flt_t},
      {Token::id_chr, Type::chr_t},
      {Token::id_boo, Type::boo_t},
      {Token::id_str, Type::str_t},
      {Token::id_vac, Type::vac_t}};
  return types[tok];
}

static void printIndent(int indent)
{
  static constexpr int width = 2;
  for (int i = 0; i < indent * width; ++i)
    std::cout << ' ';
}

template <typename L, typename Callback>
static void printList(L &list, Callback callback)
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

void FunctionHead::print(int indent)
{
  printIndent(indent);
  std::cout << name << '(';
  printList(args, [](ArgVector::value_type &e) {
    return Lexer::getTokenName(e.first) + " " + e.second;
  });
  std::cout << ") : " << getTypeName(retType) << std::endl;
}

void Function::print(int indent)
{
  printIndent(indent);
  std::cout << "* Function [" << std::endl;
  head->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

void NormalFunction::print(int indent)
{
  printIndent(indent);
  std::cout << "Normal Function [" << std::endl;
  head->print(indent + 1);
  body->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

void NativeFunction::print(int indent)
{
  printIndent(indent);
  std::cout << "Native Function [" << std::endl;
  head->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

void FunctionDecl::print(int indent)
{
  printIndent(indent);
  std::cout << "Function Declaration [" << std::endl;
  head->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

// -----------------------------------------------------------------------------

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
  std::cout << "[Bool constant: " << value << "]";
}

void StringConstExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "[String constant: " << value << "]";
}

void VarDeclExpr::print(int indent)
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

void ReturnExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "Return: [" << std::endl;
  expr->print(indent + 1);
  printIndent(indent);
  std::cout << ']' << std::endl;
}

void BlockStmt::print(int indent)
{
  printIndent(indent);
  std::cout << '{' << std::endl;
  for (auto &stmt : block) {
    if (stmt)
      stmt->print(indent + 1);
    std::cout << std::endl;
  }
  printIndent(indent);
  std::cout << '}' << std::endl;
}

void CastExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "Cast: [" << std::endl;
  expr->print(indent + 1);
  printIndent(indent);
  std::cout << "] to " << getTypeName(newType) << std::endl;
}

void IfExpr::print(int indent)
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
  std::cout  << std::endl;
}

void WhileExpr::print(int indent)
{
  printIndent(indent);
  std::cout << "While: (";
  cond->print();
  std::cout << ")" << std::endl;
  body->print(indent);
  printIndent(indent);
  std::cout  << std::endl;
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
