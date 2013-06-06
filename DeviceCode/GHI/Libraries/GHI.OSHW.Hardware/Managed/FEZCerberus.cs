////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) GHI Electronics, LLC.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using System;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;

namespace GHI.OSHW.Hardware
    {
    /// <summary>
    /// Exposes FEZ Cerberus specific functionality.
    /// </summary>
    public static class FEZCerberus
        {
        /// <summary>
        /// Board Led Pin (PC4)
        /// </summary>
        public const Cpu.Pin Led = Pin.PC4;

        /// <summary>
        /// Available SPI ports
        /// </summary>
        public class SPIPort 
            {
            /// <summary>
            /// No SPI port specified
            /// </summary>
            public const SPI.SPI_module None = (SPI.SPI_module)(-1);
            /// <summary>
            /// SPI port 1 (SCLK=PB3, MISO=PB4, MOSI=PB5)
            /// </summary>
            public const SPI.SPI_module SPI1 = SPI.SPI_module.SPI1;
            }

        /// <summary>
        /// Available serial ports
        /// </summary>
        public class COMPort
            {
            /// <summary>
            /// COM port 1 (TX=PC6, RX=PC7)
            /// </summary>
            public const String COM1 = "COM1";
            /// <summary>
            /// COM port 2 (TX=PA2, RX=PA3, CTS=PA0, RTS=PA1)
            /// </summary>
            public const String COM2 = "COM2";
            /// <summary>
            /// COM port 3 (TX=PB10, RX=PB11)
            /// </summary>
            public const String COM3 = "COM3";
            }
        

        /// <summary>
        /// PWM output available channels
        /// </summary>
        public class PWMChannel
            {
            /// <summary>
            /// No PWM channel specified
            /// </summary>
            public const Cpu.PWMChannel None = Cpu.PWMChannel.PWM_NONE;
            /// <summary>
            /// PWM channel 0 (PC6) 
            /// </summary>
            public const Cpu.PWMChannel PC6 = (Cpu.PWMChannel)0;
            /// <summary>
            /// PWM channel 1 (PA7) 
            /// </summary>
            public const Cpu.PWMChannel PA7 = (Cpu.PWMChannel)1;
            /// <summary>
            /// PWM channel 2 (PC7) 
            /// </summary>
            public const Cpu.PWMChannel PC7 = (Cpu.PWMChannel)2;
            /// <summary>
            /// PWM channel 3 (PA8) 
            /// </summary>
            public const Cpu.PWMChannel PA8 = (Cpu.PWMChannel)3;
            /// <summary>
            /// PWM channel 4 (PB0) 
            /// </summary>
            public const Cpu.PWMChannel PB0 = (Cpu.PWMChannel)4;
            /// <summary>
            /// PWM channel 5 (PB1) 
            /// </summary>
            public const Cpu.PWMChannel PB1 = (Cpu.PWMChannel)5;
            /// <summary>
            /// PWM channel 6 (PB5) 
            /// </summary>
            public const Cpu.PWMChannel PB5 = (Cpu.PWMChannel)6;
            /// <summary>
            /// PWM channel 7 (PB4) 
            /// </summary>
            public const Cpu.PWMChannel PB4 = (Cpu.PWMChannel)7;
            /// <summary>
            /// PWM channel8  (PB3) 
            /// </summary>
            public const Cpu.PWMChannel PB3 = (Cpu.PWMChannel)8;
            /// <summary>
            /// PWM channel 9 (PB11) 
            /// </summary>
            public const Cpu.PWMChannel PB11 = (Cpu.PWMChannel)9;
            /// <summary>
            /// PWM channel 10 (PB10) 
            /// </summary>
            public const Cpu.PWMChannel PB10 = (Cpu.PWMChannel)10;
            /// <summary>
            /// PWM channel 11 (PA10) 
            /// </summary>
            public const Cpu.PWMChannel PA10 = (Cpu.PWMChannel)11;
            /// <summary>
            /// PWM channel 12 (PA9) 
            /// </summary>
            public const Cpu.PWMChannel PA9 = (Cpu.PWMChannel)12;
            /// <summary>
            /// PWM channel 13 (PA15) 
            /// </summary>
            public const Cpu.PWMChannel PA15 = (Cpu.PWMChannel)13;
            /// <summary>
            /// PWM channel 14 (PB8) 
            /// </summary>
            public const Cpu.PWMChannel PB8 = (Cpu.PWMChannel)14;
            /// <summary>
            /// PWM channel 15 (PB9) 
            /// </summary>
            public const Cpu.PWMChannel PB9 = (Cpu.PWMChannel)15;

            }

        /// <summary>
        /// Analog output available channels
        /// </summary>
        public class AnalogOutputChannel
            {
            /// <summary>
            /// No analog output channel specified
            /// </summary>
            public const Cpu.AnalogOutputChannel None = Cpu.AnalogOutputChannel.ANALOG_OUTPUT_NONE;
            /// <summary>
            /// Analog output channel 0 (PA4) 
            /// </summary>
            public const Cpu.AnalogOutputChannel PA4 = (Cpu.AnalogOutputChannel)0;
            /// <summary>
            /// Analog output channel 1 (PA5) 
            /// </summary>
            public const Cpu.AnalogOutputChannel PA5 = (Cpu.AnalogOutputChannel)1;
            }

        /// <summary>
        /// Analog input available channels
        /// </summary>
        public class AnalogChannel
            {
            /// <summary>
            /// No analog channel specified
            /// </summary>
            public const Cpu.AnalogChannel None = Cpu.AnalogChannel.ANALOG_NONE;
            /// <summary>
            /// Analog channel 0 (PA6) 
            /// </summary>
            public const Cpu.AnalogChannel PA6 = (Cpu.AnalogChannel)0;
            /// <summary>
            /// Analog channel 1 (PA2) 
            /// </summary>
            public const Cpu.AnalogChannel PA2 = (Cpu.AnalogChannel)1;
            /// <summary>
            /// Analog channel 2 (PA3) 
            /// </summary>
            public const Cpu.AnalogChannel PA3 = (Cpu.AnalogChannel)2;
            /// <summary>
            /// Analog channel 3 (PC0) 
            /// </summary>
            public const Cpu.AnalogChannel PC0 = (Cpu.AnalogChannel)3;
            /// <summary>
            /// Analog channel 4 (PC1) 
            /// </summary>
            public const Cpu.AnalogChannel PC1 = (Cpu.AnalogChannel)4;
            /// <summary>
            /// Analog channel 5 (PA4) 
            /// </summary>
            public const Cpu.AnalogChannel PA4 = (Cpu.AnalogChannel)5;
            /// <summary>
            /// Analog channel 6 (PC2) 
            /// </summary>
            public const Cpu.AnalogChannel PC2 = (Cpu.AnalogChannel)6;
            /// <summary>
            /// Analog channel 7 (PC3) 
            /// </summary>
            public const Cpu.AnalogChannel PC3 = (Cpu.AnalogChannel)7;
            /// <summary>
            /// Analog channel 8 (PA5) 
            /// </summary>
            public const Cpu.AnalogChannel PA5 = (Cpu.AnalogChannel)8;
            /// <summary>
            /// Analog channel 9 (PB0) 
            /// </summary>
            public const Cpu.AnalogChannel PB0 = (Cpu.AnalogChannel)9;
            /// <summary>
            /// Analog channel 10 (PB1) 
            /// </summary>
            public const Cpu.AnalogChannel PB1 = (Cpu.AnalogChannel)10;
            }
        /// <summary>
        /// Provides Pin definitions for FEZ Cerberus.
        /// </summary>
        public class Pin
            {
            /// <summary>A value indicating that no GPIO pin is specified.</summary>
            public const Cpu.Pin GPIO_NONE = Cpu.Pin.GPIO_NONE;

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA0 = (Cpu.Pin)((0 * 16) + 0);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA1 = (Cpu.Pin)((0 * 16) + 1);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA2 = (Cpu.Pin)((0 * 16) + 2);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA3 = (Cpu.Pin)((0 * 16) + 3);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA4 = (Cpu.Pin)((0 * 16) + 4);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA5 = (Cpu.Pin)((0 * 16) + 5);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA6 = (Cpu.Pin)((0 * 16) + 6);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA7 = (Cpu.Pin)((0 * 16) + 7);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA8 = (Cpu.Pin)((0 * 16) + 8);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA9 = (Cpu.Pin)((0 * 16) + 9);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA10 = (Cpu.Pin)((0 * 16) + 10);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA11 = (Cpu.Pin)((0 * 16) + 11);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA12 = (Cpu.Pin)((0 * 16) + 12);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA13 = (Cpu.Pin)((0 * 16) + 13);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA14 = (Cpu.Pin)((0 * 16) + 14);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PA15 = (Cpu.Pin)((0 * 16) + 15);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB0 = (Cpu.Pin)((1 * 16) + 0);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB1 = (Cpu.Pin)((1 * 16) + 1);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB2 = (Cpu.Pin)((1 * 16) + 2);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB3 = (Cpu.Pin)((1 * 16) + 3);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB4 = (Cpu.Pin)((1 * 16) + 4);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB5 = (Cpu.Pin)((1 * 16) + 5);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB6 = (Cpu.Pin)((1 * 16) + 6);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB7 = (Cpu.Pin)((1 * 16) + 7);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB8 = (Cpu.Pin)((1 * 16) + 8);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB9 = (Cpu.Pin)((1 * 16) + 9);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB10 = (Cpu.Pin)((1 * 16) + 10);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB11 = (Cpu.Pin)((1 * 16) + 11);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB12 = (Cpu.Pin)((1 * 16) + 12);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB13 = (Cpu.Pin)((1 * 16) + 13);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB14 = (Cpu.Pin)((1 * 16) + 14);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PB15 = (Cpu.Pin)((1 * 16) + 15);



            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC0 = (Cpu.Pin)((2 * 16) + 0);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC1 = (Cpu.Pin)((2 * 16) + 1);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC2 = (Cpu.Pin)((2 * 16) + 2);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC3 = (Cpu.Pin)((2 * 16) + 3);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC4 = (Cpu.Pin)((2 * 16) + 4);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC5 = (Cpu.Pin)((2 * 16) + 5);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC6 = (Cpu.Pin)((2 * 16) + 6);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC7 = (Cpu.Pin)((2 * 16) + 7);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC8 = (Cpu.Pin)((2 * 16) + 8);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC9 = (Cpu.Pin)((2 * 16) + 9);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC10 = (Cpu.Pin)((2 * 16) + 10);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC11 = (Cpu.Pin)((2 * 16) + 11);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC12 = (Cpu.Pin)((2 * 16) + 12);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC13 = (Cpu.Pin)((2 * 16) + 13);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC14 = (Cpu.Pin)((2 * 16) + 14);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PC15 = (Cpu.Pin)((2 * 16) + 15);



            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD0 = (Cpu.Pin)((3 * 16) + 0);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD1 = (Cpu.Pin)((3 * 16) + 1);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD2 = (Cpu.Pin)((3 * 16) + 2);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD3 = (Cpu.Pin)((3 * 16) + 3);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD4 = (Cpu.Pin)((3 * 16) + 4);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD5 = (Cpu.Pin)((3 * 16) + 5);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD6 = (Cpu.Pin)((3 * 16) + 6);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD7 = (Cpu.Pin)((3 * 16) + 7);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD8 = (Cpu.Pin)((3 * 16) + 8);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD9 = (Cpu.Pin)((3 * 16) + 9);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD10 = (Cpu.Pin)((3 * 16) + 10);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD11 = (Cpu.Pin)((3 * 16) + 11);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD12 = (Cpu.Pin)((3 * 16) + 12);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD13 = (Cpu.Pin)((3 * 16) + 13);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD14 = (Cpu.Pin)((3 * 16) + 14);

            /// <summary>Digital I/O.</summary>
            public const Cpu.Pin PD15 = (Cpu.Pin)((3 * 16) + 15);

            }

        }
    }
