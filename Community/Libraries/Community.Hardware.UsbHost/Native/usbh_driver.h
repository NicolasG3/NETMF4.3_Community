#ifndef _USBH_DRIVER_
#define _USBH_DRIVER_ 1

#include "tinyHal.h"

BOOL USBH_Initialize( int Controller );
//static BOOL Uninitialize( int Controller );
void USBH_SetState(int driveID, int state );


#endif