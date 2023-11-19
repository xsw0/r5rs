include_directories(${CMAKE_SOURCE_DIR})

set(CPPS
  String.cpp
  Token.cpp
  GetRef.cpp
  GC.cpp
  Parse.cpp
  Lex.cpp
  Env.cpp
  Expressions.cpp
  Interpreter.cpp
)

add_library(r5rs_lib STATIC ${CPPS})
target_include_directories(r5rs_lib PUBLIC .)
