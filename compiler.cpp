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

#include "compiler.h"

void Compiler::parseArguments(int argc, char* argv[])
{
  po::positional_options_description posArgs;
  posArgs.add("input-file", 1);
  po::options_description desc("Options");
  desc.add_options()                      //
    ("help,h", "print this help message") //
    (",O", po::value<int>(&optLevel)->default_value(0),
     "optimization level") //
    ("emit-debug-info,g", "emit debug information") //
    ("input-file", po::value<std::string>(&inFile),
     "input file") //
    ("output-file,o", po::value<std::string>(&outFile),
     "output file") //
    ("run", "run main method of generated code");

  po::variables_map vm;
  try
  {
    po::store(po::command_line_parser(argc, argv)
                .options(desc)
                .positional(posArgs)
                .run(),
              vm);

    if (vm.count("help")) {
      std::cout << "Usage: " << argv[0] << " [options] file..." << std::endl;
      std::cout << desc << std::endl << std::endl;
      //         rad::OptionPrinter::printStandardAppDesc(appName,
      //                                                  std::cout,
      //                                                  desc,
      //                                                  &positionalOptions);
      exit(0);
    }

    po::notify(vm);

    if (!vm.count("input-file")) {
      std::cerr << "error: no input files" << std::endl;
      exit(1);
    }
    if (!vm.count("output-file")) {
      outFile = inFile + ".ll";
    }
    if (vm.count("emit-debug-info")) {
       emitDebugInfo = true;
    }
    if (vm.count("run")) {
      runCode = true;
    }
  }
  catch (boost::program_options::required_option& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    exit(1);
  }
  catch (boost::program_options::error& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    exit(1);
  }
}
