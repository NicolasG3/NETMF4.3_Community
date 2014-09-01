////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Nicolas Gallerand
//
//  *** Analog signal generator for NETMF ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __arm
#undef _WIN32

#include "Community_Hardware_AnalogSignalGenerator.h"
#include "Community_Hardware_AnalogSignalGenerator_Community_Hardware_AnalogSignalGenerator.h"
#include <tinyhal.h>
#include <cpu.h>
#include <tinyCLR_Runtime.h>
#include <AnalogSignalGenerator_decl.h>

using namespace Community::Hardware;

//status bits
#define SG_READY	1
#define SG_ACTIVE	2
#define SG_PENDING	3
#define SG_ERROR	4


struct AnalogSignalInfo {
	//repetition decounter
	INT32 repeatCount;					
	//current rawLevels array index
	UINT32	index;
	//status flags
	UINT32	status;
	//never ending repetition
	bool repeatforever;
	//rawLevels array
	UINT16	*rawLevels;
	//rawLevels array length
	UINT32	length;
	//indicate the original pinning of rawLevels array heap block
	BOOL	rawLevelsBlockPinned;
	DA_CHANNEL channel;
};

void SampleTimeTick(void* param) {
	AnalogSignalInfo* signalInfo = (AnalogSignalInfo*)param;
	DA_Write(signalInfo->channel, (int)signalInfo->rawLevels[signalInfo->index]);
	signalInfo->index ++;
	if (signalInfo->index >= signalInfo->length) {
		signalInfo->index  = 0;
		signalInfo->repeatCount --;
		if (signalInfo->repeatCount == 0 && !signalInfo->repeatforever) {
			ASG_Uninitialize(signalInfo->channel);
			signalInfo->status == SG_READY;
		}
	}
	signalInfo->status = SG_ACTIVE;
}

//Initialize DAC output and generation timer
void AnalogSignalGenerator::NativeInitialize( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
	//Initialize DAC with the default precision
	UINT32 size = 1;
	INT32 precisions[1];
	if (!DA_GetAvailablePrecisionsForChannel((DA_CHANNEL)Get__channel(pMngObj), precisions, size)) {
		hr = CLR_E_INVALID_OPERATION;
		return;
	}
	if (!DA_Initialize((DA_CHANNEL)Get__channel(pMngObj), precisions[0])) {
		hr = CLR_E_INVALID_OPERATION;
		return;
	}
	//debug check : the managed field _signalInfo is ready to contain the native struct SignalInfo
	CLR_RT_HeapBlock_Array *pHBA_SignalInfo = NULL;
	pHBA_SignalInfo =  pMngObj[ Library_Community_Hardware_AnalogSignalGenerator_Community_Hardware_AnalogSignalGenerator::FIELD___signalInfo ].DereferenceArray();
	if(!pHBA_SignalInfo) {
		hr = CLR_E_FAIL;
		return;
	}
	if (pHBA_SignalInfo->m_numOfElements * pHBA_SignalInfo->m_sizeOfElement < sizeof(AnalogSignalInfo)){
		hr = CLR_E_BUFFER_TOO_SMALL;
		return;
	}
	//end of debug check
	hr = ASG_Initialize(Get__channel(pMngObj));
}
void AnalogSignalGenerator::NativeDispose( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
	DA_Uninitialize((DA_CHANNEL)Get__channel(pMngObj));
}

void AnalogSignalGenerator::Start( CLR_RT_HeapBlock* pMngObj, INT32 repeatCount, HRESULT &hr )
{
	CLR_RT_HeapBlock_Array* pHBA_rawLevels =  pMngObj[ Library_Community_Hardware_AnalogSignalGenerator_Community_Hardware_AnalogSignalGenerator::FIELD___rawLevels].DereferenceArray();
	CLR_RT_HeapBlock_Array* pHBA_SignalInfo =  pMngObj[ Library_Community_Hardware_AnalogSignalGenerator_Community_Hardware_AnalogSignalGenerator::FIELD___signalInfo ].DereferenceArray();
	AnalogSignalInfo* signalInfo = (AnalogSignalInfo*) pHBA_SignalInfo->GetFirstElement();

	//check LevelTimes not null or empty
	if(!pHBA_rawLevels) {
		hr = CLR_E_INVALID_OPERATION;
		return;
	}
	if (pHBA_rawLevels->m_numOfElements == 0){
		return;
	}
	//pin heap blocks to keep valid pointers during signal generation
	pHBA_SignalInfo->Pin();
	//keep original status to not unpin a rawLevels array shared by several signals
	signalInfo->rawLevelsBlockPinned = pHBA_rawLevels->IsPinned();
	pHBA_rawLevels->Pin();
	//keep a pointer to pinned array
	signalInfo->rawLevels = (UINT16*)pHBA_rawLevels->GetFirstElement();
	signalInfo->length = pHBA_rawLevels->m_numOfElements;
	signalInfo->index = 0;
	signalInfo->repeatCount = repeatCount;
	signalInfo->repeatforever = (repeatCount == 0);
	signalInfo->status = SG_PENDING;
	signalInfo->channel = (DA_CHANNEL)Get__channel(pMngObj);

	//hr = ASG_Start(Get__channel(pMngObj), Get__clockDivisor(pMngObj), SampleTimeTick, signalInfo); 
	hr = ASG_StartDMA(Get__channel(pMngObj), Get__clockDivisor(pMngObj), signalInfo->rawLevels, signalInfo->length); 
}

void AnalogSignalGenerator::Start( CLR_RT_HeapBlock* pMngObj, UINT64 param0, INT32 param1, HRESULT &hr )
{
	hr = CLR_E_NOTIMPL;
}

void AnalogSignalGenerator::Stop( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
	ASG_Stop(Get__channel(pMngObj));
	//unpin heap blocks
	CLR_RT_HeapBlock_Array* pHBA_SignalInfo =  pMngObj[ Library_Community_Hardware_AnalogSignalGenerator_Community_Hardware_AnalogSignalGenerator::FIELD___signalInfo ].DereferenceArray();
	if (!pHBA_SignalInfo->IsPinned()) {
		//signal already stopped
		return;
	}
	AnalogSignalInfo* signalInfo = (AnalogSignalInfo*) pHBA_SignalInfo->GetFirstElement();
	signalInfo->status = 0;
	//unpin the heap block only if we are responsible of the pinning
	if (!signalInfo->rawLevelsBlockPinned) {
		CLR_RT_HeapBlock_Array* pHBA_rawLevels =  pMngObj[ Library_Community_Hardware_AnalogSignalGenerator_Community_Hardware_AnalogSignalGenerator::FIELD___rawLevels].DereferenceArray();
		pHBA_rawLevels->Unpin();
	}
	pHBA_SignalInfo->Unpin();
}

INT32 AnalogSignalGenerator::GetChannelCount( HRESULT &hr )
{
	return ASG_ChannelCount();
}

INT32 AnalogSignalGenerator::GetPin( INT32 channel, HRESULT &hr ) {
	return ASG_GetPinForChannel(channel);
}

UINT16 AnalogSignalGenerator::GetMinClockDivisor( HRESULT &hr )
{
	return ASG_GetMinClockDivisor();
}

