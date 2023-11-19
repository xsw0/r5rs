include_directories(${CMAKE_SOURCE_DIR})

set(CPPS
  typer/String.cpp

  parse/Token.cpp
  parse/Parse.cpp
  parse/Lex.cpp

  ast/Expressions.cpp

  interpret/GetRef.cpp
  interpret/GC.cpp
  interpret/Env.cpp
  interpret/Interpreter.cpp
)

add_library(r5rs_lib STATIC ${CPPS})
target_include_directories(r5rs_lib PUBLIC . ./typer ./parse ./ast ./interpret)
