/*
 * wxTrailingSpacesRemover.h
 */

#ifndef _WX_TRAILING_SPACES_REMOVER_H_
#define _WX_TRAILING_SPACES_REMOVER_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

class wxTrailingSpacesRemover: public wxStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxTrailingSpacesRemover);

    public:

    wxTrailingSpacesRemover(void);

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;

    protected:

    wxRegEx m_reTrailingSpaces;
};

#endif

