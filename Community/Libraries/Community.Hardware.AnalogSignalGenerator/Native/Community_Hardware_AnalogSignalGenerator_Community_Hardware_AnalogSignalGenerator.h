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


#ifndef _COMMUNITY_HARDWARE_ANALOGSIGNALGENERATOR_COMMUNITY_HARDWARE_ANALOGSIGNALGENERATOR_H_
#define _COMMUNITY_HARDWARE_ANALOGSIGNALGENERATOR_COMMUNITY_HARDWARE_ANALOGSIGNALGENERATOR_H_

namespace Community
{
    namespace Hardware
    {
        struct AnalogSignalGenerator
        {
            // Helper Functions to access fields of managed object
            //static UNSUPPORTED_TYPE& Get__signalInfo( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_Community_Hardware_AnalogSignalGenerator_Community_Hardware_AnalogSignalGenerator::FIELD___signalInfo ); }

            static INT32& Get__channel( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_Community_Hardware_AnalogSignalGenerator_Community_Hardware_AnalogSignalGenerator::FIELD___channel ); }

            static INT32& Get__pin( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_Community_Hardware_AnalogSignalGenerator_Community_Hardware_AnalogSignalGenerator::FIELD___pin ); }

            static UNSUPPORTED_TYPE& Get__rawLevels( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_Community_Hardware_AnalogSignalGenerator_Community_Hardware_AnalogSignalGenerator::FIELD___rawLevels ); }

            static UINT16& Get__clockDivisor( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UINT16( pMngObj, Library_Community_Hardware_AnalogSignalGenerator_Community_Hardware_AnalogSignalGenerator::FIELD___clockDivisor ); }

            // Declaration of stubs. These functions are implemented by Interop code developers
            static void NativeInitialize( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static void NativeDispose( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static void Start( CLR_RT_HeapBlock* pMngObj, INT32 param0, HRESULT &hr );
            static void Start( CLR_RT_HeapBlock* pMngObj, UINT64 param0, INT32 param1, HRESULT &hr );
            static void Stop( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static INT32 GetChannelCount( HRESULT &hr );
            static INT32 GetPin( INT32 param0, HRESULT &hr );
            static UINT16 GetMinClockDivisor( HRESULT &hr );
        };
    }
}
#endif  //_COMMUNITY_HARDWARE_ANALOGSIGNALGENERATOR_COMMUNITY_HARDWARE_ANALOGSIGNALGENERATOR_H_
