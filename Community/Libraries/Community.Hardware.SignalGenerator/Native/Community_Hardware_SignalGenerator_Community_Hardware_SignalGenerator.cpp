////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Nicolas Gallerand
//
//  *** Signal generator for NETMF ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Community_Hardware_SignalGenerator.h"
#include "Community_Hardware_SignalGenerator_Community_Hardware_SignalGenerator.h"
#include <tinyhal.h>
#include <cpu.h>
//#include <platform_selector.h>
#include <tinyCLR_Runtime.h>
using namespace Community::Hardware;


//Instrument handler execution time in SignalInfo structure readable in managed field SignalGenerator._signalInfo 
#define SG_INSTRUMENTED


//status bits
#define SG_READY	1
#define SG_ACTIVE	2
#define SG_PENDING	3
#define SG_ERROR	4

//Warning: this structure is accessed by the managed code. Do not modify
struct SignalInfo {
	//repetition decounter
	INT32 repeatCount;					
	//current levelTimeArray index
	UINT32	index;
	//status flags
	UINT32	status;
	//generation error monitoring
	INT32	delayCount;
	INT32	delayCount_max;
#ifdef SG_INSTRUMENTED
	//debug instrumentation
	INT32 completionMinTime;
	INT32 completionMaxTime;
#endif
	//never ending repetition
	bool repeatforever;
	//output invert
	BOOL invert;	
	//completion structure
	HAL_COMPLETION completion;
	//pin
	GPIO_PIN pin;
	//level times array
	UINT32	*levelTimeArray;
	//levelTimeArray length
	UINT32	length;
	//last signal toggle time, accurate base to set next toggle time 
	UINT64	time;
	//last rising edge time (signal or carrier), base to set carrier toggle time
	UINT64	risingEdgeTime;
	//carrier settings
	BOOL	setCarrierLowLevel;
	INT32	carrierHighTime;
	INT32	carrierPeriod;
	//indicate the original pinning of levelTimes array heap block
	BOOL	levelTimesBlockPinned;
} ;


static void SignalToggleCompletionHandler(void* param) {
	GLOBAL_LOCK(irq);
#ifdef SG_INSTRUMENTED
	UINT64 t0 = HAL_Time_CurrentTicks();
#endif

	SignalInfo* signalInfo = (SignalInfo*)param;

	//error check
	if (signalInfo->delayCount >= signalInfo->delayCount_max) {
		//signal too fast to be properly generated : set error flag and stop generation
		signalInfo->status = SG_ERROR;
		//restore initial state
		CPU_GPIO_SetPinState(signalInfo->pin, 0 ^ signalInfo->invert); 
		return;
	}
	//end of buffer?
	if (signalInfo->index >= signalInfo->length) {
		//reset signal (a few CPU cycle overhead here only for last level, negligible compared to 1µs resolution) 
		signalInfo->index = 0;
		signalInfo->delayCount = 0;
		signalInfo->repeatCount--;
		if (signalInfo->repeatCount == 0 && !signalInfo->repeatforever) {
			//restore initial state
			CPU_GPIO_SetPinState(signalInfo->pin, 0 ^ signalInfo->invert); 
			//no more completion
			signalInfo->status = SG_READY;
			return;
		}
	}

	//toggle pin
	if ((signalInfo->index & 1)){
		//enforce low level
		CPU_GPIO_SetPinState(signalInfo->pin, 0 ^ signalInfo->invert); 
		//signalInfo->port->ODR &= ~signalInfo->pinMask;
		//enqueue next high level
		signalInfo->time += signalInfo->levelTimeArray[signalInfo->index];
		signalInfo->risingEdgeTime = signalInfo->time;
		signalInfo->setCarrierLowLevel = FALSE;
		signalInfo->index++;
		//signal generation error check
		INT64 delay =  signalInfo->time - HAL_Time_CurrentTicks();
		if(delay < 2) {
			//not enough time to enqueue safely the next toggle
			signalInfo->delayCount ++;
		}
		signalInfo->completion.EnqueueTicks(signalInfo->time);
	}
	else {
		//enforce high level
		CPU_GPIO_SetPinState(signalInfo->pin, 1 ^ signalInfo->invert); 
		UINT64 nextSignalLowLevelTime = signalInfo->time + signalInfo->levelTimeArray[signalInfo->index];
		if (signalInfo->carrierHighTime == 0) {
			//no carrier -> enqueue next signal low level
			signalInfo->time = nextSignalLowLevelTime;
			signalInfo->index++;
			//signal generation error check
			INT64 delay =  signalInfo->time - HAL_Time_CurrentTicks();
			if(delay < 2) {
				//not enough time to enqueue safely the next toggle
				signalInfo->delayCount ++;
			}
			signalInfo->completion.EnqueueTicks(signalInfo->time);
		}
		else {
			//set next carrier toggle time
			UINT64 nextToggleTime = 0;
			if (signalInfo->setCarrierLowLevel) {
				//enforce carrier low level
				CPU_GPIO_SetPinState(signalInfo->pin, 0 ^ signalInfo->invert); 
				//set next time
				signalInfo->risingEdgeTime += signalInfo->carrierPeriod;
				nextToggleTime = signalInfo->risingEdgeTime;
				signalInfo->setCarrierLowLevel = FALSE;
			}
			else {
				//high carrier level (already set): set next toggle time
				nextToggleTime = signalInfo->risingEdgeTime + signalInfo->carrierHighTime;
				signalInfo->setCarrierLowLevel = TRUE;
			}
			//enqueue next toggle for carrier if there is enough time
			if (nextToggleTime < nextSignalLowLevelTime) {
				//signal generation error check
				INT64 delay =  nextToggleTime - HAL_Time_CurrentTicks();
				if(delay < 2) {
					//not enough time to enqueue safely the next toggle
					signalInfo->delayCount ++;
				}
				signalInfo->completion.EnqueueTicks(nextToggleTime);
			}
			else {
				//enqueue next signal low level
				signalInfo->time= nextSignalLowLevelTime;
				signalInfo->index++;
				//signal generation error check
				INT64 delay =  signalInfo->time - HAL_Time_CurrentTicks();
				if(delay < 2) {
					//not enough time to enqueue safely the next toggle
					signalInfo->delayCount ++;
				}
				signalInfo->completion.EnqueueTicks(signalInfo->time);
			}
		}
	}
	//update status at first call (and every next times...)
	signalInfo->status = SG_ACTIVE;

#ifdef SG_INSTRUMENTED
	t0 = HAL_Time_CurrentTicks() - t0;
	if (t0 > signalInfo->completionMaxTime)
		signalInfo->completionMaxTime = t0;
	if (t0 < signalInfo->completionMinTime)
		signalInfo->completionMinTime = t0;
#endif

};

void SignalGenerator::NativeInitialize( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
	//debug check : the managed field _signalInfo is ready to contain the native struct SignalInfo
	CLR_RT_HeapBlock_Array *pHBA_SignalInfo = NULL;
	pHBA_SignalInfo =  pMngObj[ Library_Community_Hardware_SignalGenerator_Community_Hardware_SignalGenerator::FIELD___signalInfo ].DereferenceArray();
	if(!pHBA_SignalInfo) {
		hr = CLR_E_FAIL;
		return;
	}
	if (pHBA_SignalInfo->m_numOfElements * pHBA_SignalInfo->m_sizeOfElement < sizeof(SignalInfo)){
		hr = CLR_E_BUFFER_TOO_SMALL;
		return;
	}
	//end of debug check
	//configure pin for output
	CPU_GPIO_EnableOutputPin(Get__pin(pMngObj), Get__invert(pMngObj));
}

INT8 SignalGenerator::get_PinState( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
	return CPU_GPIO_GetPinState(SignalGenerator::Get__pin(pMngObj));
}
void SignalGenerator::set_PinState( CLR_RT_HeapBlock* pMngObj, INT8 state, HRESULT &hr )
{
	CPU_GPIO_SetPinState(SignalGenerator::Get__pin(pMngObj), state);
}

void SignalGenerator::NativeDispose( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
	GLOBAL_LOCK(irq);
	//unpin heap blocks
	Stop(pMngObj, hr);
	//disable pin
	CPU_GPIO_DisablePin(Get__pin(pMngObj), RESISTOR_DISABLED, 0, GPIO_ALT_PRIMARY);
}
void SignalGenerator::Start( CLR_RT_HeapBlock* pMngObj, INT32 param0, double param1, double param2, HRESULT &hr )
{
	Start(pMngObj,0, param0,param1,param2, hr);
}

void SignalGenerator::Start( CLR_RT_HeapBlock* pMngObj, UINT64 startTime, INT32 repeatCount, double carrierFrequency, double dutyCycle, HRESULT &hr )
{
	//range check (carrierFrequency == 0 && dutyCycle == 0 is allowed) 
	if (carrierFrequency < 0 || carrierFrequency > 50000 || dutyCycle < 0 || dutyCycle >= 1) {
		hr = CLR_E_OUT_OF_RANGE;
		return;
	}
	int	carrierPeriod = 0;
	int carrierHighTime = 0; 
	if (carrierFrequency != 0) {
		carrierPeriod = 1000000 / carrierFrequency;
		//carrier high time rounded to nearest
		carrierHighTime = (int)(0.5 + dutyCycle * carrierPeriod);
		//minimal check
		if (carrierHighTime < 1) {
			hr = CLR_E_OUT_OF_RANGE;
			return;
		}
	}

	CLR_RT_HeapBlock_Array* pHBA_LevelTimes =  pMngObj[ Library_Community_Hardware_SignalGenerator_Community_Hardware_SignalGenerator::FIELD___levelTimes].DereferenceArray();
	CLR_RT_HeapBlock_Array* pHBA_SignalInfo =  pMngObj[ Library_Community_Hardware_SignalGenerator_Community_Hardware_SignalGenerator::FIELD___signalInfo ].DereferenceArray();
	SignalInfo* signalInfo = (SignalInfo*) pHBA_SignalInfo->GetFirstElement();

	//check LevelTimes not null or empty
	if(!pHBA_LevelTimes) {
		hr = CLR_E_INVALID_OPERATION;
		return;
	}
	if (pHBA_LevelTimes->m_numOfElements == 0){
		return;
	}
	//pin heap blocks to keep valid pointers during signal generation
	pHBA_SignalInfo->Pin();
	//keep original status to not unpin a levelTimes array shared by several signals
	signalInfo->levelTimesBlockPinned = pHBA_LevelTimes->IsPinned();
	pHBA_LevelTimes->Pin();

	//init Signal info
#ifdef SG_INSTRUMENTED
	signalInfo->completionMaxTime = 0;
	signalInfo->completionMinTime = 0xFFFF;
#endif
	signalInfo->status = SG_PENDING;
	signalInfo->completion.InitializeForISR(&SignalToggleCompletionHandler, signalInfo);
	signalInfo->pin = Get__pin(pMngObj);
	signalInfo->index = 0;
	signalInfo->repeatCount = repeatCount;
	signalInfo->repeatforever = (repeatCount == 0);
	signalInfo->invert = Get__invert(pMngObj);
	//keep a pointer to pinned array for completion handler
	signalInfo->levelTimeArray = (UINT32*)pHBA_LevelTimes->GetFirstElement();
	signalInfo->length = pHBA_LevelTimes->m_numOfElements;
	//carrier params defined upper
	signalInfo->carrierPeriod = carrierPeriod;
	signalInfo->carrierHighTime = carrierHighTime; 
	//init security check for too fast signal
	signalInfo->delayCount = 0;
	//delayCount_max must be > 1 because the first toggle is always delayed if synchronized with another signal
	//delayCount_max must be < signalInfo->length to be reached
	signalInfo->delayCount_max = MIN(5, MAX(2, signalInfo->length / 2));

	//set the first toggle time
	if (startTime == 0) {
		//immediate start
		signalInfo->time = HAL_Time_CurrentTicks();
		signalInfo->risingEdgeTime = signalInfo->time;
		SignalToggleCompletionHandler(signalInfo);
	}
	else {
		UINT64 localTime;
		INT64 dt0 = 0;
		for (int i=0; i<5; i++) {
			signalInfo->time = HAL_Time_CurrentTicks();
			localTime = Time_GetLocalTime();
			//repeat at least 2 times to ensure there is no extra count between the time methods calls
			INT64 dt = HAL_Time_CurrentTicks() - signalInfo->time;
			if (dt <= dt0) break;
			dt0 = dt;
		}
		//note: localTime and signalInfo->time are not the same time but the difference is repeatable and it's OK for synchronization
		INT64 delayTime = startTime - localTime;
		if (delayTime <= 0) {
			//immediate start
			signalInfo->time = HAL_Time_CurrentTicks();
			signalInfo->risingEdgeTime = signalInfo->time;
			SignalToggleCompletionHandler(signalInfo);
		}
		else {
			//delayed start need to be converted to ticks
			UINT64 timeUnitBykTicks = HAL_Time_TicksToTime(1000);
			UINT64 delayTicks = (delayTime * 1000 ) / timeUnitBykTicks; 
			signalInfo->time += delayTicks;
			signalInfo->risingEdgeTime = signalInfo->time;
			signalInfo->completion.EnqueueTicks(signalInfo->time);
		}
	}
}

void SignalGenerator::Stop( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
	GLOBAL_LOCK(irq);
	//unpin heap blocks
	CLR_RT_HeapBlock_Array* pHBA_SignalInfo =  pMngObj[ Library_Community_Hardware_SignalGenerator_Community_Hardware_SignalGenerator::FIELD___signalInfo ].DereferenceArray();
	if (!pHBA_SignalInfo->IsPinned()) {
		//signal already stopped
		return;
	}
	SignalInfo* signalInfo = (SignalInfo*) pHBA_SignalInfo->GetFirstElement();
	signalInfo->completion.Abort();
	signalInfo->status = 0;
	//unpin the heap block only if we are responsible of the pinning
	if (!signalInfo->levelTimesBlockPinned) {
		CLR_RT_HeapBlock_Array* pHBA_LevelTimes =  pMngObj[ Library_Community_Hardware_SignalGenerator_Community_Hardware_SignalGenerator::FIELD___levelTimes].DereferenceArray();
		pHBA_LevelTimes->Unpin();
	}
	pHBA_SignalInfo->Unpin();
}