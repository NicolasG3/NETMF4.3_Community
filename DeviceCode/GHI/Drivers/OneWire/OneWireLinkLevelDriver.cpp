//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//---------------------------------------------------------------------------
//
//  DS520LNK.C - Link Layer functions required by general 1-Wire drive
//           implementation for DS80C520 microcontroller.
//
//  Version: 2.00
//
//  History: 1.00 -> 1.01  Added function msDelay.
//           1.02 -> 1.03  Added function msGettick.
//           1.03 -> 2.00  Changed 'MLan' to 'ow'. Added support for
//                         multiple ports.
//           2.10 -> 3.00  Added owReadBitPower and owWriteBytePower
//

#include "ownet.h"
#include "CPU_GPIO_decl.h"


//#define tRSTL           	600        // 480uS < tRSTL < 960uS
//#define tRST_SAMPLE      	80
//#define tRSTH           	(600 - tRST_SAMPLE)        // 480uS < tRSTH < 960uS
//#define tREC             	3//10
//#define tLOWR             	2
//#define tRDV             	15
// #define tRELEASE         	55//30
// #define tLOW           		6//10
// #define tSLOT           	60//80

#define PIN_DIR_OUTPUT		0
#define PIN_DIR_INPUT		1

//Assume that every command takes 3us
#define US_LATE 3

#define DELAY_US_A  (6-US_LATE) 
#define DELAY_US_B  (64-US_LATE)
#define DELAY_US_C  (60-US_LATE)
#define DELAY_US_D  (10-US_LATE) 
#define DELAY_US_E  (9-US_LATE) 
#define DELAY_US_F  (55-US_LATE)
#define DELAY_US_G  0
#define DELAY_US_H  500 // range from 480-600 is fine
#define DELAY_US_I  (70-US_LATE) 
#define DELAY_US_J  (410-US_LATE) 

//void usDelay(unsigned int delay);
static SMALLINT USpeed; // current 1-Wire Net communication speed
static SMALLINT ULevel; // current 1-Wire Net level
void OW_usDelay(UINT32 delay)
{
	HAL_Time_Sleep_MicroSeconds_InterruptEnabled(delay);
}

void OW_SetPinDir(int pin, int state)
{
	if(state)
	{
		CPU_GPIO_EnableInputPin(pin, FALSE, NULL, GPIO_INT_NONE, RESISTOR_DISABLED);
	}
	else
	{
		CPU_GPIO_EnableOutputPin(pin, FALSE);
	}
}

int OW_GetPinState(int pin)
{
	return CPU_GPIO_GetPinState(pin);
}


//--------------------------------------------------------------------------
// Reset all of the devices on the 1-Wire Net and return the result.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns: TRUE(1):  presense pulse(s) detected, device(s) reset
//          FALSE(0): no presense pulses detected
//
SMALLINT owTouchReset(int portnum)
{
   uchar result;

   UINT32 pin = (UINT32)portnum; 	
	//OW_usDelay(DELAY_US_G);
   // Code from appnote 126.
   CPU_GPIO_EnableOutputPin( pin, false );  // impulse start OW_PORT = 0; // drive bus low.

   OW_usDelay(DELAY_US_H);  	

   // 500-(3% error) ~= 480 us

   // OW_PORT = 1; // bus high. 
   // Note that the 1Wire bus will need external pullup to supply the required current
   CPU_GPIO_EnableInputPin( pin, false, NULL, GPIO_INT_NONE, RESISTOR_DISABLED );

   OW_usDelay(DELAY_US_I); 

   result = CPU_GPIO_GetPinState(pin); 	//!OW_PORT; // get presence detect pulse.
	 
   OW_usDelay(DELAY_US_J); 							// 372-(3% error) ~= 360 us

   
	
   return result == 0;
}

SMALLINT owTouchBitWrite(int portnum, SMALLINT sendbit)
{
	//unsigned char result=0;
	//int i;
	UINT32 pin = (UINT32)portnum; 
	if(sendbit)
	{
		OW_SetPinDir (pin, PIN_DIR_OUTPUT);
		OW_usDelay (DELAY_US_A);
		OW_SetPinDir (pin, PIN_DIR_INPUT);
		// OW_usDelay (tSLOT - tLOW);
		// OW_usDelay (tREC );
		OW_usDelay (DELAY_US_B);
	}
	else
	{
		OW_SetPinDir (pin, PIN_DIR_OUTPUT);
		//OW_usDelay (tSLOT);
		OW_usDelay (DELAY_US_C);
		OW_SetPinDir (pin, PIN_DIR_INPUT);
		//OW_usDelay (tREC);
		OW_usDelay (DELAY_US_D);
	}
	return TRUE;
}

SMALLINT owTouchBitRead(int portnum, SMALLINT sendbit)
{
	unsigned char result;
	UINT32 pin = (UINT32)portnum; 
	OW_SetPinDir (pin, PIN_DIR_OUTPUT);
	OW_usDelay (DELAY_US_A);
	OW_SetPinDir (pin, PIN_DIR_INPUT);
	OW_usDelay (DELAY_US_E);
	result = OW_GetPinState (pin);
	OW_usDelay (DELAY_US_F);
	//OW_usDelay (tREC);
	return result;
	
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and return the
// result 1 bit read from the 1-Wire Net.  The parameter 'sendbit'
// least significant bit is used and the least significant bit
// of the result is the return bit.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbit'    - the least significant bit is the bit to send
//
// Returns: 0:   0 bit read from sendbit
//          1:   1 bit read from sendbit
//
SMALLINT owTouchBit(int portnum, SMALLINT sendbit)
{
	int ret;
	UINT32 pin = (UINT32)portnum; 
	GLOBAL_LOCK(irq); //EA = 0;
	OW_SetPinDir (pin, PIN_DIR_OUTPUT);
	OW_usDelay (DELAY_US_A);
	if (sendbit==1)
	{
		OW_SetPinDir (pin, PIN_DIR_INPUT);
		OW_usDelay (DELAY_US_A);
		ret = OW_GetPinState (pin);
		OW_usDelay (DELAY_US_F);
	}
	else 
	{
		OW_usDelay (DELAY_US_F);
		OW_SetPinDir (pin, PIN_DIR_INPUT);
	}
	OW_usDelay (DELAY_US_A);
	irq.Release();
	return (ret & 0x00000001);
	
}
/*
SMALLINT owTouchBit1(int portnum, SMALLINT sendbit)
{
	unsigned char result=0;

	UINT32 pin = (UINT32)portnum; 

	// iming critical, so I'll disable interrupts here
	GLOBAL_LOCK(irq); //EA = 0;

	// start timeslot.
	CPU_GPIO_EnableOutputPin( pin, false );

	usDelay(5);

	if (sendbit == 1)
	{
		// send 1 bit out.
		// sample result @ 15 us.
		CPU_GPIO_EnableInputPin( pin, false, NULL, GPIO_INT_NONE, RESISTOR_DISABLED );
		usDelay(5);
		result = CPU_GPIO_GetPinState(pin); 	
		usDelay(55);
	}
	else
	{
		// send 0 bit out.
		::CPU_GPIO_SetPinState(pin, 0); 	
		usDelay(70);
	}
	// timeslot done. Set output high.
	CPU_GPIO_EnableOutputPin( pin, true );

	usDelay(5);

	// estore interrupts
	irq.Release();

	return (result & 0x00000001);
}
*/
SMALLINT owTouchByteWrite(int portnum, SMALLINT sendbyte)
{
	UINT32 pin = (UINT32)portnum; 
	unsigned char mask;
	GLOBAL_LOCK(irq);
	for (mask = 0x01; mask; mask <<= 1)
	{
		if (sendbyte & mask)
			owTouchBitWrite(pin, 1);
		else
			owTouchBitWrite(pin, 0);
	}		
	return sendbyte;
}
SMALLINT owTouchByteRead(int portnum, SMALLINT sendbyte)
{
   UINT32 pin = (UINT32)portnum; 
	unsigned char mask, ret;
	GLOBAL_LOCK(irq);
	for (ret = 0, mask = 0x01; mask; mask <<= 1)
	{
		if (owTouchBitRead(pin, 0xFF))
			ret |= mask;
	}		
	return ret;
}
//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and return the
// result 8 bits read from the 1-Wire Net.  The parameter 'sendbyte'
// least significant 8 bits are used and the least significant 8 bits
// of the result is the return byte.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  8 bits read from sendbyte
//
SMALLINT owTouchByte(int portnum, SMALLINT sendbyte)
{
   return 0;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
SMALLINT owWriteByte(int portnum, SMALLINT sendbyte)
{
   return (owTouchByteWrite(portnum,sendbyte) == sendbyte) ? TRUE : FALSE;
}

//--------------------------------------------------------------------------
// Send 8 bits of read communication to the 1-Wire Net and and return the
// result 8 bits read from the 1-Wire Net.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns:  8 bytes read from 1-Wire Net
//
SMALLINT owReadByte(int portnum)
{
   return owTouchByteRead(portnum,0xFF);
}

//--------------------------------------------------------------------------
// Set the 1-Wire Net communucation speed.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'new_speed'  - new speed defined as
//                MODE_NORMAL     0x00
//                MODE_OVERDRIVE  0x01
//
// Returns:  current 1-Wire Net speed
//
SMALLINT owSpeed(int portnum, SMALLINT new_speed)
{
   portnum = 0;

   USpeed = new_speed;
   // not supported yet

   // TODO: Research overdrive.  33mhz should handle it.

   return 0;
}

//--------------------------------------------------------------------------
// Set the 1-Wire Net line level.  The values for NewLevel are
// as follows:
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'new_level'  - new level defined as
//                MODE_NORMAL     0x00
//                MODE_STRONG5    0x02
//                MODE_PROGRAM    0x04
//                MODE_BREAK      0x08
//
// Returns:  current 1-Wire Net level
//
// Note: Strong and Program not supported on 520 target.
SMALLINT owLevel(int portnum, SMALLINT new_level)
{
   portnum = 0;
   
   UINT32 pin = (UINT32)portnum; 
   
   switch(new_level)
   {
      case MODE_BREAK:
         //::CPU_GPIO_SetPinState(pin, false); //OW_PORT = 0; //send a break on one-wire port
		  CPU_GPIO_EnableOutputPin( pin, false );
         break;
      default:
      case MODE_NORMAL:
         //::CPU_GPIO_SetPinState(pin, true); //OW_PORT = 1;
          CPU_GPIO_EnableOutputPin( pin, true );
		  break;
   }
   ULevel = new_level;
   return new_level;
}

//--------------------------------------------------------------------------
// This procedure creates a fixed 480 microseconds 12 volt pulse
// on the 1-Wire Net for programming EPROM iButtons.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns:  TRUE  successful
//           FALSE program voltage not available
//
SMALLINT owProgramPulse(int portnum)
{
   portnum = 0;
   // Not supported
   return 0;
}

//--------------------------------------------------------------------------
//  Description:
//     Delay for at least 'len' ms
//
void msDelay(unsigned int delay)
{
   while(delay--)
   {
      usDelay(1030); // -3% error puts us right at 1ms.
	}
}

//--------------------------------------------------------------------------
//  Description:
//     Delay for at least 'len' microseconds
//
// Adapted from Kevin Vigor's TINI 1Wire code.
//
// Works only @33Mhz.  We need 8.25 machine cycles per iteration to get the microsecond
// delay.  Can't have .25 of a cycle, so this function uses 8 cycles per iteration. This
// produces an error of 3% ( .03 = (8.25-8)/8 ).  Most of the use of this function by
// 1-Wire routines calls for delays under 50 microseconds, in which case it is under by
// less than 1.5 microseconds.  It is assumed the overhead of calling the function will
// add enough to account for the difference.  For values much greater than 50 (only in
// reset function), a "fudge factor" is added to account for any error.
//
//
void usDelay(unsigned int delay)
{
	 // int i;
	 //or(i=0; i<10*delay; i++);
	// if (delay<=15) 
	// {
		// for(i=0;i<delay-1; i++);
	// }
	// else
	// {	 
		// if (delay>100) 
			// HAL_Time_Sleep_MicroSeconds( delay-20);
		// else 
			// HAL_Time_Sleep_MicroSeconds( delay-15);
	// }
	OW_usDelay(delay);
}

//--------------------------------------------------------------------------
// Get the current millisecond tick count.  Does not have to represent
// an actual time, it just needs to be an incrementing timer.
//
long msGettick(void)
{
   // not supported yet
   return 0;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).  
// The parameter 'sendbyte' least significant 8 bits are used.  After the
// 8 bits are sent change the level of the 1-Wire net.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
// 'sendbyte' - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same 
//
SMALLINT owWriteBytePower(int portnum, SMALLINT sendbyte)
{	
	// not supported on the by the DS520
	return FALSE;
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and verify that the
// response matches the 'applyPowerResponse' bit and apply power delivery
// to the 1-Wire net.  Note that some implementations may apply the power
// first and then turn it off if the response is incorrect.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
// 'applyPowerResponse' - 1 bit response to check, if correct then start
//                        power delivery 
//
// Returns:  TRUE: bit written and response correct, strong pullup now on
//           FALSE: response incorrect
//
SMALLINT owReadBitPower(int portnum, SMALLINT applyPowerResponse)
{
	// not supported on the by the DS520
	return FALSE;
}

//--------------------------------------------------------------------------
// This procedure indicates wether the adapter can deliver power.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  if adapter is capable of delivering power. 
//
SMALLINT owHasPowerDelivery(int portnum)
{
   // add adapter specific code here
   return TRUE;
}

//--------------------------------------------------------------------------
// This procedure indicates wether the adapter can deliver power.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  if adapter is capable of over drive. 
//
SMALLINT owHasOverDrive(int portnum)
{
   // add adapter specific code here
   return TRUE;
}
//--------------------------------------------------------------------------
// This procedure creates a fixed 480 microseconds 12 volt pulse 
// on the 1-Wire Net for programming EPROM iButtons.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  program volatage available
//           FALSE program voltage not available  
SMALLINT owHasProgramPulse(int portnum)
{
   // add adapter specific code here
   return TRUE;
}
