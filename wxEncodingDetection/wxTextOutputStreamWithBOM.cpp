/*
	wxUTF8TextOutputStream.cpp
*/

#include "StdWx.h"
#include <wxEncodingDetection/wxEncodingDetection.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>

wxTextOutputStream* const wxTextOutputStreamWithBOMFactory::wxNullTextOutputStream = (wxTextOutputStream* const)NULL;

class wxTextOutputStreamWithBOM :public wxTextOutputStream
{
public:

	wxTextOutputStreamWithBOM(
		wxOutputStream& s,
		wxEOL mode,
		bool bWriteBOM,
		const wxMBConv& conv,
		const wxTextOutputStreamWithBOMFactory::wxByteBuffer& bom )
	:wxTextOutputStream( s, mode, conv )
	{
		if ( bWriteBOM )
		{
			wxTextOutputStreamWithBOMFactory::WriteBOM( s, bom );
		}
	}
};


wxTextOutputStream* wxTextOutputStreamWithBOMFactory::Create( wxOutputStream& s, wxEOL mode, bool bWriteBOM, wxUint32 nCodePage, bool bUseMLang )
{
	wxByteBuffer bom;
	if ( wxEncodingDetection::GetBOM( nCodePage, bom ) )
	{
		wxString sDescription;
		wxSharedPtr<wxMBConv> pConv( wxEncodingDetection::GetStandardMBConv( nCodePage, bUseMLang, sDescription ) );
		return new wxTextOutputStreamWithBOM( s, mode, bWriteBOM, *pConv, bom );
	}
	else
	{
		return wxNullTextOutputStream;
	}
}

wxTextOutputStream* wxTextOutputStreamWithBOMFactory::CreateUTF8( wxOutputStream& s, wxEOL mode, bool bWriteBOM, bool bUseMLang )
{
	return Create( s, mode, bWriteBOM, wxEncodingDetection::CP::UTF8, bUseMLang );
}

void wxTextOutputStreamWithBOMFactory::WriteBOM( wxOutputStream& s, const wxEncodingDetection::wxByteBuffer& bom )
{
	s.Write( bom.data(), bom.length() );
}
