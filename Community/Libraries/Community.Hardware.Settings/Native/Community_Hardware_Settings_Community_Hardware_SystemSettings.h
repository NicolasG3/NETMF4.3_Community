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


#ifndef _COMMUNITY_HARDWARE_SETTINGS_COMMUNITY_HARDWARE_SYSTEMSETTINGS_H_
#define _COMMUNITY_HARDWARE_SETTINGS_COMMUNITY_HARDWARE_SYSTEMSETTINGS_H_

namespace Community
{
    namespace Hardware
    {
        struct SystemSettings
        {
            // Helper Functions to access fields of managed object
            // Declaration of stubs. These functions are implemented by Interop code developers
            static INT32 get_DebugInterface( HRESULT &hr );
            static void SetDebugInterface( INT32 param0, HRESULT &hr );
            static void NativeGetHardwareSerial( CLR_RT_TypedArray_UINT8 param0, HRESULT &hr );
            static INT8 WriteConfigBlock( LPCSTR param0, CLR_RT_TypedArray_UINT8 param1, HRESULT &hr );
            static INT8 ReadConfigBlock( LPCSTR param0, CLR_RT_TypedArray_UINT8 param1, HRESULT &hr );
        };
    }
}
#endif  //_COMMUNITY_HARDWARE_SETTINGS_COMMUNITY_HARDWARE_SYSTEMSETTINGS_H_
