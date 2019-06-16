cmake_minimum_required(VERSION 2.6)

# add_definitions( -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS )

project(shlc)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_executable(shlc stdlib.cpp ast_functions.cpp ast_expressions.cpp ast_statements.cpp ast_types.cpp context.cpp ast_base.cpp parser.cpp lexer.cpp codegen.cpp type.cpp codegenerator.cpp compiler.cpp main.cpp)
add_executable(fuzzshl stdlib.cpp ast_functions.cpp ast_expressions.cpp ast_statements.cpp ast_types.cpp context.cpp ast_base.cpp parser.cpp lexer.cpp codegen.cpp type.cpp codegenerator.cpp fuzz.cpp)

set_target_properties(shlc PROPERTIES COMPILE_FLAGS "-fsanitize=address -fPIC")
set_target_properties(fuzzshl PROPERTIES CMAKE_CXX_COMPILER "clang++")
set_target_properties(fuzzshl PROPERTIES COMPILE_FLAGS "-fsanitize=fuzzer,address -fPIC")
set_target_properties(fuzzshl PROPERTIES LINK_FLAGS "-fsanitize=fuzzer,address -fPIC")

llvm_config(shlc USE_SHARED core mcjit native ipo support executionengine)
llvm_config(fuzzshl USE_SHARED core mcjit native ipo support executionengine)

add_library(stdshl stdlib.cpp)

target_link_libraries(shlc PRIVATE ${Boost_LIBRARIES} stdshl)
target_link_libraries(fuzzshl PRIVATE ${Boost_LIBRARIES} stdshl)

install(TARGETS shlc RUNTIME DESTINATION bin)
