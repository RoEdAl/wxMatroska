/*
 *      MyAppConsole.cpp
 */
#include "StdWx.h"
#include <wxConsoleApp/MyAppTraits.h>
#include <wxConsoleApp/MyAppConsole.h>

// ================================================================================

const wxChar MyAppConsole::APP_VENDOR_NAME[]   = wxS( "Edmunt Pienkowsky" );
const wxChar MyAppConsole::APP_AUTHOR[]		   = wxS( "Edmunt Pienkowsky - roed@onet.eu" );
const wxChar MyAppConsole::LICENSE_FILE_NAME[] = wxS( "license.txt" );

static const size_t MAX_LICENSE_FILE_SIZE = 4 * 1024;

// ================================================================================

MyAppConsole::MyAppConsole( void ):
	m_sSeparator( wxT( '=' ), 75 )
{}

bool MyAppConsole::OnInit()
{
	SetVendorName( APP_VENDOR_NAME );
	SetVendorDisplayName( APP_AUTHOR );

	if ( !CheckLicense() )
	{
		wxLogError( _( "Cannot find or load license file" ) );
		return false;
	}

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
	wxLog::FlushActive();
	int res = wxAppConsole::OnExit();

	CoUninitialize();
	wxLogInfo( _( "Done" ) );
	return res;
}

void MyAppConsole::AddSeparator( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( m_sSeparator );
}

bool MyAppConsole::CheckLicense()
{
#ifdef _DEBUG
	return true;

#else
	const wxStandardPaths& paths = wxStandardPaths::Get();
	wxFileName			   fn( paths.GetExecutablePath() );
	fn.SetFullName( LICENSE_FILE_NAME );

	if ( !fn.IsFileReadable() )
	{
		return false;
	}

	wxULongLong fs( fn.GetSize() );

	if ( fs == wxInvalidSize )
	{
		wxLogInfo( _( "Unable to read license \u201C%s\u201D" ), fn.GetFullPath() );
		return false;
	}

	wxULongLong maxSize( 0, MAX_LICENSE_FILE_SIZE );

	if ( fs > maxSize )
	{
		wxLogInfo( _( "License file \u201C%s\u201D is too big" ), fn.GetFullPath() );
		return false;
	}
	return true;
#endif
}

void MyAppConsole::ShowLicense( wxMessageOutput& out )
{
	const wxStandardPaths& paths = wxStandardPaths::Get();
	wxFileName			   fn( paths.GetExecutablePath() );

	fn.SetFullName( LICENSE_FILE_NAME );

	if ( !fn.IsFileReadable() )
	{
		wxLogError( _( "Cannot find license file \u201C%s\u201D" ), fn.GetFullPath() );
		return;
	}

	wxULongLong fs( fn.GetSize() );

	if ( fs == wxInvalidSize )
	{
		wxLogError( _( "Unable to read license \u201C%s\u201D" ), fn.GetFullPath() );
		return;
	}

	wxULongLong maxSize( 0, MAX_LICENSE_FILE_SIZE );

	if ( fs > maxSize )
	{
		wxLogError( _( "License file \u201C%s\u201D is too big" ), fn.GetFullPath() );
		return;
	}

	wxFileInputStream fis( fn.GetFullPath() );

	if ( !fis.IsOk() )
	{
		wxLogError( _( "Cannot open license file \u201C%s\u201D" ), fn.GetFullPath() );
		return;
	}

	wxTextInputStream tis( fis, wxEmptyString, wxConvISO8859_1 );
	while ( !fis.Eof() )
	{
		out.Output( tis.ReadLine() );
	}
}

