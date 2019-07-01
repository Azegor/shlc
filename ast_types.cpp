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
#include "ast_types.h"

Type *getTypeFromToken(int tok)
{
  switch(tok) {
    case Token::id_int:
      return TypeRegistry::getBuiltinType(BuiltinTypeKind::int_t);
    case Token::id_flt:
      return TypeRegistry::getBuiltinType(BuiltinTypeKind::flt_t);
    case Token::id_chr:
      return TypeRegistry::getBuiltinType(BuiltinTypeKind::chr_t);
    case Token::id_boo:
      return TypeRegistry::getBuiltinType(BuiltinTypeKind::boo_t);
    case Token::id_str:
      return TypeRegistry::getBuiltinType(BuiltinTypeKind::str_t);
    case Token::id_vac:
      return TypeRegistry::getBuiltinType(BuiltinTypeKind::vac_t);
    default:
      throw CodeGenError("token '" + Lexer::getTokenName(tok) + "' is no type");
  }
}

void BuiltinType::print(int indent) {
    printIndent(indent);
    std::cout << getName();
}

const std::string& BuiltinType::getName() const
{
    return getTypeName(typeKind);
}

void ClassType::print(int indent) {
    printIndent(indent);
    std::cout << "cls " << name << " { ... }";
}

const ClassField *ClassType::getField(const std::string &name) const
{
    auto pos = fieldMap.find(name);
    if (pos == fieldMap.end()) {
        return nullptr;
    }
    return pos->second;
}

void OpaqueType::print(int indent) {
    printIndent(indent);
    std::cout << "cls " << name << " <opaque>";
}

BuiltinTypeArray::BuiltinTypeArray()
{
    // none & cls stay nullptr!
    for (int btc = (int)BuiltinTypeKind::none + 1; btc < (int)BuiltinTypeKind::cls_t; ++btc) {
        types[btc] = std::make_unique<BuiltinType>((BuiltinTypeKind)btc);
    }
}

TypeRegistry::TypeRegistry() : structureTypes()
{
}

const BuiltinTypeArray TypeRegistry::builtinTypes;
