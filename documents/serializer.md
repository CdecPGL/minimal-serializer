# Serializer Usage

## C++

### Available Types

- Size Defined Integer Types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t
- Boolean Type
- std::array
- Enum Types: enum, enum class, enum struct
- fixed_string
- Trivial Custom Classes

### Not Available Types

Types whose size is different in each environment or will be change dynamically are not supported.

- Size Undefined Integer Types: char, int, long, etc. (Because some of types has different size in different environment. It may not occur errors but it may not works well)
- Float Types: float, double, etc. (Due to boost::endian library. This will be available in future)
- Not Trivial Custom Classes
- Dynamic Containers: std::vector, std::map, std::string, etc.
- Raw array (Because the size is inconstant)

### Usage

#### Serialize

```cpp
int value;
auto data = minimal_serializer::serialize(value);
```

#### Deserialize

```cpp
int value;
std::vector<uint8_t> data{...};
minimal_serializer::deserialize(value, data);
```

#### Get Serialized Size

```cpp
int32_t value=0;
size_t size = minimal_serializer::get_serialized_size(value);
```

The size is always same if the type is same.

### Serialize Custom Type

To make custom type serializable, there are two ways as below.
The type must be a trivial type.

#### Intrusive Way: Define on_serialize(serializer&) Member Function

```cpp
struct Data{
    int32_t value1;
    uint8_t value2;

    static void on_serialize(Data obj, serializer& serializer){
        serializer += obj.value1;
        serializer += obj.value2;
    }
};
```

#### Non Intrusive Way: Define on_serialize(T& value, serializer&) Global Function

```cpp
struct Data{
    int32_t value1;
    uint8_t value2;
};

void on_serialize(Data& data, serializer& serializer){
    serializer += data.value1;
    serializer += data.value2;
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

## C#

### Available Types

- Integer Types: sbyte, ubyte, short, ushort, int, uint, long, ulong
- Boolean Type
- Raw Array with FixedLength attribute
- Enum Types
- String with FixedLength attribute
- Custom Structs or Classes whose StructLayout is LayoutKind.Sequential (Struct is LayoutKind.Sequentia in default)

### Not Available Types

Types whose size will be change dynamically or layout is not constant are not supported.

- Float Types: float, double, etc. (Due to minimal_serializer_cpp restriction. This will be available in future)
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

## Common

### Portability

In serialization, minimal-serializer converts all data to big endian from the endian the machine uses.

In deserialization, minimal-serializer converts all data to the endian the machine uses from big endian.

Because of this behavior of minimal-serializer, serialized data by minimal-serializer is deserializable in other machines.

***Please note that machines whose endian is nether big endian nor little endian is not supported.***
