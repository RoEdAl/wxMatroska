/*
 * wxSmallLetterParenthesizedCorrector.h
 */

#ifndef _WX_SMALL_LETTER_PARTENTHESIZED_H_
#define _WX_SMALL_LETTER_PARTENTHESIZED_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include "wxStringProcessor.h"
#endif

class wxSmallLetterParenthesizedCorrector: public wxStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxSmallLetterParenthesizedCorrector);

    public:

    static void ShowCharacters(wxMessageOutput&);

    wxSmallLetterParenthesizedCorrector(void);

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;

    protected:

    wxRegEx m_re;
};

#endif

