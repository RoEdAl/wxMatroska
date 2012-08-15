/*
	MyAppConsole.cpp
*/
#include "StdWx.h"
#include <wxConsoleApp/MyAppTraits.h>
#include <wxConsoleApp/MyAppConsole.h>

// ================================================================================

const wxChar MyAppConsole::APP_VENDOR_NAME[] = wxT( "Edmunt Pienkowsky" );
const wxChar MyAppConsole::APP_AUTHOR[]		= wxT( "Edmunt Pienkowsky - roed@onet.eu" );

// ================================================================================

MyAppConsole::MyAppConsole(void) :
	m_sSeparator( wxT( '=' ), 75 )
{
}

bool MyAppConsole::OnInit()
{
	SetVendorName( APP_VENDOR_NAME );
	SetVendorDisplayName( APP_AUTHOR );

	CoInitializeEx( NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE );

	if ( !wxAppConsole::OnInit() )
	{
		return false;
	}

	return true;
}


wxAppTraits* MyAppConsole::CreateTraits()
{
	wxAppTraits* pAppTraits = wxAppConsole::CreateTraits();

	return new MyAppTraits( pAppTraits );
}

int MyAppConsole::OnExit()
{
	int res = wxAppConsole::OnExit();

	CoUninitialize();
	wxLogInfo( _( "Done" ) );
	return res;
}

void MyAppConsole::AddSeparator( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( m_sSeparator );
}

