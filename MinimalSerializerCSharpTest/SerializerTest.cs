/*
The MIT License (MIT)

Copyright (c) 2019 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

using System;
using System.Linq;
using System.Runtime.InteropServices;
using CdecPGL.MinimalSerializer;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CdecPGL.Test
{
    [Serializable]
    internal struct FixedStringStruct
    {
        [FixedLength(16)] public string Str;

        public static FixedStringStruct GetDefault()
        {
            return new FixedStringStruct {Str = "abc"};
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
            {
                return false;
            }

            var other = (FixedStringStruct) obj;
            return Str == other.Str;
        }

        public override string ToString()
        {
            return base.ToString() + "(" + Str + ")";
        }
    }

    [Serializable]
    internal struct FixedArrayStruct
    {
        [FixedLength(32)] public int[] Array;

        public static FixedArrayStruct GetDefault()
        {
            return new FixedArrayStruct
            {
                Array = new[]
                {
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                    27, 28, 29, 30, 31
                }
            };
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
            {
                return false;
            }

            var other = (FixedArrayStruct) obj;
            return Array.SequenceEqual(other.Array);
        }
    }

    [Serializable]
    internal struct TestStruct
    {
        public short A;
        public uint B;
        public double C;
        [FixedLength(16)] public string Str;
        [FixedLength(32)] public int[] Array;

        public static TestStruct GetDefault()
        {
            return new TestStruct
            {
                A = -12,
                B = 3456,
                C = 3.1415,
                Str = "abcdefg",
                Array = new[]
                {
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                    27, 28, 29, 30, 31
                }
            };
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
            {
                return false;
            }

            var other = (TestStruct) obj;
            return A == other.A && B == other.B && C == other.C && Array.SequenceEqual(other.Array);
        }
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    internal sealed class TestClass
    {
        public int a;

        public static TestClass GetDefault()
        {
            return new TestClass
            {
                a = 1234
            };
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
            {
                return false;
            }

            var other = (TestClass) obj;
            return a == other.a;
        }
    }

    [Serializable]
    internal struct NestedTestStruct
    {
        public uint A;
        public double B;
        public TestStruct C;

        public static NestedTestStruct GetDefault()
        {
            return new NestedTestStruct
            {
                A = 12345,
                B = -123.345,
                C = TestStruct.GetDefault()
            };
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
            {
                return false;
            }

            var other = (NestedTestStruct) obj;
            return A == other.A && B == other.B && C.Equals(other.C);
        }
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    internal sealed class NestedTestClass
    {
        public uint a;
        public double b;
        public TestClass c;

        public static NestedTestClass GetDefault()
        {
            return new NestedTestClass
            {
                a = 12345,
                b = -123.345,
                c = TestClass.GetDefault()
            };
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
            {
                return false;
            }

            var other = (NestedTestClass) obj;
            return a == other.a && b == other.b && c.Equals(other.c);
        }
    }

    internal enum TestEnum : uint
    {
        A,
        B,
        C,
        D
    }

    internal struct NotSerializableStruct
    {
        public int A;
    }

    [Serializable]
    internal class NotSequentialClass
    {
        public int A;
    }

    [Serializable]
    internal struct NotFixedStringStruct
    {
        public string Str;
    }

    [Serializable]
    internal struct NotFixedArrayStruct
    {
        public int[] A;
    }

    [Serializable]
    internal struct TooLongFixedStringStruct
    {
        [FixedLength(16)] public string Str;

        public static TooLongFixedStringStruct GetDefault()
        {
            return new TooLongFixedStringStruct {Str = "abcdefghijklmnopqrstuvwxyz"};
        }
    }

    [Serializable]
    internal struct TooLongFixedArrayStruct
    {
        [FixedLength(32)] public int[] Array;

        public static TooLongFixedArrayStruct GetDefault()
        {
            return new TooLongFixedArrayStruct
            {
                Array = new int[64]
            };
        }
    }

    [Serializable]
    internal struct TooShortFixedArrayStruct
    {
        [FixedLength(32)] public int[] Array;

        public static TooShortFixedArrayStruct GetDefault()
        {
            return new TooShortFixedArrayStruct
            {
                Array = new int[16]
            };
        }
    }


    [TestClass]
    [TestCategory("ClientSerializer")]
    public class SerializerTest
    {
        [DataTestMethod]
        [DataRow(typeof(bool), 1)]
        [DataRow(typeof(byte), 1)]
        [DataRow(typeof(sbyte), 1)]
        [DataRow(typeof(short), 2)]
        [DataRow(typeof(ushort), 2)]
        [DataRow(typeof(int), 4)]
        [DataRow(typeof(uint), 4)]
        [DataRow(typeof(long), 8)]
        [DataRow(typeof(ulong), 8)]
        [DataRow(typeof(float), 4)]
        [DataRow(typeof(double), 8)]
        [DataRow(typeof(FixedStringStruct), 16)]
        [DataRow(typeof(FixedArrayStruct), 128)]
        [DataRow(typeof(TestStruct), 158)]
        [DataRow(typeof(TestClass), 4)]
        [DataRow(typeof(NestedTestStruct), 170)]
        [DataRow(typeof(NestedTestClass), 16)]
        [DataRow(typeof(TestEnum), 4)]
        public void GetSerializedSizeTest(Type type, int expected)
        {
            Assert.AreEqual(expected, MinimalSerializer.Serializer.GetSerializedSize(type));
        }

        [DataTestMethod]
        [DataRow(typeof(NotSerializableStruct))]
        [DataRow(typeof(NotSequentialClass))]
        [DataRow(typeof(NotFixedStringStruct))]
        [DataRow(typeof(NotFixedArrayStruct))]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void GetSerializedSizeErrorTest(Type type)
        {
            MinimalSerializer.Serializer.GetSerializedSize(type);
        }

        [DataTestMethod]
        [DataRow(true, DisplayName = nameof(SerializeConsistencyTest) + "_bool")]
        [DataRow((byte) 123, DisplayName = nameof(SerializeConsistencyTest) + "_byte")]
        [DataRow((sbyte) -123, DisplayName = nameof(SerializeConsistencyTest) + "_sbyte")]
        [DataRow((short) -12345, DisplayName = nameof(SerializeConsistencyTest) + "_short")]
        [DataRow((ushort) 12345, DisplayName = nameof(SerializeConsistencyTest) + "_ushort")]
        [DataRow(-12345678, DisplayName = nameof(SerializeConsistencyTest) + "_int")]
        [DataRow(12345678u, DisplayName = nameof(SerializeConsistencyTest) + "_uint")]
        [DataRow(-12345678912345, DisplayName = nameof(SerializeConsistencyTest) + "_long")]
        [DataRow(12345678912345u, DisplayName = nameof(SerializeConsistencyTest) + "_ulong")]
        [DataRow(12e-3f, DisplayName = nameof(SerializeConsistencyTest) + "_float")]
        [DataRow(123e-45, DisplayName = nameof(SerializeConsistencyTest) + "_double")]
        [DataRow(TestEnum.C, DisplayName = nameof(SerializeConsistencyTest) + "_TestEnum")]
        public void SerializeConsistencyTest(object obj)
        {
            var data1 = MinimalSerializer.Serializer.Serialize(obj);
            var data2 = MinimalSerializer.Serializer.Serialize(obj);
            CollectionAssert.AreEqual(data1, data2);
        }

        [TestMethod]
        public void SerializeConsistencyTest_FixedStringStruct()
        {
            SerializeConsistencyTest(FixedStringStruct.GetDefault());
        }

        [TestMethod]
        public void SerializeConsistencyTest_FixedArrayStruct()
        {
            SerializeConsistencyTest(FixedArrayStruct.GetDefault());
        }

        [TestMethod]
        public void SerializeConsistencyTest_TestStruct()
        {
            SerializeConsistencyTest(TestStruct.GetDefault());
        }

        [TestMethod]
        public void SerializeConsistencyTest_TestClass()
        {
            SerializeConsistencyTest(TestClass.GetDefault());
        }

        [TestMethod]
        public void SerializeConsistencyTest_NestedTestStruct()
        {
            SerializeConsistencyTest(NestedTestStruct.GetDefault());
        }

        [TestMethod]
        public void SerializeConsistencyTest_NestedTestClass()
        {
            SerializeConsistencyTest(NestedTestClass.GetDefault());
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void SerializeTooLongFixedStringErrorTest()
        {
            MinimalSerializer.Serializer.Serialize(TooLongFixedStringStruct.GetDefault());
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void SerializeTooLongFixedArrayErrorTest()
        {
            MinimalSerializer.Serializer.Serialize(TooLongFixedArrayStruct.GetDefault());
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidSerializationException))]
        public void SerializeTooShortFixedArrayErrorTest()
        {
            MinimalSerializer.Serializer.Serialize(TooShortFixedArrayStruct.GetDefault());
        }

        private void DeserializeTest<T>(T obj)
        {
            var data = MinimalSerializer.Serializer.Serialize(obj);
            var obj2 = MinimalSerializer.Serializer.Deserialize<T>(data);
            Assert.AreEqual(obj, obj2);
        }

        [TestMethod]
        public void DeserializeTest_bool()
        {
            DeserializeTest(true);
        }

        [TestMethod]
        public void DeserializeTest_byte()
        {
            DeserializeTest((byte) 123);
        }

        [TestMethod]
        public void DeserializeTest_sbyte()
        {
            DeserializeTest((sbyte) -123);
        }

        [TestMethod]
        public void DeserializeTest_short()
        {
            DeserializeTest((short) -12345);
        }

        [TestMethod]
        public void DeserializeTest_ushort()
        {
            DeserializeTest((ushort) 12345);
        }

        [TestMethod]
        public void DeserializeTest_int()
        {
            DeserializeTest(-12345678);
        }

        [TestMethod]
        public void DeserializeTest_uint()
        {
            DeserializeTest(12345678u);
        }

        [TestMethod]
        public void DeserializeTest_long()
        {
            DeserializeTest(-123456787912345);
        }

        [TestMethod]
        public void DeserializeTest_ulong()
        {
            DeserializeTest(12345678912345u);
        }

        [TestMethod]
        public void DeserializeTest_float()
        {
            DeserializeTest(12e-3f);
        }

        [TestMethod]
        public void DeserializeTest_double()
        {
            DeserializeTest(123e-45);
        }

        [TestMethod]
        public void DeserializeTest_FixedStringStruct()
        {
            DeserializeTest(FixedStringStruct.GetDefault());
        }

        [TestMethod]
        public void DeserializeTest_FixedArrayStruct()
        {
            DeserializeTest(FixedArrayStruct.GetDefault());
        }

        [TestMethod]
        public void DeserializeTest_TestStruct()
        {
            DeserializeTest(TestStruct.GetDefault());
        }

        [TestMethod]
        public void DeserializeTest_TestClass()
        {
            DeserializeTest(TestClass.GetDefault());
        }

        [TestMethod]
        public void DeserializeTest_NestedTestStruct()
        {
            DeserializeTest(NestedTestStruct.GetDefault());
        }

        [TestMethod]
        public void DeserializeTest_NestedTestClass()
        {
            DeserializeTest(NestedTestClass.GetDefault());
        }

        [TestMethod]
        public void DeserializeTest_TestEnum()
        {
            DeserializeTest(TestEnum.C);
        }
    }
}