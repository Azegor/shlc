cmake_minimum_required(VERSION 2.6)

# add_definitions( -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS )

project(compiler)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -fsanitize=address -fPIC")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(compiler stdlib.cpp ast_functions.cpp ast_expressions.cpp ast_statements.cpp context.cpp ast_base.cpp parser.cpp lexer.cpp codegen.cpp type.cpp codegenerator.cpp compiler.cpp main.cpp)
add_library(stdshl stdlib.cpp)

find_package( Boost COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

llvm_map_components_to_libnames(llvm_libs core mcjit native jit ipo)
target_link_libraries(compiler ${llvm_libs} ${Boost_LIBRARIES})

install(TARGETS compiler RUNTIME DESTINATION bin)
