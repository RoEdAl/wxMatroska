/*
 * wxAsciiToUnicode.h
 */

#ifndef _WX_ASCII_TO_UNICODE_H_
#define _WX_ASCII_TO_UNICODE_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include "wxStringProcessor.h"
#endif

class wxAsciiToUnicode: public wxStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxAsciiToUnicode);

    public:

    static void ShowCharacters(wxMessageOutput&);

    wxAsciiToUnicode(void);

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;

    protected:

    wxRegEx m_re;
};

#endif

