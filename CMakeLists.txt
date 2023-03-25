cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 20)
project(r5rs VERSION 0.1.0)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include_directories(${CMAKE_SOURCE_DIR})

set(CPPS
    ${CMAKE_SOURCE_DIR}/String.cpp
    ${CMAKE_SOURCE_DIR}/GetRef.cpp
    ${CMAKE_SOURCE_DIR}/GC.cpp
)

add_library(r5rs_lib STATIC ${CPPS})

add_executable(r5rs main.cpp)
target_link_libraries(r5rs r5rs_lib)

add_subdirectory(test)