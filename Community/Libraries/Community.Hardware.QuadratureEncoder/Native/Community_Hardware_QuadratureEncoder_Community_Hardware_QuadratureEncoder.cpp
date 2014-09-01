////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Nicolas Gallerand
//
//  *** Quadrature encoder driver ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "Community_Hardware_QuadratureEncoder.h"
#include "Community_Hardware_QuadratureEncoder_Community_Hardware_QuadratureEncoder.h"
#include <tinyhal.h>
#include <cpu.h>
#include <QuadratureEncoder_decl.h>

using namespace Community::Hardware;

static CLR_RT_HeapBlock_NativeEventDispatcher *g_Context = NULL;

// -- NativeEventDispatcher methods implementation --
static HRESULT Initialize_QuadratureEncoder_Driver( CLR_RT_HeapBlock_NativeEventDispatcher *pContext, UINT64 userData )
{
	g_Context  = pContext;
	return S_OK;
}

static HRESULT SetEnable_QuadratureEncoder_Driver( CLR_RT_HeapBlock_NativeEventDispatcher *pContext, bool fEnable )
{
	//GLOBAL_LOCK(irq);
	return S_OK;
}

static HRESULT Cleanup_QuadratureEncoder_Driver( CLR_RT_HeapBlock_NativeEventDispatcher *pContext )
{
	GLOBAL_LOCK(irq);
	g_Context = NULL;
	CleanupNativeEventsFromHALQueue( pContext );
	return S_OK;
}
// -- end of NativeEventDispatcher methods implementation --

// -- Declarations required by NativeEventDispatcher --
const CLR_RT_DriverInterruptMethods g_Community_Hardware_QuadratureEncoder_Driver_InterruptMethods = 
{ 
	Initialize_QuadratureEncoder_Driver,
	SetEnable_QuadratureEncoder_Driver,
	Cleanup_QuadratureEncoder_Driver
};

//Must be declared in featureproj: <InteropFeature Include="Community_Hardware_QuadratureEncoder_Driver" />
const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Community_Hardware_QuadratureEncoder_Driver =
{
	"Community_QuadratureEncoder_Driver", 
	DRIVER_INTERRUPT_METHODS_CHECKSUM,
	&g_Community_Hardware_QuadratureEncoder_Driver_InterruptMethods
};

// -- end of declarations required by NativeEventDispatcher --

static void QuadratureEncoderIOEvent (int channel, int IOIndex, int count, BOOL overlap) {
	//relay to NativeEventDispatcher
	if (overlap)
		IOIndex |= 0x10;
	SaveNativeEventToHALQueue( g_Context, IOIndex, count);
}

INT32 QuadratureEncoder::GetIOPinCount( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    return ::QED_GetIOCountForChannel(Get__channel(pMngObj));
}

INT32 QuadratureEncoder::GetIOPin( CLR_RT_HeapBlock* pMngObj, INT32 IOIndex, HRESULT &hr )
{
    return ::QED_GetIOPinForChannel(Get__channel(pMngObj), IOIndex);
}

INT32 QuadratureEncoder::GetIOStatus( CLR_RT_HeapBlock* pMngObj, INT32 IOIndex, HRESULT &hr )
{
	return ::QED_GetIOStatus(Get__channel(pMngObj), IOIndex);
}

void QuadratureEncoder::InitOutputCompare( CLR_RT_HeapBlock* pMngObj, INT32 IOIndex, INT32 value, INT8 invert, INT32 pulseLength, HRESULT &hr )
{
	hr = ::QED_InitOutputCompare(Get__channel(pMngObj), IOIndex, value, invert, pulseLength, QuadratureEncoderIOEvent);
}

void QuadratureEncoder::InitInputCapture( CLR_RT_HeapBlock* pMngObj, INT32 IOIndex, INT8 invert, HRESULT &hr )
{
	hr = ::QED_InitInputCapture(Get__channel(pMngObj), IOIndex, invert, QuadratureEncoderIOEvent);
}

void QuadratureEncoder::ReleaseIO( CLR_RT_HeapBlock* pMngObj, INT32 IOIndex, HRESULT &hr )
{
    ::QED_ReleaseIO(Get__channel(pMngObj), IOIndex);
}

INT32 QuadratureEncoder::GetChannelCount( HRESULT &hr )
{
    return ::QED_ChannelCount();
}

void QuadratureEncoder::GetQuadratureEncoderPinsForChannel( INT32 channel, INT32 * pinA, INT32 * pinB, HRESULT &hr )
{
	hr = ::QED_GetPinsForChannel(channel, (GPIO_PIN*)pinA, (GPIO_PIN*)pinB);
}

void QuadratureEncoder::Initialize( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    hr = ::QED_Initialize(Get__channel(pMngObj), Get__mode(pMngObj));
}

void QuadratureEncoder::Uninitialize( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    ::QED_Uninitialize(Get__channel(pMngObj));
}

INT32 QuadratureEncoder::get_Count( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    return ::QED_GetCount(Get__channel(pMngObj));
}

void QuadratureEncoder::set_Count( CLR_RT_HeapBlock* pMngObj, INT32 value, HRESULT &hr )
{
	hr = ::QED_SetCount(Get__channel(pMngObj), value);
}

INT8 QuadratureEncoder::get_CountEnabled( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
	return ::QED_GetCountEnable(Get__channel(pMngObj));
}

void QuadratureEncoder::set_CountEnabled( CLR_RT_HeapBlock* pMngObj, INT8 value, HRESULT &hr )
{
	hr = ::QED_SetCountEnable(Get__channel(pMngObj), value);
}

