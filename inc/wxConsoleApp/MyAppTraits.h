/*
 *      MyAppTraits.h
 */

#ifndef _MY_APP_TRAITS_H_
#define _MY_APP_TRAITS_H_

class MyAppTraits:
    public wxAppTraits
{
    public:

    MyAppTraits(wxAppTraits*);

    public:

    virtual wxEventLoopBase* CreateEventLoop();

#if wxUSE_TIMER
    virtual wxTimerImpl* CreateTimerImpl(wxTimer*);
#endif

    virtual void* BeforeChildWaitLoop();
    virtual void AfterChildWaitLoop(void*);

#if wxUSE_THREADS
    virtual bool DoMessageFromThreadWait();

    virtual WXDWORD WaitForThread(WXHANDLE, int);
#endif  // wxUSE_THREADS

    virtual bool CanUseStderr();
    virtual bool WriteToStderr(const wxString&);
    virtual WXHWND GetMainHWND() const;

#if wxUSE_LOG
    virtual wxLog* CreateLogTarget();
#endif  // wxUSE_LOG

    virtual wxMessageOutput* CreateMessageOutput();

#if wxUSE_FONTMAP
    virtual wxFontMapper* CreateFontMapper();
#endif  // wxUSE_FONTMAP

    virtual wxRendererNative* CreateRenderer();
    virtual bool ShowAssertDialog(const wxString&);
    virtual bool HasStderr();
    virtual wxPortId GetToolkitVersion(int* verMaj = NULL, int* verMin = NULL, int* microVer = NULL) const;
    virtual bool IsUsingUniversalWidgets() const;
    virtual wxString GetDesktopEnvironment() const;
    virtual wxString GetStandardCmdLineOptions(wxArrayString& names, wxArrayString& desc) const;

    protected:

    wxScopedPtr< wxAppTraits > m_pAppTraits;
};

#endif

