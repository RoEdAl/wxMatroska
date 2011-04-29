/*
	wxTextOutputStreamWithBOM.h
*/

#ifndef _WX_TEXT_OUTPUT_STREAM_WITH_BOM_H_
#define _WX_TEXT_OUTPUT_STREAM_WITH_BOM_H_

class wxTextOutputStreamWithBOMFactory
{
public:

	typedef wxCharTypeBuffer<wxByte> wxByteBuffer;
	static wxTextOutputStream* const wxNullTextOutputStream;

	static wxTextOutputStream* Create( wxOutputStream&, wxEOL, bool, wxUint32, bool );
	static wxTextOutputStream* CreateUTF8( wxOutputStream&, wxEOL, bool, bool );

	static void WriteBOM( wxOutputStream&, const wxByteBuffer& );
};

#endif
