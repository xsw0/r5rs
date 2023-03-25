set(TESTS
    GCTest
    GCTestMuti
)

foreach(TEST ${TESTS})
    add_executable(${TEST} ${TEST}.cpp ${CPPS})
    target_link_libraries(${TEST} r5rs_lib)
    add_test(NAME ${TEST} COMMAND ${TEST})
endforeach()
