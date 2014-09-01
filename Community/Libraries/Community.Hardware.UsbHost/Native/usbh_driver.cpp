//#define __IO volatile
//typedef unsigned int uint32_t


#include "usbh_driver.h"
//#include "usb_decl.h"
#include "stm32f4xx.h"
#include "usb_def.h"


// PHY turnaround time
// (4 AHB clocks + 1 Phy clock in Phy clocks)
#define STM32F4_USB_TRDT ((4 * 48000000 - 1) / SYSTEM_CYCLE_CLOCK_HZ + 2)

void USBH_ISR (void* param){
	GLOBAL_LOCK(irq);
	CPU_GPIO_SetPinState(18, 1);

	OTG_TypeDef* OTG = (OTG_TypeDef*)param;
	if (OTG->HPRT & OTG_HPRT_PCDET) {
		//device detected
		//clear bit
		OTG->HPRT |= OTG_HPRT_PCDET;
	}
}
//Initialize USB Host
// USB OTG Full Speed is controller 0
// USB OTG High Speed is controller 1
BOOL USBH_Initialize( int Controller ) {
	CPU_GPIO_EnableOutputPin(16+2, TRUE);
	// enable USB clock
	OTG_TypeDef* OTG;
	if (Controller == 0) {
		RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;
		OTG = OTG_FS;
	}
	else if (Controller == 1) {
		RCC->AHB1ENR |= RCC_AHB1ENR_OTGHSEN;
		OTG = OTG_HS;
	}
	else {
		return FALSE;
	}
	//disable int
	OTG->GAHBCFG &= ~OTG_GAHBCFG_GINTMSK;

	//core init 1
	OTG->GINTSTS |= OTG_GINTSTS_RXFLVL;
	OTG->GAHBCFG |= OTG_GAHBCFG_PTXFELVL;
	//core init 2
	OTG->GUSBCFG &= ~OTG_GUSBCFG_HNPCAP;
	OTG->GUSBCFG &= ~OTG_GUSBCFG_SRPCAP;
	OTG->GUSBCFG &= ~OTG_GUSBCFG_TRDT;
	OTG->GUSBCFG |= STM32F4_USB_TRDT<<10;
	OTG->GUSBCFG |= OTG_GUSBCFG_PHYSEL;
	//core init 3
	OTG->GINTMSK |= OTG_GINTMSK_OTGINT | OTG_GINTMSK_MMISM;
	//force host mode
	OTG->GUSBCFG |= OTG_GUSBCFG_FHMOD;
	//host init
	OTG->GINTMSK |= OTG_GINTMSK_PRTIM;
	OTG->HCFG = OTG_HCFG_FSLSPCS_48MHZ;
	OTG->HPRT |= OTG_HPRT_PPWR;

	OTG->GCCFG |= (OTG_GCCFG_VBUSBSEN | OTG_GCCFG_VBUSASEN | OTG_GCCFG_NOVBUSSENS | OTG_GCCFG_PWRDWN);
	//
	//config pins and ISR
	if (Controller == 0) {
		CPU_GPIO_DisablePin(10, RESISTOR_DISABLED, 0, (GPIO_ALT_MODE)0xA2);
		CPU_GPIO_DisablePin(11, RESISTOR_DISABLED, 0, (GPIO_ALT_MODE)0xA2);
		CPU_INTC_ActivateInterrupt(OTG_FS_IRQn,         USBH_ISR, OTG);
	}
	else {
		CPU_GPIO_DisablePin(16+14, RESISTOR_DISABLED, 0, (GPIO_ALT_MODE)0xC2);
		CPU_GPIO_DisablePin(16+15, RESISTOR_DISABLED, 0, (GPIO_ALT_MODE)0xC2);
		CPU_INTC_ActivateInterrupt(OTG_HS_IRQn,         USBH_ISR, OTG);
	}

	for (int i = 0; i < 3; i++)
	{
		CPU_GPIO_SetPinState(18, 1);
		HAL_Time_Sleep_MicroSeconds(200*1000);
		CPU_GPIO_SetPinState(18, 0);
		HAL_Time_Sleep_MicroSeconds(200*1000);
	}

	//enable interrupt
	OTG->GINTSTS = 0xFFFFFFFF;
	OTG->GAHBCFG |= OTG_GAHBCFG_GINTMSK;
	return TRUE;
}