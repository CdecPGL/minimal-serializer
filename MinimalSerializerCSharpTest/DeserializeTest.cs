/*
The MIT License (MIT)

Copyright (c) 2019-2022 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

using System;
using CdecPGL.MinimalSerializer;
using CdecPGL.Test.SerializableTypes;
using CdecPGL.Test.UnserializableTypes;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CdecPGL.Test
{
    [TestClass]
    [TestCategory("ClientSerializer")]
    public class DeserializeSizeTest
    {
        private static void DeserializeTestImpl<T>(T obj)
        {
            var data = Serializer.Serialize(obj);
            var obj2 = Serializer.Deserialize<T>(data);
            Assert.AreEqual(obj, obj2);
        }

        [TestMethod]
        public void DeserializeTest_bool() => DeserializeTestImpl(true);

        [TestMethod]
        public void DeserializeTest_byte() => DeserializeTestImpl((byte)123);

        [TestMethod]
        public void DeserializeTest_sbyte() => DeserializeTestImpl((sbyte)-123);

        [TestMethod]
        public void DeserializeTest_short() => DeserializeTestImpl((short)-12345);

        [TestMethod]
        public void DeserializeTest_ushort() => DeserializeTestImpl((ushort)12345);

        [TestMethod]
        public void DeserializeTest_int() => DeserializeTestImpl(-12345678);

        [TestMethod]
        public void DeserializeTest_uint() => DeserializeTestImpl(12345678u);

        [TestMethod]
        public void DeserializeTest_long() => DeserializeTestImpl(-12345678912345);

        [TestMethod]
        public void DeserializeTest_ulong() => DeserializeTestImpl(12345678912345u);

        [TestMethod]
        public void DeserializeTest_float() => DeserializeTestImpl(12e-3f);

        [TestMethod]
        public void DeserializeTest_double() => DeserializeTestImpl(123e-45);

        [TestMethod]
        public void DeserializeTest_enum() => DeserializeTestImpl(IntEnum.C);

        [TestMethod]
        public void DeserializeTest_FixedStringStruct() => DeserializeTestImpl(FixedStringStruct.GetDefault());

        [TestMethod]
        public void DeserializeTest_FixedArrayStruct() => DeserializeTestImpl(FixedArrayStruct.GetDefault());

        [TestMethod]
        public void DeserializeTest_CompositeStruct() => DeserializeTestImpl(CompositeStruct.GetDefault());

        [TestMethod]
        public void DeserializeTest_SequentialClass() => DeserializeTestImpl(SequentialClass.GetDefault());

        [TestMethod]
        public void DeserializeTest_NestedStruct() => DeserializeTestImpl(NestedStruct.GetDefault());

        [TestMethod]
        public void DeserializeTest_NestedClass() => DeserializeTestImpl(NestedClass.GetDefault());

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void DeserializeErrorTest_string() => Serializer.Deserialize<string>(Array.Empty<byte>());

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void DeserializeErrorTest_array() => Serializer.Deserialize<int[]>(Array.Empty<byte>());

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void DeserializeErrorTest_char() => Serializer.Deserialize<char>(new byte[2]);

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void DeserializeTest_decimal() => Serializer.Deserialize<decimal>(new byte[16]);

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void DeserializeErrorTest_nint() =>
            Serializer.Deserialize<nint>(Environment.Is64BitProcess ? new byte[8] : new byte[4]);

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void DeserializeTest_nuint() =>
            Serializer.Deserialize<nuint>(Environment.Is64BitProcess ? new byte[8] : new byte[4]);

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void DeserializeErrorTest_NotSerializableStruct() =>
            Serializer.Deserialize<NotSerializableStruct>(new byte[4]);

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void DeserializeErrorTest_NotSequentialClass() =>
            Serializer.Deserialize<NotSequentialClass>(new byte[4]);

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void DeserializeErrorTest_NotFixedStringStruct() =>
            Serializer.Deserialize<NotFixedStringStruct>(Array.Empty<byte>());

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void DeserializeErrorTest_NotFixedArrayStruct() =>
            Serializer.Deserialize<NotFixedArrayStruct>(Array.Empty<byte>());

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void DeserializeErrorTest_EmptyStruct() => Serializer.Deserialize<EmptyStruct>(Array.Empty<byte>());

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void DeserializeSizeErrorTest() => Serializer.Deserialize<int>(new byte[1]);
    }
}