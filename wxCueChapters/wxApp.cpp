/*
	wxApp.cpp
*/

#include "StdWx.h"
#include "wxConfiguration.h"
#include <wxIndex.h>
#include <wxTrack.h>
#include <wxCueSheetReader.h>
#include <wxTextCueSheetRenderer.h>
#include "wxXmlCueSheetRenderer.h"
#include "wxApp.h"

const wxChar wxMyApp::APP_VERSION[] = wxT("0.1 beta");
const wxChar wxMyApp::APP_AUTHOR[] = wxT("Edmunt Pienkowsky: roed@onet.eu");

wxIMPLEMENT_APP(wxMyApp);

wxMyApp::wxMyApp(void)
	:m_sSeparator( wxT('='), 70 )
{
}

wxMyApp::~wxMyApp(void)
{
}

void wxMyApp::AddSeparator( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( m_sSeparator );
}

void wxMyApp::AddVersionInfos( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( wxString::Format( wxT("Application version: %s"), APP_VERSION ) );
	cmdline.AddUsageText( wxString::Format( wxT("Author: %s"), APP_AUTHOR ) );
	cmdline.AddUsageText( wxString::Format( wxT("Operating system: %s"), wxPlatformInfo::Get().GetOperatingSystemDescription() ) );
	cmdline.AddUsageText( wxString::Format( wxT("wxWidgets version: %d.%d.%d"), wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER ) );
}

void wxMyApp::AddFormatDescription( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( _("Formating directives:") );

	cmdline.AddUsageText( _("\t%da% - disc arranger") );
	cmdline.AddUsageText( _("\t%dc% - disc composer") );
	cmdline.AddUsageText( _("\t%dp% - disc performer") );
	cmdline.AddUsageText( _("\t%ds% - disc songwriter") );
	cmdline.AddUsageText( _("\t%dt% - disc title") );

	cmdline.AddUsageText( _("\t%n% - track number") );
	cmdline.AddUsageText( _("\t%ta% - track arranger") );
	cmdline.AddUsageText( _("\t%tc% - track composer") );
	cmdline.AddUsageText( _("\t%tp% - track performer") );
	cmdline.AddUsageText( _("\t%ts% - track songwriter") );
	cmdline.AddUsageText( _("\t%tt% - track title") );

	cmdline.AddUsageText( _("\t%aa% - track or disc arranger") );
	cmdline.AddUsageText( _("\t%ac% - track or disc composer") );
	cmdline.AddUsageText( _("\t%ap% - track or disc performer") );
	cmdline.AddUsageText( _("\t%as% - track or disc songwriter") );
	cmdline.AddUsageText( _("\t%at% - track or disc title") );
}

void wxMyApp::OnInitCmdLine( wxCmdLineParser& cmdline )
{
	wxAppConsole::OnInitCmdLine( cmdline );
	wxConfiguration::AddCmdLineParams( cmdline );
	cmdline.SetLogo( _("This application converts cue sheet files to Matroska XML chapter files in a more advanced way than standard Matroska tools.") );
	AddSeparator( cmdline );
	AddFormatDescription( cmdline );
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

	if ( !m_cfg.Read( cmdline ) )
	{
		return false;
	}

	wxLogMessage( _("%s ver. %s"), GetAppDisplayName(), APP_VERSION );
	m_cfg.Dump();
	return true;
}

 bool wxMyApp::OnInit()
 {
	 SetAppName( wxT("cue2mkc") );
	 SetVendorName( wxT("Edmunt Pienkowsky") );
	 SetVendorDisplayName( wxT("Edmunt Pienkowsky: roed@onet.eu") );

	 if ( !wxAppConsole::OnInit() )
	 {
		 return false;
	 }
	 return true;
 }

int wxMyApp::ConvertCueSheet( const wxString& sInputFile, const wxCueSheet& cueSheet )
{
	wxString sOutputFile( m_cfg.GetOutputFile( sInputFile ) );

	if ( m_cfg.SaveCueSheet() )
	{
		wxLogInfo( _("Saving cue scheet to \u201C%s\u201D"), sOutputFile );
		wxFileOutputStream fos( sOutputFile );
		if ( !fos.IsOk() )
		{
			wxLogError( _("Fail to open \u201C%s\u201D"), sOutputFile );
			return 1;
		}

		wxTextOutputStream tos( fos );
		wxTextCueSheetRenderer renderer( &tos );
		if ( !renderer.Render( cueSheet ) )
		{
			return 1;
		}
	}
	else
	{
		wxLogInfo( _("Converting cue scheet to XML format") );
		wxLogInfo( _("Output file \u201C%s\u201D"), sOutputFile );

		wxXmlCueSheetRenderer renderer( m_cfg, sInputFile, sOutputFile );
		if ( renderer.Render( cueSheet ) )
		{
			if ( !renderer.SaveXmlDoc() )
			{
				wxLogError( _("Fail to save XML document to \u201C%s\u201D"), sOutputFile );
				return 1;
			}
		}
		else
		{
			wxLogError( _("Fail to export cue sheet to Matroska chapters") );
			return 1;
		}
	}
	return 0;
}

int wxMyApp::ProcessCueFile( wxCueSheetReader& reader, const wxString& sInputFile )
{
	wxLogMessage( _("Processing \u201C%s\u201D"), sInputFile );

	if ( m_cfg.IsEmbedded() )
	{
		wxLogInfo( _("Reading cue sheet from media file") );
		if ( !reader.ReadEmbeddedCueSheet( sInputFile ) )
		{
			wxLogError( _("Fail to read embedded sue sheet from \u201C%s\u201D or parse error"), sInputFile );
			return 1;
		}
	}
	else
	{
		wxLogInfo( _("Reading cue sheet from text file") );
		if ( !reader.ReadCueSheet( sInputFile ) )
		{
			wxLogError( _("Fail to read or parse input cue file \u201C%s\u201D"), sInputFile );
			return 1;
		}
	}

	if ( m_cfg.HasSingleDataFile() || m_cfg.IsEmbedded() )
	{
		wxCueSheet cueSheet( reader.GetCueSheet() );
		if ( !m_cfg.HasSingleDataFile() )
		{
			wxDataFile dataFile( sInputFile, wxDataFile::WAVE );
			cueSheet.SetSingleDataFile( dataFile );
		}
		else
		{
			wxDataFile dataFile( m_cfg.GetSingleDataFile(), wxDataFile::WAVE );
			wxLogInfo( _("Setting data file to \u201C%s\u201D"), dataFile.GetFileName() );
			cueSheet.SetSingleDataFile( dataFile );
		}
		return ConvertCueSheet( sInputFile, cueSheet );
	}
	else
	{
		return ConvertCueSheet( sInputFile, reader.GetCueSheet() );
	}
}

int wxMyApp::OnRun()
{
	wxCueSheetReader reader;
	reader.UsePolishQuotationMarks( m_cfg.UsePolishQuotationMarks() );

	int res = 0;
	const wxArrayString& inputFile = m_cfg.GetInputFiles();
	for( size_t i=0; i<inputFile.Count(); i++ )
	{
		wxFileName fn( inputFile[i] );
		if ( !wxDir::Exists( fn.GetPath() ) )
		{
			wxLogMessage( _("Directory \u201C%s\u201D doesn't exists"), fn.GetPath() );
			res = 1;
			if ( m_cfg.AbortOnError() )
			{
				break;
			}
			else
			{
				continue;
			}
		}

		wxDir dir( fn.GetPath() );
		if ( !dir.IsOpened() )
		{
			wxLogError( _("Cannot open directory \u201C%s\u201D"), fn.GetPath() );
			res = 1;
			if ( m_cfg.AbortOnError() )
			{
				break;
			}
			else
			{
				continue;
			}
		}

		wxString sFileSpec( fn.GetFullName() );
		wxString sInputFile;
		if ( dir.GetFirst( &sInputFile, sFileSpec, wxDIR_FILES ) )
		{
			while( true )
			{
				fn.SetFullName( sInputFile );
				res = ProcessCueFile( reader, fn.GetFullPath() );
				if ( (res != 0) && m_cfg.AbortOnError() ) break;

				if ( !dir.GetNext( &sInputFile ) ) break;
			}
		}
	}

	return m_cfg.AbortOnError()? res : 0;
}

int wxMyApp::OnExit()
{
	int res = wxAppConsole::OnExit();
	wxLogMessage( _("Bye") );
	return res;
}