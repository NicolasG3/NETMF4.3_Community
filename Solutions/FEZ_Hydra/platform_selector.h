////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
// This file is part of the Microsoft .NET Micro Framerwork Porting Kit Code Samples and is unsupported. 
// Copyright (C) Microsoft Corporation. All rights reserved. Use of this sample source code is subject to 
// the terms of the Microsoft license agreement under which you licensed this sample source code. 
// 
// THIS SAMPLE CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
// 
//
// Portions Copyright (c) GHI Electronics, LLC.
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PLATFORM_FEZ_Hydra_SELECTOR_H_
#define _PLATFORM_FEZ_Hydra_SELECTOR_H_ 1

/////////////////////////////////////////////////////////
//
// processor and features
//

#if defined(PLATFORM_ARM_FEZ_Hydra)
#define HAL_SYSTEM_NAME                 "FEZ_Hydra"
#define PLATFORM_ARM_SAM9RL64_ANY        1
    
//
// processor and features
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// Platform Select for shared OSHW drivers
//

#define FEZ_HYDRA	1
    
//
// Platform Select for shared OSHW drivers
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// Solution info
//
#define GHI_VERSION_MAJOR 4
#define GHI_VERSION_MINOR 2
#define GHI_VERSION_BUILD 5
#define GHI_VERSION_REVISION 0
#define GHIOEMSTRING "Copyright (C) GHI Electronics, LLC"

//
// Solution info
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// constants
//

#define SYSTEM_CYCLE_CLOCK_HZ           200000000
#define SYSTEM_PERIPHERAL_CLOCK_HZ      (SYSTEM_CYCLE_CLOCK_HZ / 2)
#define SYSTEM_CLOCK_HZ                 781250

#define CLOCK_COMMON_FACTOR             250

#define SLOW_CLOCKS_PER_SECOND          SYSTEM_CLOCK_HZ
#define SLOW_CLOCKS_TEN_MHZ_GCD         CLOCK_COMMON_FACTOR

#define SLOW_CLOCKS_MILLISECOND_GCD     CLOCK_COMMON_FACTOR

//#define FLASH_MEMORY_Base   0x20000000
//#define FLASH_MEMORY_Size   0x10000000

//#define SRAM1_MEMORY_Base   0x20000000
//#define SRAM1_MEMORY_Size   0x04000000

#define SDRAM_MEMORY_Base               0x20000000
#define SDRAM_MEMORY_Size               (16*1024*1024)

#define SRAM_MEMORY_Base                0x00300000
#define SRAM_MEMORY_Size                (64*1024)

#define FLASH_MEMORY_Base   0x30000000
#define FLASH_MEMORY_Size   0x00420000
#define SRAM1_MEMORY_Base   0x20000000
#define SRAM1_MEMORY_Size   0x01000000

#define TXPROTECTRESISTOR               RESISTOR_DISABLED
#define RXPROTECTRESISTOR               RESISTOR_DISABLED
#define CTSPROTECTRESISTOR              RESISTOR_DISABLED
#define RTSPROTECTRESISTOR              RESISTOR_DISABLED

#define INSTRUMENTATION_H_GPIO_PIN      GPIO_PIN_NONE

#if 1
#define DEBUG_TEXT_PORT    USB1
#define STDIO              USB1
#define DEBUGGER_PORT      USB1
#define MESSAGING_PORT     USB1
#else
#define DEBUG_TEXT_PORT    COM1
#define STDIO              COM1
#define DEBUGGER_PORT      COM1
#define MESSAGING_PORT     COM1
#endif

#define DRIVER_PAL_BUTTON_MAPPING                                           \
    { GPIO_PIN_NONE,         BUTTON_B0 },  \
    { GPIO_PIN_NONE,         BUTTON_B1 },  \
    { AT91_GPIO_Driver::PB0, BUTTON_B2 },  \
    { GPIO_PIN_NONE, 		 BUTTON_B3 },  \
    { AT91_GPIO_Driver::PB1, BUTTON_B4 },  \
    { GPIO_PIN_NONE,         BUTTON_B5 }, 

#define AT91_UDP_INTERNAL_PULLUP      1
//#define AT91_VBUS_DETECTION         1
//#define AT91_VBUS                   AT91_GPIO_Driver::PA8

#define LCD_BIT_PIX 4
#define LCD_RGB_CONVERTION  0


//
// constants
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// macros
//

#define GLOBAL_LOCK(x)             SmartPtr_IRQ x
#define DISABLE_INTERRUPTS()       SmartPtr_IRQ::ForceDisabled()
#define ENABLE_INTERRUPTS()        SmartPtr_IRQ::ForceEnabled()
#define INTERRUPTS_ENABLED_STATE() SmartPtr_IRQ::GetState()
#define GLOBAL_LOCK_SOCKETS(x)     SmartPtr_IRQ x

#if defined(_DEBUG)
#define ASSERT_IRQ_MUST_BE_OFF()   ASSERT(!SmartPtr_IRQ::GetState())
#define ASSERT_IRQ_MUST_BE_ON()    ASSERT( SmartPtr_IRQ::GetState())
#else
#define ASSERT_IRQ_MUST_BE_OFF()
#define ASSERT_IRQ_MUST_BE_ON()
#endif


//
// macros
//Ready Busy Pin of NandFlash
#define AT91_NAND_RB		AT91_GPIO_Driver::PD17
#define AT91_NAND_CE		AT91_GPIO_Driver::PB6
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// global functions
//

//
// global functions
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// communicaiton facilities
//

// Port definitions
#define TOTAL_USART_PORT       4
#define COM1                   ConvertCOM_ComHandle(0)
#define COM2                   ConvertCOM_ComHandle(1)

#define TOTAL_USB_CONTROLLER   1
#define USB1                   ConvertCOM_UsbHandle(0)

#define TOTAL_SOCK_PORT        0

#define TOTAL_DEBUG_PORT       1
#define COM_DEBUG              ConvertCOM_DebugHandle(0)

#define COM_MESSAGING          ConvertCOM_MessagingHandle(0)

#define USART_TX_IRQ_INDEX(x)       ( (x) ? 0 : 0 )     /* TODO set right indexes */
#define USART_DEFAULT_PORT          COM1
#define USART_DEFAULT_BAUDRATE      115200

#define USB_IRQ_INDEX               0  // TODO set right index


#define PLATFORM_DEPENDENT_TX_USART_BUFFER_SIZE    (1024 * 4)  // there is one TX for each usart port
#define PLATFORM_DEPENDENT_RX_USART_BUFFER_SIZE    (1024 * 4) // there is one RX for each usart port
#define PLATFORM_DEPENDENT_USB_QUEUE_PACKET_COUNT  32  // there is one queue for each pipe of each endpoint and the size of a single packet is sizeof(USB_PACKET64) == 68 bytes
//
// communicaiton facilities
/////////////////////////////////////////////////////////

#include <processor_selector.h>

#endif // PLATFORM_ARM_FEZ_Hydra

#endif // _PLATFORM_FEZ_Hydra_SELECTOR_H_
