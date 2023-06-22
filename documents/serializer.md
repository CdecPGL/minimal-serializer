# Serializer Usage

## C++

### Available Types

Types whose size is defined in compile time are supported.

- Size Defined Integer Types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t
- Float Types: float, double (Boost Library 1.74.0 or higher is required)
  - **Systems whose `float` size is not 32 bit or `double` size is not 64 bit are not supported**
- Boolean Type
- Static Containers: std::array, std::tuple, std::pair
- Enum Types: enum, enum class, enum struct
- Fixed Length UTF-8 String: minimal_serializer::fixed_string, minimal_serializer::fixed_u8string (C++20), boost::static_strings::static_string, boost::static_strings::static_u8string (C++20)
- Trivial Custom Classes

### Not Available Types

Types whose size is different in each environment or will be change dynamically are not supported.

- Size Undefined Integer Types: char, int, long, etc. (Because some of types has different size in different environment. It may not occur errors but it may not works well)
- Dynamic Containers: std::vector, std::map, std::string, raw array etc.
- Dynamic or Not UTF-8 String String: std::string, std::u16string, std::u32string, std::wstring, boost::static_strings::static_u16string, boost::static_strings::static_u32string, boost::static_strings::static_wstring
- Not Trivial Custom Classes

### Usage

#### Serialize

Simplest way is pass a object you want to serialize to `minimal_serializer::serialize()` function.
The function returns serialized data by `std::array<uint8_t, S>` whose size `S` is serialized size.

```cpp
int value;
auto byte_array = minimal_serializer::serialize(value);
```

If you want to serialize a object to buffer you prepared, pass it to second parameter.

The type of buffer must be have `data()` and `size()` as its member function and the element type of it must be `uint8_t` such as `std::array<uint8_t, S>` and `std::vector<uint8_t>`.

```cpp
int value;
std::vector<uint8_t> buffer{...};
minimal_serializer::serialize(value, buffer);
```

It is possible to indicate offset of the buffer where serialized data is written.

```cpp
int value;
std::vector<uint8_t> buffer{...};
minimal_serializer::serialize(value, buffer, 10);
```

Output stream can be also available.

```cpp
int value;
std::ostream ostream{...};
minimal_serializer::serialize(value, ostream);
```

#### Deserialize

Simplest way is pass a object you want to serialize and buffer which has serialzied data to `minimal_serializer::deserialize()` function.

The type of buffer must be have `data()` and `size()` as its member function and the element type of it must be `uint8_t` such as `std::array<uint8_t, S>` and `std::vector<uint8_t>`.

```cpp
int value;
std::vector<uint8_t> buffer{...};
minimal_serializer::deserialize(value, buffer);
```

It is possible to indicate offset of the buffer where serialized data is written.

```cpp
int value;
std::vector<uint8_t> buffer{...};
minimal_serializer::deserialize(value, buffer, 10);
```

Input stream can be also available.

```cpp
int value;
std::istream istream{...};
minimal_serializer::deserialize(value, istream);
```

#### Get Serialized Size

```cpp
size_t size = minimal_serializer::serialized_size_v<int32_t>;
```

The size is calculated in compile time and always same if the type is same.

### Serialize Custom Type

To make custom type serializable, there are two ways as below.
The type must be a trivial type.

#### Intrusive Way: Define serialize_targets as Public Member Alias

This is convenience when you want to serialize private member variables of your custom classes or structs.

```cpp
struct Data{
    int32_t value1;
    uint8_t value2;

    using serialize_targets = minimal_serializer::serialize_target_container<&Data::value1, &Data::value2>;
};
```

#### Non Intrusive Way: Define template specification of serialize_targets class template

This is convenience when you want to serialize pre-defined classes or structs of other libraries.

```cpp
struct Data{
    int32_t value1;
    uint8_t value2;
};

namespace minimal_serializer {
    template<>
    struct serialize_targets<Data> {
        using type = minimal_serializer::serialize_target_container<&Data::value1, &Data::value2>;
    }
}
```

### Utility Types

#### fixed_string

A string type whose size is fixed.
Use this class instead of std::string if you want to serialize string.

```cpp
// OK: The length of string is 16 bytes (terminated string "\0" is ignored), which is less than 16 bytes
minimal_serializer::fixed_string<16> ok_english = "HelloGoodMorning";
// OK: The length of string is 15 bytes (each Japanese multibyte character is 3 bytes), which is less than 16 bytes
minimal_serializer::fixed_string<16> ok_japanese = u8"こんにちは";
// NG: The length of string is 27 bytes (each Japanese multibyte character is 3 bytes), which is less than 16 bytes
minimal_serializer::fixed_string<16> ng_japanese = u8"おはようございます";
```

### Character Endoding

This library uses UTF-8 as character encoding according to defact standard in Web.

In C++17, `minimal_serializer::fixed_string` which is similar to `std::string` is available as UTF-8 string type.
However, the type does not guarantee that character encoding is UTF-8 because there are no character type which guarantees that character encoding is UTF-8 in C++17.
Because of this reason, ***it is a part of your responsibilities to ensure the character encoding is UTF-8 by using `u8` string literal.***

In C++20, `minimal_serializer::fixed_u8string` which is similar to `std::u8string` is available as UTF-8 string type.
The type guarantees that character encoding is UTF-8 so you can safely use UTF-8 string with `u8` string literal.
`minimal_serializer::fixed_string` is deprecated in C++20.

## C#

### Available Types

- Integer Types: sbyte, ubyte, short, ushort, int, uint, long, ulong
- Float Types: float, double
- Boolean Type
- Raw Array with FixedLength attribute
- Enum Types
- String with FixedLength attribute
- Custom Structs or Classes whose StructLayout is LayoutKind.Sequential (Struct is LayoutKind.Sequentia in default)

### Not Available Types

Types whose size will be change dynamically or layout is not constant are not supported.

- Custom Structs or Classes whose StructLayout is not LayoutKind.Sequential
- Dynamic Containers: List, Dictionary, etc.
- Raw array wichout FixedLength attribute
- String wichout FixedLength attribute

### Usage

#### Serialize

```csharp
int value;
var data = CdecPGL.MinimalSerializer.Serializer.Serialize(value);
```

#### Deserialize

```cpp
int value;
var data = new byte[1];
value = CdecPGL.MinimalSerializer.Serializer.Deserialize<int>(data);
```

#### Get Serialized Size

```cpp
var size = CdecPGL.MinimalSerializer.Serializer.GetSerializedSize<int>();
var size2 = CdecPGL.MinimalSerializer.Serializer.GetSerializedSize(typeof(int));
```

The size is always same if the type is same.

### Serialize Custom Type

There are two or three steps to make custom type serializable.

1. Add `System.Serializable` attribute
1. Add `FixedLength` attribute to string and array members
1. (If class) Add `[StructLayout(LayoutKind.Sequential)]` attribute

### Character Endoding

This library uses UTF-8 as character encoding according to defact standard in Web.

In C#, `string` has characters as UTF-16 internally but this library converts encodings between UTF-16 and UTF-8 automatically when serialization and deserialization.
Because of this, you can use `string` without considering character encoding.

## Common

### Portability

In serialization, minimal-serializer converts all data to big endian from the endian the machine uses.

In deserialization, minimal-serializer converts all data to the endian the machine uses from big endian.

Because of this behavior of minimal-serializer, serialized data by minimal-serializer is deserializable in other machines.

***Please note that machines whose endian is nether big endian nor little endian is not supported.***

## Type Support Table

### Supported Types

|Type|C++|C#|
|:---|:---|:---|
|Boolean|bool|bool|
|8bit Signed Integer|int8_t|sbyte|
|8bit Unsigned Integer|uint8_t|byte|
|16bit Signed Integer|int16_t|short|
|16bit Unsigned Integer|uint16_t|ushort|
|32bit Signed Integer|int32_t|int|
|32bit Unsigned Integer|uint32_t|uint|
|64bit Signed Integer|int64_t|long|
|64bit Unsigned Integer|uint64_t|ulong|
|32bit Floating Point Value|float|float|
|64bit Floating Point Value|double|double|
|Enum|enum, enum class|enum|
|String|minimal_serializer::fixed_string, minimal_serializer::fixed_u8string (C++20), boost::static_strings::static_string, boost::static_strings::static_u8string (C++20)|x (Supported in Future)|
|String Field|minimal_serializer::fixed_string, minimal_serializer::fixed_u8string (C++20), boost::static_strings::static_string, boost::static_strings::static_u8string (C++20)|string with `[FixedLength]`|
|Array|std::array, std::tuple, std::pair|x (Supported in Future)|
|Array Field|std::array, std::tuple, std::pair|array with `[FixedLength]`|
|Class|Trivial Classes|Classes with `[Serializable][StructLayout(LayoutKind.Sequential)]`|
|Struct|Trivial Struct|Struct with `[Serializable]`|

### Unsupported Types

|Type|C++|C#|Future Support|
|:---|:---|:---|:---:|
|Char|char, signed char, unsigned char|char|x|
|Platform Dependent Sized Integer|int, unsigned int, etc|nint, nuint|x|
|Pointer|raw pointer, std:shared_ptr, etc|pointer|o|
|Reference|Reference|-|o|
|String|raw string, std::string, std::u16string, std::u32string, std::wstring, boost::static_strings::static_u16string, boost::static_strings::static_u32string, boost::static_strings::static_wstring|string without `[FixedLength]`|x|
|Array|std::vector, std::list, etc|array without `[FixedLength]`|x|
|Language Unique Types|-|decimal|x|
|Union|union|-|x|
