/*
	wxTextOutputStreamOnString.cpp
*/

#include "StdWx.h"
#include "wxTextOutputStreamOnString.h"

wxTextOutputStreamOnString::wxTextOutputStreamOnString()
	:m_outputStream( (wxString*)NULL, m_conv ),
	 m_textOutputStream( m_outputStream, wxEOL_NATIVE, m_conv )
{
}

wxTextOutputStreamOnString::wxTextOutputStreamOnString( wxString& s )
	:m_outputStream( &s, m_conv ),
	 m_textOutputStream( m_outputStream, wxEOL_NATIVE, m_conv )
{
}

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

void wxTextOutputStreamOnString::Flush()
{
	m_textOutputStream.Flush();
}
