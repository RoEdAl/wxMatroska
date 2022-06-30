/*
 *  wxTextStreamUtils.h
 */

#ifndef _WX_TEXT_STREAM_UTILS_H_
#define _WX_TEXT_STREAM_UTILS_H_

class wxTextStreamUtils
{
    wxDECLARE_NO_COPY_CLASS(wxTextStreamUtils);

    public:

    static size_t Copy(wxTextInputStream&, wxTextOutputStream&);
    static size_t Copy(wxTextInputStream&, wxTextOutputStream&, const wxString&);
    static wxMBConv* GetUnaccentMBConv();
};

#endif
