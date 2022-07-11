/*
 * wxNumberFullStopCorrector.h
 */

#ifndef _WX_NUMBER_FULL_STOP_CORRECTOR_H_
#define _WX_NUMBER_FULL_STOP_CORRECTOR_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

class wxNumberFullStopCorrector: public wxStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxNumberFullStopCorrector);

    public:

    wxNumberFullStopCorrector(void);

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;

    protected:

    wxRegEx m_re;
};

#endif

