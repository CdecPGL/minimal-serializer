cmake_minimum_required (VERSION 3.8)

# find source files
set (includes "include")
file(GLOB_RECURSE source_files "*.cpp")

if (${is_header_only})
  add_library (minimal_serializer_cpp INTERFACE)

  # Headers in this project
  target_include_directories(minimal_serializer_cpp INTERFACE ${includes})
else()
  add_library (minimal_serializer_cpp ${source_files})

  # Headers in this project
  target_include_directories(minimal_serializer_cpp PUBLIC ${includes})

  # Boost Library
  find_package(Boost REQUIRED)
  file(GLOB_RECURSE boost_library_paths "${Boost_LIBRARY_DIRS}/*boost*")
  # to avoid warnings, use SYSTEM (-isystem option of gcc)
  target_include_directories(minimal_serializer_cpp SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
  target_link_libraries(minimal_serializer_cpp ${boost_library_paths})

  # Avoid to error of Boost Library 1.70 for MSVC
  if (MSVC)
    add_definitions(-D_WIN32_WINNT=0x0601)
  endif()
endif()

# Install library
install (TARGETS minimal_serializer_cpp LIBRARY DESTINATION lib)
install (FILES includes DESTINATION include)
