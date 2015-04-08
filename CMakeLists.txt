project(compiler)

add_executable(compiler stdlib.cpp ast_functions.cpp ast_expressions.cpp ast_statements.cpp context.cpp ast.cpp parser.cpp lexer.cpp main.cpp)

install(TARGETS compiler RUNTIME DESTINATION bin)
