/*
	wxUTF8TextOutputStream.cpp
*/

#include "StdWx.h"
#include <wxEncodingDetection/wxEncodingDetection.h>
#include <wxEncodingDetection/wxUTF8TextOutputStream.h>

wxUTF8TextOutputStream* wxUTF8TextOutputStream::Create( wxOutputStream& s, wxEOL mode, bool bWriteBOM, bool bUseMLang )
{
	wxString sDescription;
	wxSharedPtr<wxMBConv> pConv( wxEncodingDetection::GetStandardMBConv( wxEncodingDetection::CP::UTF8, bUseMLang, sDescription ) );
	wxEncodingDetection::wxByteBuffer bom( wxEncodingDetection::BOM::UTF8, 3 );
	return new wxUTF8TextOutputStream( s, mode, bWriteBOM, *pConv, bom );
}

wxUTF8TextOutputStream::wxUTF8TextOutputStream(
	wxOutputStream& s,
    wxEOL mode,
	bool bWriteBOM,
	const wxMBConv& conv,
	const wxEncodingDetection::wxByteBuffer& bom )
	:wxTextOutputStream( s, mode, conv )
{
	if ( bWriteBOM )
	{
		WriteBOM( s, bom );
	}
}

void wxUTF8TextOutputStream::WriteBOM( wxOutputStream& s, const wxEncodingDetection::wxByteBuffer& bom )
{
	s.Write( bom.data(), bom.length() );
}
