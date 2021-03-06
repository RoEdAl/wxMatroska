/*
 *      MyAppTraits.cpp
 */
#include <wxConsoleApp/MyMessageOutputStderr.h>
#include <wxConsoleApp/MyLogStderr.h>
#include <wxConsoleApp/MyAppTraits.h>

 // =======================================================================

MyMessageOutputStderr::MyMessageOutputStderr(FILE* fp)
{
    if (fp == NULL) m_fp = stderr;
    else m_fp = fp;
}

void MyMessageOutputStderr::Output(const wxString& str)
{
    if (AppendLineFeedIfNeeded(str)) wxFputs(str + wxS('\n'), m_fp);
    else wxFputs(str, m_fp);
    fflush(m_fp);
}

// =======================================================================

MyLogStderr::MyLogStderr(FILE* fp)
{
    if (fp == NULL) m_fp = stderr;
    else m_fp = fp;

    // under GUI systems such as Windows or Mac, programs usually don't have
    // stderr at all, so show the messages also somewhere else, typically in
    // the debugger window so that they go at least somewhere instead of being
    // simply lost
    if (m_fp == stderr)
    {
        wxAppTraits* traits = wxTheApp ? wxTheApp->GetTraits() : NULL;

        if (traits && !traits->HasStderr()) m_pAdditionalMessageOutput.reset(new wxMessageOutputDebug());
    }
}

void MyLogStderr::DoLogText(const wxString& msg)
{
    wxFputs(msg + wxS('\n'), m_fp);
    fflush(m_fp);

    if (m_pAdditionalMessageOutput) m_pAdditionalMessageOutput->Output(msg + wxS('\n'));
}

// =======================================================================

MyAppTraits::MyAppTraits(wxAppTraits* pAppTraits):
    m_pAppTraits(pAppTraits)
{
}

wxEventLoopBase* MyAppTraits::CreateEventLoop()
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->CreateEventLoop();
}

#if wxUSE_TIMER
wxTimerImpl* MyAppTraits::CreateTimerImpl(wxTimer* timer)
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->CreateTimerImpl(timer);
}

#endif

void* MyAppTraits::BeforeChildWaitLoop()
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->BeforeChildWaitLoop();
}

void MyAppTraits::AfterChildWaitLoop(void* p)
{
    wxASSERT(m_pAppTraits);

    m_pAppTraits->AfterChildWaitLoop(p);
}

#if wxUSE_THREADS
bool MyAppTraits::DoMessageFromThreadWait()
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->DoMessageFromThreadWait();
}

WXDWORD MyAppTraits::WaitForThread(WXHANDLE handle, int flags)
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->WaitForThread(handle, flags);
}

#endif  // wxUSE_THREADS

bool MyAppTraits::CanUseStderr()
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->CanUseStderr();
}

bool MyAppTraits::WriteToStderr(const wxString& s)
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->WriteToStderr(s);
}

WXHWND MyAppTraits::GetMainHWND() const
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->GetMainHWND();
}

#if !wxUSE_CONSOLE_EVENTLOOP
wxEventLoopBase* MyAppTraits::CreateEventLoop()
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->CreateEventLoop();
}

#endif  // !wxUSE_CONSOLE_EVENTLOOP

#if wxUSE_LOG
wxLog* MyAppTraits::CreateLogTarget()
{
    wxASSERT(m_pAppTraits);

    return new MyLogStderr(stdout);

    // return m_pAppTraits->CreateLogTarget();
}

#endif  // wxUSE_LOG

wxMessageOutput* MyAppTraits::CreateMessageOutput()
{
    wxASSERT(m_pAppTraits);

    return new MyMessageOutputStderr(stdout);

    // return m_pAppTraits->CreateMessageOutput();
}

#if wxUSE_FONTMAP
wxFontMapper* MyAppTraits::CreateFontMapper()
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->CreateFontMapper();
}

#endif  // wxUSE_FONTMAP

wxRendererNative* MyAppTraits::CreateRenderer()
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->CreateRenderer();
}

bool MyAppTraits::ShowAssertDialog(const wxString& msg)
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->ShowAssertDialog(msg);
}

bool MyAppTraits::HasStderr()
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->HasStderr();
}

wxPortId MyAppTraits::GetToolkitVersion(int* verMaj, int* verMin, int* microVer) const
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->GetToolkitVersion(verMaj, verMin, microVer);
}

bool MyAppTraits::IsUsingUniversalWidgets() const
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->IsUsingUniversalWidgets();
}

wxString MyAppTraits::GetDesktopEnvironment() const
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->GetDesktopEnvironment();
}

wxString MyAppTraits::GetStandardCmdLineOptions(wxArrayString& names, wxArrayString& desc) const
{
    wxASSERT(m_pAppTraits);

    return m_pAppTraits->GetStandardCmdLineOptions(names, desc);
}

