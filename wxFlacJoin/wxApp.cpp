/*
	wxApp.cpp
*/

#include "StdWx.h"
#include "wxApp.h"

const wxChar wxMyApp::APP_VERSION[] = wxT("0.1 beta");
const wxChar wxMyApp::APP_AUTHOR[] = wxT("Edmunt Pienkowsky - roed@onet.eu");
const wxChar wxMyApp::LICENSE_FILE_NAME[] = wxT("license.txt");

static const size_t MAX_LICENSE_FILE_SIZE = 4 * 1024;

wxIMPLEMENT_APP(wxMyApp);

wxMyApp::wxMyApp(void)
{
}

wxMyApp::~wxMyApp(void)
{
}

bool wxMyApp::CheckLicense()
{
#ifdef _DEBUG
	return true;
#else
	const wxStandardPaths& paths = wxStandardPaths::Get();
	wxFileName fn( paths.GetExecutablePath() );
	fn.SetFullName( LICENSE_FILE_NAME );
	if ( !fn.FileExists() ) return false;

	wxULongLong fs( fn.GetSize() );
	if ( fs == wxInvalidSize )
	{
		wxLogInfo( wxT("Unable to read license \u201C%s\u201D."), fn.GetFullPath() );
		return false;
	}

	wxULongLong maxSize( 0, MAX_LICENSE_FILE_SIZE );
	if ( fs > maxSize )
	{
		wxLogInfo( wxT("License file \u201C%s\u201D is too big."), fn.GetFullPath() );
		return false;
	}

	return true;
#endif
}

void wxMyApp::ShowLicense()
{
	const wxStandardPaths& paths = wxStandardPaths::Get();
	wxFileName fn( paths.GetExecutablePath() );
	fn.SetFullName( LICENSE_FILE_NAME );
	if ( !fn.FileExists() )
	{
		wxLogError( wxT("Cannot find license file \u201C%s\u201D."), fn.GetFullPath() );
		return;
	}

	wxULongLong fs( fn.GetSize() );
	if ( fs == wxInvalidSize )
	{
		wxLogError( wxT("Unable to read license \u201C%s\u201D."), fn.GetFullPath() );
		return;
	}

	wxULongLong maxSize( 0, MAX_LICENSE_FILE_SIZE );
	if ( fs > maxSize )
	{
		wxLogError( wxT("License file \u201C%s\u201D is too big."), fn.GetFullPath() );
		return;
	}

	wxFileInputStream fis( fn.GetFullPath() );
	if ( !fis.IsOk() )
	{
		wxLogError( wxT("Cannot open license file \u201C%s\u201D"), fn.GetFullPath() );
		return;
	}

	wxTextInputStream tis( fis, wxEmptyString, wxConvISO8859_1 );
	while( !fis.Eof() )
	{
		wxPrintf( tis.ReadLine() );
		wxPrintf( wxT("\n") );
	}
}

bool wxMyApp::OnCmdLineParsed( wxCmdLineParser& cmdline )
{
	if ( !wxAppConsole::OnCmdLineParsed( cmdline ) )
	{
		return false;
	}

	if ( cmdline.Found( wxT("license") ) )
	{
		ShowLicense();
		return false;
	}

	wxLogMessage( _("%s ver. %s"), GetAppDisplayName(), APP_VERSION );
	return true;
}

 bool wxMyApp::OnInit()
 {
	 SetAppName( wxT("flacmerge") );
	 SetVendorName( wxT("Edmunt Pienkowsky") );
	 SetVendorDisplayName( APP_AUTHOR );

	 wxDateTime dt( wxDateTime::Now() );
	 srand( dt.GetTicks() );

	 if ( !CheckLicense() )
	 {
		 wxLogError( "Cannot find or load license file." );
		 return false;
	 }

	 if ( !wxAppConsole::OnInit() )
	 {
		 return false;
	 }
	 return true;
 }

int wxMyApp::OnRun()
{
	return 0;
}

int wxMyApp::OnExit()
{
	int res = wxAppConsole::OnExit();
	wxLogMessage( _("Bye") );
	return res;
}
