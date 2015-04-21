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

#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>

#include <boost/program_options.hpp>

#include "parser.h"
#include "codegenerator.h"

namespace po = boost::program_options;

class Compiler
{
  int optLevel;
  std::string inFile, outFile;
  bool runCode = false;

public:
  Compiler(int argc, char* argv[]) { parseArguments(argc, argv); }

  void parseArguments(int argc, char* argv[]);

  int run()
  {
    CodeGenerator codegen(inFile);
    codegen.generateCode(optLevel);
    if (runCode)
      codegen.runFunction("main");
    else
      codegen.writeCodeToFile(outFile);
    return 0;
  }
};

#endif // COMPILER_H
