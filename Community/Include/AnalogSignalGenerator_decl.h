////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Nicolas Gallerand
//
//  *** Analog signal generator Driver Interface ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRIVERS_ANALOGSIGNALGENERATOR_DECL_H_
#define _DRIVERS_ANALOGSIGNALGENERATOR_DECL_H_ 1

//Get Analog signal generator channel count for this hardware
UINT32		ASG_ChannelCount	();
//Get the output pin for the specified channel
GPIO_PIN ASG_GetPinForChannel(int channel);
//Get the min clock divisor from System Clock to define max output sample clock for this hardware
UINT16		ASG_GetMinClockDivisor	();

//Initialize the signal generation for the specified channel
HRESULT ASG_Initialize	(int channel);
//Unitialize signal timer
void ASG_Uninitialize(int channel);

//Start the DMA signal generation from the specified buffer with the specified sample clock divisor from system clock
HRESULT ASG_StartDMA(int channel, int clockDivisor, UINT16* buffer, int bufferLen);

void ASG_Stop(int channel);

//Start timer
HRESULT ASG_Start(int channel, int clockDivisor, HAL_CALLBACK_FPN timerTick, void* param);

#endif // _DRIVERS_ANALOGSIGNALGENERATOR_DECL_H_
