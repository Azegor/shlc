/*
 * Copyright 2015 <copyright holder> <email>
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

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include "parser.h"
#include "context.h"

class CodeGenerator
{
  std::string fileName;
  Parser parser;
  GlobalContext gl_ctx;
  llvm::Function* mainFn = nullptr;

public:
  CodeGenerator(std::string fileName) : fileName(fileName) {}

  void generateCode(int optLevel);

  void writeCodeToFile(const std::string& fileName);

  void runFunction(std::string name);
};

#endif // CODEGENERATOR_H
