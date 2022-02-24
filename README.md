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
|Boost Serializer (Binary)|Yes|No|No|No|Almost All Types|
|Cereal (Binary)|Yes|No|Yes|Yes|Almost All Types|
|C# Built-in Serializer (BinaryFormatter)|No|Yes|No|No|Almost All Types|

## Example

See [Serializer Usage](documents/serializer.md) to know usage of minimal-serializer.

### C++

```cpp
#include "minimal_serializer/serializer.hpp"

struct Data {
    uint32_t value;
    std::array<int64_t, 16> array;
    minimal_serializer::fixed_string<32> string; // Ensure encoding is UTF-8. We strongly recommend to use u8 literal. This type is deprecated when using C++20
    minimal_serializer::fixed_u8string<32> u8string; // (C++20) A string whose encoding is guaranteed to be UTF-8. Use this type instead of fixed_string in C++20

    // Intrusive definition of serialize targets
    using serialize_targets = minimal_serializer::serialize_target_container<&Data::value, &Data::array, &Data::string>, &Data::u8string>;
}

// Not intrusive definition is also available
namespace minimal_serializer {
    template<>
    struct serialize_targets<Data> {
        using type = minimal_serializer::serialize_target_container<&Data::value, &Data::array, &Data::string, &Data::u8string>;
    }
}

int main(){
    // Get serialized size. The size is calculated in compile time
    constexpr auto size = minimal_serializer::serialized_size_v<Data>;

    // Serialize to fixed size byte array
    Data data;
    auto byte_array = minimal_serializer::serialize(data);

    // Serialize to buffer
    std::vector<uint8_t> buffer1{...};
    minimal_serializer::serialize(data, buffer);

    // Serialize to buffer with offset
    std::vector<uint8_t> buffer2{...};
    minimal_serializer::serialize(data, buffer, 10);

    // Serialize to output stream
    std::ostream ostream{...};
    minimal_serializer::serialize(data, ostream);

    // Deserialize from fixed size byte array
    minimal_serializer::deserialize(data, byte_array);

    // Deserialize from buffer
    minimal_serializer::deserialize(data, buffer1);

    // Deserialize from buffer with offset
    minimal_serializer::deserialize(data, buffer2, 10);

    // Deserialize from input stream
    std::istream istream{...};
    minimal_serializer::deserialize(data, istream);

    // Get a flag representing the type is serializable, which is judged in compile time.
    constexpr auto is_serializable = minimal_serializer::is_serializable_v<Data>;

    // (C++20) A concept to constrain types to serializable with minimal serializer.
    template<minimal_serializer::serializable T>
    void func(T data) {
        // Awesome codes...
    }
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

- A compiler which is compatible with C++17 (VS15.7, g++7 or clang5)
  - Concepts and the UTF-8 string type are supported in compilers which is compatible with C++20
- Boost Library
  - Minimal Support: 1.67 or higher
  - Floating Point Number Type Support: 1.74 or higher
  - C++20 Support: 1.77 or higher
- nameof C++ (this is included in this repository)

#### Tested Compilers

- g++ 10.3.1
- Clang 12.0.1
- MSVC 14.30 (VS 17.0.2)

### C#

- .NET FrameWork or .NET Core which is compatible with .NET Standard 2.0

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

Instead of installing required packages, you can use docker container "`cdec/minimal-serializer-dev`" to build and run tests like below command.

```bash
docker run -v <A Path of Minimal Serialier Repogitory>:/minimal-serializer -itd cdec/minimal-serializer-dev:latest
```

`cdec/minimal-serializer-dev` is built from `docker/dev/Dockerfile` in this repository.
You can build a docker image yourself with `docker/dev/Dockerfile` if you need.

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

The codes in this repository are lisenced unfer the [the MIT License](https://github.com/CdecPGL/minimal-serializer/blob/master/LICENSE).

This repogitory includes following libraries from other repogitories. The licenses of these codes follows each repogitories.

- [nameof C++](https://github.com/Neargye/nameof) ([The MIT License](https://github.com/Neargye/nameof/blob/master/LICENSE))
- [cereal](https://github.com/USCiLab/cereal) ([The 3-Clause BSD License](https://github.com/USCiLab/cereal/blob/master/LICENSE))
