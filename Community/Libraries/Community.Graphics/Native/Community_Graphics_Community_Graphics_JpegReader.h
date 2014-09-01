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


#ifndef _COMMUNITY_GRAPHICS_COMMUNITY_GRAPHICS_JPEGREADER_H_
#define _COMMUNITY_GRAPHICS_COMMUNITY_GRAPHICS_JPEGREADER_H_

namespace Community
{
    namespace Graphics
    {
        struct JpegReader
        {
            // Helper Functions to access fields of managed object
            static UNSUPPORTED_TYPE& Get__jpegObj( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___jpegObj ); }

            //static UNSUPPORTED_TYPE& Get__fileStream( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___fileStream ); }
			static CLR_RT_FileStream* JpegReader::GetFileStream( CLR_RT_StackFrame& stack);

            static INT32& Get__width( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___width ); }

            static INT32& Get__height( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___height ); }

            static UNSUPPORTED_TYPE& Get__line( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___line ); }

            static INT32& Get__lineIndex( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_Community_Graphics_Community_Graphics_JpegReader::FIELD___lineIndex ); }

            // Declaration of stubs. These functions are implemented by Interop code developers
            static void Initialize( CLR_RT_HeapBlock* pMngObj, LPCSTR param0, HRESULT &hr );
            static void NativeDispose( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            static INT8 ReadNextLine( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
        };
    }
}
#endif  //_COMMUNITY_GRAPHICS_COMMUNITY_GRAPHICS_JPEGREADER_H_
