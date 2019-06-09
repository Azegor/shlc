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
#include <vector>

class Type : public AstNode
{
public:
    Type(SourceLocation loc) : AstNode(loc) {}
    virtual ~Type() {}
};

class BuiltinType : public Type
{
public:
    BuiltinType(BuiltinTypeKind tk) : Type({}), typeKind(tk) {}
    virtual ~BuiltinType() {}

    virtual void print(int indent = 0) override;

private:
    BuiltinTypeKind typeKind;
};

struct ClassField
{
    ClassField(std::string name, Type *type) : name(std::move(name)), type(type) {}

    std::string name;
    Type *type;
};

using ClassFieldVec = std::vector<ClassField>;

class ClassType : public Type
{
public:
    ClassType(SourceLocation loc, std::string name, ClassFieldVec fields) : Type(loc), name(std::move(name)), fields(std::move(fields)) {}

    virtual void print(int indent = 0) override;
private:
    std::string name;
    ClassFieldVec fields;
};

using ClassTypePtr = std::unique_ptr<ClassType>;

class TypeRegistry
{
private:
    static const int numBuiltinTypes = (int)BuiltinTypeKind::vac_t+1;
    std::unique_ptr<BuiltinType> builtinTypes[numBuiltinTypes];
    std::vector<std::unique_ptr<ClassType>> classTypes;

public:
    TypeRegistry();
    BuiltinType *getBuiltinType(BuiltinTypeKind tk) {
      return builtinTypes[(int)tk].get();
    }
    void registerClassType(std::unique_ptr<ClassType> ct) {
      classTypes.push_back(std::move(ct));
    }
};

#endif // ASTTYPES_H
