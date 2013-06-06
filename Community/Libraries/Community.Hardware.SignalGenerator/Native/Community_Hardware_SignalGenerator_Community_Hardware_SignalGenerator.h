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


#ifndef _COMMUNITY_HARDWARE_SIGNALGENERATOR_COMMUNITY_HARDWARE_SIGNALGENERATOR_H_
#define _COMMUNITY_HARDWARE_SIGNALGENERATOR_COMMUNITY_HARDWARE_SIGNALGENERATOR_H_

namespace Community
{
    namespace Hardware
    {
        struct SignalGenerator
        {
            // Helper Functions to access fields of managed object
            //static UNSUPPORTED_TYPE& Get__signalInfo( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_Community_Hardware_SignalGenerator_Community_Hardware_SignalGenerator::FIELD___signalInfo ); }

            static INT32& Get__pin( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_Community_Hardware_SignalGenerator_Community_Hardware_SignalGenerator::FIELD___pin ); }

            static INT8& Get__invert( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT8( pMngObj, Library_Community_Hardware_SignalGenerator_Community_Hardware_SignalGenerator::FIELD___invert ); }

            //static UNSUPPORTED_TYPE& Get__levelTimes( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_Community_Hardware_SignalGenerator_Community_Hardware_SignalGenerator::FIELD___levelTimes ); }

            // Declaration of stubs. These functions are implemented by Interop code developers
            static INT8 get_PinState( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static void set_PinState( CLR_RT_HeapBlock* pMngObj, INT8 param0, HRESULT &hr );
            static void NativeInitialize( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static void NativeDispose( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static void Start( CLR_RT_HeapBlock* pMngObj, INT32 param0, double param1, double param2, HRESULT &hr );
            static void Start( CLR_RT_HeapBlock* pMngObj, UINT64 param0, INT32 param1, double param2, double param3, HRESULT &hr );
            static void Stop( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
        };
    }
}
#endif  //_COMMUNITY_HARDWARE_SIGNALGENERATOR_COMMUNITY_HARDWARE_SIGNALGENERATOR_H_
