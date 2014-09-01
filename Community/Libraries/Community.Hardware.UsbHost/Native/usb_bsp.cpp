/**
******************************************************************************
* @file    usb_bsp.c
* @author  MCD Application Team
* @version V2.1.0
* @date    19-March-2012
* @brief   This file is responsible to offer board support package and is
*          configurable by user.
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************
*/ 

/* Includes ------------------------------------------------------------------*/
#include "tinyHal.h"
//#include "usb_bsp.h"
#include "usb_hcd_int.h"
#include "usbh_core.h"
#include "stm32f4xx.h"
#include "usb_def.h"

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
//extern USBH_HOST                     USB_Host;
extern HAL_COMPLETION g_completion;

//// PHY turnaround time
//// (4 AHB clocks + 1 Phy clock in Phy clocks)
//#define STM32F4_USB_TRDT ((4 * 48000000 - 1) / SYSTEM_CYCLE_CLOCK_HZ + 2)

//void BSP_Init(void) {
//}
/**
* @brief  OTG_FS_IRQHandler
*          This function handles USB-On-The-Go FS global interrupt request.
*          requests.
* @param  None
* @retval None
*/
void IRQHandler(void* param)
{
	USBH_OTG_ISR_Handler(&USB_OTG_Core);
	//HOST_State oldState;
	//do {
	//	oldState = USB_Host.gState;
	//	USBH_Process(&USB_OTG_Core, &USB_Host);
	//} while (USB_Host.gState != oldState);
	g_completion.Enqueue();
}
void USB_OTG_BSP_Init (USB_OTG_CORE_HANDLE *pdev){
	// enable USB clock HS on AHB1
	RCC->AHB1ENR |= RCC_AHB1ENR_OTGHSEN;
	// enable USB clock FS on AHB2
	//RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;

	//GPIO_ALT_MODE altMode = (GPIO_ALT_MODE)0x2A2;
	GPIO_ALT_MODE altMode = (GPIO_ALT_MODE)0x2C2;

	CPU_GPIO_DisablePin(16+14, RESISTOR_DISABLED, 0, altMode);
	CPU_GPIO_DisablePin(16+15, RESISTOR_DISABLED, 0, altMode);
}

void USB_OTG_BSP_uDelay (const uint32_t usec){
	HAL_Time_Sleep_MicroSeconds(usec);
}
void USB_OTG_BSP_mDelay (const uint32_t msec){
	HAL_Time_Sleep_MicroSeconds(msec*1000);
}



void USB_OTG_BSP_EnableInterrupt (USB_OTG_CORE_HANDLE *pdev){
	CPU_INTC_ActivateInterrupt(OTG_HS_IRQn, IRQHandler, 0);

}
#ifdef USE_HOST_MODE
void USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev){
}
void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev,uint8_t state){
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
