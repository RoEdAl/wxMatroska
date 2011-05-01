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

	const wxString& GetString() const;
	void Flush();

protected:

	wxMBConvUTF16 m_conv;
	wxStringOutputStream m_outputStream;
	wxTextOutputStream m_textOutputStream;

};

