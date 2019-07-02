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

#ifndef ASTTYPES_H
#define ASTTYPES_H

#include "ast_base.h"
#include <unordered_map>

Type *getTypeFromToken(int tok);

class Type : public AstNode
{
public:
    Type(SourceLocation loc, BuiltinTypeKind tk) : AstNode(loc), typeKind(tk) {}
    virtual ~Type() {}
    virtual const std::string& getName() const = 0;

    BuiltinTypeKind getKind() const { return typeKind; }

protected:
    BuiltinTypeKind typeKind;
};

class BuiltinType : public Type
{
public:
    BuiltinType(BuiltinTypeKind tk) : Type({}, tk) {}
    virtual ~BuiltinType() {}

    virtual void print(int indent = 0) override;
    virtual const std::string& getName() const override;
};

struct ClassField
{
    ClassField(std::string name, Type *type, unsigned idx) : name(std::move(name)), type(type), index(idx) {}

    std::string name;
    Type *type;
    unsigned index;
    llvm::Type *llvmType = nullptr;
};

class StructureType : public Type
{
    friend class LLVMTypeRegistry;
public:
    StructureType(SourceLocation loc, BuiltinTypeKind btk, std::string name)
        : Type(loc, btk),
          name(std::move(name))
    {
    }

    virtual const std::string& getName() const override { return name; }

protected:
  std::string name;
};

using StructureTypePtr = std::unique_ptr<StructureType>;

using ClassFieldVec = std::vector<ClassField>;

class ClassType : public StructureType
{
    friend class LLVMTypeRegistry;
public:
    ClassType(SourceLocation loc, std::string name, ClassFieldVec classFields)
        : StructureType(loc, BuiltinTypeKind::cls_t, std::move(name)),
          fields(std::move(classFields))
    {
        for (auto &f : fields) {
            fieldMap.emplace(f.name, &f);
        }
    }

    virtual void print(int indent = 0) override;
    virtual const std::string& getName() const override { return name; }
    const ClassField *getField(const std::string &name) const;
private:
    ClassFieldVec fields;
    std::unordered_map<std::string, ClassField*> fieldMap;
};

using ClassTypePtr = std::unique_ptr<ClassType>;

class OpaqueType : public StructureType
{
public:
    OpaqueType(SourceLocation loc, std::string name)
        : StructureType(loc, BuiltinTypeKind::opq_t, std::move(name))
    {}
    virtual void print(int indent = 0) override;
};

using OpaqueTypePtr = std::unique_ptr<OpaqueType>;

struct BuiltinTypeArray
{
    BuiltinTypeArray();
    static const int numBuiltinTypes = (int)BuiltinTypeKind::cls_t+1;
    std::unique_ptr<BuiltinType> types[numBuiltinTypes];
};

class TypeRegistry
{
private:
    std::unordered_map<std::string, StructureTypePtr> structureTypes;
    static const BuiltinTypeArray builtinTypes;

public:
    TypeRegistry();
    static BuiltinType *getBuiltinType(BuiltinTypeKind tk) {
      return builtinTypes.types[(int)tk].get();
    }
    static BuiltinType *getVoidType() {
      return builtinTypes.types[(int)BuiltinTypeKind::vac_t].get();
    }

    void registerStructureType(StructureTypePtr st) {
      std::string name = st->getName(); // copy
      structureTypes.emplace(std::move(name), std::move(st));
    }
    Type *findStructureType(const std::string &name) {
        return structureTypes[name].get(); // default should return nullptr
    }
};

#endif // ASTTYPES_H
