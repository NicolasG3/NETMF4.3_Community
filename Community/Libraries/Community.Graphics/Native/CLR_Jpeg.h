#ifndef _CLR_JPEG_H_
#define _CLR_JPEG_H_

#include <TinyCLR_Types.h>
#include <TinyCLR_Runtime.h>
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"

struct CLR_JpegDecompress
{
public:
	jpeg_decompress_struct* j_decompress_ptr;
    

public:
    static HRESULT CreateInstance( CLR_RT_HeapBlock& ref, LPCSTR path, int bufferSize );

    static void RelocationHandler( CLR_RT_HeapBlock_BinaryBlob* ptr );

    static HRESULT SplitFilePath( LPCSTR fullPath, LPCSTR* nameSpace, UINT32* nameSpaceLength, LPCSTR* relativePath );

    //--//

    void Relocate();

    //--//

    HRESULT AssignStorage ( BYTE* storageIn, size_t sizeIn, BYTE* storageOut, size_t sizeOut );

    HRESULT Read          ( BYTE* buffer, int    count , int*   bytesRead    );
    HRESULT Write         ( BYTE* buffer, int    count , int*   bytesWritten );
}
#endif //_CLR_JPEG_H_