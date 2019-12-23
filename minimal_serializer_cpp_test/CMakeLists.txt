cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE source_files "*.cpp")
# to avoid link error in clang
add_definitions(-DBOOST_TEST_DYN_LINK)
add_executable (minimal_serializer_cpp_test ${source_files})

# Boost Library
find_package(Boost 1.67.0 REQUIRED COMPONENTS unit_test_framework)
# to avoid warnings, use SYSTEM (-isystem option of gcc)
target_include_directories(minimal_serializer_cpp_test SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(minimal_serializer_cpp_test ${Boost_LIBRARIES})

target_link_libraries(minimal_serializer_cpp_test minimal_serializer_cpp)

# Test
add_test(
  NAME minimal_serializer_cpp_test
  COMMAND $<TARGET_FILE:minimal_serializer_cpp_test>
)
