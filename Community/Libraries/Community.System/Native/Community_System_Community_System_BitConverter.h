//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------


#ifndef _COMMUNITY_SYSTEM_COMMUNITY_SYSTEM_BITCONVERTER_H_
#define _COMMUNITY_SYSTEM_COMMUNITY_SYSTEM_BITCONVERTER_H_

namespace Community
{
    namespace System
    {
        struct BitConverter
        {
            // Helper Functions to access fields of managed object
            // Declaration of stubs. These functions are implemented by Interop code developers
            static INT64 DoubleToInt64Bits( double param0, HRESULT &hr );
            static double Int64BitsToDouble( INT64 param0, HRESULT &hr );
            static void SetBytes( INT16 param0, CLR_RT_TypedArray_UINT8 param1, INT32 param2, HRESULT &hr );
            static void SetBytes( INT32 param0, CLR_RT_TypedArray_UINT8 param1, INT32 param2, HRESULT &hr );
            static void SetBytes( INT64 param0, CLR_RT_TypedArray_UINT8 param1, INT32 param2, HRESULT &hr );
            static void SetBytes( float param0, CLR_RT_TypedArray_UINT8 param1, INT32 param2, HRESULT &hr );
            static void SetBytes( double param0, CLR_RT_TypedArray_UINT8 param1, INT32 param2, HRESULT &hr );
            static float ToSingle( CLR_RT_TypedArray_UINT8 param0, INT32 param1, HRESULT &hr );
            static double ToDouble( CLR_RT_TypedArray_UINT8 param0, INT32 param1, HRESULT &hr );
            static INT16 ToInt16( CLR_RT_TypedArray_UINT8 param0, INT32 param1, HRESULT &hr );
            static INT32 ToInt32( CLR_RT_TypedArray_UINT8 param0, INT32 param1, HRESULT &hr );
            static INT64 ToInt64( CLR_RT_TypedArray_UINT8 param0, INT32 param1, HRESULT &hr );
        };
    }
}
#endif  //_COMMUNITY_SYSTEM_COMMUNITY_SYSTEM_BITCONVERTER_H_
