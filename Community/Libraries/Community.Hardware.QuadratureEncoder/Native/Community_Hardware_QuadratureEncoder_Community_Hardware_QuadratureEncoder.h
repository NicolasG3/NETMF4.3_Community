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


#ifndef _COMMUNITY_HARDWARE_QUADRATUREENCODER_COMMUNITY_HARDWARE_QUADRATUREENCODER_H_
#define _COMMUNITY_HARDWARE_QUADRATUREENCODER_COMMUNITY_HARDWARE_QUADRATUREENCODER_H_

namespace Community
{
    namespace Hardware
    {
        struct QuadratureEncoder
        {
            // Helper Functions to access fields of managed object
            //static UNSUPPORTED_TYPE& Get__dispatcher( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_Community_Hardware_QuadratureEncoder_Community_Hardware_QuadratureEncoder::FIELD___dispatcher ); }

            static INT32& Get__channel( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_Community_Hardware_QuadratureEncoder_Community_Hardware_QuadratureEncoder::FIELD___channel ); }

            //static void& Get__pin_A( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_void( pMngObj, Library_Community_Hardware_QuadratureEncoder_Community_Hardware_QuadratureEncoder::FIELD___pin_A ); }

            //static void& Get__pin_B( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_void( pMngObj, Library_Community_Hardware_QuadratureEncoder_Community_Hardware_QuadratureEncoder::FIELD___pin_B ); }

            static INT32& Get__mode( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_Community_Hardware_QuadratureEncoder_Community_Hardware_QuadratureEncoder::FIELD___mode ); }

             //static UNSUPPORTED_TYPE& Get_IOChanged( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_Community_Hardware_QuadratureEncoder_Community_Hardware_QuadratureEncoder::FIELD__IOChanged ); }

            // Declaration of stubs. These functions are implemented by Interop code developers
            static INT32 GetIOPinCount( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static INT32 GetIOPin( CLR_RT_HeapBlock* pMngObj, INT32 param0, HRESULT &hr );
            static INT32 GetIOStatus( CLR_RT_HeapBlock* pMngObj, INT32 param0, HRESULT &hr );
            static void InitOutputCompare( CLR_RT_HeapBlock* pMngObj, INT32 param0, INT32 param1, INT32 param2, INT8 param3, HRESULT &hr );
            static void InitInputCapture( CLR_RT_HeapBlock* pMngObj, INT32 param0, INT8 param1, HRESULT &hr );
            static void ReleaseIO( CLR_RT_HeapBlock* pMngObj, INT32 param0, HRESULT &hr );
            static INT32 GetQuadratureEncoderChannelCount( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static void GetQuadratureEncoderPinsForChannel( CLR_RT_HeapBlock* pMngObj, INT32 param0, INT32 * param1, INT32 * param2, HRESULT &hr );
            static void Initialize( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static void Uninitialize( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static INT32 get_Count( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static void set_Count( CLR_RT_HeapBlock* pMngObj, INT32 param0, HRESULT &hr );
            static INT8 get_CountEnabled( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static void set_CountEnabled( CLR_RT_HeapBlock* pMngObj, INT8 param0, HRESULT &hr );
        };
    }
}

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Community_Hardware_QuadratureEncoder_Driver;


#endif  //_COMMUNITY_HARDWARE_QUADRATUREENCODER_COMMUNITY_HARDWARE_QUADRATUREENCODER_H_
