/*
 * wxSmallLetterParenthesizedCorrector.h
 */

#ifndef _WX_SMALL_LETTER_PARTENTHESIZED_H_
#define _WX_SMALL_LETTER_PARTENTHESIZED_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

class wxSmallLetterParenthesizedCorrector:
    public wxStringProcessor
{
    wxDECLARE_DYNAMIC_CLASS(wxSmallLetterParenthesizedCorrector);

    public:

    static const char REG_EX[];

    wxSmallLetterParenthesizedCorrector(void);

    virtual wxStringProcessor* const Clone() const;
    virtual bool Process(const wxString&, wxString&) const;

    protected:

    wxRegEx m_re;
};

#endif

