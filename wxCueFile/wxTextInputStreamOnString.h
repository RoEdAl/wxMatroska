/*
   wxTextInputStreamOnString.h
 */

#ifndef _WX_TEXT_INPUT_STREAM_ON_STRING_H_
#define _WX_TEXT_INPUT_STREAM_ON_STRING_H_

class wxTextInputStreamOnString: public wxObject
{
public:

	wxTextInputStreamOnString( const wxString& );
	const wxStringInputStream& GetStringStream() const;
	wxTextInputStream&		   GetStream();
	wxTextInputStream&		   operator *();
	bool Eof() const;

protected:

	wxStringInputStream m_inputStream;
	wxTextInputStream	m_textInputStream;
};

#endif

