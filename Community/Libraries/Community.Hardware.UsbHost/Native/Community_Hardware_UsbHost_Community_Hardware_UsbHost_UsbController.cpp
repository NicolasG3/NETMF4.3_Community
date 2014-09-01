////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Nicolas Gallerand
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include "Community_Hardware_UsbHost.h"
#include "Community_Hardware_UsbHost_Community_Hardware_UsbHost_UsbController.h"
#include "usbh_core.h"
#include "usbh_msc_core.h"
#include "usb_def.h"
#include "usbh_driver.h"
#include "USB_MSC_driver.h"

using namespace Community::Hardware::UsbHost;

HAL_COMPLETION g_completion;

//-------------------------------------
//		NativeEventDispatcher
//-------------------------------------
CLR_RT_HeapBlock_NativeEventDispatcher *g_Context = NULL;

// -- NativeEventDispatcher methods implementation --
static HRESULT Initialize_UsbHost_Driver( CLR_RT_HeapBlock_NativeEventDispatcher *pContext, UINT64 userData )
{
	g_Context  = pContext;
	return S_OK;
}

static HRESULT SetEnable_UsbHost_Driver( CLR_RT_HeapBlock_NativeEventDispatcher *pContext, bool fEnable )
{
	//GLOBAL_LOCK(irq);
	return S_OK;
}

static HRESULT Cleanup_UsbHost_Driver( CLR_RT_HeapBlock_NativeEventDispatcher *pContext )
{
	GLOBAL_LOCK(irq);
	g_Context = NULL;
	CleanupNativeEventsFromHALQueue( pContext );
	return S_OK;
}
// -- end of NativeEventDispatcher methods implementation --

// -- Declarations required by NativeEventDispatcher --
const CLR_RT_DriverInterruptMethods g_Community_Hardware_UsbHost_Driver_InterruptMethods = 
{ 
	Initialize_UsbHost_Driver,
	SetEnable_UsbHost_Driver,
	Cleanup_UsbHost_Driver
};

//Must be declared in featureproj: <InteropFeature Include="Community_Hardware_UsbHost_Driver" />
const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Community_Hardware_UsbHost_Driver =
{
	"Community_Hardware_UsbHost_Driver", 
	DRIVER_INTERRUPT_METHODS_CHECKSUM,
	&g_Community_Hardware_UsbHost_Driver_InterruptMethods
};

// -- end of declarations required by NativeEventDispatcher --

//-------------------------------------
//		/NativeEventDispatcher
//-------------------------------------

//-------------------------------------
//		USB Core
//-------------------------------------
USB_OTG_CORE_HANDLE          USB_OTG_Core;
USBH_HOST                     USB_Host;

static struct BlockStorageDevice		g_USB_BS;
extern struct IBlockStorageDevice g_USB_MSC_DeviceTable; 


void USBH_StateMachineCompletionHandler(void* param) {
	HOST_State oldState;
	do {
		oldState = USB_Host.gState;
		USBH_Process(&USB_OTG_Core, &USB_Host);
	} while (USB_Host.gState != oldState);
}


void USBH_USR_Init(void) {
	g_USB_BS.m_context = 0;

}
void USBH_USR_DeInit(void){

}
void USBH_USR_DeviceAttached(void){
	SaveNativeEventToHALQueue( g_Context, 0x100, 0);
}
void USBH_USR_ResetDevice(void){

}
void USBH_USR_DeviceDisconnected (void){
	if (g_USB_BS.m_context != 0) {
		FS_UnmountVolume(&g_USB_BS);
		BlockStorageList::RemoveDevice(&g_USB_BS, TRUE);
		g_USB_BS.m_context = 0;
		SaveNativeEventToHALQueue( g_Context, 0x008, 0x00);
	}
	else {
		SaveNativeEventToHALQueue( g_Context, 0,0);
	}

}
void USBH_USR_OverCurrentDetected (void){

}
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed){

}
void USBH_USR_Device_DescAvailable(void *param){

}
void USBH_USR_DeviceAddressAssigned(void){

}
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
										  USBH_InterfaceDesc_TypeDef *itfDesc,
										  USBH_EpDesc_TypeDef *epDesc){

}
void USBH_USR_Manufacturer_String(void *param){

}
void USBH_USR_Product_String(void *param){

}
void USBH_USR_SerialNum_String(void *param){

}
void USBH_USR_EnumerationDone(void) {

}
USBH_USR_Status USBH_USR_UserInput(void){
	return USBH_USR_RESP_OK;
}

void USBH_USR_DeviceNotSupported(void){

}
void USBH_USR_UnrecoveredError(void){

}
int USBH_USR_MSC_Application(void){
	if (g_USB_BS.m_context == 0) {
		if(BlockStorageList::AddDevice(&g_USB_BS, &g_USB_MSC_DeviceTable, &USB_OTG_Core, TRUE)){
			//CLR_Debug::Printf("FS_MountVolume USB\r\n");
			FS_MountVolume("USB", 0, 0, &g_USB_BS);
		}
		SaveNativeEventToHALQueue( g_Context, 0x108, 0x00);
	}
	return 0;

}


//User callback structure for the USB Host state machine
USBH_Usr_cb_TypeDef USR_cb =
{
	USBH_USR_Init,
	USBH_USR_DeInit,
	USBH_USR_DeviceAttached,
	USBH_USR_ResetDevice,
	USBH_USR_DeviceDisconnected,
	USBH_USR_OverCurrentDetected,
	USBH_USR_DeviceSpeedDetected,
	USBH_USR_Device_DescAvailable,
	USBH_USR_DeviceAddressAssigned,
	USBH_USR_Configuration_DescAvailable,
	USBH_USR_Manufacturer_String,
	USBH_USR_Product_String,
	USBH_USR_SerialNum_String,
	USBH_USR_EnumerationDone,
	USBH_USR_UserInput,
	USBH_USR_MSC_Application,
	USBH_USR_DeviceNotSupported,
	USBH_USR_UnrecoveredError

};

//-------------------------------------
//		/USB Core
//-------------------------------------


INT8 UsbController::NativeStart( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
	INT8 retVal = 1; 
	//if (!USBH_Initialize(1)) {
	//	hr = CLR_E_FAIL;
	//	return FALSE;
	//}

	g_completion.InitializeForISR(&USBH_StateMachineCompletionHandler, NULL);
	USBH_Init(&USB_OTG_Core, 
		USB_OTG_HS_CORE_ID,
		&USB_Host,
		&USBH_MSC_cb, 
		&USR_cb);

	return retVal;
}

INT8 UsbController::NativeStop( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
	INT8 retVal = 1; 
	return retVal;
}
