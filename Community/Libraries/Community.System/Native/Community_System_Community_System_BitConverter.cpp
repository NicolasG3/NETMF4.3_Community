////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Nicolas Gallerand
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include "Community_System.h"
#include "Community_System_Community_System_BitConverter.h"

using namespace Community::System;

INT64 BitConverter::DoubleToInt64Bits( double param0, HRESULT &hr )
{
	return *(((INT64*)&param0));
}

double BitConverter::Int64BitsToDouble( INT64 param0, HRESULT &hr )
{
	return *(((double*)&param0));
}

void BitConverter::SetBytes( INT16 param0, CLR_RT_TypedArray_UINT8 param1, INT32 param2, HRESULT &hr )
{
	if (param2<0 || param1.GetSize() < param2+2){
		hr = CLR_E_OUT_OF_RANGE;
		return;
	}
	UINT8* bytePtr = (UINT8*)param1.GetBuffer() + param2;
	*((INT16*)bytePtr) = param0;
}

void BitConverter::SetBytes( INT32 param0, CLR_RT_TypedArray_UINT8 param1, INT32 param2, HRESULT &hr )
{
	if (param2<0 || param1.GetSize() < param2+4){
		hr = CLR_E_OUT_OF_RANGE;
		return;
	}
	UINT8* bytePtr = (UINT8*)param1.GetBuffer() + param2;
	*((INT32*)bytePtr) = param0;
}

void BitConverter::SetBytes( INT64 param0, CLR_RT_TypedArray_UINT8 param1, INT32 param2, HRESULT &hr )
{
	if (param2<0 || param1.GetSize() < param2+8){
		hr = CLR_E_OUT_OF_RANGE;
		return;
	}
	UINT8* bytePtr = (UINT8*)param1.GetBuffer() + param2;
	*((INT64*)bytePtr) = param0;
}

void BitConverter::SetBytes( float param0, CLR_RT_TypedArray_UINT8 param1, INT32 param2, HRESULT &hr )
{
	if (param2<0 || param1.GetSize() < param2+4){
		hr = CLR_E_OUT_OF_RANGE;
		return;
	}
	UINT8* bytePtr = (UINT8*)param1.GetBuffer() + param2;
	*((float*)bytePtr) = param0;
}

void BitConverter::SetBytes( double param0, CLR_RT_TypedArray_UINT8 param1, INT32 param2, HRESULT &hr )
{
	if (param2<0 || param1.GetSize() < param2+8){
		hr = CLR_E_OUT_OF_RANGE;
		return;
	}
	UINT8* bytePtr = (UINT8*)param1.GetBuffer() + param2;
	*((double*)bytePtr) = param0;
}

float BitConverter::ToSingle( CLR_RT_TypedArray_UINT8 param0, INT32 param1, HRESULT &hr )
{
	if (param1<0 || param0.GetSize() < param1+4){
		hr = CLR_E_OUT_OF_RANGE;
		return;
	}
	UINT8* bytePtr = (UINT8*)param0.GetBuffer() + param1;
	return *(((float*)bytePtr));
}

double BitConverter::ToDouble( CLR_RT_TypedArray_UINT8 param0, INT32 param1, HRESULT &hr )
{
	if (param1<0 || param0.GetSize() < param1+8){
		hr = CLR_E_OUT_OF_RANGE;
		return;
	}
	UINT8* bytePtr = (UINT8*)param0.GetBuffer() + param1;
	return *(((double*)bytePtr));
}

INT16 BitConverter::ToInt16( CLR_RT_TypedArray_UINT8 param0, INT32 param1, HRESULT &hr )
{
	if (param1<0 || param0.GetSize() < param1+2){
		hr = CLR_E_OUT_OF_RANGE;
		return;
	}
	UINT8* bytePtr = (UINT8*)param0.GetBuffer() + param1;
	return *(((INT16*)bytePtr));
}

INT32 BitConverter::ToInt32( CLR_RT_TypedArray_UINT8 param0, INT32 param1, HRESULT &hr )
{
	if (param1<0 || param0.GetSize() < param1+4){
		hr = CLR_E_OUT_OF_RANGE;
		return;
	}
	UINT8* bytePtr = (UINT8*)param0.GetBuffer() + param1;
	return *(((INT32*)bytePtr));
}

INT64 BitConverter::ToInt64( CLR_RT_TypedArray_UINT8 param0, INT32 param1, HRESULT &hr )
{
	if (param1<0 || param0.GetSize() < param1+8){
		hr = CLR_E_OUT_OF_RANGE;
		return;
	}
	UINT8* bytePtr = (UINT8*)param0.GetBuffer() + param1;
	return *(((INT64*)bytePtr));
}

