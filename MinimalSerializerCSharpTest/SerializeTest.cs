/*
The MIT License (MIT)

Copyright (c) 2019-2022 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

using System;
using System.Linq;
using System.Reflection;
using CdecPGL.MinimalSerializer;
using CdecPGL.Test.SerializableTypes;
using CdecPGL.Test.UnserializableTypes;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CdecPGL.Test
{
    [TestClass]
    [TestCategory("ClientSerializer")]
    public class SerializeTest
    {
        [DataTestMethod]
        [DataRow(true, DisplayName = "bool")]
        [DataRow((byte)123, DisplayName = "byte")]
        [DataRow((sbyte)-123, DisplayName = "sbyte")]
        [DataRow((short)-12345, DisplayName = "short")]
        [DataRow((ushort)12345, DisplayName = "ushort")]
        [DataRow(-12345678, DisplayName = "int")]
        [DataRow(12345678u, DisplayName = "uint")]
        [DataRow(-12345678912345, DisplayName = "long")]
        [DataRow(12345678912345u, DisplayName = "ulong")]
        [DataRow(12e-3f, DisplayName = "float")]
        [DataRow(123e-45, DisplayName = "double")]
        [DataRow(IntEnum.C, DisplayName = "enum")]
        public void SerializeConsistencyTest(object obj)
        {
            var data1 = Serializer.Serialize(obj);
            var data2 = Serializer.Serialize(obj);
            CollectionAssert.AreEqual(data1, data2);
        }

        [DataTestMethod]
        [DataRow(typeof(FixedStringStruct), DisplayName = nameof(FixedStringStruct))]
        [DataRow(typeof(FixedArrayStruct), DisplayName = nameof(FixedArrayStruct))]
        [DataRow(typeof(ShortFixedStringStruct), DisplayName = nameof(ShortFixedStringStruct))]
        [DataRow(typeof(CompositeStruct), DisplayName = nameof(CompositeStruct))]
        [DataRow(typeof(SequentialClass), DisplayName = nameof(SequentialClass))]
        [DataRow(typeof(NestedStruct), DisplayName = nameof(NestedStruct))]
        [DataRow(typeof(NestedClass), DisplayName = nameof(NestedClass))]
        public void SerializeConsistencyTestForNotConstantConstructable(Type type)
        {
            var getDefault = type.GetMethod("GetDefault", BindingFlags.Public | BindingFlags.Static);
            if (getDefault == null)
            {
                Assert.Fail();
            }

            var obj = getDefault.Invoke(null, null);
            SerializeConsistencyTest(obj);
        }

        [DataTestMethod]
        [DataRow("abcde", DisplayName = "string")]
        [DataRow(new[] { 1, 2, 3 }, DisplayName = "array")]
        [DataRow((char)123, DisplayName = "char")]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void SerializeErrorTest(object obj)
        {
            Serializer.Serialize(obj);
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void SerializeErrorTest_decimal()
        {
            Serializer.Serialize((decimal)123);
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void SerializeErrorTest_nint()
        {
            Serializer.Serialize((nint)123);
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void SerializeErrorTest_nuint()
        {
            Serializer.Serialize((nuint)123);
        }

        [DataTestMethod]
        [DataRow(typeof(NotSerializableStruct), DisplayName = nameof(NotSerializableStruct))]
        [DataRow(typeof(NotSequentialClass), DisplayName = nameof(NotSequentialClass))]
        [DataRow(typeof(NotFixedStringStruct), DisplayName = nameof(NotFixedStringStruct))]
        [DataRow(typeof(NotFixedArrayStruct), DisplayName = nameof(NotFixedArrayStruct))]
        [DataRow(typeof(TooLongFixedStringStruct), DisplayName = nameof(TooLongFixedStringStruct))]
        [DataRow(typeof(TooLongFixedArrayStruct), DisplayName = nameof(TooLongFixedArrayStruct))]
        [DataRow(typeof(ShortFixedArrayStruct), DisplayName = nameof(ShortFixedArrayStruct))]
        [DataRow(typeof(EmptyStruct), DisplayName = nameof(EmptyStruct))]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void SerializeErrorTestForNotConstantConstructable(Type type)
        {
            var getDefault = type.GetMethod("GetDefault", BindingFlags.Public | BindingFlags.Static);
            if (getDefault == null)
            {
                Assert.Fail();
            }

            var obj = getDefault.Invoke(null, null);
            SerializeErrorTest(obj);
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void SerializeNullErrorTest()
        {
            Serializer.Serialize(null);
        }

        [Serializable]
        internal struct OrderTestStruct
        {
            public byte Value1;
            public byte Value2;
            public byte Value3;
            [FixedLength(8)] public string Str;
        }

        [TestMethod]
        public void SerializeOrderConsistencyTest()
        {
            var data = new OrderTestStruct { Value1 = 1, Value2 = 2, Value3 = 3, Str = "abcde" };
            var actual = Serializer.Serialize(data);
            var expected = new byte[] { 1, 2, 3, (byte)'a', (byte)'b', (byte)'c', (byte)'d', (byte)'e', 0, 0, 0 };
            Assert.IsTrue(actual.SequenceEqual(expected),
                $"Actual: {string.Join(",", actual)}, Expected: {string.Join(",", expected)}");
            SerializeConsistencyTest(FixedStringStruct.GetDefault());
        }
    }
}