////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Nicolas Gallerand
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Community_Hardware_Settings.h"
#include "Community_Hardware_Settings_Community_Hardware_SystemSettings.h"
#include <HardwareSerial_decl.h>

using namespace Community::Hardware;

#define SETTINGS_MAX_DATA_SIZE	1024

INT32 SystemSettings::get_DebugInterface( HRESULT &hr )
{
	return HalSystemConfig.DebuggerPorts[0];
}

void SystemSettings::SetDebugInterface( INT32 port, HRESULT &hr )
{
	if (port == HalSystemConfig.DebuggerPorts[0])
		return;
	HalSystemConfig.DebuggerPorts[0] = port;
	HalSystemConfig.MessagingPorts[0] = port;
	HalSystemConfig.DebugTextPort = port;
	HalSystemConfig.stdio = port;
	BOOL success = HAL_CONFIG_BLOCK::UpdateBlockWithName(HalSystemConfig.GetDriverName(), &HalSystemConfig, sizeof(HAL_SYSTEM_CONFIG), TRUE);
	if (!success) {
		hr = CLR_E_FAIL;
		return;
	}
	CPU_Reset();
}

void SystemSettings::NativeGetHardwareSerial( CLR_RT_TypedArray_UINT8 param0, HRESULT &hr )
{
	if (param0.GetSize() < 16) {
		hr=CLR_E_BUFFER_TOO_SMALL;
		return;
	}
	if(!HWSN_GetSerial(param0.GetBuffer()))
		hr = CLR_E_FAIL;
}

INT8 SystemSettings::WriteConfigBlock( LPCSTR name, CLR_RT_TypedArray_UINT8 data, HRESULT &hr )
{
	//use this temp buffer to add the HAL_DRIVER_CONFIG_HEADER
	//note: this buffer is allocated on stack to support no native heap solution 
	UINT8 buffer[sizeof(HAL_DRIVER_CONFIG_HEADER) + SETTINGS_MAX_DATA_SIZE];
	int dataSize = data.GetSize();
	if (dataSize > SETTINGS_MAX_DATA_SIZE){
		hr = CLR_E_OUT_OF_RANGE;
		return FALSE;
	}
	((HAL_DRIVER_CONFIG_HEADER*)&buffer)->Enable = TRUE;
	memcpy(&buffer[sizeof(HAL_DRIVER_CONFIG_HEADER)], data.GetBuffer(), dataSize);
	return HAL_CONFIG_BLOCK::UpdateBlockWithName(name, buffer, dataSize + sizeof(HAL_DRIVER_CONFIG_HEADER), TRUE);
}

INT8 SystemSettings::ReadConfigBlock( LPCSTR name, CLR_RT_TypedArray_UINT8 data, HRESULT &hr )
{
	//BOOL success = HAL_CONFIG_BLOCK::ApplyConfig(name, data.GetBuffer(), data.GetSize());
	//use this temp buffer to remove the HAL_DRIVER_CONFIG_HEADER from the result 
	UINT8 buffer[sizeof(HAL_DRIVER_CONFIG_HEADER) + SETTINGS_MAX_DATA_SIZE];
	int dataSize = data.GetSize();
	if (dataSize > SETTINGS_MAX_DATA_SIZE){
		hr = CLR_E_OUT_OF_RANGE;
		return FALSE;
	}
	BOOL success = HAL_CONFIG_BLOCK::ApplyConfig(name, buffer, dataSize + sizeof(HAL_DRIVER_CONFIG_HEADER));
	if (success) {
		memcpy(data.GetBuffer(), &buffer[sizeof(HAL_DRIVER_CONFIG_HEADER)], dataSize);
	}
	return success;

}

