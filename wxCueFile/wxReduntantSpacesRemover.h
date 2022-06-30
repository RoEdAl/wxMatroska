/*
 * wxReduntantSpacesRemover.h
 */

#ifndef _WX_REDUNTANT_SPACES_REMOVER_H_
#define _WX_REDUNTANT_SPACES_REMOVER_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

class wxReduntantSpacesRemover: public wxStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxReduntantSpacesRemover);

    public:

    wxReduntantSpacesRemover(void);

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;

    protected:

    wxRegEx m_re;
    wxRegEx m_reEn;
    wxRegEx m_reEm;

    wxString m_replacementNormal;
    wxString m_replacementEn;
    wxString m_replacementEm;
};

#endif

