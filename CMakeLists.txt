cmake_minimum_required (VERSION 3.8)
enable_testing()

project ("minimal-serializer" CXX)

set(is_header_only true)

# Set build type
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Macros
macro(install_symlink filepath sympath)
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${filepath} ${sympath})")
    install(CODE "message(\"-- Created symlink: ${sympath} -> ${filepath}\")")
endmacro(install_symlink)

# C++ configurations
if(NOT "${CMAKE_CXX_STANDARD}")
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_STANDARD LESS 17)
  message(FATAL_ERROR "C++ version must be 17 or higher.")
endif()

# Compiler configurations
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /source-charset:utf-8")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
elseif((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
else()
  message(FATAL_ERROR "Not supported compiler. (${CMAKE_CXX_COMPILER_ID})")
endif()

# vckpg
if(Windows)
  if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  else()
    message(FATAL_ERROR "vcpkg is required.")
  endif()
endif()

# Subprojects
add_subdirectory ("minimal_serializer_cpp")
add_subdirectory ("minimal_serializer_cpp_test")
