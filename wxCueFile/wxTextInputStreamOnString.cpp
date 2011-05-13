/*
   wxTextInputStreamOnString.cpp
 */

#include "StdWx.h"
#include "wxTextInputStreamOnString.h"

wxTextInputStreamOnString::wxTextInputStreamOnString( const wxString& s )
	:m_inputStream( s ),
	m_textInputStream( m_inputStream, wxT( " \t" ), wxConvUTF8 )
{}

const wxStringInputStream& wxTextInputStreamOnString::GetStringStream() const
{
	return m_inputStream;
}

wxTextInputStream& wxTextInputStreamOnString::GetStream()
{
	return m_textInputStream;
}

wxTextInputStream& wxTextInputStreamOnString::operator*()
{
	return m_textInputStream;
}

bool wxTextInputStreamOnString::Eof() const
{
	return m_inputStream.Eof();
}