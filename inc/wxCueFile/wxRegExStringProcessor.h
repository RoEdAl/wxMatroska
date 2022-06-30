/*
 *      wxRegExStringProcessor.h
 */

#ifndef _WX_REGEX_STRING_PROCESSOR_H_
#define _WX_REGEX_STRING_PROCESSOR_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include "wxStringProcessor.h"
#endif

class wxRegExStringProcessor : public wxStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxRegExStringProcessor);

    public:

    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;

    protected:

    wxRegExStringProcessor(const wxString&, const wxString&);

    private:

    wxRegEx m_regEx;
    wxString m_replacement;
};

#endif

