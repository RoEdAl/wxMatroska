/*
 * wxEllipsizer.h
 */

#ifndef _WX_ELLIPSIZER_H_
#define _WX_ELLIPSIZER_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

class wxEllipsizer: public wxStringProcessor
{
    public:

    wxEllipsizer(void);

    static const char REG_EX[];
    static const wxUChar ELLIPSIS;

    virtual wxStringProcessor* const Clone() const;
    virtual bool Process(const wxString&, wxString&) const;

    protected:

    wxRegEx m_reEllipsis;
};

#endif

