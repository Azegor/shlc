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

using ClassFieldVec = std::vector<ClassField>;

class ClassType : public Type
{
    friend class LLVMTypeRegistry;
public:
    ClassType(SourceLocation loc, std::string name, ClassFieldVec classFields)
        : Type(loc, BuiltinTypeKind::cls_t),
          name(std::move(name)),
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
    std::string name;
    ClassFieldVec fields;
    std::unordered_map<std::string, ClassField*> fieldMap;
};

using ClassTypePtr = std::unique_ptr<ClassType>;

struct BuiltinTypeArray
{
    BuiltinTypeArray();
    static const int numBuiltinTypes = (int)BuiltinTypeKind::cls_t+1;
    std::unique_ptr<BuiltinType> types[numBuiltinTypes];
};

class TypeRegistry
{
private:
    std::unordered_map<std::string, std::unique_ptr<ClassType>> classTypes;
    static const BuiltinTypeArray builtinTypes;

public:
    TypeRegistry();
    static BuiltinType *getBuiltinType(BuiltinTypeKind tk) {
      return builtinTypes.types[(int)tk].get();
    }
    static BuiltinType *getVoidType() {
      return builtinTypes.types[(int)BuiltinTypeKind::vac_t].get();
    }

    void registerClassType(std::unique_ptr<ClassType> ct) {
      std::string name = ct->getName(); // copy
      classTypes.emplace(std::move(name), std::move(ct));
    }
    Type *findClassType(const std::string &name) {
        return classTypes[name].get(); // default should return nullptr
    }
};

#endif // ASTTYPES_H
