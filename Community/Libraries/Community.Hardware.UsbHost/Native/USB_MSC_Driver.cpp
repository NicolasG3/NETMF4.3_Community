#include <tinyhal.h>
#include "USB_MSC_Driver.h"
#include "usb_core.h"
#include "usbh_driver.h"

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rwdata = "g_USB_MSC_BS_Config"
#endif

BlockRange g_USB_MSC_BlockRange[] =
{
	{ BlockRange::BLOCKTYPE_FILESYSTEM, 0, 0 }
};

BlockRegionInfo  g_USB_MSC_BlkRegion[] = 
{
	0,               // SectorAddress        Start;      // Starting Sector address
	0,               // UINT32           NumBlocks;      // total number of blocks in this region
	512,             // UINT16           BytesPerBlock;  // Number of Bytes in each block of this region 
	ARRAYSIZE_CONST_EXPR(g_USB_MSC_BlockRange),
	g_USB_MSC_BlockRange,
};

BlockDeviceInfo  g_USB_MSC_DeviceInfo=
{
	{  
		TRUE, //removable
			FALSE,//support XIP
			FALSE,//write protected
	},    
	700,			// UINT32 Duration_Max_WordWrite_uSec;
	2000000,		// UINT32 Duration_Max_SectorErase_uSec;
	512,                        // UINT32 BytesPerSector;
	0,                      // UINT32 Size;
	1,                     // UINT32 NumRegions;
	g_USB_MSC_BlkRegion,                        // const BlockRegionInfo* pRegions;
};

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rwdata
#endif

struct IBlockStorageDevice g_USB_MSC_DeviceTable = 
{
	&USB_MSC_Driver::InitializeDevice, 
	&USB_MSC_Driver::UninitializeDevice, 
	&USB_MSC_Driver::GetDeviceInfo, 
	&USB_MSC_Driver::Read, 
	&USB_MSC_Driver::Write,
	&USB_MSC_Driver::Memset,
	&USB_MSC_Driver::GetSectorMetadata,
	&USB_MSC_Driver::SetSectorMetadata,
	&USB_MSC_Driver::IsBlockErased, 
	&USB_MSC_Driver::EraseBlock, 
	&USB_MSC_Driver::SetPowerState, 
	&USB_MSC_Driver::MaxSectorWrite_uSec, 
	&USB_MSC_Driver::MaxBlockErase_uSec, 
};

extern MassStorageParameter_TypeDef USBH_MSC_Param; 
extern USBH_HOST                     USB_Host;


BOOL USB_MSC_Driver::InitializeDevice( void* context ){
	BlockDeviceInfo* pDevInfo =  &g_USB_MSC_DeviceInfo;
	//FAT32 only
	UINT32 bytesPerBlock = 512;// USBH_MSC_Param.MSPageLength;
	UINT32 lastSectorAdress = USBH_MSC_Param.MSCapacity;
	UINT64 diskSize = (UINT64)lastSectorAdress * 512; 

	pDevInfo->Size = (ByteAddress)diskSize; //overflow for more than 4GB if ByteAddress is UINT32

	BlockRegionInfo* pRegions = (BlockRegionInfo*)&pDevInfo->Regions[0];
	pRegions[0].BytesPerBlock = bytesPerBlock;
	pRegions[0].Start = 0;
	pRegions[0].NumBlocks     = diskSize / bytesPerBlock;

	BlockRange* pRanges   = (BlockRange*)&pRegions[0].BlockRanges[0];
	pRanges[0].StartBlock = 0;
	pRanges[0].EndBlock   = pRegions[0].NumBlocks - 1;
	return TRUE;
}

BOOL USB_MSC_Driver::UninitializeDevice( void* context ){
	return TRUE;
}

const BlockDeviceInfo* USB_MSC_Driver::GetDeviceInfo( void* context ){
	return &g_USB_MSC_DeviceInfo;
}


BOOL USB_MSC_Driver::Read( void* context, ByteAddress Address, UINT32 NumBytes, BYTE * pSectorBuff ){

	USB_OTG_CORE_HANDLE *pDev = (USB_OTG_CORE_HANDLE*)context;
	BYTE status = USBH_MSC_OK;

	if(!HCD_IsDeviceConnected(pDev)){
		return FALSE;
	}
	//flat adressing
	SectorAddress startSector = Address / 512;
	if (Address % 512 != 0) {
		CLR_Debug::Printf( "USB_MSC_Driver::Read error: Adress must match a sector start, sector=%08x, Address=%08x, NumBytes %d\r\n", startSector, Address, NumBytes);
		return FALSE;
	}
	//CLR_Debug::Printf( "USB_MSC_Driver::Read() address=%lld, numBytes=%d sector=%d\r\n", Address, NumBytes, startSector);
	//CLR_Debug::Printf( "USB_MSC_Driver::Read offset=%d, StartSector=0x%08x, Address=%08x, NumBytes %d\r\n", offset, StartSector, Address, NumBytes);
	USBH_MSC_BOTXferParam.CmdStateMachine = CMD_SEND_STATE;
	do {
		status = USBH_MSC_Read10(pDev, pSectorBuff, startSector, NumBytes);
		USBH_MSC_HandleBOTXfer(pDev ,&USB_Host);

		if(!HCD_IsDeviceConnected(pDev)) { 
			//CLR_Debug::Printf( "USB_MSC_Driver::Read() USB device disconnection when reading\r\n");
			return FALSE;
		}      
	} while(status == USBH_MSC_BUSY );

	if(status != USBH_MSC_OK)
		return FALSE;

	return TRUE;
}

BOOL USB_MSC_Driver::Write( void* context, ByteAddress Address, UINT32 NumBytes, BYTE * pSectorBuff, BOOL ReadModifyWrite ){
	//CLR_Debug::Printf( "USB_MSC_Driver::Write(*, %08x, %08x, *)\r\n", Address, NumBytes);

	USB_OTG_CORE_HANDLE *pDev = (USB_OTG_CORE_HANDLE*)context;
	BYTE status = USBH_MSC_OK;

	if(!HCD_IsDeviceConnected(pDev)){
		//CLR_Debug::Printf( "USB_MSC_Driver::Read() no USB device connected\r\n");
		return FALSE;
	}

	UINT32 StartSector = g_USB_MSC_DeviceInfo.PhysicalToSectorAddress( &g_USB_MSC_DeviceInfo.Regions[0], Address);
	//flat adressing
	SectorAddress startSector = Address / 512;
	if (Address % 512 != 0) {
		CLR_Debug::Printf( "USB_MSC_Driver::Write error: Adress must match a sector start, sector=%08x, Address=%08x, NumBytes %d\r\n", startSector, Address, NumBytes);
		return FALSE;
	}
	USBH_MSC_BOTXferParam.CmdStateMachine=CMD_SEND_STATE;
	do {
		status = USBH_MSC_Write10(pDev, pSectorBuff, StartSector, NumBytes);
		USBH_MSC_HandleBOTXfer(pDev ,&USB_Host);

		if(!HCD_IsDeviceConnected(pDev)) { 
			//CLR_Debug::Printf( "USB_MSC_Driver::Write() USB device disconnection when writing\r\n");
			return FALSE;
		}      
	} while(status == USBH_MSC_BUSY );

	if(status != USBH_MSC_OK)
		return FALSE;

	return TRUE;
}

BOOL USB_MSC_Driver::Memset( void* context, ByteAddress Address, UINT8 Data, UINT32 NumBytes ){
	BYTE buffer[512];
	memset (buffer, Data, sizeof(buffer));
	do {
		USB_MSC_Driver::Write(context, Address, (NumBytes>512) ? 512: NumBytes, buffer, FALSE);
		NumBytes -= 512;
	} while(NumBytes>0);

	return TRUE;
}

BOOL USB_MSC_Driver::GetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata){
	return TRUE;
}

BOOL USB_MSC_Driver::SetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata){
	return TRUE;
}

BOOL USB_MSC_Driver::IsBlockErased( void* context, ByteAddress address, UINT32 blockLength ){
	BYTE buffer[512];
	if (!USB_MSC_Driver::Read(context, address, 512, buffer))
		return FALSE;
	for (int i = 0; i < 512; i++)
	{
		if (buffer[i] != 0)
			return FALSE;
	}
	return TRUE;
}

BOOL USB_MSC_Driver::EraseBlock( void* context, ByteAddress Sector ){
	BYTE buffer[512];
	memset (buffer, 0, sizeof(buffer));
	USB_MSC_Driver::Write(context, Sector, 512, buffer, FALSE);
	return TRUE;
}

void USB_MSC_Driver::SetPowerState( void* context, UINT32 State ){
}

UINT32 USB_MSC_Driver::MaxSectorWrite_uSec( void* context ){
	return g_USB_MSC_DeviceInfo.MaxSectorWrite_uSec;
}

UINT32 USB_MSC_Driver::MaxBlockErase_uSec( void* context ){
	return g_USB_MSC_DeviceInfo.MaxBlockErase_uSec;
}
