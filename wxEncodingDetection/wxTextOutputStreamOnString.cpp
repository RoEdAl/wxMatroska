/*
 * wxTextOutputStreamOnString.cpp
 */

#include "StdWx.h"
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#include <wxEncodingDetection/wxTextInputStreamOnString.h>

// ===============================================================================

wxTextOutputStreamOnString::wxTextOutputStreamOnString() :
	m_textOutputStream( m_outputStream, wxEOL_UNIX, m_conv )
{}

wxString wxTextOutputStreamOnString::GetString() const
{
	const wxStreamBuffer* const sb = m_outputStream.GetOutputStreamBuffer();

	return wxString(
			static_cast< const char* >( sb->GetBufferStart() ),
			m_conv,
			sb->GetBufferSize() - sb->GetBytesLeft() );
}

const wxMemoryOutputStream& wxTextOutputStreamOnString::GetMemoryStream() const
{
	return m_outputStream;
}

wxTextOutputStream& wxTextOutputStreamOnString::GetStream()
{
	return m_textOutputStream;
}

wxTextOutputStream& wxTextOutputStreamOnString::operator *() const
{
	return const_cast< wxTextOutputStream& >( m_textOutputStream );
}

wxTextOutputStream* wxTextOutputStreamOnString::operator ->() const
{
	return const_cast< wxTextOutputStream* >( &m_textOutputStream );
}

void wxTextOutputStreamOnString::SaveTo( wxTextOutputStream& tos, const wxString& s )
{
	wxTextInputStreamOnString tis( s );
	wxString                  sLine;

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

