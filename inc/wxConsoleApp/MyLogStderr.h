/*
 *      MyLogStderr.h
 */

#ifndef _MY_LOG_STDERR_H_
#define _MY_LOG_STDERR_H_

class MyLogStderr:
    public wxLog
{
    public:

    MyLogStderr(FILE* = NULL);
    virtual void DoLogText(const wxString&);

    protected:

    FILE* m_fp;
    wxScopedPtr< wxMessageOutput > m_pAdditionalMessageOutput;
};

#endif

