project(compiler)

add_executable(compiler lexer.cpp main.cpp)

install(TARGETS compiler RUNTIME DESTINATION bin)
