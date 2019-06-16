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

#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <unordered_map>

enum class BuiltinTypeKind : int
{
  none,
  vac_t,
  int_t,
  flt_t,
  chr_t,
  boo_t,
  str_t,
  cls_t,
};

namespace types
{
using int_t = long long;
// using flt_t = long double;
using flt_t = double;
using chr_t = signed char;
using boo_t = bool;
// using str_t = std::string;
using str_t = char const *;
}

class Type;
class ClassType;
class GlobalContext;

namespace llvm {
class Type;
class PointerType;
class DIType;
} // namespace llvm

const std::string &getTypeName(BuiltinTypeKind t);
const std::string &getMangleName(BuiltinTypeKind t);
std::string getMangleName(Type *t);

class LLVMTypeRegistry
{
public:
    LLVMTypeRegistry(GlobalContext &gl_ctx);

    llvm::Type *getType(Type *t);
    llvm::PointerType *getClassType(ClassType *t);
    llvm::Type *getBuiltinType(BuiltinTypeKind tk);

    llvm::DIType *getDIType(Type *t);
    llvm::DIType *getDIBuiltinType(BuiltinTypeKind tk);

    llvm::PointerType *getVoidPointerType() const { return voidPointerType; }

private:
    GlobalContext &gl_ctx;
    std::unordered_map<ClassType*, llvm::PointerType*> classTypeMap;
    llvm::PointerType *voidPointerType;
    llvm::DIType *diBuiltinTypes[(int)BuiltinTypeKind::str_t + 1];
    bool diTypesInitialized = false;

    llvm::PointerType *createLLVMClassType(ClassType *ct);
    void createDIBuiltinTypes();
};

#endif // TYPE_H
