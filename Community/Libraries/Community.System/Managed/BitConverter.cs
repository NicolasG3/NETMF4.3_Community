////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Nicolas Gallerand
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using System;
using System.Runtime.CompilerServices;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;

namespace Community.System
    {
    /// <summary>
    /// Converts base data types to an array of bytes, and an array of bytes to base data types.
    /// </summary>
    public static class BitConverter
        {
        /// <summary>
        /// Indicates the byte order ("endianness") in which data is stored in this computer architecture.
        /// </summary>
        public static readonly bool IsLittleEndian = !SystemInfo.IsBigEndian;
        /// <summary>
        /// Converts the specified double-precision floating point number to a 64-bit signed integer.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Int64 DoubleToInt64Bits(double value);
        /// <summary>
        /// Converts the specified 64-bit signed integer to a double-precision floating point number.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern double Int64BitsToDouble(Int64 value);
        /// <summary>
        /// Returns the specified Boolean value as an array of bytes.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static byte[] GetBytes(bool value) {
            return new byte[] { (value ? ((byte)1) : ((byte)0)) };
            }
        /// <summary>
        /// Returns the specified Unicode character value as an array of bytes.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static byte[] GetBytes(char value) {
            return GetBytes((short)value);
            }
        /// <summary>
        /// Returns the specified 16-bit signed integer value as an array of bytes.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static byte[] GetBytes(Int16 value) {
            byte[] buffer = new byte[2];
            SetBytes(value, buffer, 0);
            return buffer;
            }
        /// <summary>
        /// Returns the specified 32-bit signed integer value as an array of bytes.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static byte[] GetBytes(Int32 value) {
            byte[] buffer = new byte[4];
            SetBytes(value, buffer, 0);
            return buffer;
            }
        /// <summary>
        /// Returns the specified 64-bit signed integer value as an array of bytes.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static byte[] GetBytes(Int64 value) {
            byte[] buffer = new byte[8];
            SetBytes(value, buffer, 0);
            return buffer;
            }
        /// <summary>
        /// Returns the specified single-precision floating point value as an array of bytes.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static byte[] GetBytes(Single value) {
            byte[] buffer = new byte[4];
            SetBytes(value, buffer, 0);
            return buffer;
            }
        /// <summary>
        /// Returns the specified double-precision floating point value as an array of bytes.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static byte[] GetBytes(Double value) {
            byte[] buffer = new byte[8];
            SetBytes(value, buffer, 0);
            return buffer;
            }
        /// <summary>
        /// Set two bytes at a specified position in a byte array with the specified 16-bit signed integer value
        /// </summary>
        /// <param name="value"></param>
        /// <param name="buffer"></param>
        /// <param name="startIndex"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetBytes(Int16 value, byte[] buffer, int startIndex);
        /// <summary>
        /// Set four bytes at a specified position in a byte array with the specified 32-bit signed integer value
        /// </summary>
        /// <param name="value"></param>
        /// <param name="buffer"></param>
        /// <param name="startIndex"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetBytes(Int32 value, byte[] buffer, int startIndex);
        /// <summary>
        /// Set eight bytes at a specified position in a byte array with the specified 64-bit signed integer value
        /// </summary>
        /// <param name="value"></param>
        /// <param name="buffer"></param>
        /// <param name="startIndex"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetBytes(Int64 value, byte[] buffer, int startIndex);
        /// <summary>
        /// Set four bytes at a specified position in a byte array with the specified single-precision floating point value
        /// </summary>
        /// <param name="value"></param>
        /// <param name="buffer"></param>
        /// <param name="startIndex"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetBytes(Single value, byte[] buffer, int startIndex);
        /// <summary>
        /// Set eight bytes at a specified position in a byte array with the specified double-precision floating point value
        /// </summary>
        /// <param name="value"></param>
        /// <param name="buffer"></param>
        /// <param name="startIndex"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetBytes(Double value, byte[] buffer, int startIndex);

        /// <summary>
        /// Returns the specified 16-bit unsigned integer value as an array of bytes.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static byte[] GetBytes(UInt16 value) {
            return GetBytes((Int16)value);
            }
        /// <summary>
        /// Returns the specified 32-bit unsigned integer value as an array of bytes.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static byte[] GetBytes(UInt32 value) {
            return GetBytes((Int32)value);
            }
        /// <summary>
        /// Returns the specified 64-bit unsigned integer value as an array of bytes.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static byte[] GetBytes(UInt64 value) {
            return GetBytes((Int64)value);
            }

        /// <summary>
        /// Returns a Boolean value converted from one byte at a specified position in a byte array.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="startIndex"></param>
        /// <returns></returns>
        public static bool ToBoolean(byte[] value, int startIndex) {
            return (value[startIndex] != 0);
            }
        /// <summary>
        /// Returns a Unicode character converted from two bytes at a specified position in a byte array.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="startIndex"></param>
        /// <returns></returns>
        public static char ToChar(byte[] value, int startIndex) {
            return (char)((UInt16)ToInt16(value, startIndex));
            }
        /// <summary>
        /// Returns a single-precision floating point number converted from four bytes at a specified position in a byte array.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="startIndex"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Single ToSingle(Byte[] value, int startIndex);
        /// <summary>
        /// Returns a double-precision floating point number converted from eight bytes at a specified position in a byte array.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="startIndex"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Double ToDouble(Byte[] value, int startIndex);
        /// <summary>
        /// Returns a 16-bit signed integer converted from two bytes at a specified position in a byte array.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="startIndex"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Int16 ToInt16(byte[] value, int startIndex);
        /// <summary>
        /// Returns a 32-bit signed integer converted from four bytes at a specified position in a byte array.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="startIndex"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Int32 ToInt32(byte[] value, int startIndex);
        /// <summary>
        /// Returns a 64-bit signed integer converted from eight bytes at a specified position in a byte array.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="startIndex"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Int64 ToInt64(byte[] value, int startIndex);
        /// <summary>
        /// Returns a 16-bit unsigned integer converted from two bytes at a specified position in a byte array.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="startIndex"></param>
        /// <returns></returns>
        public static UInt16 ToUInt16(byte[] value, int startIndex) {
            return (UInt16)ToInt16(value, startIndex);
            }
        /// <summary>
        /// Returns a 32-bit unsigned integer converted from four bytes at a specified position in a byte array.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="startIndex"></param>
        /// <returns></returns>
        public static UInt32 ToUInt32(byte[] value, int startIndex) {
            return (UInt32)ToInt32(value, startIndex);
            }
        /// <summary>
        /// Returns a 64-bit unsigned integer converted from eight bytes at a specified position in a byte array.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="startIndex"></param>
        /// <returns></returns>
        public static UInt64 ToUInt64(byte[] value, int startIndex) {
            return (UInt64)ToInt64(value, startIndex);
            }

        private static char GetHexValue(int i) {
            if (i < 10) {
                return (char)(i + 0x30);
                }
            return (char)((i - 10) + 0x41);
            }
        /// <summary>
        /// Converts the numeric value of each element of a specified array of bytes to its equivalent hexadecimal string representation.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static string ToString(byte[] value) {
            return ToString(value, 0, value.Length);
            }
        /// <summary>
        /// Converts the numeric value of each element of a specified subarray of bytes to its equivalent hexadecimal string representation.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="startIndex"></param>
        /// <returns></returns>
        public static string ToString(byte[] value, int startIndex) {
            return ToString(value, startIndex, value.Length - startIndex);
            }
        /// <summary>
        /// Converts the numeric value of each element of a specified subarray of bytes to its equivalent hexadecimal string representation.
        /// </summary>
        /// <param name="value"></param>
        /// <param name="startIndex"></param>
        /// <param name="length"></param>
        /// <returns></returns>
        public static string ToString(byte[] value, int startIndex, int length) {
            int num = length * 3;
            char[] chArray = new char[num];
            int index = 0;
            int num3 = startIndex;
            for (index = 0; index < num; index += 3) {
                byte num4 = value[num3++];
                chArray[index] = GetHexValue(num4 / 0x10);
                chArray[index + 1] = GetHexValue(num4 % 0x10);
                chArray[index + 2] = '-';
                }
            return new string(chArray, 0, chArray.Length - 1);
            }

        }
    }

