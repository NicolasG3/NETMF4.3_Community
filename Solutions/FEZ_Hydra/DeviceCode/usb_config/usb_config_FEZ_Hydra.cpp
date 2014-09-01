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

#include <tinyhal.h>

//--//

//string descriptor
#define     MANUFACTURER_NAME_SIZE  15   /* "GHI Electronics" */
// NOTE: Having more than (probably) 32 characters causes the MFUSB KERNEL driver
// to *CRASH* which, of course, causes Windows to crash
#define     PRODUCT_NAME_SIZE      25   /* "Micro Framework FEZ Hydra" */
#define     SERIAL_NAME_SIZE      10   /* "Micro Framework FEZ Hydra"" */
// NOTE: If these two strings are not present, the MFUSB KERNEL driver will *CRASH*
// which, of course, causes Windows to crash
#define     DISPLAY_NAME_SIZE       15   /* "FEZ Hydra"*/
#define     FRIENDLY_NAME_SIZE      9   /* "Gadgeteer" */
// index for the strings
#define     MANUFACTURER_NAME_INDEX 1
#define     PRODUCT_NAME_INDEX      2
//#define     SERIAL_NUMBER_INDEX     0
#define     SERIAL_NUMBER_INDEX     3
// device descriptor
#define     PRODUCT_ID         0x0106
#define     VENDOR_ID          0x1B9F
#define     MAX_EP0_SIZE            64
//configuration descriptor
//#define     USB_MAX_CURRENT     (MAX_SYSTEM_CURRENT_LOAD_MA + CHRG_CURRENT_MA)/USB_CURRENT_UNIT
#define     USB_MAX_CURRENT     280/USB_CURRENT_UNIT

#define     USB_ATTRIBUTES      (USB_ATTRIBUTE_BASE | USB_ATTRIBUTE_SELF_POWER)

// Configuration for extended descriptor
#define OS_DESCRIPTOR_EX_VERSION            0x0100

/////////////////////////////////////////////////////////////
// The following structure defines the USB descriptor
// for a basic device with a USB debug interface plus the
// Sideshow OS descriptors.
// This USB configuration is always used to define the USB
// configuration for TinyBooter.  It is also the default for
// the runtime if there is no USB configuration in the Flash
// configuration sector.

const ADS_PACKED struct GNU_PACKED USB_DYNAMIC_CONFIGURATION
{
    USB_DEVICE_DESCRIPTOR        device;
    USB_CONFIGURATION_DESCRIPTOR config;
      USB_INTERFACE_DESCRIPTOR   itfc0;
        USB_ENDPOINT_DESCRIPTOR  ep1;
        USB_ENDPOINT_DESCRIPTOR  ep2;
    USB_STRING_DESCRIPTOR_HEADER manHeader;
      USB_STRING_CHAR            manString[MANUFACTURER_NAME_SIZE];
    USB_STRING_DESCRIPTOR_HEADER prodHeader;
      USB_STRING_CHAR            prodString[PRODUCT_NAME_SIZE];
	USB_STRING_DESCRIPTOR_HEADER serialHeader;
      USB_STRING_CHAR            serialString[SERIAL_NAME_SIZE];
    USB_STRING_DESCRIPTOR_HEADER string4;
      USB_STRING_CHAR            displayString[DISPLAY_NAME_SIZE];
    USB_STRING_DESCRIPTOR_HEADER string5;
      USB_STRING_CHAR            friendlyString[FRIENDLY_NAME_SIZE];
    USB_OS_STRING_DESCRIPTOR     OS_String;
    USB_XCOMPATIBLE_OS_ID        OS_XCompatible_ID;
    USB_DESCRIPTOR_HEADER        endList;
} UsbDefaultConfiguration =
{
    // Device descriptor
    {
        {
            USB_DEVICE_DESCRIPTOR_MARKER,
            0,
            sizeof(USB_DEVICE_DESCRIPTOR)
        },
        USB_DEVICE_DESCRIPTOR_LENGTH,       // Length of device descriptor
        USB_DEVICE_DESCRIPTOR_TYPE,         // USB device descriptor type
        0x0200,                             // USB Version 1.10 (BCD)
        0,                                  // Device class (none)
        0,                                  // Device subclass (none)
        0,                                  // Device protocol (none)
        MAX_EP0_SIZE,                       // Endpoint 0 size
        VENDOR_ID,                          // Vendor ID
        PRODUCT_ID,                         // Product ID
        DEVICE_RELEASE_VERSION,             // Product version 1.00 (BCD)
        MANUFACTURER_NAME_INDEX,            // Manufacturer name string index (Freescale)
        PRODUCT_NAME_INDEX,                 // Product name string index (Micro Framework MXS Reference )
        SERIAL_NUMBER_INDEX,                                  // Serial number string index (none)
        1                                   // Number of configurations
    },

    // Configuration descriptor
    {
        {
            USB_CONFIGURATION_DESCRIPTOR_MARKER,
            0,
            sizeof(USB_CONFIGURATION_DESCRIPTOR)
                + sizeof(USB_INTERFACE_DESCRIPTOR)
                + sizeof(USB_ENDPOINT_DESCRIPTOR)
                + sizeof(USB_ENDPOINT_DESCRIPTOR)
        },
        USB_CONFIGURATION_DESCRIPTOR_LENGTH,
        USB_CONFIGURATION_DESCRIPTOR_TYPE,
        USB_CONFIGURATION_DESCRIPTOR_LENGTH
            + sizeof(USB_INTERFACE_DESCRIPTOR)
            + sizeof(USB_ENDPOINT_DESCRIPTOR)
            + sizeof(USB_ENDPOINT_DESCRIPTOR),
        1,                                          // Number of interfaces
        1,                                          // Number of this configuration
        0,                                          // Config descriptor string index (none)
        USB_ATTRIBUTES,                             // Config attributes
        USB_MAX_CURRENT                             // Device max current draw
    },

    // Interface 0 descriptor
    {
        sizeof(USB_INTERFACE_DESCRIPTOR),
        USB_INTERFACE_DESCRIPTOR_TYPE,
        0,                                          // Interface number
        0,                                          // Alternate number (main)
        2,                                          // Number of endpoints
        0xFF,                                       // Interface class (vendor)
        1,                                          // Interface subclass
        1,                                          // Interface protocol
        0                                           // Interface descriptor string index (none)
    },

    // Endpoint 1 descriptor
    {
        sizeof(USB_ENDPOINT_DESCRIPTOR),
        USB_ENDPOINT_DESCRIPTOR_TYPE,
        USB_ENDPOINT_DIRECTION_IN + 1,
        USB_ENDPOINT_ATTRIBUTE_BULK,
        64,                                         // Endpoint 1 packet size
        0                                           // Endpoint 1 interval		
    },

    // Endpoint 2 descriptor
    {
        sizeof(USB_ENDPOINT_DESCRIPTOR),
        USB_ENDPOINT_DESCRIPTOR_TYPE,
        USB_ENDPOINT_DIRECTION_OUT + 2,
        USB_ENDPOINT_ATTRIBUTE_BULK,
        64,                                         // Endpoint 2 packet size
        0                                           // Endpoint 2 interval
    },

    // Manufacturer name string descriptor
    {
        {
            USB_STRING_DESCRIPTOR_MARKER,
            MANUFACTURER_NAME_INDEX,
            sizeof(USB_STRING_DESCRIPTOR_HEADER) + (sizeof(USB_STRING_CHAR) * MANUFACTURER_NAME_SIZE)
        },
        USB_STRING_DESCRIPTOR_HEADER_LENGTH + (sizeof(USB_STRING_CHAR) * MANUFACTURER_NAME_SIZE),
        USB_STRING_DESCRIPTOR_TYPE
    },
    { 'G', 'H', 'I', ' ', 'E', 'l', 'e', 'c', 't', 'r', 'o', 'n', 'i', 'c', 's' },

    // Product name string descriptor
    {
        {
            USB_STRING_DESCRIPTOR_MARKER,
            PRODUCT_NAME_INDEX,
            sizeof(USB_STRING_DESCRIPTOR_HEADER) + (sizeof(USB_STRING_CHAR) * PRODUCT_NAME_SIZE)
        },
        USB_STRING_DESCRIPTOR_HEADER_LENGTH + (sizeof(USB_STRING_CHAR) * PRODUCT_NAME_SIZE),
        USB_STRING_DESCRIPTOR_TYPE
    },
    { 'F', 'E', 'Z', ' ', 'H', 'y', 'd', 'r', 'a'},
	
	// Serial name string descriptor
    {
        {
            USB_STRING_DESCRIPTOR_MARKER,
            SERIAL_NUMBER_INDEX,
            sizeof(USB_STRING_DESCRIPTOR_HEADER) + (sizeof(USB_STRING_CHAR) * SERIAL_NAME_SIZE)
        },
        USB_STRING_DESCRIPTOR_HEADER_LENGTH + (sizeof(USB_STRING_CHAR) * SERIAL_NAME_SIZE),
        USB_STRING_DESCRIPTOR_TYPE
    },
    { '0', '0', '0', '0', '0', '0', '0', '0', '0', '1' },
		
    // String 4 descriptor (display name)
    {
        {
            USB_STRING_DESCRIPTOR_MARKER,
            USB_DISPLAY_STRING_NUM,
            sizeof(USB_STRING_DESCRIPTOR_HEADER) + (sizeof(USB_STRING_CHAR) * DISPLAY_NAME_SIZE)
        },
        USB_STRING_DESCRIPTOR_HEADER_LENGTH + (sizeof(USB_STRING_CHAR) * DISPLAY_NAME_SIZE),
        USB_STRING_DESCRIPTOR_TYPE
    },
    { 'F', 'E', 'Z', ' ', 'H', 'y', 'd', 'r', 'a'},

    // String 5 descriptor (friendly name)
    {
        {
            USB_STRING_DESCRIPTOR_MARKER,
            USB_FRIENDLY_STRING_NUM,
            sizeof(USB_STRING_DESCRIPTOR_HEADER) + (sizeof(USB_STRING_CHAR) * FRIENDLY_NAME_SIZE)
        },
        USB_STRING_DESCRIPTOR_HEADER_LENGTH + (sizeof(USB_STRING_CHAR) * FRIENDLY_NAME_SIZE),
        USB_STRING_DESCRIPTOR_TYPE
    },
    { 'G', 'a', 'd', 'g', 'e', 't', 'e', 'e', 'r' },
/*
    // OS Descriptor string for Sideshow
    {
        {
            USB_STRING_DESCRIPTOR_MARKER,
            OS_DESCRIPTOR_STRING_INDEX,
            sizeof(USB_DESCRIPTOR_HEADER) + OS_DESCRIPTOR_STRING_SIZE
        },
        OS_DESCRIPTOR_STRING_SIZE,
        USB_STRING_DESCRIPTOR_TYPE,
        { 'M', 'S', 'F', 'T', '1', '0', '0' },
        OS_DESCRIPTOR_STRING_VENDOR_CODE,
        0x00
    },

    // OS Extended Compatible ID for Sideshow
    {
        // Generic Descriptor header
        {
            {
                USB_GENERIC_DESCRIPTOR_MARKER,
                0,
                sizeof(USB_GENERIC_DESCRIPTOR_HEADER) + USB_XCOMPATIBLE_OS_SIZE
            },
            USB_REQUEST_TYPE_IN | USB_REQUEST_TYPE_VENDOR,
            OS_DESCRIPTOR_STRING_VENDOR_CODE,
            0,                                              // Intfc # << 8 + Page #
            USB_XCOMPATIBLE_OS_REQUEST                      // Extended Compatible OS ID request
        },
        USB_XCOMPATIBLE_OS_SIZE,                            // Size of this descriptor
        OS_DESCRIPTOR_EX_VERSION,                           // Version 1.00 (BCD)
        USB_XCOMPATIBLE_OS_REQUEST,                         // Extended Compatible OS ID response
        1,                                                  // Only 1 function record
        { 0, 0, 0, 0, 0, 0, 0 },                            // (padding)
        // Extended Compatible OS ID function record
        0,                                                  // Interface 0
        1,                                                  // (reserved)
        { 'S', 'I', 'D', 'E', 'S', 'H', 'W', 0 },           // Compatible ID
       // { 'E', 'N', 'H', 'V', '1',  0,   0,  0 },           // Sub-compatible ID
		{ 'E', 'N', 'H', 'V', '2',  0,   0,  0 },           // Sub-compatible ID
        { 0, 0, 0, 0, 0, 0 }                                // Padding
    },
*/
    // End of configuration marker
    {
        USB_END_DESCRIPTOR_MARKER,
        0,
        0
    },
};



