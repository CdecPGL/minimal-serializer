cmake_minimum_required (VERSION 3.8)
enable_testing()

file(GLOB_RECURSE source_files "*.cpp")
add_executable (minimal_serializer_cpp_test ${source_files})

# Boost Library
find_package(Boost REQUIRED)
file(GLOB_RECURSE boost_library_paths "${Boost_LIBRARY_DIRS}/*boost*")
# to avoid warnings, use SYSTEM (-isystem option of gcc)
target_include_directories(minimal_serializer_cpp_test SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(minimal_serializer_cpp_test ${boost_library_paths})

target_link_libraries(minimal_serializer_cpp_test minimal_serializer_cpp)

# Test
add_test(
  NAME minimal_serializer_cpp_test
  COMMAND $<TARGET_FILE:minimal_serializer_cpp_test>
#  CONFIGURATIONS Release
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/tmp
)
