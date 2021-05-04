[![Release](https://img.shields.io/github/v/release/CdecPGL/minimal-serializer?include_prereleases&sort=semver)](https://github.com/CdecPGL/minimal-serializer/releases)
[![License](https://img.shields.io/github/license/CdecPGL/minimal-serializer)](https://github.com/CdecPGL/minimal-serializer/blob/master/LICENSE)
[![CircleCI Buld and Test Status](https://circleci.com/gh/CdecPGL/minimal-serializer/tree/master.svg?style=shield)](https://circleci.com/gh/CdecPGL/minimal-serializer/tree/master)

# minimal-serializer

A C++ and C# binary serializer which serializes data without any extra data.

- Minimal size without metadata
- Extremely fast in C++ version!
- Portable between C++ and C#
- Portable between different machines
- Serialized size is always same and predictable if type is same
- Header only in C++ version

Comparision of binary serializers for C++ and C#.

|Library|C++|C#|Minimal Size (No Metadata)|Portable|Supported Type|
|:---|:---|:---|:---|:---|:---|
|Minimal Serializer|Yes|Yes|Yes|Yes|Fixed Size Types Only|
|Boost Serializer|Yes|No|No|No|Almost All Types|
|Cereal|Yes|No|Yes|Yes|Almost All Types|
|C# Built-in Serializer (BinaryFormatter)|No|Yes|No|No|Almost All Types|

## Example

See [Serializer Usage](documents/serializer.md) to know usage of minimal-serializer.

### C++

```cpp
#include "minimal_serializer/serializer.hpp"

struct Data {
    uint32_t value;
    std::array<int64_t, 16> array;
    minimal_serializer::fixed_string<32> string;

    // Intrusive definition of serialize targets
    using serialize_targets = minimal_serializer::serialize_target_container<&Data::value, &Data::array, &Data::string>;
}

// Not intrusive definition is also available
namespace minimal_serializer {
    template<>
    struct serialize_targets<Data> {
        using type = minimal_serializer::serialize_target_container<&Data::value, &Data::array, &Data::string>;
    }
}

int main(){
    // Get serialized size. The size is calculated in compile time
    auto size = minimal_serializer::serialized_size_v<Data>;
    // Serialize to minimal_serializer::serialized_data<T>, which is std::array<uint8_t, minimal_serializer::serialized_size_v<T>>
    Data data;
    auto byte_array = minimal_serializer::serialize(data);
    // Deserialize from minimal_serializer::serialized_data
    minimal_serializer::serialize(data, byte_array);
}
```

### C#

```csharp
using System;

// By Serializable attribute, the struct and class become serializable with minimal-serializer
// Add [StructLayout(LayoutKind.Sequential)] if you want to serialize class
[Serializable]
class Data {
    public uint value;
    // To inform the size to serializer, FixedLength attribute is required
    public [FixedLength(16)] long[] array;
    public [FixedLength(32)] string string;
}

public class Main {
    public static void Main(){
        // Get serialized size. The size is always same is typpe is same
        var size = CdecPGL.MinimalSerializerSerializer.GetSerializedSize<Data>();
        // Serialize to byte[]
        var data = new Data();
        var byteArray = CdecPGL.MinimalSerializer.Serializer.Serialize(data);
        // Deserialize from byte[]
        data = CdecPGL.MinimalSerializer.Serializer.Deserialize(byteArray);
    }
}
```

## Requirement

### C++

- A compiler which is compatible with C++17 (VC++15.7, g++7 or clang5)
- Boost Library
  - Minimal Support: 1.67 or higher
  - Float Type Support (Beta): 1.74 or higher
- nameof C++ (this is included in this repository)

#### Tested Compilers

- g++ 8.3.0
- Clang 8.0.0
- MSVC 14.6.2

### C#

- .NET FrameWork or .NET Core wich is compatible with .NET Standard 2.0

## Install

### C++

This is header only library.

Copy `minimal_serializer_cpp/include` directory to your project.

### C#

Copy all files in `MinimalSerializerCSharp/Source` directory to your project.

## Run Tests

### C++

#### Visual Studio

1. Open `minimal-serializer.sln` with Visual Studio
1. Build `minimal_serializer_cpp_test` project
1. Run the tests with test runner of Visual Studio

#### CMake

1. Install a compiler which is compatible with C++17
1. Install Boost Library 1.69 or higher (It is convenience to use vcpkg in Windows to install Boort Library)
1. Install CMake 3.8 or higher
1. Move to the root directory of minimal-serializer
1. `mkdir build`
1. `cmake ..`
1. `make install`
1. `ctest`

Instead of installing required packages, you can use docker container "`cdec/minimal-serializer:dev-alpine`" to build and run tests like below command.

```bash
docker run -v <A Path of Minimal Serialier Repogitory>:/minimal-serializer -itd cdec/minimal-serializer:dev-alpine
```

`cdec/minimal-serializer:dev-alpine` is built from `docker/dev-alpine/Dockerfile` in this repository.
You can build a docker image yourself with `docker/dev-alpine/Dockerfile` if you need.

### C#

1. Open `minimal-serializer.sln` with Visual Studio
1. Build `MinimalSerializerCSharpTest` project
1. Run the tests with test runner of Visual Studio

## Contribution

Please submit issues ([https://github.com/CdecPGL/minimal-serializer/issues](https://github.com/CdecPGL/minimal-serializer/issues)) or create Pull Requests ([https://github.com/CdecPGL/minimal-serializer/pulls](https://github.com/CdecPGL/minimal-serializer/pulls)) if you find bugs or want to propose new features.

You can create pull requests by below steps.

1. Fork ([https://github.com/CdecPGL/minimal-serializer/fork](https://github.com/CdecPGL/minimal-serializer/fork))
1. Create a feature branch
1. Commit your changes
1. Rebase your local changes against the master branch
1. Run tests and confirm that it passes
1. Create new Pull Request

## License

The codes in this repository are lisenced unfer the [MIT License](https://github.com/CdecPGL/minimal-serializer/blob/master/LICENSE).
