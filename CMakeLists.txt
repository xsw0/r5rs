cmake_minimum_required(VERSION 3.0.0)

cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 23)
project(r5rs VERSION 0.1.0)

include(CTest)
include(GoogleTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(${CMAKE_SOURCE_DIR})

set(CPPS
        ${CMAKE_SOURCE_DIR}/String.cpp
        ${CMAKE_SOURCE_DIR}/Token.cpp
        ${CMAKE_SOURCE_DIR}/GetRef.cpp
        ${CMAKE_SOURCE_DIR}/GC.cpp
        ${CMAKE_SOURCE_DIR}/Parse.cpp
        ${CMAKE_SOURCE_DIR}/Lex.cpp
        ${CMAKE_SOURCE_DIR}/Expressions.cpp
        )

add_library(r5rs_lib STATIC ${CPPS})

add_executable(r5rs main.cpp)
target_link_libraries(r5rs r5rs_lib)

add_subdirectory(test)
