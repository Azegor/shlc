project(compiler)

add_executable(compiler ast.cpp parser.cpp lexer.cpp main.cpp)

install(TARGETS compiler RUNTIME DESTINATION bin)
