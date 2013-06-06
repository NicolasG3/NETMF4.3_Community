////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Nicolas Gallerand
//
//  *** Quadrature encoder Driver for STM32F4  ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <tinyhal.h>
#include <cpu.h>
#include "Targets\Native\STM32F4\DeviceCode\stm32f4xx.h"
#include <QuadratureEncoder_decl.h>

#define uint16_t	UINT16
#define uint32_t	UINT32

static QED_EVENT g_eventCallback[2];
static int g_pulseLength[2];
static bool g_isOutputActive[2];
static HAL_COMPLETION g_completion[2];
static int g_IOStatus[2];

//Event sink function
typedef void (*QED_EVENT) (int channel, int IOIndex, int count, BOOL overlap);

//Reset the specified output state
static void QED_ResetOutput(int IOIndex) {
	if (IOIndex == 0) {
		UINT16 ccmr2 =  TIM2->CCMR2;
		UINT16 ccmr2_reset =  ccmr2;
		ccmr2_reset &= ~TIM_CCMR2_OC3M;
		ccmr2_reset |= TIM_CCMR2_OC3M_2;
		TIM2->CCMR2 = ccmr2_reset;
		//restore inital settings
		TIM2->CCMR2 = ccmr2;
	}
	else if (IOIndex == 1) {
		UINT16 ccmr2 =  TIM2->CCMR2;
		UINT16 ccmr2_reset =  ccmr2;
		ccmr2_reset &= ~TIM_CCMR2_OC4M;
		ccmr2_reset |= TIM_CCMR2_OC4M_2;
		TIM2->CCMR2 = ccmr2_reset;
		//restore inital settings
		TIM2->CCMR2 = ccmr2;
	}
	g_isOutputActive[IOIndex] = false;
}

static void PulseCompletionHandler(void* param) {
	GLOBAL_LOCK(irq);
	int IOIndex = (int) param;
	QED_ResetOutput(IOIndex);
}

//Interrupt handler 
static void STM32F4_TIM2_Interrupt(void* param)
{
	GLOBAL_LOCK(irq);

	//capture/compare on PA2
	if ((TIM2->SR & TIM_SR_CC3IF) != 0) {
		if (g_eventCallback[0] != 0) {
			BOOL overlap = FALSE;
			if ((TIM2->SR & TIM_SR_CC3OF) != 0) {
				TIM2->SR &= ~TIM_SR_CC3OF;
				overlap = TRUE;
			}
			if (g_IOStatus[0] == IOStatus_OutputCompare){
				if (g_isOutputActive[0])
					overlap = TRUE;
				g_isOutputActive[0] = true;
			}
			g_eventCallback[0](0, 0, TIM2->CCR3, overlap);
		}
		//completion to set the pulse length
		if (g_pulseLength[0]>0)
			g_completion[0].EnqueueDelta(g_pulseLength[0]);
		//reset interrupt flag for next time
		TIM2->SR &= ~TIM_SR_CC3IF;
	}
	//capture/compare on PA3
	if ((TIM2->SR & TIM_SR_CC4IF) != 0) {
		if (g_eventCallback[1] != 0) {
			BOOL overlap = FALSE;
			if ((TIM2->SR & TIM_SR_CC4OF) != 0) {
				TIM2->SR &= ~TIM_SR_CC4OF;
				overlap = TRUE;
			}
			if (g_IOStatus[1] == IOStatus_OutputCompare){
				if (g_isOutputActive[1])
					overlap = TRUE;
				g_isOutputActive[1] = true;
			}
			g_eventCallback[1](0, 1, TIM2->CCR4, overlap);
		}
		//completion to set the pulse length
		if (g_pulseLength[1]>0)
			g_completion[1].EnqueueDelta(g_pulseLength[1]);
		//reset interrupt flag for next time
		TIM2->SR &= ~TIM_SR_CC4IF;
	}
};


//Get Quadrature Encoder channel count for this hardware
UINT32		QED_ChannelCount	() {
	return 1;
}
//Get the input pins for the specified channel
HRESULT		QED_GetPinsForChannel	(int channel, GPIO_PIN* pinA, GPIO_PIN* pinB) {
	if (channel != 0)
		return FALSE;
	*pinA = 0;//PA0
	*pinB = 1;//PA1
	return S_OK;
}
//Get IO count for the specified Quadrature Encoder channel (IO are used for InputCapture or OutputCompare)
UINT32		QED_GetIOCountForChannel	(int channel) {
	if (channel == 0)
		return 2;
	return 0;
}
//Get the hardware pin for the specified IO index of the specified channel
GPIO_PIN	QED_GetIOPinForChannel	(int channel, int IOIndex) {
	if (channel == 0) {
		if (IOIndex == 0)
			return 2;//PA2
		if (IOIndex == 1)
			return 3;//PA3
	}
	return GPIO_PIN_NONE;
}
//Get the specified IO status (see #define IOStatus_xxx)
INT32	QED_GetIOStatus	(int channel, int IOIndex) {
	if (channel != 0)
		return IOStatus_None;
	if (IOIndex < 0 || IOIndex > 1)
		return IOStatus_None;
	return g_IOStatus[IOIndex];
}

//Initialize the specified channel. Reserve A and B pins (not the IO pins)
//countMode X1=1, X2=2, X4=3
HRESULT	QED_Initialize		(int channel, int countMode) {
	if (channel != 0)
		return CLR_E_INVALID_PARAMETER;
	if (countMode < 1 || countMode > 3)
		return CLR_E_INVALID_PARAMETER;
	//Reserve PA0 and PA1
	if (CPU_GPIO_ReservePin(0, TRUE) == false) 
		return CLR_E_PIN_UNAVAILABLE;
	if (CPU_GPIO_ReservePin(1, TRUE) == false) 
		return CLR_E_PIN_UNAVAILABLE;
	//Power port A (should be already done in bootstrap) and TIM2
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	//PA0 and PA1 mode alternate: 10b
	GPIOA->MODER &= ~(GPIO_MODER_MODER1 | GPIO_MODER_MODER0);
	GPIOA->MODER |= (GPIO_MODER_MODER1_1 | GPIO_MODER_MODER0_1);
	//Alternate function AF1 : TIM2 input for PA1 and PA0
	GPIOA->AFR[0] &= ~0x000000FF;
	GPIOA->AFR[0] |= 0x00000011;
	//pull up
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR1);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR1_0);
	TIM2->CR2 = 0;
	//external clock enable
	TIM2->SMCR |= TIM_SMCR_ECE;
	//Encoder X mode count 
	TIM2->SMCR &= ~TIM_SMCR_SMS;
	TIM2->SMCR |= countMode;
	//if (countMode == 3)
	//	TIM2->SMCR |= TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0;	//X4
	//else 
	//	TIM2->SMCR |= TIM_SMCR_SMS_1;	//X2 on PA0
	//filter N=8
	TIM2->SMCR &= ~TIM_SMCR_ETF;
	TIM2->SMCR |= TIM_SMCR_ETF_1 | TIM_SMCR_ETF_0;
	//no polarity invertion, rising edge
	TIM2->CCER = 0;
	//Count up to 32bits
	TIM2->ARR = 0xFFFFFFFF;

	for (int IOIndex = 0; IOIndex < 2; IOIndex++) {
		g_completion[IOIndex].InitializeForISR(&PulseCompletionHandler, (void*)IOIndex);
		g_pulseLength[IOIndex] = 0;
		g_IOStatus[IOIndex] = IOStatus_None; 
	}
	//activate interrupt and enable counter
	CPU_INTC_ActivateInterrupt(TIM2_IRQn, STM32F4_TIM2_Interrupt, 0);
	TIM2->CR1 |= TIM_CR1_CEN;

	return S_OK;
}
//Unitialize the specified channel
void	QED_Uninitialize	(int channel) {
	if (channel !=0) 
		return CLR_E_INVALID_PARAMETER;	
	GLOBAL_LOCK(irq);
	//disable counter and interrupt
	QED_SetCountEnable(0, FALSE);
	CPU_INTC_DeactivateInterrupt(TIM2_IRQn);
	//release A and B pins
	CPU_GPIO_ReservePin(0, FALSE);
	CPU_GPIO_ReservePin(1, FALSE);
	for (int IOIndex = 0; IOIndex < 2; IOIndex++) {
		//release IO pins and discard capture/compare
		QED_ReleaseIO(0, IOIndex);
		g_completion[IOIndex].Uninitialize();
	}

}
//Get the actual count of the specified channel
int		QED_GetCount		(int channel) {
	if (channel != 0)
		return 0;
	return TIM2->CNT;
}
//Set the actual count of the specified channel
HRESULT	QED_SetCount		(int channel, int value) {
	if (channel != 0)
		return CLR_E_INVALID_PARAMETER;
	TIM2->CNT = value;
	return S_OK;
}
//Indicate if couting is enabled for the specified channel
BOOL	QED_GetCountEnable	(int channel) {
	if (channel != 0)
		return FALSE;
	if (TIM2->CR1  & TIM_CR1_CEN)
		return TRUE;
	return FALSE;
}

//Set if counting is enabled for the specified channel
//When not enabled, count value is kept unchanged regardless of A and B inputs
HRESULT	QED_SetCountEnable	(int channel, BOOL enable) {
	if (channel != 0)
		return CLR_E_INVALID_PARAMETER;
	if (enable == FALSE) {
		TIM2->CR1 &= ~TIM_CR1_CEN;
	}
	else {
		TIM2->CR1 |= TIM_CR1_CEN;
	}
	return S_OK;
}

//Initialize the specified IO for output compare, negative pulse if invert is true
//pulseLength=0 means infinite pulse
//Reserve the IO pin and set the output state at first call, support successive calls (do not set the output state if not first call)
HRESULT	QED_InitOutputCompare(int channel, int IOIndex, int value, int pulseLength, BOOL invert, QED_EVENT onOutputCompare) {
	if (channel != 0)
		return CLR_E_INVALID_PARAMETER;
	if (IOIndex < 0 || IOIndex > 1)
		return CLR_E_INVALID_PARAMETER;
	if (g_IOStatus[IOIndex] == IOStatus_InputCapture) 
		return CLR_E_PIN_UNAVAILABLE;
	if (g_IOStatus[IOIndex] == IOStatus_None) {
		GPIO_PIN IOPin = QED_GetIOPinForChannel(channel, IOIndex);
		if (CPU_GPIO_ReservePin(IOPin, TRUE) == false) 
			return CLR_E_PIN_UNAVAILABLE;
		g_IOStatus[IOIndex] = IOStatus_OutputCompare;
	}
	if (IOIndex == 0) {
		//disable capture/compare
		TIM2->CCER &= ~TIM_CCER_CC3E;
		//PA3 mode alternate: 10b
		GPIOA->MODER &= ~GPIO_MODER_MODER2;
		GPIOA->MODER |= GPIO_MODER_MODER2_1;
		//Alternate function AF1 : TIM2 CH3 for PA2
		GPIOA->AFR[0] &= ~0x00000F00;
		GPIOA->AFR[0] |= 0x00000100;
		//set compare on ch3
		TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
		//set output on match
		TIM2->CCMR2 &= ~TIM_CCMR2_OC3M;
		TIM2->CCMR2 |= TIM_CCMR2_OC3M_0;
		//polarity
		if (invert)
			TIM2->CCER |= TIM_CCER_CC3P;
		else 
			TIM2->CCER &= ~TIM_CCER_CC3P;
		//init comparison value
		TIM2->CCR3 = value;
		//enable output
		TIM2->CCER |= TIM_CCER_CC3E;
		//enable interrupt, no DMA
		TIM2->DIER |= TIM_DIER_CC3IE;
		TIM2->DIER &= ~TIM_DIER_CC3DE;
	}
	else if (IOIndex == 1) {
		//disable capture/compare
		TIM2->CCER &= ~TIM_CCER_CC4E;
		//PA3 mode alternate: 10b
		GPIOA->MODER &= ~GPIO_MODER_MODER3;
		GPIOA->MODER |= GPIO_MODER_MODER3_1;
		//Alternate function AF1 : TIM2 CH4 for PA3
		GPIOA->AFR[0] &= ~0x0000F000;
		GPIOA->AFR[0] |= 0x00001000;
		//set compare on ch4
		TIM2->CCMR2 &= ~TIM_CCMR2_CC4S;
		//set output on match
		TIM2->CCMR2 &= ~TIM_CCMR2_OC4M;
		TIM2->CCMR2 |= TIM_CCMR2_OC4M_0;
		//polarity
		if (invert)
			TIM2->CCER |= TIM_CCER_CC4P;
		else 
			TIM2->CCER &= ~TIM_CCER_CC4P;
		//init comparison value
		TIM2->CCR4 = value;
		//enable output
		TIM2->CCER |= TIM_CCER_CC4E;
		//enable interrupt, no DMA
		TIM2->DIER |= TIM_DIER_CC4IE;
		TIM2->DIER &= ~TIM_DIER_CC4DE;
	}
	//init output state at first call or if precedent setting was infinite pulse length
	if (g_pulseLength[IOIndex] == 0)
		QED_ResetOutput(IOIndex);
	g_pulseLength[IOIndex] = pulseLength;
	g_eventCallback[IOIndex] = onOutputCompare;
	return S_OK;
}
//Initialize the specified IO for input capture on raising edge or falling edge if invert is true
//Reserve the IO pin at first call
HRESULT	QED_InitInputCapture(int channel, int IOIndex, BOOL invert, QED_EVENT onInputCapture) {
	if (channel != 0)
		return CLR_E_INVALID_PARAMETER;
	if (IOIndex < 0 || IOIndex > 1)
		return CLR_E_INVALID_PARAMETER;
	if (g_IOStatus[IOIndex] == IOStatus_OutputCompare) 
		return CLR_E_PIN_UNAVAILABLE;
	if (g_IOStatus[IOIndex] == IOStatus_None) {
		if (CPU_GPIO_ReservePin(QED_GetIOPinForChannel(channel, IOIndex), TRUE) == false) 
			return CLR_E_PIN_UNAVAILABLE;
		g_IOStatus[IOIndex] = IOStatus_InputCapture;
	}
	if (IOIndex == 0) {
		//disable capture/compare
		TIM2->CCER &= ~TIM_CCER_CC3E;
		//PA2 mode alternate: 10b
		GPIOA->MODER &= ~GPIO_MODER_MODER2;
		GPIOA->MODER |= GPIO_MODER_MODER2_1;
		//Alternate function AF1 : TIM2 CH3 for PA2
		GPIOA->AFR[0] &= ~0x00000F00;
		GPIOA->AFR[0] |= 0x00000100;
		//pull up
		GPIOA->PUPDR &= ~ GPIO_PUPDR_PUPDR2;
		GPIOA->PUPDR |= GPIO_PUPDR_PUPDR2_0;
		//capture on ch3
		TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
		TIM2->CCMR2 |= TIM_CCMR2_CC3S_0;
		//input filter N=8
		TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
		TIM2->CCMR2 |= TIM_CCMR2_IC3F_1 | TIM_CCMR2_IC3F_0;
		//no prescaler
		TIM2->CCMR2 &= ~TIM_CCMR2_IC3PSC;
		//non inverting/rising edge
		TIM2->CCER &= ~TIM_CCER_CC3NP;
		if (invert) {
			//inverting/falling edge
			TIM2->CCER &= ~TIM_CCER_CC3P;
		}
		else {
			//non inverting/rising edge
			TIM2->CCER &= ~TIM_CCER_CC3P;
		}
		//no DMA
		TIM2->DIER &= ~TIM_DIER_CC3DE;
		//enabled
		TIM2->CCER |= TIM_CCER_CC3E;
	} else if (IOIndex == 1) {
		//disable capture/compare
		TIM2->CCER &= ~TIM_CCER_CC4E;
		//PA3 mode alternate: 10b
		GPIOA->MODER &= ~GPIO_MODER_MODER3;
		GPIOA->MODER |= GPIO_MODER_MODER3_1;
		//Alternate function AF1 : TIM2 CH4 for PA3
		GPIOA->AFR[0] &= ~0x0000F000;
		GPIOA->AFR[0] |= 0x00001000;
		//pull up
		GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR3;
		GPIOA->PUPDR |= GPIO_PUPDR_PUPDR3_0;
		//capture on ch4 pins
		TIM2->CCMR2 &= ~TIM_CCMR2_CC4S;
		TIM2->CCMR2 |= TIM_CCMR2_CC4S_0;
		//input filter N=8
		TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
		TIM2->CCMR2 |= TIM_CCMR2_IC4F_1 | TIM_CCMR2_IC4F_0;
		//no prescaler
		TIM2->CCMR2 &= ~TIM_CCMR2_IC4PSC;
		//polarity
		TIM2->CCER &= ~TIM_CCER_CC4NP;
		if (invert) {
			//inverting/falling edge
			TIM2->CCER &= ~TIM_CCER_CC4P;
		}
		else {
			//non inverting/rising edge
			TIM2->CCER &= ~TIM_CCER_CC4P;
		}
		//no DMA
		TIM2->DIER &= ~TIM_DIER_CC4DE;
		//capture enabled
		TIM2->CCER |= TIM_CCER_CC4E;
	}
	g_pulseLength[IOIndex] = 0;
	g_eventCallback[IOIndex] = onInputCapture;
	return S_OK;
}
//Release the specified IO pin if reserved by QED_InitOutputCompare() or QED_InitInputCapture()
void	QED_ReleaseIO(int channel, int IOIndex) {
	if (channel != 0)
		return;
	if (IOIndex < 0 || IOIndex > 1)
		return;
	if (g_IOStatus[IOIndex] != IOStatus_None) {
		if (IOIndex == 0) {
			//no capture/compare selection
			TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
			//disable capture compare 
			TIM2->CCER &= ~TIM_CCER_CC3E;
		}
		else if (IOIndex == 1) {
			TIM2->CCMR2 &= ~TIM_CCMR2_CC4S;
			TIM2->CCER &= ~TIM_CCER_CC4E;
		}
		CPU_GPIO_ReservePin(QED_GetIOPinForChannel(channel, IOIndex), FALSE);
		g_IOStatus[IOIndex] = IOStatus_None; 
		g_pulseLength[IOIndex] = 0;
	}
}
