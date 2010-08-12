/*
	wxApp.cpp
*/

#include "StdWx.h"
#include "wxConfiguration.h"
#include "wxApp.h"
#include "wxFlacEncoder.h"
#include "wxFlacOutputStream.h"
#include "wxFlacDecoder.h"
#include "wxFlacDecoderAndEncoder.h"

const wxChar wxMyApp::APP_VERSION[] = wxT("0.1 beta");
const wxChar wxMyApp::APP_AUTHOR[] = wxT("Edmunt Pienkowsky - roed@onet.eu");
const wxChar wxMyApp::LICENSE_FILE_NAME[] = wxT("license.txt");
const wxChar wxMyApp::FLAC_LICENSE_FILE_NAME[] = wxT("flac_license.txt");

static const size_t MAX_LICENSE_FILE_SIZE = 4 * 1024;

wxIMPLEMENT_APP(wxMyApp);

wxMyApp::wxMyApp(void)
	:m_sSeparator( wxT('='), 75 )
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

void wxMyApp::AddSeparator( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( m_sSeparator );
}

void wxMyApp::AddVersionInfos( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( wxString::Format( wxT("Application version: %s"), APP_VERSION ) );
	cmdline.AddUsageText( wxString::Format( wxT("Author: %s"), APP_AUTHOR ) );
	cmdline.AddUsageText( wxT("License: Simplified BSD License - http://www.opensource.org/licenses/bsd-license.php") );
	wxString sFlacVersion( FLAC__VERSION_STRING );
	cmdline.AddUsageText( wxString::Format( wxT("FLAC library version %s Copyright (C) 2000,2001,2002,2003,2004,2005,2006,2007,2008,2009  Josh Coalson"), sFlacVersion ) );
	cmdline.AddUsageText( wxString::Format( wxT("wxWidgets version %d.%d.%d Copyright (C) 1992-2008 Julian Smart, Robert Roebling, Vadim Zeitlin and other members of the wxWidgets team"), wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER ) );
	cmdline.AddUsageText( wxString::Format( wxT("Operating system: %s"), wxPlatformInfo::Get().GetOperatingSystemDescription() ) );
}

void wxMyApp::ShowLicense( bool bFlac )
{
	const wxStandardPaths& paths = wxStandardPaths::Get();
	wxFileName fn( paths.GetExecutablePath() );
	fn.SetFullName( bFlac? FLAC_LICENSE_FILE_NAME: LICENSE_FILE_NAME );
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

void wxMyApp::OnInitCmdLine( wxCmdLineParser& cmdline )
{
	wxAppConsole::OnInitCmdLine( cmdline );
	cmdline.AddSwitch( wxEmptyString, wxT("license"), _("Show license"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdline.AddSwitch( wxEmptyString, wxT("flac-license"), _("Show FLAC library license"), wxCMD_LINE_PARAM_OPTIONAL );
	wxConfiguration::AddCmdLineParams( cmdline );
	cmdline.SetLogo( _("This tool joins two or more FLAC files") );
	AddSeparator( cmdline );
	AddVersionInfos( cmdline );
	AddSeparator( cmdline );
}

bool wxMyApp::OnCmdLineParsed( wxCmdLineParser& cmdline )
{
	if ( !wxAppConsole::OnCmdLineParsed( cmdline ) )
	{
		return false;
	}

	if ( cmdline.Found( wxT("license") ) )
	{
		ShowLicense( false );
		return false;
	}

	if ( cmdline.Found( wxT("flac-license") ) )
	{
		ShowLicense( true );
		return false;
	}

	if ( !m_cfg.Read( cmdline ) )
	{
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
	wxTempFileOutputStream* pOutputStream = (wxTempFileOutputStream*)NULL;
	wxFlacDecoderAndEncoder* pDecEnc = (wxFlacDecoderAndEncoder*)NULL;

	int nRes = 0;
	const wxArrayString& inputFile = m_cfg.GetInputFiles();
	for( wxArrayString::const_iterator i = inputFile.begin(); (i != inputFile.end()) && (nRes==0); i++ )
	{
		wxFileName fn( *i );

		if ( !wxDir::Exists( fn.GetPath() ) )
		{
			wxLogMessage( _("Directory \u201C%s\u201D doesn't exists"), fn.GetPath() );
			nRes = 1;
			break;
		}

		wxDir dir( fn.GetPath() );
		if ( !dir.IsOpened() )
		{
			wxLogError( _("Cannot open directory \u201C%s\u201D"), fn.GetPath() );
			nRes = 1;
			break;
		}

		wxString sFileSpec( fn.GetFullName() );
		wxString sInputFile;
		if ( dir.GetFirst( &sInputFile, sFileSpec, wxDIR_FILES ) )
		{
			while( true )
			{
				fn.SetFullName( sInputFile );
				wxLogMessage( wxT("Processing \u201C%s\u201D"), fn.GetFullName() );

				wxFileInputStream is( fn.GetFullPath() );
				if ( !is.IsOk() )
				{
					wxLogError( wxT("Fail to open file \u201C%s\u201D"), fn.GetFullPath() );
					nRes = 2;
					break;
				}

				if ( pDecEnc == (wxFlacDecoderAndEncoder*)NULL )
				{
					wxString sOutputFile( m_cfg.GetOutputFile( fn.GetFullPath() ) );
					wxLogMessage( wxT("Output file \u201C%s\u201D"), sOutputFile );
					pOutputStream = new wxTempFileOutputStream( sOutputFile );
					if ( !pOutputStream->IsOk() )
					{
						delete pOutputStream;
						pOutputStream = (wxTempFileOutputStream*)NULL;
						nRes = 3;
						break;
					}

					pDecEnc = new wxFlacDecoderAndEncoder( *pOutputStream );
				}

				if ( !pDecEnc->SetInputStream( is ) )
				{
					nRes = 2;
					break;
				}

				wxLogInfo( wxT("Processing metadata") );
				bool res = pDecEnc->GetDecoder().process_until_end_of_metadata();
				if ( !res )
				{
					nRes = 1;
					break;
				}

				if ( !pDecEnc->IsValidStream() )
				{
					nRes = 1;
					break;
				}

				wxLogInfo( wxT("Processing audio stream") );
				// TODO: Show progress
				// TODO: Allow cancel
				res = pDecEnc->GetDecoder().process_until_end_of_stream();
				if ( !res )
				{
					nRes = 1;
					break;
				}

				if ( !dir.GetNext( &sInputFile ) ) break;
			}
		}
	}

	if ( pDecEnc != (wxFlacDecoderAndEncoder*)NULL )
	{
		if ( !pDecEnc->Finish() )
		{
			nRes = 5;
		}

		if ( nRes != 0 )
		{
			pOutputStream->Discard();
		}
		else
		{
			if ( !pOutputStream->Commit() )
			{
				wxLogError( wxT("Fail to save output file") );
				nRes = 4;
			}
		}

		delete pDecEnc;
		delete pOutputStream;
	}
	return nRes;
}

int wxMyApp::OnExit()
{
	int res = wxAppConsole::OnExit();
	wxLogMessage( _("Bye") );
	return res;
}

const wxConfiguration& wxMyApp::GetConfiguration() const
{
	return m_cfg;
}