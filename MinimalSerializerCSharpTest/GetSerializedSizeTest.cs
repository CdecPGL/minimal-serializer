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
    public class GetSerializeSizeTest
    {
        [DataTestMethod]
        [DataRow(typeof(bool), 1, DisplayName = "bool")]
        [DataRow(typeof(byte), 1, DisplayName = "byte")]
        [DataRow(typeof(sbyte), 1, DisplayName = "sbyte")]
        [DataRow(typeof(short), 2, DisplayName = "short")]
        [DataRow(typeof(ushort), 2, DisplayName = "ushort")]
        [DataRow(typeof(int), 4, DisplayName = "int")]
        [DataRow(typeof(uint), 4, DisplayName = "uint")]
        [DataRow(typeof(long), 8, DisplayName = "long")]
        [DataRow(typeof(ulong), 8, DisplayName = "ulong")]
        [DataRow(typeof(float), 4, DisplayName = "float")]
        [DataRow(typeof(double), 8, DisplayName = "double")]
        [DataRow(typeof(IntEnum), 4, DisplayName = "enum")]
        [DataRow(typeof(FixedStringStruct), 16, DisplayName = nameof(FixedStringStruct))]
        [DataRow(typeof(FixedArrayStruct), 128, DisplayName = nameof(FixedArrayStruct))]
        [DataRow(typeof(CompositeStruct), 158, DisplayName = nameof(CompositeStruct))]
        [DataRow(typeof(SequentialClass), 4, DisplayName = nameof(SequentialClass))]
        [DataRow(typeof(NestedStruct), 170, DisplayName = nameof(NestedStruct))]
        [DataRow(typeof(NestedClass), 16, DisplayName = nameof(NestedClass))]
        public void GetSerializedSizeTest(Type type, int expected)
        {
            Assert.AreEqual(expected, Serializer.GetSerializedSize(type));
        }

        [TestMethod]
        public void GetSerializedSizeWithoutParameterTest()
        {
            Assert.AreEqual(Serializer.GetSerializedSize(typeof(bool)), Serializer.GetSerializedSize<bool>());
        }

        [TestMethod]
        public void GetSerializedSizeCacheTest()
        {
            Assert.AreEqual(4, Serializer.GetSerializedSize(typeof(int)));
            Assert.AreEqual(4, Serializer.GetSerializedSize(typeof(int)));
        }

        [DataTestMethod]
        [DataRow(typeof(string), DisplayName = "string")]
        [DataRow(typeof(int[]), DisplayName = "array")]
        [DataRow(typeof(char), DisplayName = "char")]
        [DataRow(typeof(decimal), DisplayName = "decimal")]
        [DataRow(typeof(nint), DisplayName = "nint")]
        [DataRow(typeof(nuint), DisplayName = "nuint")]
        [DataRow(typeof(NotSerializableStruct), DisplayName = nameof(NotSerializableStruct))]
        [DataRow(typeof(NotSequentialClass), DisplayName = nameof(NotSequentialClass))]
        [DataRow(typeof(NotFixedStringStruct), DisplayName = nameof(NotFixedStringStruct))]
        [DataRow(typeof(NotFixedArrayStruct), DisplayName = nameof(NotFixedArrayStruct))]
        [DataRow(typeof(EmptyStruct), DisplayName = nameof(EmptyStruct))]
        [DataRow(typeof(Interface), DisplayName = nameof(Interface))]
        [DataRow(typeof(AbstractClass), DisplayName = nameof(AbstractClass))]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void GetSerializedSizeErrorTest(Type type)
        {
            Serializer.GetSerializedSize(type);
        }
    }
}