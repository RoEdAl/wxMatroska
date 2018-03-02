/*
 * wxTextOutputStreamWithBOM.h
 */

#ifndef _WX_TEXT_OUTPUT_STREAM_WITH_BOM_H_
#define _WX_TEXT_OUTPUT_STREAM_WITH_BOM_H_

class wxTextOutputStreamWithBOMFactory
{
	public:

		typedef wxCharTypeBuffer< wxByte > wxByteBuffer;
		typedef wxSharedPtr< wxTextOutputStream > wxTextOutputStreamSharedPtr;

		static wxTextOutputStreamSharedPtr Create( wxOutputStream&, wxEOL, bool, wxUint32, bool );
		static wxTextOutputStreamSharedPtr CreateUTF8( wxOutputStream&, wxEOL, bool, bool );
		static wxTextOutputStreamSharedPtr CreateUTF16( wxOutputStream&, wxEOL, bool, bool );

		static void WriteBOM( wxOutputStream&, const wxByteBuffer& );
};

#endif

