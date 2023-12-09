Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests value_ref_test.cpp)
target_link_libraries(
  tests
  PRIVATE
  r5rs_lib
  Catch2::Catch2WithMain
)
