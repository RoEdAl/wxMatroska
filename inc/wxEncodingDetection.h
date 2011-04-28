/*
	wxEncodingDetection.h
*/

#ifndef _WX_ENCODING_DETECTION_H_
#define _WX_ENCODING_DETECTION_H_

class wxEncodingDetection :public wxObject
{
	public:

	typedef wxCharTypeBuffer<wxByte> wxByteBuffer;
	static wxMBConv* const wxNullMBConv;

	static const wxByte BOM_UTF32_BE[4];
	static const wxByte BOM_UTF32_LE[4];
	static const wxByte BOM_UTF16_BE[2];
	static const wxByte BOM_UTF16_LE[2];
	static const wxByte BOM_UTF8[3];

	static wxMBConv* GetFileEncodingFromBOM( const wxFileName&, wxString& );
	static wxMBConv* GetFileEncoding( const wxFileName&, wxString& );

	protected:

	static bool test_bom( const wxByteBuffer&, const wxByte*, size_t );

};

#endif