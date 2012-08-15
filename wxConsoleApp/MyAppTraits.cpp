/*
	MyAppTraits.cpp
*/
#include "StdWx.h"
#include <wxConsoleApp/MyAppTraits.h>

namespace
{

class MyMessageOutputStderr : public wxMessageOutput
{
public:

    MyMessageOutputStderr(FILE *fp = stderr) : m_fp(fp) { }

    virtual void Output(const wxString& str)
	{
		if ( AppendLineFeedIfNeeded(str) )
		{
			wxFputs(str + '\n', m_fp);
		}
		else
		{
			wxFputs(str, m_fp);
		}
		fflush(m_fp);
	}

protected:

    // return the string with "\n" appended if it doesn't already terminate
    // with it (in which case it's returned unchanged)
    static bool AppendLineFeedIfNeeded(const wxString& str)
	{
		return ( str.empty() || *str.rbegin() != '\n' );
	}

    FILE *m_fp;
};

class MyLogStderr :public wxLog
{
	public:

	MyLogStderr(FILE *fp = stderr) :m_fp( fp )
	{
		// under GUI systems such as Windows or Mac, programs usually don't have
		// stderr at all, so show the messages also somewhere else, typically in
		// the debugger window so that they go at least somewhere instead of being
		// simply lost
		if ( m_fp == stderr )
		{
			wxAppTraits *traits = wxTheApp ? wxTheApp->GetTraits() : NULL;
			if ( traits && !traits->HasStderr() )
			{
				m_pAdditionalMessageOutput.reset( new wxMessageOutputDebug() );
			}
		}
	}

	void DoLogText(const wxString& msg)
	{
		wxFputs(msg + '\n', m_fp);
		fflush(m_fp);

		if ( m_pAdditionalMessageOutput )
		{
			m_pAdditionalMessageOutput->Output(msg + wxS('\n'));
		}
	}

    FILE *m_fp;
	wxScopedPtr<wxMessageOutput> m_pAdditionalMessageOutput;
};

}


MyAppTraits::MyAppTraits(wxAppTraits* pAppTraits)
	:m_pAppTraits( pAppTraits )
{
}

wxEventLoopBase* MyAppTraits::CreateEventLoop()
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->CreateEventLoop();
}

#if wxUSE_TIMER
wxTimerImpl* MyAppTraits::CreateTimerImpl(wxTimer *timer)
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->CreateTimerImpl( timer );
}

#endif

void* MyAppTraits::BeforeChildWaitLoop()
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->BeforeChildWaitLoop();
}

void MyAppTraits::AfterChildWaitLoop(void* p)
{
	wxASSERT( m_pAppTraits );

	m_pAppTraits->AfterChildWaitLoop( p );
}


#if wxUSE_THREADS
bool MyAppTraits::DoMessageFromThreadWait()
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->DoMessageFromThreadWait();
}

WXDWORD MyAppTraits::WaitForThread(WXHANDLE handle, int flags)
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->WaitForThread( handle, flags );
}

#endif // wxUSE_THREADS

bool MyAppTraits::CanUseStderr()
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->CanUseStderr();
}

bool MyAppTraits::WriteToStderr(const wxString& s)
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->WriteToStderr( s );
}

#if !wxUSE_CONSOLE_EVENTLOOP
wxEventLoopBase* MyAppTraits::CreateEventLoop()
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->CreateEventLoop();
}
#endif // !wxUSE_CONSOLE_EVENTLOOP

#if wxUSE_LOG
wxLog* MyAppTraits::CreateLogTarget()
{
	wxASSERT( m_pAppTraits );

	return new MyLogStderr();                 
	//return m_pAppTraits->CreateLogTarget();
}
#endif // wxUSE_LOG

wxMessageOutput* MyAppTraits::CreateMessageOutput()
{
	wxASSERT( m_pAppTraits );

	return new MyMessageOutputStderr();
	//return m_pAppTraits->CreateMessageOutput();
}

#if wxUSE_FONTMAP
wxFontMapper* MyAppTraits::CreateFontMapper()
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->CreateFontMapper();
}
#endif // wxUSE_FONTMAP

wxRendererNative* MyAppTraits::CreateRenderer()
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->CreateRenderer();
}

bool MyAppTraits::ShowAssertDialog(const wxString& msg )
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->ShowAssertDialog( msg );
}

bool MyAppTraits::HasStderr()
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->HasStderr();
}

wxPortId MyAppTraits::GetToolkitVersion(int *verMaj, int *verMin) const
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->GetToolkitVersion( verMaj, verMin );
}

bool MyAppTraits::IsUsingUniversalWidgets() const
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->IsUsingUniversalWidgets();
}

wxString MyAppTraits::GetDesktopEnvironment() const
{
	wxASSERT( m_pAppTraits );

	return m_pAppTraits->GetDesktopEnvironment();
}

