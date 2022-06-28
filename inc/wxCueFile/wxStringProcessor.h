/*
 *      wxStringProcessor.h
 */

#ifndef _WX_STRING_PROCESSOR_H_
#define _WX_STRING_PROCESSOR_H_

class wxStringProcessor;
WX_DECLARE_OBJARRAY(wxStringProcessor, wxArrayStringProcessor);

class wxStringProcessor
{
    public:

    virtual ~wxStringProcessor();
    virtual wxStringProcessor* const Clone() const = 0;
    virtual bool Process(const wxString&, wxString&) const = 0;

    public:

    bool operator ()(wxString&) const;
    wxString operator ()(const wxString&) const;

    protected:

    wxStringProcessor();
};

#endif

