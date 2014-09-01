////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for Discovery4 board (STM32F4): Copyright (c) Oberon microsystems, Inc.
//
//  Discovery4 specific definitions
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PLATFORM_Discovery4_SELECTOR_H_
#define _PLATFORM_Discovery4_SELECTOR_H_ 1

/////////////////////////////////////////////////////////
//
// processor and features
//

#if defined(PLATFORM_ARM_Discovery4)
#define HAL_SYSTEM_NAME                     "Discovery4"

#define PLATFORM_ARM_STM32F4  // STM32F407 cpu

#define USB_ALLOW_CONFIGURATION_OVERRIDE  1


//
// processor and features
//
/////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////
//
// constants
//

// legal SYSCLK: 24, 30, 32, 36, 40, 42, 48, 54, 56, 60, 64, 72, 84, 96, 108, 120, 144, 168MHz
#define SYSTEM_CLOCK_HZ                 168000000  // 168MHz
#define SYSTEM_CYCLE_CLOCK_HZ           168000000  // 168MHz
#define SYSTEM_APB1_CLOCK_HZ            42000000   // 42MHz
#define SYSTEM_APB2_CLOCK_HZ            84000000   // 84MHz
#define SYSTEM_CRYSTAL_CLOCK_HZ         8000000    // 8MHz external clock
#define SUPPLY_VOLTAGE_MV               3000       // 3.0V supply

#define CLOCK_COMMON_FACTOR             1000000    // GCD(SYSTEM_CLOCK_HZ, 1M)

#define SLOW_CLOCKS_PER_SECOND          1000000    // 1MHz
#define SLOW_CLOCKS_TEN_MHZ_GCD         1000000    // GCD(SLOW_CLOCKS_PER_SECOND, 10M)
#define SLOW_CLOCKS_MILLISECOND_GCD     1000       // GCD(SLOW_CLOCKS_PER_SECOND, 1k)

#define FLASH_MEMORY_Base               0x08000000
#define FLASH_MEMORY_Size               0x00100000
#define SRAM1_MEMORY_Base               0x20000000
#define SRAM1_MEMORY_Size               0x00020000

#define TXPROTECTRESISTOR               RESISTOR_DISABLED
#define RXPROTECTRESISTOR               RESISTOR_DISABLED
#define CTSPROTECTRESISTOR              RESISTOR_DISABLED
#define RTSPROTECTRESISTOR              RESISTOR_DISABLED

#define TOTAL_GPIO_PORT                 5 // PA - PE
#define INSTRUMENTATION_H_GPIO_PIN      GPIO_PIN_NONE

#define TOTAL_USART_PORT                6
#define USART_DEFAULT_PORT              COM3
#define USART_DEFAULT_BAUDRATE          115200

#define DEBUG_TEXT_PORT                 USB1
#define STDIO                           USB1
#define DEBUGGER_PORT                   USB1
#define MESSAGING_PORT                  USB1

#define TOTAL_USB_CONTROLLER            2
#define USB_MAX_QUEUES                  6  // 6 endpoints (EP0 + 5)

#define TOTAL_SOCK_PORT                 0


// System Timer Configuration
#define STM32F4_32B_TIMER 2
#define STM32F4_16B_TIMER 3


// Pin Configuration
#define STM32F4_ADC 1
#define STM32F4_AD_CHANNELS {1,2,3,8,9,14,15} // PA1,PA2,PA3,PB0,PB1,PC4,PC5

#define STM32F4_PWM_TIMER {4,4,4,4,1,1,1,1}
#define STM32F4_PWM_CHNL  {0,1,2,3,0,1,2,3}
#define STM32F4_PWM_PINS  {60,61,62,63,73,75,77,78} // D12-D15,E9,E11,E13,E14

#define STM32F4_SPI_SCLK_PINS {5, 29, 42} // PA5, PB13, PC10
#define STM32F4_SPI_MISO_PINS {6, 30, 43} // PA6, PB14, PC11
#define STM32F4_SPI_MOSI_PINS {7, 31, 44} // PA7, PB15, PC12

#define STM32F4_I2C_PORT     1
#define STM32F4_I2C_SCL_PIN  22 // PB6
#define STM32F4_I2C_SDA_PIN  25 // PB9

#define STM32F4_UART_RXD_PINS {10,  3, 57, 43, 50, 39} // A10, A3, D9, C11, D2, C7
#define STM32F4_UART_TXD_PINS { 9,  2, 56, 42, 44, 38} // A9, A2, D8, C10, C12, C6
#define STM32F4_UART_CTS_PINS {11, 51, 59} // A11, D3, D11
#define STM32F4_UART_RTS_PINS {12,  1, 60} // A12, A1, D12


//
// constants
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// global functions
//

//
// global functions
//
/////////////////////////////////////////////////////////

#include <processor_selector.h>

#endif // PLATFORM_ARM_Discovery4
//
// drivers
/////////////////////////////////////////////////////////

#endif // _PLATFORM_Discovery4_SELECTOR_H_
