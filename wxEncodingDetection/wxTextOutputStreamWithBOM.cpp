/*
 * wxUTF8TextOutputStream.cpp
 */

#include <wxEncodingDetection/wxEncodingDetection.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>

 // ===============================================================================

void wxTextOutputStreamWithBOMFactory::WriteBOM(wxOutputStream& s, const wxEncodingDetection::wxByteBuffer& bom)
{
    s.Write(bom.data(), bom.length());
}

class wxTextOutputStreamWithBOM: public wxTextOutputStream
{
    public:

    wxTextOutputStreamWithBOM(wxOutputStream& s,
                               wxEOL mode,
                               bool writeBOM,
                               const wxMBConv& conv,
                               const wxTextOutputStreamWithBOMFactory::wxByteBuffer& bom):
        wxTextOutputStream(s, mode, conv)
    {
        if (writeBOM) wxTextOutputStreamWithBOMFactory::WriteBOM(s, bom);
    }
};

wxTextOutputStream* wxTextOutputStreamWithBOMFactory::Create(
        wxOutputStream& s,
        wxEOL mode,
        bool writeBOM,
        wxUint32 codePage,
        bool useMLang)
{
    wxByteBuffer bom;

    if (wxEncodingDetection::GetBOM(codePage, bom))
    {
        wxString description;
        wxMBConv* const conv(wxEncodingDetection::GetStandardMBConv(codePage, useMLang, description));
        return new wxTextOutputStreamWithBOM(s, mode, writeBOM, *conv, bom);
    }

    return nullptr;
}

wxTextOutputStream* wxTextOutputStreamWithBOMFactory::CreateUTF8(
        wxOutputStream& s,
        wxEOL mode,
        bool writeBOM,
        bool useMLang)
{
    return Create(s, mode, writeBOM, wxEncodingDetection::CP::UTF8, useMLang);
}

wxTextOutputStream* wxTextOutputStreamWithBOMFactory::CreateUTF16(wxOutputStream& s, wxEOL mode, bool writeBOM, bool useMLang)
{
#if WORDS_BIGENDIAN
    return Create(s, mode, writeBOM, wxEncodingDetection::CP::UTF16_BE, useMLang);
#else
    return Create(s, mode, writeBOM, wxEncodingDetection::CP::UTF16_LE, useMLang);
#endif
}
