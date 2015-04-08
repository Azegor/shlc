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

void printIndent(int indent)
{
  static constexpr int width = 2;
  for (int i = 0; i < indent * width; ++i)
    std::cout << ' ';
}

// -----------------------------------------------------------------------------

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
