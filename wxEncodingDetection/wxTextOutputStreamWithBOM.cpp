/*
   wxUTF8TextOutputStream.cpp
 */

#include "StdWx.h"
#include <wxEncodingDetection/wxEncodingDetection.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>

// ===============================================================================

void wxTextOutputStreamWithBOMFactory::WriteBOM( wxOutputStream& s, const wxEncodingDetection::wxByteBuffer& bom )
{
	s.Write( bom.data(), bom.length() );
}

class wxTextOutputStreamWithBOM:
	public wxTextOutputStream
{
public:

	wxTextOutputStreamWithBOM( wxOutputStream& s,
							   wxEOL mode,
							   bool bWriteBOM,
							   const wxMBConv& conv,
							   const wxTextOutputStreamWithBOMFactory::
							   wxByteBuffer& bom ):
		wxTextOutputStream( s, mode, conv )
	{
		if ( bWriteBOM )
		{
			wxTextOutputStreamWithBOMFactory::WriteBOM( s, bom );
		}
	}
};

wxTextOutputStreamWithBOMFactory::wxTextOutputStreamSharedPtr wxTextOutputStreamWithBOMFactory::Create(
	wxOutputStream& s,
	wxEOL mode,
	bool bWriteBOM,
	wxUint32 nCodePage,
	bool bUseMLang )
{
	wxTextOutputStreamSharedPtr pRes;
	wxByteBuffer				bom;

	if ( wxEncodingDetection::GetBOM( nCodePage, bom ) )
	{
		wxString			  sDescription;
		wxSharedPtr<wxMBConv> pConv( wxEncodingDetection::GetStandardMBConv( nCodePage, bUseMLang, sDescription ) );
		pRes = new wxTextOutputStreamWithBOM( s, mode, bWriteBOM, *pConv, bom );
	}

	return pRes;
}

wxTextOutputStreamWithBOMFactory::wxTextOutputStreamSharedPtr wxTextOutputStreamWithBOMFactory::CreateUTF8(
	wxOutputStream& s,
	wxEOL mode,
	bool bWriteBOM,
	bool bUseMLang )
{
	return Create( s, mode, bWriteBOM, wxEncodingDetection::CP::UTF8, bUseMLang );
}

wxTextOutputStreamWithBOMFactory::wxTextOutputStreamSharedPtr wxTextOutputStreamWithBOMFactory::CreateUTF16( wxOutputStream& s, wxEOL mode, bool bWriteBOM, bool bUseMLang )
{
#if WORDS_BIGENDIAN
	return Create( s, mode, bWriteBOM, wxEncodingDetection::CP::UTF16_BE, bUseMLang );

#else
	return Create( s, mode, bWriteBOM, wxEncodingDetection::CP::UTF16_LE, bUseMLang );
#endif
}

