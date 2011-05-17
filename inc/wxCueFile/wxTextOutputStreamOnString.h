/*
   wxTextOutputStreamOnString.h
 */

#ifndef _WX_TEXT_OUTPUT_STREAM_ON_STRING_H_
#define _WX_TEXT_OUTPUT_STREAM_ON_STRING_H_

class wxTextOutputStreamOnString:
	public wxObject
{
public:

	wxTextOutputStreamOnString();
	wxTextOutputStreamOnString( wxString& );

	const wxStringOutputStream& GetStringStream() const;
	wxTextOutputStream&			GetStream();
	wxTextOutputStream&			operator *();

	const wxString& GetString() const;
	bool SaveTo( const wxString&, bool ) const;

protected:

	wxMBConvUTF16		 m_conv;
	wxStringOutputStream m_outputStream;
	wxTextOutputStream	 m_textOutputStream;
};

#endif

