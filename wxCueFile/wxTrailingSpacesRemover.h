/*
 * wxTrailingSpacesRemover.h
 */

#ifndef _WX_TRAILING_SPACES_REMOVER_H_
#define _WX_TRAILING_SPACES_REMOVER_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

class wxTrailingSpacesRemover:
    public wxStringProcessor
{
    wxDECLARE_DYNAMIC_CLASS(wxTrailingSpacesRemover);

    public:

    static const char REG_EX[];

    wxTrailingSpacesRemover(void);

    virtual wxStringProcessor* const Clone() const;
    virtual bool Process(const wxString&, wxString&) const;

    const wxRegEx& GetRegEx() const;

    protected:

    wxRegEx m_reTrailingSpaces;
};

#endif

