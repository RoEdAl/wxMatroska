/*
 * wxEncodingDetection.h
 */

#ifndef _WX_ENCODING_DETECTION_H_
#define _WX_ENCODING_DETECTION_H_

class wxEncodingDetection
{
    public:

    typedef wxCharTypeBuffer< wxByte > wxByteBuffer;

    struct BOM
    {
        static const wxByte UTF32_BE[4];
        static const wxByte UTF32_LE[4];
        static const wxByte UTF16_BE[2];
        static const wxByte UTF16_LE[2];
        static const wxByte UTF8[3];
    };

    struct CP
    {
        static const wxUint32 UTF32_BE;
        static const wxUint32 UTF32_LE;
        static const wxUint32 UTF16_BE;
        static const wxUint32 UTF16_LE;
        static const wxUint32 UTF8;
    };

    static bool GetBOM(wxUint32, wxByteBuffer&);
    static wxMBConv* GetDefaultEncoding(bool, wxString&);
    static wxUint32 GetDefaultEncoding();

    static wxMBConv* GetStandardMBConv(wxUint32, bool, wxString&);
    static wxMBConv* GetFileEncodingFromBOM(const wxFileName&, bool, wxString&);
    static wxMBConv* GetFileEncoding(const wxFileName&, bool, wxString&);

    protected:

    static bool test_bom(const wxByteBuffer&, const wxByte*, size_t);
};

#endif

