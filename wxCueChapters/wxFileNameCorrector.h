/*
 * wxFileNameCorrector.h
 */

#ifndef _WX_FILE_NAME_CORRECTOR_H_
#define _WX_FILE_NAME_CORRECTOR_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

class wxFileNameCorrector: public wxStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxFileNameCorrector);

    public:

    wxFileNameCorrector(void);

    static const char REG_EX[];

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;

    protected:

    wxRegEx m_reNonPrintable;
};

#endif

