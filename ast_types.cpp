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

void BuiltinType::print(int indent) {
    printIndent(indent);
    std::cout << getTypeName(typeKind);
}

void ClassType::print(int indent) {
    printIndent(indent);
    std::cout << "cls " << name << " { ... }";
}

TypeRegistry::TypeRegistry() :
    builtinTypes(), classTypes()
{
    for (int btc = (int)BuiltinTypeKind::none; btc <= (int)BuiltinTypeKind::vac_t; ++btc) {
        builtinTypes[btc] = std::make_unique<BuiltinType>((BuiltinTypeKind)btc);
    }
}
