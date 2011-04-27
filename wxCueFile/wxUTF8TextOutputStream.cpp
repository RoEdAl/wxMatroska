/*
	wxUTF8TextOutputStream.cpp
*/

#include "StdWx.h"
#include <wxUTF8TextOutputStream.h>

const wxByte wxUTF8TextOutputStream::BOM[3] = {
	0xEF, 0xBB, 0xBF
};

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
	s.Write( BOM, 3 );
}
