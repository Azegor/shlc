cmake_minimum_required(VERSION 2.6)

# add_definitions( -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS )

project(compiler)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_COMPILER clang++)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -fsanitize=address -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fsanitize=address -fPIC -fsanitize-coverage=edge")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
llvm_map_components_to_libnames(LLVM_LIBS core mcjit native ipo support executionengine)
message(STATUS "Linking the following LLVM libs: ${LLVM_LIBS}")

link_directories(/home/tpriesner/Programming/shlc/libFuzzer)

add_executable(compiler stdlib.cpp ast_functions.cpp ast_expressions.cpp ast_statements.cpp context.cpp ast_base.cpp parser.cpp lexer.cpp codegen.cpp type.cpp codegenerator.cpp compiler.cpp main.cpp)
add_executable(fuzzshl stdlib.cpp ast_functions.cpp ast_expressions.cpp ast_statements.cpp context.cpp ast_base.cpp parser.cpp lexer.cpp codegen.cpp type.cpp codegenerator.cpp fuzz.cpp)

# add_executable(fuzzshl stdlib.cpp ast_functions.cpp ast_expressions.cpp ast_statements.cpp context.cpp ast_base.cpp parser.cpp lexer.cpp codegen.cpp type.cpp codegenerator.cpp fuzz.cpp)

# set_target_properties(fuzzshl PROPERTIES COMPILE_FLAGS "-std=c++14 -fsanitize=address -fPIC -fsanitize-coverage=edge")
# set_target_properties(fuzzshl PROPERTIES CMAKE_CXX_COMPILER "clang++")

add_library(stdshl stdlib.cpp)
target_link_libraries(compiler ${Boost_LIBRARIES} ${LLVM_LIBS})
target_link_libraries(fuzzshl ${Boost_LIBRARIES} ${LLVM_LIBS} Fuzzer)


# target_link_libraries(fuzzshl Fuzzer pthread asan ${LLVM_LIBS})

install(TARGETS compiler RUNTIME DESTINATION bin)
