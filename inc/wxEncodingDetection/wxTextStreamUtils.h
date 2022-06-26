/*
 *  wxTextStreamUtils.h
 */

#ifndef _WX_TEXT_STREAM_UTILS_H_
#define _WX_TEXT_STREAM_UTILS_H_

class wxTextStreamUtils
{
    public:

    static size_t Copy(wxTextInputStream&, wxTextOutputStream&);
    static size_t Copy(wxTextInputStream&, wxTextOutputStream&, const wxString&);
};

#endif
