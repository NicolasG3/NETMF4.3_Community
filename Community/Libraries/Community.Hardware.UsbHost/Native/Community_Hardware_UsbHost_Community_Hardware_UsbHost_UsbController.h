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


#ifndef _COMMUNITY_HARDWARE_USBHOST_COMMUNITY_HARDWARE_USBHOST_USBCONTROLLER_H_
#define _COMMUNITY_HARDWARE_USBHOST_COMMUNITY_HARDWARE_USBHOST_USBCONTROLLER_H_

namespace Community
{
    namespace Hardware
    {
        namespace UsbHost
        {
            struct UsbController
            {
                // Helper Functions to access fields of managed object
                static UNSUPPORTED_TYPE& Get__dispatcher( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_Community_Hardware_UsbHost_Community_Hardware_UsbHost_UsbController::FIELD___dispatcher ); }

                // Declaration of stubs. These functions are implemented by Interop code developers
                static INT8 NativeStart( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
                static INT8 NativeStop( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            };
        }
    }
}
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Community_Hardware_UsbHost_Driver;

#endif  //_COMMUNITY_HARDWARE_USBHOST_COMMUNITY_HARDWARE_USBHOST_USBCONTROLLER_H_
