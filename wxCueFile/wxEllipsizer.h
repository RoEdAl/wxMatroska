/*
 * wxEllipsizer.h
 */

#ifndef _WX_ELLIPSIZER_H_
#define _WX_ELLIPSIZER_H_

#ifndef _WX_REGEX_STRING_PROCESSOR_H_
#include <wxCueFile/wxRegExStringProcessor.h>
#endif

class wxEllipsizer: public wxRegExStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxEllipsizer);

    public:

    wxEllipsizer(void);

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
};

#endif

