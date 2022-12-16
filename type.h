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

#include <llvm/IR/DerivedTypes.h>
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
  opq_t,
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
class OpaqueType;
class GlobalContext;

namespace llvm {
class Type;
class PointerType;
class DIType;
class Function;
} // namespace llvm

const std::string &getTypeName(BuiltinTypeKind t);
const std::string &getMangleName(BuiltinTypeKind t);
std::string getMangleName(Type *t);
std::string getMangleDestructorName(ClassType *t);

class LLVMTypeRegistry
{
public:
    LLVMTypeRegistry(GlobalContext &gl_ctx);

    llvm::Type *getType(Type *t);
    llvm::StructType *getClassType(ClassType *ct);
    llvm::PointerType *getClassPtrType(ClassType *ct);
    llvm::Function *getClassDestructor(ClassType *ct);
    llvm::PointerType *getOpaqueType(OpaqueType *ot);
    llvm::Type *getBuiltinType(BuiltinTypeKind tk) const;

    llvm::DIType *getDIType(Type *t);
    llvm::DIType *getDIBuiltinType(BuiltinTypeKind tk);
    llvm::DIType *getDIClassType(ClassType *ct);
    llvm::DIType *getDIOpaqueType(OpaqueType *ot);

    llvm::PointerType *getVoidPointerType() const { return voidPointerType; }
    llvm::Type *getRefCounterType() const { return getBuiltinType(BuiltinTypeKind::int_t); }
    llvm::PointerType *getRefCounterPointerType() const { return refCounterPtrType; }
    llvm::FunctionType *getDestructorType() const { return destrType; }
    llvm::PointerType *getDestructorPointerType() const { return destrPtrType; }

private:
    GlobalContext &gl_ctx;
    std::unordered_map<ClassType*, llvm::PointerType*> classTypeMap;
    std::unordered_map<OpaqueType*, llvm::PointerType*> opaqueTypeMap;
    llvm::PointerType *voidPointerType;
    llvm::PointerType *refCounterPtrType;
    llvm::FunctionType *destrType;
    llvm::PointerType *destrPtrType;
    llvm::DIType *diBuiltinTypes[(int)BuiltinTypeKind::str_t + 1];
    std::unordered_map<ClassType*, llvm::DIType*> diClassTypes;
    std::unordered_map<OpaqueType*, llvm::DIType*> diOpaqueTypes;
    bool diTypesInitialized = false;

    llvm::StructType *createLLVMClassType(ClassType *ct);
    llvm::Function *createLLVMClassDestructor(ClassType *ct);
    void createLLVMClassTypeAndDestructor(ClassType *ct);
    llvm::PointerType *createLLVMOpaqueType(OpaqueType *ct);
    void createDIBuiltinTypes();
    llvm::DIType *createDIClassType(ClassType *ct);
    llvm::DIType *createDIOpaqueType(OpaqueType *ot);
};

#endif // TYPE_H
