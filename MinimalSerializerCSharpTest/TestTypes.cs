/*
The MIT License (MIT)

Copyright (c) 2019-2022 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

using System;
using System.Linq;
using System.Runtime.InteropServices;
using CdecPGL.MinimalSerializer;

namespace CdecPGL.Test
{
    namespace SerializableTypes
    {
        [Serializable]
        internal struct FixedStringStruct
        {
            [FixedLength(16)] public string Str;

            public static FixedStringStruct GetDefault()
            {
                return new FixedStringStruct { Str = "abcdabcdabcdabcd" };
            }

            public override bool Equals(object obj)
            {
                if (obj == null)
                {
                    return false;
                }

                var other = (FixedStringStruct)obj;
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
                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                        24, 25, 26, 27, 28, 29, 30, 31
                    }
                };
            }

            public override bool Equals(object obj)
            {
                if (obj == null)
                {
                    return false;
                }

                var other = (FixedArrayStruct)obj;
                return Array.SequenceEqual(other.Array);
            }
        }

        [Serializable]
        internal struct ShortFixedStringStruct
        {
            [FixedLength(16)] public string Str;

            public static ShortFixedStringStruct GetDefault()
            {
                return new ShortFixedStringStruct { Str = "abc" };
            }

            public override bool Equals(object obj)
            {
                if (obj == null)
                {
                    return false;
                }

                var other = (ShortFixedStringStruct)obj;
                return Str == other.Str;
            }

            public override string ToString()
            {
                return base.ToString() + "(" + Str + ")";
            }
        }

        [Serializable]
        internal struct CompositeStruct
        {
            public short A;
            public uint B;
            public double C;
            [FixedLength(16)] public string Str;
            [FixedLength(32)] public int[] Array;

            public static CompositeStruct GetDefault()
            {
                return new CompositeStruct
                {
                    A = -12,
                    B = 3456,
                    C = 3.1415,
                    Str = "abcdefg",
                    Array = new[]
                    {
                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                        26, 27, 28, 29, 30, 31
                    }
                };
            }

            public override bool Equals(object obj)
            {
                if (obj == null)
                {
                    return false;
                }

                var other = (CompositeStruct)obj;
                return A == other.A && B == other.B && C == other.C && Array.SequenceEqual(other.Array);
            }
        }

        [Serializable]
        [StructLayout(LayoutKind.Sequential)]
        internal sealed class SequentialClass
        {
            public int a;

            public static SequentialClass GetDefault()
            {
                return new SequentialClass { a = 1234 };
            }

            public override bool Equals(object obj)
            {
                if (obj == null)
                {
                    return false;
                }

                var other = (SequentialClass)obj;
                return a == other.a;
            }
        }

        [Serializable]
        internal struct NestedStruct
        {
            public uint A;
            public double B;
            public CompositeStruct C;

            public static NestedStruct GetDefault()
            {
                return new NestedStruct { A = 12345, B = -123.345, C = CompositeStruct.GetDefault() };
            }

            public override bool Equals(object obj)
            {
                if (obj == null)
                {
                    return false;
                }

                var other = (NestedStruct)obj;
                return A == other.A && B == other.B && C.Equals(other.C);
            }
        }

        [Serializable]
        [StructLayout(LayoutKind.Sequential)]
        internal sealed class NestedClass
        {
            public uint a;
            public double b;
            public SequentialClass c;

            public static NestedClass GetDefault()
            {
                return new NestedClass { a = 12345, b = -123.345, c = SequentialClass.GetDefault() };
            }

            public override bool Equals(object obj)
            {
                if (obj == null)
                {
                    return false;
                }

                var other = (NestedClass)obj;
                return a == other.a && b == other.b && c.Equals(other.c);
            }
        }

        internal enum IntEnum : uint
        {
            A,
            B,
            C,
            D
        }
    }

    namespace UnserializableTypes
    {
        [Serializable]
        internal struct TooLongFixedStringStruct
        {
            [FixedLength(16)] public string Str;

            public static TooLongFixedStringStruct GetDefault()
            {
                return new TooLongFixedStringStruct { Str = "abcdefghijklmnopqrstuvwxyz" };
            }
        }

        [Serializable]
        internal struct TooLongFixedArrayStruct
        {
            [FixedLength(32)] public int[] Array;

            public static TooLongFixedArrayStruct GetDefault()
            {
                return new TooLongFixedArrayStruct { Array = new int[64] };
            }
        }

        [Serializable]
        internal struct ShortFixedArrayStruct
        {
            [FixedLength(32)] public int[] Array;

            public static ShortFixedArrayStruct GetDefault()
            {
                return new ShortFixedArrayStruct { Array = new[] { 0, 1, 2, 3, 4, 5, 6 } };
            }

            public override bool Equals(object obj)
            {
                if (obj == null)
                {
                    return false;
                }

                var other = (ShortFixedArrayStruct)obj;
                return Array.SequenceEqual(other.Array);
            }
        }

        internal struct NotSerializableStruct
        {
            public int A;

            public static NotSerializableStruct GetDefault()
            {
                return new NotSerializableStruct { A = 123 };
            }
        }

        [Serializable]
        internal class NotSequentialClass
        {
            public int A;

            public static NotSequentialClass GetDefault()
            {
                return new NotSequentialClass { A = 123 };
            }
        }

        [Serializable]
        internal struct NotFixedStringStruct
        {
            public string Str;

            public static NotFixedStringStruct GetDefault()
            {
                return new NotFixedStringStruct { Str = "abcd" };
            }
        }

        [Serializable]
        internal struct NotFixedArrayStruct
        {
            public int[] A;

            public static NotFixedArrayStruct GetDefault()
            {
                return new NotFixedArrayStruct { A = new int[16] };
            }
        }

        [Serializable]
        internal struct EmptyStruct
        {
            public static EmptyStruct GetDefault()
            {
                return new EmptyStruct();
            }
        }

        internal interface Interface
        {
            public int A { get; }
        }

        [Serializable]
        internal abstract class AbstractClass
        {
            public int A;
        }
    }
}