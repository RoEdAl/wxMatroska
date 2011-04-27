/*
	wxUTF8TextOutputStream.h
*/

#ifndef _WX_UTF8_OUTPUT_STREAM_H_
#define _WX_UTF8_OUTPUT_STREAM_H_

class wxUTF8TextOutputStream :public wxTextOutputStream
{
public:

	wxUTF8TextOutputStream(
		wxOutputStream&,
        wxEOL = wxEOL_NATIVE,
		bool bWriteBOM = false );

	static const wxByte BOM[3];
	static void WriteBOM( wxOutputStream& );

};

#endif
