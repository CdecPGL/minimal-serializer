[![License](https://img.shields.io/github/license/CdecPGL/minimal-serializer)](https://github.com/CdecPGL/minimal-serializer/blob/master/LICENSE)
[![CircleCI Buld and Test Status](https://circleci.com/gh/CdecPGL/minimal-serializer/tree/master.svg?style=shield)](https://circleci.com/gh/CdecPGL/minimal-serializer/tree/master)

# minimal-serializer

A C++ and C# binary serializer which serializes data without any extra data.

## minimal_serializer_cpp

minimal-serializer for C++.

### Requirements

- Boost Library 1.70 or higher
- nameof C++ (this is included in this repository)
- A compiler which is compatible with C++17(VC++15.7, g++7 or clang5)
- CMake 3.8 or higher (used for build)

#### Tested Compilers

- g++-9
- Clang-9
- MSVC 15.a

### Install

This is header only library.

***Copy all files in `minimal_serializer_cpp/include` directory to your project.***

To install via CMake, follow the below steps.

1. Install a compiler which is compatible with C++17 (VC++15.7, g++7 or clang5)
1. Install Boost Library 1.70 or higher (It is convenience to use vcpkg in Windows to install Boort Library)
1. Install CMake 3.8 or higher
1. Move to the root directory of minimal-serializer
1. `mkdir build`
1. `cmake ..`
1. `make install`
1. `make test`

## MinimalSerializerCSharp

minimal-serializer for C#.

## Requrements

- Nothing

### Install

Copy all files in `MinimalSerializerCSharp/Source` directory to your project.

## License

The codes in this repository are lisenced unfer the [MIT License](https://github.com/CdecPGL/minimal-serializer/blob/master/LICENSE).
