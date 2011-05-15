/*
   wxTextOutputStreamOnString.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxTextOutputStreamOnString.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include <wxCueFile/wxTextInputStreamOnString.h>

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

bool wxTextOutputStreamOnString::SaveTo( const wxString& sOutputFile, bool bUseMLang ) const
{
	wxFileOutputStream os( sOutputFile );

	if ( os.IsOk() )
	{
		wxSharedPtr<wxTextOutputStream> pStream( wxTextOutputStreamWithBOMFactory::CreateUTF8( os, wxEOL_NATIVE, true, bUseMLang ) );
		pStream->WriteString( GetString() );
		pStream->Flush();
		return true;
	}
	else
	{
		return false;
	}
}