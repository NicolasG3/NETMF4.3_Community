////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Portions Copyright (c) GHI Electronics, LLC.
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include <TinyBooterEntry.h>

#define BUTTON_ENTR     BUTTON_B4
#define BUTTON_UP_IDX   BUTTON_B2_BITIDX
#define BUTTON_DOWN_IDX BUTTON_B5_BITIDX


////////////////////////////////////////////////////////////////////////////////
//  Tinybooter_ProgramWordCheck
//
//  Returns the value of the first binary word of an application in FLASH
//  Tinybooter will search for this value at the beginning of each flash sector
//  at boot time and will execute the first instance.
////////////////////////////////////////////////////////////////////////////////

UINT32 Tinybooter_ProgramWordCheck()
{

    return 0xE321F0DF;
}


////////////////////////////////////////////////////////////////////////////////
//  Tinybooter_ImageIsCompressed
//
//  Returns true if the image is compressed in flash and needs to be decompressed
//  and run from ram.  The image must be compressed by the BuildHelper tool.
////////////////////////////////////////////////////////////////////////////////
bool Tinybooter_ImageIsCompressed()
{
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//  Tinybooter_CompressedImageStart
//
//  Returns the address of the entry point after the image is decompressed into
//  RAM. 
////////////////////////////////////////////////////////////////////////////////
UINT32 Tinybooter_CompressedImageStart( const CompressedImage_Header& header )
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//  Tinybooter_PrepareForDecompressedLaunch
//
//  Some platforms require some initialzation before they can be executed after
//  decompression.
////////////////////////////////////////////////////////////////////////////////
void Tinybooter_PrepareForDecompressedLaunch()
{
}


////////////////////////////////////////////////////////////////////////////////
// The WaitForTinyBooterUpload method was designed to allow porting kit partners
// to define how/when tinybooter mode is entered as well as configure default
// timeout values.  
//
// timeout_ms   - this parameter determines the time in milliseconds TinyBooter is
//                supposed to wait for commands from the host.  A -1 value will 
//                indicate to wait forever.
// return value - the boolean return value indicates whether TinyBooter should enter
//                upload mode.  If false is returned the booter will attempt to
//                launch the first application in FLASH that it finds.  If the return
//                value is true, TinyBooter will wait for the given timeout value
//                (parameter timeout_ms) for valid commands before launching the first
//                application
////////////////////////////////////////////////////////////////////////////////
bool WaitForTinyBooterUpload( INT32 &timeout_ms )
{
    bool enterBooterMode = false;
    //4.1// GPIO_BUTTON_CONFIG *  ButtonConfig = &g_GPIO_BUTTON_Config;

    timeout_ms = 5000;
    enterBooterMode = false; //4.1// true;

	
	
// always wait forever when using RAM build
#if defined(TARGETLOCATION_RAM)
    enterBooterMode = true;
    timeout_ms = -1;
#endif

    // user override (UP+DOWN buttons held)
    //4.1// if ((ButtonConfig->Mapping[BUTTON_DOWN_IDX].m_HW != GPIO_PIN_NONE) && (ButtonConfig->Mapping[BUTTON_UP_IDX].m_HW != GPIO_PIN_NONE))
    if( CPU_GPIO_EnableInputPin(((1 * 32) + 27), FALSE, NULL, GPIO_INT_NONE, RESISTOR_PULLUP)) //(1 * 32) + 27) = PB27 Pin9 on Socket 14
	{
        Events_WaitForEvents(0,100); // wait for buttons to init
        //4.1// if(!CPU_GPIO_GetPinState( ButtonConfig->Mapping[BUTTON_DOWN_IDX].m_HW ) && !CPU_GPIO_GetPinState( ButtonConfig->Mapping[BUTTON_UP_IDX].m_HW ))
        if(!CPU_GPIO_GetPinState(((1 * 32) + 27)))
        {
            // user override, so lets stay forever
            timeout_ms = -1;
            enterBooterMode = true;
        }
    }
    return enterBooterMode;
}

////////////////////////////////////////////////////////////////////////////////
// The TinyBooter_OnStateChange method is an event handler for state changes in 
// the TinyBooter.  It is designed to help porting kit users control the tinybooter
// execution and allow them to add diagnostics.
////////////////////////////////////////////////////////////////////////////////

void TinyBooter_OnStateChange( TinyBooterState state, void* data, void ** retData )
{
    switch(state)
    {
        ////////////////////////////////////////////////////////////////////////////////////
        // State_EnterBooterMode - TinyBooter has entered upload mode
        ////////////////////////////////////////////////////////////////////////////////////
        case State_EnterBooterMode:
            hal_fprintf( STREAM_LCD, "Waiting\r" );
            break;

        ////////////////////////////////////////////////////////////////////////////////////
        // State_ButtonPress - A button was pressed while Tinybooter 
        // The data parameter is a pointer to the timeout value for the booter mode.
        ////////////////////////////////////////////////////////////////////////////////////
        case State_ButtonPress:
            if(NULL != data)
            {
                UINT32 down, up;
                INT32* timeout_ms = (INT32*)data;
                
                // wait forever if a button was pressed
                *timeout_ms = -1;

                // process buttons
                while(Buttons_GetNextStateChange(down, up))
                {
                    // leave a way to exit boot mode incase it was accidentally entered
                     if(0 != (down & BUTTON_ENTR)) 
                     {
                        // force an enumerate and launch
                        *timeout_ms = 0; 
                     }
                }
            }
            break;

        ////////////////////////////////////////////////////////////////////////////////////
        // State_ValidCommunication - TinyBooter has received valid communication from the host
        // The data parameter is a pointer to the timeout value for the booter mode.
        ////////////////////////////////////////////////////////////////////////////////////
        case State_ValidCommunication:
            if(NULL != data)
            {
                INT32* timeout_ms = (INT32*)data;

                // if we received any com/usb data then let's change the timeout to at least 20 seconds
                if(*timeout_ms != -1 && *timeout_ms < 20000)
                {
                    *timeout_ms = 20000;
                }
            }
            break;

        ////////////////////////////////////////////////////////////////////////////////////
        // State_Timeout - The default timeout for TinyBooter has expired and TinyBooter will 
        // perform an EnumerateAndLaunch
        ////////////////////////////////////////////////////////////////////////////////////
        case State_Timeout:
            break;

        ////////////////////////////////////////////////////////////////////////////////////
        // State_MemoryXXX - Identifies memory accesses.
        ////////////////////////////////////////////////////////////////////////////////////
        case State_MemoryWrite:
            hal_fprintf( STREAM_LCD, "Wr: 0x%08x\r", (UINT32)data );
            break;
        case State_MemoryErase:
            hal_fprintf( STREAM_LCD, "Er: 0x%08x\r", (UINT32)data );
            break;

            
        ////////////////////////////////////////////////////////////////////////////////////
        // State_CryptoXXX - Start and result of Crypto signature check
        ////////////////////////////////////////////////////////////////////////////////////
        case State_CryptoStart:
            hal_fprintf( STREAM_LCD,     "Chk signature \r" );
            hal_printf( "Chk signature \r" );
            break;
        // The data parameter is a boolean that represents signature PASS/FAILURE
        case State_CryptoResult:
            if((bool)data)
            {
                hal_fprintf( STREAM_LCD, "Signature PASS\r\n\r\n" );
                hal_printf( "Signature PASS\r\n\r\n" );
            }
            else
            {
                hal_fprintf( STREAM_LCD, "Signature FAIL\r\n\r\n" );
                hal_printf( "Signature FAIL\r\n\r\n"  );
            }
            DebuggerPort_Flush(HalSystemConfig.DebugTextPort);
            break;

        ////////////////////////////////////////////////////////////////////////////////////
        // State_Launch - The host has requested to launch an application at a given address, 
        //                or a timeout has occured and TinyBooter is about to launch the 
        //                first application it finds in FLASH.
        //
        // The data parameter is a UINT32 value representing the launch address
        ////////////////////////////////////////////////////////////////////////////////////
        case State_Launch:
            UINT32 address = (UINT32)data;

#define CODE_BASEADDRESS   0x00084000 //0x00060000
#define EXCODE_BASEADDRESS 0x20100000 //0x20040000 

//4.1//#define DATA_BASEADDRESS   0x001C0000
//4.1//#define EXDATA_BASEADDRESS 0x200C0000

//4.1//#define CONFIG_BASEADDRESS 0x00020000
//4.1//#define CONGIG_Size        0x00020000

//4.1//#define DEPLOY_BASEADDRESS 0x00200000

#define CODE_SIZE          0x00200000//4.1// DATA_BASEADDRESS-CODE_BASEADDRESS
//4.1//#define DATA_SIZE           DEPLOY_BASEADDRESS-DATA_BASEADDRESS


            if(NULL != data)
            {
                hal_fprintf( STREAM_LCD, "Starting application at 0x%08x\r\n", (UINT32)data );
                if (address == CODE_BASEADDRESS)
                {
                    BlockStorageDevice *device;
                    ByteAddress ByteAddress;
                    UINT32 physicalAddress = (UINT32)data;
                    
                    if (BlockStorageList::FindDeviceForPhysicalAddress( &device, physicalAddress, ByteAddress)) 
                    {
                        BlockStorageStream stream;

                        if(stream.Initialize(BlockUsage::CODE, device))
                        {
                            // load ER_FLASH only
                            BYTE *dst;
                            dst =(BYTE *) EXCODE_BASEADDRESS  ;


                            stream.Read( &dst, CODE_SIZE );

                            if (retData != NULL)
                            {
                                *retData =(void*) EXCODE_BASEADDRESS;
                            }
                        }
                    }

                }
                else if(retData != NULL)
                {
                   *retData =(void*) data;
                }

            }
            
            break;
    }
            
}


////////////////////////////////////////////////////////////////////////////////
// The SectorOverlapsBootstrapRegion method enables you to deny access for writing
// certain sectors.  Returning true does not guarrantee that Tinybooter will be 
// able to write to the sector.  It performes other checks (including signature 
// validation) to determine if the write operation is valid.
////////////////////////////////////////////////////////////////////////////////
bool CheckFlashSectorPermission( BlockStorageDevice *pDevice, ByteAddress address )
{
    bool fAllowWrite = false;
    UINT32 BlockType, iRegion, iRange;
    

    if(pDevice->FindRegionFromAddress(address, iRegion, iRange))
    {
        const BlockRange& range = pDevice->GetDeviceInfo()->Regions[iRegion].BlockRanges[iRange];
        
        if (range.IsBootstrap())
        {
#if defined(TARGETLOCATION_RAM) // do not allow overwriting the bootstrap sector unless Tinybooter is RAM build.
                fAllowWrite = true;
#else
                hal_printf( "Trying to write to bootstrap region\r\n" );
                fAllowWrite = false;
#endif
        }
        else
        {
                fAllowWrite = true;
        }
    }    
    return fAllowWrite;   
}


////////////////////////////////////////////////////////////////////////////////
// TinyBooter_GetOemInfo
//
// Return in version and oeminfostring the compile-time values of some OEM-specific
// data, which you provide in the ReleaseInfo.settings file to apply to all projects
// built in this Porting Kit, or in the settings file specific to this solution.
//
// The properties to set are 
//  MajorVersion, MinorVersion, BuildNumber, RevisionNumber, and OEMSystemInfoString.
// If OEMSystemInfoString is not provided, it will be created by concatenating your
// settings for MFCompanyName and MFCopyright, also defined in ReleaseInfo.settings
// or your solution's own settings file.
//
// It is typically not necessary actually to modify this function. If you do, note
// that the return value indicates to the caller whether the releaseInfo structure
// has been fully initialized and is valid. It is safe to return false if 
// there is no useful build information you wish to report.
////////////////////////////////////////////////////////////////////////////////
BOOL TinyBooter_GetReleaseInfo(MfReleaseInfo& releaseInfo)
{
    MfReleaseInfo::Init(
                    releaseInfo, 
                    GHI_VERSION_MAJOR /*VERSION_MAJOR*/, GHI_VERSION_MINOR /*VERSION_MINOR*/, GHI_VERSION_BUILD/*VERSION_BUILD*/, GHI_VERSION_REVISION/*VERSION_REVISION*/, 
                    GHIOEMSTRING/*OEMSYSTEMINFOSTRING*/, hal_strlen_s(GHIOEMSTRING/*OEMSYSTEMINFOSTRING*/)
                    );
    return TRUE;
}

