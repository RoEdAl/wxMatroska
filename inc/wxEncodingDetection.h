/*
	wxEncodingDetection.h
*/

#ifndef _WX_ENCODING_DETECTION_H_
#define _WX_ENCODING_DETECTION_H_

class wxEncodingDetection :public wxObject
{
	public:

	static wxMBConv* const wxNullMBConv;
	static wxMBConv* GetFileEncoding( const wxFileName&, wxString& );
};

#endif