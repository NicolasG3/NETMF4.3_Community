@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ 
@  
@  
@  This file is part of the Microsoft .NET Micro Framerwork Porting Kit Code Samples and is unsupported. 
@  Copyright (C) Microsoft Corporation. All rights reserved. Use of this sample source code is subject to 
@  the terms of the Microsoft license agreement under which you licensed this sample source code. 
@  
@  THIS SAMPLE CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, 
@  INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
@  
@  
@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ 

     .section   tinyclr_metadata, "a", %progbits

     .align 4

    .global  TinyClr_Dat_Start
    .global  TinyClr_Dat_End

TinyClr_Dat_Start:
    .incbin "tinyclr.dat"
TinyClr_Dat_End:

    .end

