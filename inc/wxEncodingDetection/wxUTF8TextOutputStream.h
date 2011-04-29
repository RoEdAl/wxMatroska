/*
	wxUTF8TextOutputStream.h
*/

#ifndef _WX_UTF8_OUTPUT_STREAM_H_
#define _WX_UTF8_OUTPUT_STREAM_H_

class wxUTF8TextOutputStream :public wxTextOutputStream
{
public:

	static wxUTF8TextOutputStream* Create( wxOutputStream&, wxEOL = wxEOL_NATIVE, bool = false, bool = true );

	static void WriteBOM( wxOutputStream&, const wxEncodingDetection::wxByteBuffer& );

protected:

	wxUTF8TextOutputStream(
		wxOutputStream&,
        wxEOL,
		bool,
		const wxMBConv&,
		const wxEncodingDetection::wxByteBuffer& );
};

#endif
