#include <string>
#include <sstream>

#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/MCJIT.h> // necessary to link MCJIT (with static linking)

#include "codegenerator.h"
#include "compilationunit.h"

bool init_llvm()
{

  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

//   llvm::InitializeAllTargetInfos();
//   llvm::InitializeAllTargets();
//   llvm::InitializeAllTargetMCs();
//   llvm::InitializeAllAsmParsers();
//   llvm::InitializeAllAsmPrinters();
  return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {

    static bool initialized = init_llvm();
    (void)initialized;

    std::istringstream input(std::string(reinterpret_cast<const char*>(data), size));
    CodeGenerator codegenerator(CompilationUnit{"<fuzz_input>", &input});

    try
    {
        codegenerator.generateLLVM(0, true);
    }
    catch (CompileError &e)
    {
        // ignore
    }

  return 0;  // Non-zero return values are reserved for future use.
}
