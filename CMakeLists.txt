cmake_minimum_required(VERSION 3.13.4)
project(shlc LANGUAGES CXX C)

set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR ON)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

find_package(MLIR REQUIRED CONFIG)

message(STATUS "Found MLIR ${MLIR_PACKAGE_VERSION}")
message(STATUS "Using MLIRConfig.cmake in: ${MLIR_DIR}")

set(CMAKE_CXX_COMPILER clang++)

set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/bin)
set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/lib)
set(MLIR_BINARY_DIR ${CMAKE_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(TableGen)
include(AddLLVM)
include(AddMLIR)
include(HandleLLVMOptions)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${MLIR_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_BINARY_DIR}/include)

link_directories(${LLVM_BUILD_LIBRARY_DIR})

add_definitions(${LLVM_DEFINITIONS})
add_definitions(${MLIR_DEFINITIONS})

add_subdirectory(include)
add_subdirectory(lib)

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# add_compile_definitions(STD_LIBRARY_PATH="${CMAKE_SOURCE_DIR}/include")

add_executable(shlc stdlib.cpp ast_functions.cpp ast_expressions.cpp ast_statements.cpp ast_types.cpp context.cpp ast_base.cpp parser.cpp lexer.cpp codegen.cpp cleanup.cpp type.cpp codegenerator.cpp compiler.cpp main.cpp)
add_executable(fuzzshl stdlib.cpp ast_functions.cpp ast_expressions.cpp ast_statements.cpp ast_types.cpp context.cpp ast_base.cpp parser.cpp lexer.cpp codegen.cpp cleanup.cpp type.cpp codegenerator.cpp fuzz.cpp)

target_compile_options(shlc PRIVATE -fsanitize=address -fPIC)
target_link_options(shlc PRIVATE -fsanitize=address -fPIC)

# set_target_properties(fuzzshl PROPERTIES CMAKE_CXX_COMPILER "clang++")
target_compile_options(fuzzshl PRIVATE -fsanitize=fuzzer,address -fPIC)
target_link_options(fuzzshl PRIVATE -fsanitize=fuzzer,address -fPIC)

llvm_config(shlc USE_SHARED core mcjit native ipo support executionengine)
llvm_config(fuzzshl USE_SHARED core mcjit native ipo support executionengine)

add_library(stdshl stdlib.cpp)

target_link_libraries(shlc PRIVATE ${Boost_LIBRARIES} stdshl LLVMSupport)
target_link_libraries(fuzzshl PRIVATE ${Boost_LIBRARIES} stdshl LLVMSupport)

install(TARGETS shlc RUNTIME DESTINATION bin)
