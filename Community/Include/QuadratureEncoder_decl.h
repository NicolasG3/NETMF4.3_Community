////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Nicolas Gallerand
//
//  *** Quadrature encoder Driver interface  ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRIVERS_QUADRATUREENCODER_DECL_H_
#define _DRIVERS_QUADRATUREENCODER_DECL_H_ 1

//QED_GetIOStatus() return values
#define IOStatus_None				0
#define IOStatus_InputCapture		1
#define IOStatus_OutputCompare		2

//Capture/compare callback function type
typedef void (*QED_EVENT) (int channel, int IOIndex, int count, BOOL overlap);


//Get Quadrature Encoder channel count for this hardware
UINT32		QED_ChannelCount	();
//Get the input pins for the specified channel
HRESULT		QED_GetPinsForChannel	(int channel, GPIO_PIN* pinA, GPIO_PIN* pinB);
//Get IO count for the specified Quadrature Encoder channel (IO are used for InputCapture or OutputCompare)
UINT32		QED_GetIOCountForChannel	(int channel);
//Get the hardware pin for the specified IO index of the specified channel
GPIO_PIN	QED_GetIOPinForChannel	(int channel, int IOIndex);
//Get the specified IO reservation status (see #define IOStatus_xxx)
INT32	QED_GetIOStatus	(int channel, int IOIndex);

//Initialize the specified channel. Reserve A and B pins (not the IO pins)
//countMode X1=1, X2=2, X4=3
HRESULT	QED_Initialize		(int channel, int countMode);
//Unitialize the specified channel
//Release A and B pins and IO pins if required
void	QED_Uninitialize	(int channel);
//Get the actual count of the specified channel
int		QED_GetCount		(int channel);
//Set the actual count of the specified channel
HRESULT	QED_SetCount		(int channel, int value);
//Indicate wether counting is enabled for the specified channel
BOOL QED_GetCountEnable	(int channel);
//Set if counting is enabled for the specified channel
//When not enabled, count value is kept unchanged regardless of A and B inputs
HRESULT	QED_SetCountEnable	(int channel, BOOL enable);
//Initialize the specified IO for output compare, negative pulse if invert is true
//pulseLength=0 means infinite pulse
//Reserve the IO pin and set the output state at first call, support successive calls (do not set the output state if not first call)
HRESULT	QED_InitOutputCompare(int channel, int IOIndex, int value, BOOL invert, int pulseLength, QED_EVENT onOutputCompare);
//Initialize the specified IO for input capture on raising edge or falling edge if invert is true
//Reserve the IO pin at first call
HRESULT	QED_InitInputCapture(int channel, int IOIndex, BOOL invert, QED_EVENT onInputCapture);
//Release the specified IO pin if reserved by QED_InitOutputCompare() or QED_InitInputCapture()
void	QED_ReleaseIO(int channel, int IOIndex);


#endif // _DRIVERS_QUADRATUREENCODER_DECL_H_
