/*
	wxUTF8TextOutputStream.cpp
*/

#include "StdWx.h"
#include <wxEncodingDetection.h>
#include <wxUTF8TextOutputStream.h>

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
	s.Write( wxEncodingDetection::BOM_UTF8, 3 );
}
