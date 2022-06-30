/*
 * wxAsciiToUnicode.h
 */

#ifndef _WX_ASCII_TO_UNICODE_H_
#define _WX_ASCII_TO_UNICODE_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

class wxAsciiToUnicode: public wxStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxAsciiToUnicode);

    public:

    struct ASCII2UNI
    {
        const char* str;
        const char* regex;
        int uniChar;
    };

    static const ASCII2UNI REPL_TABLE[];
    static void ShowReplTable(wxMessageOutput&);

    wxAsciiToUnicode(void);

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;

    protected:

    wxRegEx m_re;
};

#endif

