////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Nicolas Gallerand
//
//  *** Analog signal generator Driver for STM32F4 ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __arm
#undef _WIN32
//#define PLATFORM_ARM_FEZCerberus	1
#include <tinyhal.h>
#include <cpu.h>
#include "Targets\Native\STM32F4\DeviceCode\stm32f4xx.h"
#include <AnalogSignalGenerator_decl.h>
#include <platform_selector.h>
#define uint16_t	UINT16
#define uint32_t	UINT32


//timer clock source
//#if SYSTEM_APB1_CLOCK_HZ == SYSTEM_CYCLE_CLOCK_HZ
//#define TIM_CLK_HZ SYSTEM_APB1_CLOCK_HZ
//#else
#define TIM_CLK_HZ (SYSTEM_APB1_CLOCK_HZ * 2)
//#endif

//global variables
static HAL_CALLBACK_FPN g_channelFunction[2];
static void* g_channelFunctionParam[2];

//Timer ISR
static void OnTimerUpdate(void* param) {
	GLOBAL_LOCK(irq);
	if (DAC->SR & DAC_SR_DMAUDR1) {
		if (g_channelFunction[0] != 0)
			g_channelFunction[0](0);
	}
	//reset interrupt flag
	if (TIM6->SR & TIM_SR_UIF) {
		if (g_channelFunction[0] != 0)
			g_channelFunction[0](g_channelFunctionParam[0]);
		TIM6->SR = 0;
	}
	else if (TIM7->SR & TIM_SR_UIF) {
		if (g_channelFunction[1] != 0)
			g_channelFunction[1](g_channelFunctionParam[1]);
		TIM7->SR = 0;
	}
}

//Get Analog signal generator channel count for this hardware
UINT32		ASG_ChannelCount	() 
{
	return 2;
}
GPIO_PIN ASG_GetPinForChannel(int channel)
{
	if (channel == 0)
		return 4; //PA4
	else if (channel == 1)
		return 5;  //PA5
	else
		return GPIO_PIN_NONE;
}
//Get the min clock divisor from Slow Clock to define max output sample clock for this hardware
UINT16		ASG_GetMinClockDivisor	()
{
	return 168;
}
//Initialize signal timer
HRESULT		ASG_Initialize(int channel){
	// enable DAC and DMA1 clock
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	if (channel == 0) {
		RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
		TIM6->CR1 = 0;
		TIM6->CR2 |= TIM_CR2_MMS_1;
		TIM6->PSC = 0;
		// enable DAC channel 1
		CPU_GPIO_DisablePin(4, RESISTOR_DISABLED, 0, GPIO_ALT_MODE_1);
		//trigger on TIM6
		DAC->CR |= DAC_CR_DMAEN1;
		DAC->CR |= DAC_CR_TEN1;
		DAC->SR = DAC_SR_DMAUDR1 ;              // Write 1 to clear flag

		DAC->DHR12L1 = 0xFFFF / 2;
		DAC->CR |= DAC_CR_EN1;
	}
	else if (channel == 1){
		RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
		TIM7->CR1 = 0;
		TIM7->CR2 |= TIM_CR2_MMS_1;
		TIM7->DIER = TIM_DIER_UIE; // enable update interrupt
		TIM7->PSC = 0;
		// enable DAC channel 2
		CPU_GPIO_DisablePin(5, RESISTOR_DISABLED, 0, GPIO_ALT_MODE_1);
		//trigger on TIM7
		DAC->CR |= DAC_CR_TEN2 | DAC_CR_TSEL2_1;
		DAC->DHR12L2 = 0xFFFF / 2;
		DAC->CR |= DAC_CR_EN2;
	}
	else
		return CLR_E_INVALID_OPERATION;
	return S_OK;
}

//Unitialize DAC
void ASG_Uninitialize(int channel)
{
	if (channel == 0) {
		// disable TIM6 and clock
		TIM6->CR1 &= ~TIM_CR1_CEN; 
		RCC->APB1ENR &= ~RCC_APB1ENR_TIM6EN;
		// disable DAC 1
		DAC->CR &= ~DAC_CR_EN1;
	}
	else if (channel == 1){
		// disable TIM7 and clock
		TIM7->CR1 &= ~TIM_CR1_CEN;
		RCC->APB1ENR &= ~RCC_APB1ENR_TIM7EN;
		// disable DAC 2
		DAC->CR &= ~DAC_CR_EN2;
	}
	// disable DAC clock if all channels off
	if ((DAC->CR & (DAC_CR_EN1 | DAC_CR_EN2)) == 0) {
		RCC->APB1ENR &= ~RCC_APB1ENR_DACEN;
	}
}

HRESULT	ASG_Start(int channel, int clockDivisor, HAL_CALLBACK_FPN timerTick, void* param)
{
	if (channel == 0) {
		if (!CPU_INTC_ActivateInterrupt(TIM6_DAC_IRQn, OnTimerUpdate, NULL))
			return CLR_E_FAIL;
		TIM6->ARR = clockDivisor - 1; 
		TIM6->CR1 |= TIM_CR1_CEN; // enable timer
		//TIM6->EGR = TIM_EGR_UG; // enforce first update
	}
	else if (channel == 1) {
		if (!CPU_INTC_ActivateInterrupt(TIM7_IRQn, OnTimerUpdate, NULL))
			return CLR_E_FAIL;
		TIM7->ARR = clockDivisor - 1; 
		TIM7->CR1 |= TIM_CR1_CEN; // enable timer
	}
	else {
		return CLR_E_INVALID_OPERATION;
	}
	g_channelFunction[channel] = timerTick;
	g_channelFunctionParam[channel] = param;

	return S_OK;
}
void ASG_Stop(int channel) {
	GLOBAL_LOCK(irq);
	if (channel == 0) {
		TIM6->CR1 &= ~TIM_CR1_CEN; // disable timer
		CPU_INTC_DeactivateInterrupt(TIM6_DAC_IRQn);
	}
	else if (channel == 1) {
		TIM7->CR1 &= ~TIM_CR1_CEN; // disable timer
		CPU_INTC_DeactivateInterrupt(TIM7_IRQn);
	}
	else {
		return;
	}
	g_channelFunction[channel] = 0;
	g_channelFunctionParam[channel] = 0;
}
void ASG_StopDMA(int channel) {
	if (channel == 0) {
		// Disable DMA 1 stream 5
		DMA1_Stream5->CR &= ~DMA_SxCR_EN ;
		////wait for stream stop
		//int timeoutCount = 0;
		//while (DMA1_Stream5->CR & DMA_SxCR_EN) {
		//	if (timeoutCount>100000) 
		//		return;
		//}
		TIM6->CR1 &= ~TIM_CR1_CEN; // disable timer
	}
	else if (channel == 1) {
		TIM7->CR1 &= ~TIM_CR1_CEN; // disable timer
		CPU_INTC_DeactivateInterrupt(TIM7_IRQn);
	}
	else {
		return;
	}
	g_channelFunction[channel] = 0;
	g_channelFunctionParam[channel] = 0;
}

//Start the DMA signal generation from the specified buffer
HRESULT ASG_StartDMA(int channel, int clockDivisor, UINT16* buffer, int bufferLen) {
	if (channel == 0) {
		if (!CPU_INTC_ActivateInterrupt(TIM6_DAC_IRQn, OnTimerUpdate, NULL))
			return CLR_E_FAIL;
		// Disable DMA 1 stream 5
		DMA1_Stream5->CR &= ~DMA_SxCR_EN ;
		//wait for stream stop
		int timeoutCount = 0;
		while (DMA1_Stream5->CR & DMA_SxCR_EN) {
			if (timeoutCount>100000) 
				return CLR_E_FAIL;
		}
		//Reset previous config (write ones to clear bits)
		DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; 
		//target DAC reg
		DMA1_Stream5->PAR = (UINT32)&DAC->DHR12L1;
		//source memory
		DMA1_Stream5->M0AR = (UINT32)buffer;
		DMA1_Stream5->NDTR = bufferLen ;
		UINT32 CR = 0;
		//channel 7 (DAC1)
		CR |= DMA_SxCR_CHSEL_2 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0;
		//priority medium
		CR |= DMA_SxCR_PL_0;
		//mem to DAC
		CR |= DMA_SxCR_DIR_0;
		//DAC data size 16bits
		CR |= DMA_SxCR_PSIZE_0;
		//source mem ptr data size 16bits
		CR |= DMA_SxCR_MSIZE_0;
		//circular auto incremented mem ptr
		CR |= DMA_SxCR_CIRC;
		CR |= DMA_SxCR_MINC;
		DMA1_Stream5->CR = CR;

		//FIFO mode
		DMA1_Stream5->FCR = DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0;

		//activate the stream
		DMA1_Stream5->CR |= DMA_SxCR_EN;

		TIM6->ARR = clockDivisor - 1; 
		TIM6->CR1 |= TIM_CR1_CEN; // enable timer
		//TIM6->EGR = TIM_EGR_UG; // enforce first update
	}
	else if (channel == 1) {
		TIM7->ARR = clockDivisor - 1; 
		TIM7->CR1 |= TIM_CR1_CEN; // enable timer
	}
	else {
		return CLR_E_INVALID_OPERATION;
	}
	//// Chan 7, 16-bit wide, Medium priority, memory increments
	//// Write ones to clear bits
	//DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; 

	//DMAStream->CR = DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_2 | DMA_SxCR_PL_0 |
	//	DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_CIRC ;
	//DMAStream->PAR = DAC->DHR12R1;
	//DMAStream->M0AR = CONVERT_PTR(Sine_values);
	//DMAStream->FCR = 0x05 ; //DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
	//DMAStream->NDTR = 100 ;

	//DAC->DHR12R1 = DAC_DHR12R1_DACC1DHR;
	//DAC->SR = DAC_SR_DMAUDR1 ;              // Write 1 to clear flag
	//DAC->CR = DAC_CR_TEN1 | DAC_CR_EN1 ;                    // Enable DAC

}


