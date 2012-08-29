/*
 * wxTextOutputStreamOnString.cpp
 */

#include "StdWx.h"
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#include <wxEncodingDetection/wxTextInputStreamOnString.h>

// ===============================================================================

wxTextOutputStreamOnString::wxTextOutputStreamOnString():
	m_outputStream( ( wxString* )NULL, m_conv ),
	m_textOutputStream( m_outputStream, wxEOL_UNIX, m_conv )
{}

wxTextOutputStreamOnString::wxTextOutputStreamOnString( wxString& s ):
	m_outputStream( &s, m_conv ),
	m_textOutputStream( m_outputStream, wxEOL_UNIX, m_conv )
{}

const wxString& wxTextOutputStreamOnString::GetString() const
{
	return m_outputStream.GetString();
}

const wxStringOutputStream& wxTextOutputStreamOnString::GetStringStream() const
{
	return m_outputStream;
}

wxTextOutputStream& wxTextOutputStreamOnString::GetStream()
{
	return m_textOutputStream;
}

wxTextOutputStream& wxTextOutputStreamOnString::operator *()
{
	return m_textOutputStream;
}

void wxTextOutputStreamOnString::SaveTo( wxTextOutputStream& tos, const wxString& s )
{
	wxTextInputStreamOnString tis( s );
	wxString sLine;

	while ( !tis.Eof() )
	{
		sLine = ( *tis ).ReadLine();

		if ( sLine.IsEmpty() )
		{
			tos << endl;
		}
		else
		{
			tos << sLine << endl;
		}
	}

	tos.Flush();
}

void wxTextOutputStreamOnString::SaveTo( wxTextOutputStream& tos ) const
{
	SaveTo( tos, GetString() );
}
