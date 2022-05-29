/*
 * wxApp.cpp
 */

#include <app_config.h>
#include "wxApp.h"
#include "wxMainFrame.h"

// ===============================================================================

const char wxMyApp::APP_NAME[]          = "cue2mkc-frontend";
const char wxMyApp::APP_VERSION[]       = WXMATROSKA_VERSION_STR;
const char wxMyApp::APP_VENDOR_NAME[]   = "Edmunt Pienkowsky";
const char wxMyApp::APP_AUTHOR[]        = "Edmunt Pienkowsky - roed@onet.eu";
const char wxMyApp::LICENSE_FILE_NAME[] = "license.txt";

// ===============================================================================

wxIMPLEMENT_APP( wxMyApp );

wxMyApp::wxMyApp( void )
{}

void wxMyApp::OnInitCmdLine( wxCmdLineParser& parser )
{
	wxApp::OnInitCmdLine( parser );

	parser.AddLongSwitch( "log-timestamps", _( "Show/hide log timestamps" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
}

bool wxMyApp::OnCmdLineParsed( wxCmdLineParser& parser )
{
	wxASSERT( parser.AreLongOptionsEnabled() );

	const bool res = wxApp::OnCmdLineParsed( parser );

	if ( res )
	{
		const wxCmdLineSwitchState state = parser.FoundSwitch( "verbose" );
		switch ( state )
		{
			case wxCMD_SWITCH_ON:
			{
				// wxLog::SetLogLevel(wxLOG_Info);
				wxLog::SetVerbose( true );
				break;
			}

			default:
			{
				// wxLog::SetLogLevel(wxLOG_Message);
				wxLog::SetVerbose( false );
				break;
			}
		}

		if ( parser.FoundSwitch( "log-timestamps" ) == wxCMD_SWITCH_OFF ) wxLog::DisableTimestamp();
	}

	return res;
}

bool wxMyApp::OnInit()
{
	SetAppName( APP_NAME );
	SetVendorName( APP_VENDOR_NAME );
	SetVendorDisplayName( APP_AUTHOR );

	if ( !wxApp::OnInit() ) return false;

	wxLog::EnableLogging( false );

	// cue2mkc tool

	m_cue2MkcPath.Assign( wxStandardPaths::Get().GetExecutablePath() );
	m_cue2MkcPath.SetName( "cue2mkc" );

	if ( !m_cue2MkcPath.IsFileExecutable() )
	{
#ifdef NDEBUG
		wxLogDebug( _( "Using cue2mkc tool from current directory" ) );

		const wxString fullName( m_cue2MkcPath.GetFullName() );
		m_cue2MkcPath.AssignCwd();
		m_cue2MkcPath.SetFullName( fullName );
#else
		const wxArrayString& dirs = m_cue2MkcPath.GetDirs();
		const wxString       lastDir( dirs.Last() );
		m_cue2MkcPath.RemoveLastDir();
		m_cue2MkcPath.RemoveLastDir();
		m_cue2MkcPath.AppendDir( "wxCueChapters" );
		m_cue2MkcPath.AppendDir( lastDir );
#endif
	}

	// mkvmerge tool

	m_mkvmergePath.Assign( wxStandardPaths::Get().GetExecutablePath() );
	m_mkvmergePath.SetName( "mkvmerge" );

#ifdef __WINDOWS__
	if ( !m_mkvmergePath.IsFileExecutable() )
	{
		wxString sProgramFiles;

		if ( wxGetEnv( "ProgramFiles", &sProgramFiles ) || wxGetEnv( "ProgramFiles(x86)", &sProgramFiles ) )
		{
			wxFileName fname = wxFileName::DirName( sProgramFiles );
			fname.AppendDir( "MKVToolNix" );
			fname.SetName( m_mkvmergePath.GetName() );
			fname.SetExt( m_mkvmergePath.GetExt() );

			if ( fname.IsFileExecutable() ) m_mkvmergePath = fname;
		}
	}
#endif

	if ( !m_mkvmergePath.IsFileExecutable() )
	{
		wxLogWarning( _( "Mkvmerge tool not found." ) );
		m_mkvmergePath = wxFileName::FileName( m_mkvmergePath.GetFullName() );
	}

	wxFrame* const pFrame = new wxMainFrame();
	pFrame->Show( true );

	return true;
}

const wxFileName& wxMyApp::GetCue2MkcPath() const
{
	return m_cue2MkcPath;
}

const wxFileName& wxMyApp::GetMkvmergePath() const
{
	return m_mkvmergePath;
}

