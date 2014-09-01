////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Portions Copyright (c) GHI Electronics, LLC.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "AT91_LCD.h"
#include <..\DeviceCode\GHI\include\OSHW_Configuration.h>

//////////////////////////////////////////////////////////////////////////////

#undef  TRACE_ALWAYS
#define TRACE_ALWAYS               0x00000001

#undef  DEBUG_TRACE
#define DEBUG_TRACE (TRACE_ALWAYS)


OSHW_Configurations_Structure		OSHW_Configurations;

//////////////////////////////////////////////////////////////////////////////
static UINT32* AT91_LCD_Screen_Buffer = (UINT32*)0x20F00000;

BOOL AT91_LCD_Driver::Initialize(DISPLAY_CONTROLLER_CONFIG& config)
{
	OSHW_Configuration_Read();

	 if(OSHW_Configurations.lcd.PixelClockRateKHz == 0x0)
	 {
      // ugly hack to prevent system crash!
           if( OSHW_Configurations.lcd.Width >=64)
                config.Width						= OSHW_Configurations.lcd.Width;
           if( OSHW_Configurations.lcd.Height >=64)
                config.Height						= OSHW_Configurations.lcd.Height;
		
           return true;
	 }

	 if(OSHW_Configurations.lcd.Magic == 0xdeadbeef)
	 {
		 config.Width						= OSHW_Configurations.lcd.Width;
		 config.Height						= OSHW_Configurations.lcd.Height;
		 config.PixelPolarity				= OSHW_Configurations.lcd.PixelPolarity;
		 config.FirstLineMarkerPolarity		= OSHW_Configurations.lcd.HorizontalSyncPolarity;
		 config.LinePulsePolarity			= OSHW_Configurations.lcd.VerticalSyncPolarity;
		 config.OutputEnablePolarity			= OSHW_Configurations.lcd.OutputEnablePolarity;

		 // Pixel Clock Divider is calculated by this equation Frequency = System Clock / ((Divider + 1) * 2)
		 // The following equation should calculate Pixel Clock Divider
		 // Divider = ((SYSTEM_PERIPHERAL_CLOCK_HZ / 2) - (OSHW_Configurations.lcd.PixelClockRateKHz * 1000)) / (OSHW_Configurations.lcd.PixelClockRateKHz * 1000);
		 if(OSHW_Configurations.lcd.PixelClockRateKHz > 25000) // 25000 KHz is maximum supported frequency
			 OSHW_Configurations.lcd.PixelClockRateKHz = 25000;

		 config.PixelClockDivider			= ((SYSTEM_PERIPHERAL_CLOCK_HZ / 2) - (OSHW_Configurations.lcd.PixelClockRateKHz * 1000)) / (OSHW_Configurations.lcd.PixelClockRateKHz * 1000);

		 if(((SYSTEM_PERIPHERAL_CLOCK_HZ / 2) - (OSHW_Configurations.lcd.PixelClockRateKHz * 1000)) % (OSHW_Configurations.lcd.PixelClockRateKHz * 1000) > 0)
			 config.PixelClockDivider += 1;

		 if (config.PixelClockDivider > 511) // Pixel clock divider cannot exceed a 9 bit number which is 511 in Decimal.
			 config.PixelClockDivider = 511;

		 config.HorizontalSyncPulseWidth		= OSHW_Configurations.lcd.HorizontalSyncPulseWidth;
		 config.HorizontalSyncWait1			= OSHW_Configurations.lcd.HorizontalBackPorch;
		 config.HorizontalSyncWait2			= OSHW_Configurations.lcd.HorizontalFrontPorch;
		 config.VerticalSyncPulseWidth		= OSHW_Configurations.lcd.VerticalSyncPulseWidth;
		 config.VerticalSyncWait1			= OSHW_Configurations.lcd.VerticalBackPorch;
		 config.VerticalSyncWait2			= OSHW_Configurations.lcd.VerticalFrontPorch;
	 }


/////////////////////////////////////////////

    UINT32 pin;
    UINT32 value;

    /* Selected as LCD pins */
    for(pin = 0; pin < ARRAYSIZE(c_LCD_1); pin++)
    {
        CPU_GPIO_DisablePin( c_LCD_1[pin], RESISTOR_DISABLED, 0, GPIO_ALT_MODE_1);
    }
    for(pin = 0; pin < ARRAYSIZE(c_LCD_2); pin++)
    {
        CPU_GPIO_DisablePin( c_LCD_2[pin], RESISTOR_DISABLED, 0, GPIO_ALT_MODE_2);
    }
    /* Enable CS for LCD */
    //CPU_GPIO_EnableOutputPin((GPIO_PIN)AT91_LCDC_CS, 0);

	if(OSHW_Configurations.lcd.OutputEnableIsFixed)
		CPU_GPIO_EnableOutputPin(AT91_LCDDEN, config.OutputEnablePolarity);
	else
		CPU_GPIO_DisablePin(AT91_LCDDEN, RESISTOR_DISABLED, 0, GPIO_ALT_MODE_1);

    AT91_PMC &pmc = AT91::PMC();
    pmc.EnablePeriphClock(AT91C_ID_LCDC);    

    AT91_LCDC &lcdc = AT91::LCDC();

    /* Turn off the LCD controller and the DMA controller */
    lcdc.LCDC_PWRCON = 0x0C;
    lcdc.LCDC_DMACON = 0;

    // Reset LCDC DMA
    lcdc.LCDC_DMACON = AT91_LCDC::LCDC_DMARST;
  
    /* ...set frame size and burst length = 8 words (?) */
    value = (config.Height * config.Width * config.BitsPerPixel) >> 5; // / 32;
    value |= 0x04000000;
    lcdc.LCDC_FRMCFG = value;

    /* Set pixel clock divider */
    if(config.PixelClockDivider == 0)
        lcdc.LCDC_LCDCON1 = AT91_LCDC::LCDC_BYPASS;
    else
        lcdc.LCDC_LCDCON1 = config.PixelClockDivider << 12;

    /* Set configurations for LCDCON2 */
    if(config.EnableTFT)
        value = ( AT91_LCDC::LCDC_MEMOR_LITTLEIND | AT91_LCDC::LCDC_DISTYPE_TFT | AT91_LCDC::LCDC_CLKMOD);
    else
        value = ( AT91_LCDC::LCDC_MEMOR_LITTLEIND | AT91_LCDC::LCDC_CLKMOD);

    if(config.FirstLineMarkerPolarity == FALSE) // Original was if it is true set 1
        value |= 1 << 10;   /* INVLINE */
    if (config.LinePulsePolarity == FALSE) // Original was if it is true set 1
        value |= 1 << 9;    /* INVFRAME */

    switch(config.BitsPerPixel)
    {
        //Bits per pixel: 0 = 1, 1 = 2, 2 = 4, 3 = 8, 4 = 16, 5 = 24
        case 1:
            value |= (0 << 5);
            break;
        case 2:
            value |=  (1 << 5);
            break;
        case 4:
            value |=  (2 << 5);
            break;
        case 8:
            value |=  (3 << 5);
            break;
        case 16:
            value |=  (4 << 5);
            break;
        case 24:
            value |=  (5 << 5);
            break;
        default:
            debug_printf("Unsupported LCD Bus Width : %d\r\n", config.BitsPerPixel);
            return FALSE;    
    };
    
    lcdc.LCDC_LCDCON2 = value;

    for(UINT32 i = 0; i < 256; i++) 
    {
        lcdc.LCDC_LUT_ENTRY[i] = 
           ((c_rgbPalette[i].Red) >> 3 |
           ((c_rgbPalette[i].Green & 0xf8) << 2) |
           ((c_rgbPalette[i].Blue & 0xf8) << 7));
    }

    /* Horizontal timing */
    value = (config.HorizontalSyncWait2 - 1) << 21;
    value |= (config.HorizontalSyncPulseWidth -1) << 8;
    value |= config.HorizontalSyncWait1 - 1;
    lcdc.LCDC_TIM2 = value;

    /* Vertical timing */
    value = (config.VerticalSyncPulseWidth - 1) << 16;
    
	
	
	
	config.VerticalSyncWait1 += config.VerticalSyncPulseWidth; //1;//10; For Hydra or Atmel you need to add the VPW to the VBP in order for the screen to align properly.
	
	value |= config.VerticalSyncWait1 << 8;

	value |= config.VerticalSyncWait2;
    lcdc.LCDC_TIM1 = value;

    value = (config.Width - 1) << 21;
    value |= (config.Height - 1);

    lcdc.LCDC_LCDFRCFG = value;

    /* FIFO Threshold: Use formula from data sheet */
    value = SIDSAFB_FIFO_SIZE - (2 * SIDSAFB_DMA_BURST_LEN + 3);
    lcdc.LCDC_FIFO = value;

    /* Toggle LCD_MODE every frame */
    //lcdc.LCDC_MVAL = 0;

    //STN Dithering
    if(!(config.EnableTFT))
    {
        lcdc.LCDC_DP1_2     = 0xA5;
        lcdc.LCDC_DP4_7     = 0x05AF0FA5;
        lcdc.LCDC_DP3_5     = 0x000A5A5F;
        lcdc.LCDC_DP2_3     = 0x00000A5F;
        lcdc.LCDC_DP5_7     = 0x0FAF5FA5;
        lcdc.LCDC_DP3_4     = 0x0000FAF5;
        lcdc.LCDC_DP4_5     = 0x000FAF5F;
        lcdc.LCDC_DP6_7     = 0x0F5FFAFF;
    }

    /* Disable all interrupts */
    lcdc.LCDC_IDR = ~0UL;
    // Set contrast
    value = AT91_LCDC::LCDC_PS_DIVIDEDBYEIGHT | AT91_LCDC::LCDC_POL_POSITIVEPULSE | AT91_LCDC::LCDC_ENA_PWMGEMENABLED;
    lcdc.LCDC_CTRSTCON = value;
    lcdc.LCDC_CTRSTVAL = 0xDA; 

    lcdc.LCDC_BA1 = (UINT32)AT91_LCD_Screen_Buffer;
    lcdc.LCDC_FRMCFG = (4 << 24) + (config.Height * config.Width * config.BitsPerPixel >> 5);

    return TRUE;
}

BOOL AT91_LCD_Driver::Uninitialize()
{    
    // Turn off LCDC
    AT91_PMC &pmc = AT91::PMC();
    pmc.DisablePeriphClock(AT91C_ID_LCDC);    

    return TRUE;
}

BOOL AT91_LCD_Driver::Enable(BOOL fEnable)
{
	OSHW_Configuration_Read();

	if(OSHW_Configurations.lcd.PixelClockRateKHz == 0x0)
		return true;
//////////////////////////////////////////////////

    AT91_LCDC &lcdc = AT91::LCDC();

    if(fEnable)
    {
        lcdc.LCDC_DMACON    = AT91_LCDC::LCDC_DMAEN;
        lcdc.LCDC_PWRCON    = AT91_LCDC::LCDC_PWR | (0x0F<<1);
    }
    else
    {
        lcdc.LCDC_PWRCON = 0x0C;
        lcdc.LCDC_DMACON = 0;
    }
    return TRUE;
}

UINT32* AT91_LCD_Driver::GetScreenBuffer(void)
{
    return AT91_LCD_Screen_Buffer;
}
