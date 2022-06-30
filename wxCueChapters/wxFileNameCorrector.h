/*
 * wxFileNameCorrector.h
 */

#ifndef _WX_FILE_NAME_CORRECTOR_H_
#define _WX_FILE_NAME_CORRECTOR_H_

#ifndef _WX_REGEX_STRING_PROCESSOR_H_
#include <wxCueFile/wxRegExStringProcessor.h>
#endif

class wxFileNameCorrector: public wxRegExStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxFileNameCorrector);

    public:

    wxFileNameCorrector(void);

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;
};

#endif

