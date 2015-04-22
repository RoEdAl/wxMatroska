/*
 *      MyAppConsole.cpp
 */
#include "StdWx.h"
#include <wxConsoleApp/MyAppTraits.h>
#include <wxConsoleApp/MyAppConsole.h>

// ================================================================================

const char MyAppConsole::APP_VENDOR_NAME[]	 = "Edmunt Pienkowsky";
const char MyAppConsole::APP_AUTHOR[]		 = "Edmunt Pienkowsky - roed@onet.eu";
const char MyAppConsole::LICENSE_FILE_NAME[] = "license.txt";

namespace
{
    const size_t MAX_LICENSE_FILE_SIZE = 4 * 1024;
}

// ================================================================================

MyAppConsole::MyAppConsole( void ):
	m_sSeparator( '=', 75 )
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

	//
#if defined( __WXMSW__ ) && defined( __VISUALC__ ) && defined( UNICODE )
	_setmode( _fileno( stderr ), _O_U8TEXT );
	_setmode( _fileno( stdout ), _O_U8TEXT );
#endif

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

	wxTextInputStream tis( fis, wxEmptyString, wxConvUTF8 );
	while ( !fis.Eof() )
	{
		out.Output( tis.ReadLine() );
	}
}

