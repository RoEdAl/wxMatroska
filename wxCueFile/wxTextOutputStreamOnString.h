/*
	wxTextOutputStreamOnString.h
*/

class wxTextOutputStreamOnString :public wxObject
{
public:

	wxTextOutputStreamOnString();
	wxTextOutputStreamOnString( wxString& );

	const wxStringOutputStream& GetStringStream() const;
	wxTextOutputStream& GetStream();
	wxTextOutputStream& operator *();

	const wxString& GetString() const;

protected:

	wxMBConvUTF16 m_conv;
	wxStringOutputStream m_outputStream;
	wxTextOutputStream m_textOutputStream;

};

