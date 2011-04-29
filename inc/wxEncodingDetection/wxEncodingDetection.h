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

	static class BOM
	{
		public:

		static const wxByte UTF32_BE[4];
		static const wxByte UTF32_LE[4];
		static const wxByte UTF16_BE[2];
		static const wxByte UTF16_LE[2];
		static const wxByte UTF8[3];
	};

	static class CP
	{
		public:

		static const wxUint32 UTF32_BE;
		static const wxUint32 UTF32_LE;
		static const wxUint32 UTF16_BE;
		static const wxUint32 UTF16_LE;
		static const wxUint32 UTF8;
	};

	static bool GetBOM( wxUint32, wxByteBuffer& );
	static wxMBConv* GetDefaultEncoding( bool, wxString& );
	static wxMBConv* GetStandardMBConv( wxUint32, bool, wxString& );
	static wxMBConv* GetFileEncodingFromBOM( const wxFileName&, bool, wxString& );
	static wxMBConv* GetFileEncoding( const wxFileName&, bool, wxString& );

	protected:

	static bool test_bom( const wxByteBuffer&, const wxByte*, size_t );

};

#endif