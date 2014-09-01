//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------


#include "Community_Graphics.h"
#include "Community_Graphics_Community_Graphics_JpegReader.h"
#include "TinyCLR_Types.h"
#include "TinyCLR_Stream.h"
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"

using namespace Community::Graphics;

static CLR_RT_FileStream* g_sourceFileStream; 

static void jpeg_nop_init_source (j_decompress_ptr cinfo) {}
static void jpeg_nop_skip_input_data (j_decompress_ptr cinfo, long num_bytes) {}
static jpeg_boolean jpeg_nop_resync_to_restart (j_decompress_ptr cinfo, int desired) {return FALSE;}
static void jpeg_nop_term_source (j_decompress_ptr cinfo) {}
static jpeg_boolean jpeg_fill_input_buffer (j_decompress_ptr cinfo) {
	BYTE* buffer = (BYTE*)&cinfo[1];
	int readCount = 0;
	g_sourceFileStream->Read(buffer, 512, &readCount);
	cinfo->src->next_input_byte = buffer;
	cinfo->src->bytes_in_buffer = readCount;
	CLR_Debug::Printf("jpeg_fill_input_buffer readCount=%d", readCount);
	return TRUE;
}


CLR_RT_FileStream* JpegReader::GetFileStream( CLR_RT_StackFrame& stack)
{
	CLR_RT_HeapBlock_BinaryBlob* blob;
	CLR_RT_HeapBlock* pThis = stack.This();

	blob = pThis[ Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___fileStream ].DereferenceBinaryBlob(); 

	if(!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD)
		return NULL;
	return (CLR_RT_FileStream*)blob->GetData();
}
static void SetSourceFileStream( CLR_RT_HeapBlock* pMngObj) {
	g_sourceFileStream = 0;
	CLR_RT_HeapBlock& fs_ref = pMngObj[ Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___fileStream];
	CLR_RT_HeapBlock_BinaryBlob* blob = fs_ref.DereferenceBinaryBlob();
    if(!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD)
		return;
	g_sourceFileStream = (CLR_RT_FileStream*) blob->GetData();
}

void JpegReader::Initialize( CLR_RT_HeapBlock* pMngObj, LPCSTR path, HRESULT &hr )
{
	CLR_RT_HeapBlock& fs_ref = pMngObj[ Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___fileStream];
	hr = CLR_RT_FileStream::CreateInstance(fs_ref, path, 0);
	if FAILED(hr)
		return;
	SetSourceFileStream(pMngObj);

	//allocate the jpeg_decompress_struct
	CLR_RT_HeapBlock& ref = pMngObj[ Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___jpegObj];
	int blobSize = sizeof(jpeg_decompress_struct) + 512;
	hr = CLR_RT_HeapBlock_BinaryBlob::CreateInstance(ref, blobSize, 0, 0, 0);
	if FAILED(hr)
		return;

	CLR_RT_HeapBlock_BinaryBlob* blob = ref.DereferenceBinaryBlob();
	jpeg_decompress_struct* cinfo   = (jpeg_decompress_struct*)blob->GetData();

	// Clear the memory
	CLR_RT_Memory::ZeroFill( cinfo, blobSize );

	// Create the decompression engine
	jpeg_create_decompress( cinfo );
	//init src manager
	cinfo->src->init_source = jpeg_nop_init_source;
	cinfo->src->skip_input_data = jpeg_nop_skip_input_data;
	cinfo->src->resync_to_restart = jpeg_nop_resync_to_restart;
	cinfo->src->term_source = jpeg_nop_term_source;
	cinfo->src->fill_input_buffer = jpeg_fill_input_buffer;
	//cinfo->

	cinfo->src->next_input_byte = (JOCTET*)&cinfo[1];
	jpeg_read_header      ( cinfo, TRUE );

	// Set output to 16bit 5-6-5 RGB format
	// We can add support for other bit-depth in the future
	cinfo->out_color_space     = JCS_RGB;
	cinfo->do_fancy_upsampling = FALSE;

	jpeg_start_decompress( cinfo );

	Get__width(pMngObj) = cinfo->image_width;
	Get__height(pMngObj) = cinfo->image_height;
}

void JpegReader::NativeDispose( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
	pMngObj[  Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___jpegObj ].SetObjectReference( NULL );
	SetSourceFileStream(pMngObj);
	if (g_sourceFileStream)
		g_sourceFileStream->Close();
	pMngObj[  Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___fileStream ].SetObjectReference( NULL );
}

INT8 JpegReader::ReadNextLine( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
	SetSourceFileStream(pMngObj);
	CLR_RT_HeapBlock& ref = pMngObj[ Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___jpegObj];
	CLR_RT_HeapBlock_BinaryBlob* blob = ref.DereferenceBinaryBlob();
	jpeg_decompress_struct* cinfo   = (jpeg_decompress_struct*)blob->GetData();

	CLR_RT_HeapBlock_Array* buffer_array = pMngObj[  Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___line].DereferenceArray();
	
	jpeg_read_scanlines( cinfo, (JSAMPARRAY)buffer_array->GetFirstElement(), 1 );

	return TRUE;
}

