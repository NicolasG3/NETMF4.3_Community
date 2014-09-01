#ifndef _RLPLiteDriver_H_
#define _RLPLiteDriver_H_

#include "HAL.h"
//#include <TinyCLR_Interop.h>
typedef int (*USER_FUNCTION)(void*, int *, unsigned char *);

INT32 InvokeRLP(UINT32 address, void* param0, int* param1, unsigned char* param2);

#endif //_RLPLiteDriver_H_
