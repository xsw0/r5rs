add_library(test_lib Record.cpp)

set(
    TESTS
    GCTest
    # GCTestMuti
    parseToken
    value_ref_test
)

foreach (TEST ${TESTS})
    add_executable(${TEST} ${TEST}.cpp)
    target_include_directories(${TEST} PUBLIC ${CMAKE_SOURCE_DIR})
    target_link_libraries(${TEST} test_lib r5rs_lib)
    add_test(NAME ${TEST} COMMAND ${TEST})
endforeach ()


# set(
#     GTESTS
#     t
# )

# foreach (TEST ${GTESTS})
#     add_executable(${TEST} ${TEST}.cpp)
#     target_link_libraries(${TEST} r5rs_lib GTest::gtest_main)
#     gtest_discover_tests(${TEST})
# endforeach ()
