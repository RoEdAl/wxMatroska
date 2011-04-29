/*
	wxUTF8TextOutputStream.cpp
*/

#include "StdWx.h"
#include <wxEncodingDetection/wxEncodingDetection.h>
#include <wxCueFile/wxUTF8TextOutputStream.h>

wxUTF8TextOutputStream::wxUTF8TextOutputStream(
	wxOutputStream& s,
    wxEOL mode /*= wxEOL_NATIVE*/,
	bool bWriteBOM /*= false*/ )
	:wxTextOutputStream( s, mode, wxConvUTF8 )
{
	if ( bWriteBOM )
	{
		WriteBOM( s );
	}
}

void wxUTF8TextOutputStream::WriteBOM( wxOutputStream& s )
{
	s.Write( wxEncodingDetection::BOM::UTF8, 3 );
}
