/*
	wxApp.cpp
*/

#include "StdWx.h"
#include "wxConfiguration.h"
#include <wxTagSynonims.h>
#include <wxSamplingInfo.h>
#include <wxIndex.h>
#include <wxTrack.h>
#include <wxCueSheetReader.h>
#include <wxTextCueSheetRenderer.h>
#include "wxXmlCueSheetRenderer.h"
#include "wxApp.h"

const wxChar wxMyApp::APP_VERSION[] = wxT("0.4 beta");
const wxChar wxMyApp::APP_AUTHOR[] = wxT("Edmunt Pienkowsky - roed@onet.eu");
const wxChar wxMyApp::LICENSE_FILE_NAME[] = wxT("license.txt");
const wxChar wxMyApp::FLAC_LICENSE_FILE_NAME[] = wxT("flac_license.txt");

static const size_t MAX_LICENSE_FILE_SIZE = 4 * 1024;

wxIMPLEMENT_APP(wxMyApp);

wxMyApp::wxMyApp(void)
	:m_sSeparator( wxT('='), 75 ),
	m_pRenderer(wxXmlCueSheetRenderer::Null)
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
	cmdline.AddUsageText( wxT("License: Simplified BSD License - http://www.opensource.org/licenses/bsd-license.php") );
	wxString sFlacVersion( FLAC__VERSION_STRING );
	cmdline.AddUsageText( wxString::Format( wxT("FLAC library version %s Copyright (C) 2000,2001,2002,2003,2004,2005,2006,2007,2008,2009  Josh Coalson"), sFlacVersion ) );
	cmdline.AddUsageText( wxString::Format( wxT("wxWidgets version %d.%d.%d Copyright (C) 1992-2008 Julian Smart, Robert Roebling, Vadim Zeitlin and other members of the wxWidgets team"), wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER ) );
	cmdline.AddUsageText( wxString::Format( wxT("Operating system: %s"), wxPlatformInfo::Get().GetOperatingSystemDescription() ) );
}

void wxMyApp::AddInputFileFormatDescription( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( _("Input file format specification:") );

	cmdline.AddUsageText( _("Input file may be a wildcard:") );
	cmdline.AddUsageText( _("\t*.cue") );
	cmdline.AddUsageText( _("When -ec is used input file may be a path to media file with embedded cue sheet:") );
	cmdline.AddUsageText( _("\t*.flac test.ape") );
	cmdline.AddUsageText( _("To read embedded cue sheet MediaInfo library is used.") );
	cmdline.AddUsageText( _("For FLAC files the native FLAC library is used to read cuesheet and metadata.") );
	cmdline.AddUsageText( wxString::Format( _("You may also specify data files after cue file using %c as separator."), wxInputFile::SEPARATOR ) );
	cmdline.AddUsageText( wxString::Format( _("\t\"test.cue%ctest.flac\""), wxInputFile::SEPARATOR ) );
	cmdline.AddUsageText( _("This allow you to override data file specification in cue sheet file.") );
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
	cmdline.AddSwitch( wxEmptyString, wxT("license"), _("Show license"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdline.AddSwitch( wxEmptyString, wxT("flac-license"), _("Show FLAC library license"), wxCMD_LINE_PARAM_OPTIONAL );
	wxConfiguration::AddCmdLineParams( cmdline );
	cmdline.SetLogo( _("This application converts cue sheet files to Matroska XML chapter files in a more advanced way than standard Matroska tools.") );
	AddSeparator( cmdline );
	AddInputFileFormatDescription( cmdline );
	AddSeparator( cmdline );
	AddFormatDescription( cmdline );
	AddSeparator( cmdline );
	AddVersionInfos( cmdline );
	AddSeparator( cmdline );
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
	m_cfg.Dump();
	return true;
}

 bool wxMyApp::OnInit()
 {
	 SetAppName( wxT("cue2mkc") );
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

int wxMyApp::ConvertCueSheet( const wxInputFile& inputFile, const wxCueSheet& cueSheet )
{
	if ( m_cfg.SaveCueSheet() )
	{
		wxString sOutputFile( m_cfg.GetOutputFile( inputFile ) );
		wxLogInfo( _("Saving cue scheet to \u201C%s\u201D"), sOutputFile );
		wxFileOutputStream fos( sOutputFile );
		if ( !fos.IsOk() )
		{
			wxLogError( _("Fail to open \u201C%s\u201D"), sOutputFile );
			return 1;
		}

		wxTextOutputStream tos( fos, wxEOL_NATIVE, m_cfg.GetCueSheetFileEncoding() );
		wxTextCueSheetRenderer renderer( &tos );
		if ( !renderer.Render( cueSheet ) )
		{
			return 1;
		}
	}
	else
	{
		wxLogInfo( _("Converting cue scheet to XML format") );
		wxXmlCueSheetRenderer& renderer = GetXmlRenderer( inputFile );
		if ( renderer.Render( cueSheet ) )
		{
			if ( !m_cfg.GetMerge() )
			{
				if ( !renderer.SaveXmlDoc() )
				{
					return 1;
				}
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

int wxMyApp::ProcessCueFile( wxCueSheetReader& reader, const wxInputFile& inputFile )
{
	wxString sInputFile( inputFile.GetInputFile().GetFullPath() );
	wxLogMessage( _("Processing \u201C%s\u201D"), sInputFile );

	if ( m_cfg.IsEmbedded() )
	{
		wxLogInfo( _("Reading cue sheet from media file") );
		if ( !reader.ReadEmbeddedCueSheet( sInputFile, m_cfg.GetEmbeddedModeFlags() ) )
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

	if ( inputFile.HasDataFiles() )
	{
		wxCueSheet cueSheet( reader.GetCueSheet() );
		wxArrayDataFile dataFiles;
		inputFile.GetDataFiles( dataFiles, wxDataFile::WAVE );
		cueSheet.SetDataFiles( dataFiles );
		return ConvertCueSheet( inputFile, cueSheet );
	}
	else if ( m_cfg.IsEmbedded() )
	{
		wxCueSheet cueSheet( reader.GetCueSheet() );
		wxDataFile dataFile( sInputFile, wxDataFile::WAVE );
		cueSheet.SetSingleDataFile( dataFile );
		return ConvertCueSheet( inputFile, cueSheet );
	}
	else
	{
		return ConvertCueSheet( inputFile, reader.GetCueSheet() );
	}
}

int wxMyApp::OnRun()
{
	wxCueSheetReader reader;
	reader.CorrectQuotationMarks( m_cfg.CorrectQuotationMarks(), m_cfg.GetLang() );
	reader.SetParseComments( m_cfg.GenerateTagsFromComments() );

	int res = 0;
	const wxArrayInputFile& inputFile = m_cfg.GetInputFiles();
	for( size_t i=0; i<inputFile.Count(); i++ )
	{
		wxFileName fn( inputFile[i].GetInputFile() );

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
			wxInputFile singleFile( inputFile[i] );
			while( true )
			{
				fn.SetFullName( sInputFile );
				singleFile.SetInputFile( fn );

				res = ProcessCueFile( reader, singleFile );
				if ( (res != 0) && (m_cfg.AbortOnError() || m_cfg.GetMerge()) ) break;

				if ( !dir.GetNext( &sInputFile ) ) break;
			}
		}
	}

	if ( m_cfg.GetMerge() && (res==0) && HasXmlRenderer() )
	{
		wxXmlCueSheetRenderer& renderer = GetXmlRenderer();
		if ( !renderer.SaveXmlDoc() )
		{
			res = 1;
		}
	}

	return (m_cfg.AbortOnError() || m_cfg.GetMerge())? res : 0;
}

int wxMyApp::OnExit()
{
	int res = wxAppConsole::OnExit();
	if ( m_pRenderer != wxXmlCueSheetRenderer::Null )
	{
		delete m_pRenderer;
	}
	wxLogMessage( _("Bye") );
	return res;
}

wxXmlCueSheetRenderer& wxMyApp::GetXmlRenderer(const wxInputFile& inputFile)
{
	bool bShowInfo = false;

	if ( m_cfg.GetMerge() )
	{
		if ( m_pRenderer == wxXmlCueSheetRenderer::Null )
		{
			m_pRenderer = new wxXmlCueSheetRenderer( m_cfg, inputFile );
			bShowInfo = true;
		}
		else
		{
			m_pRenderer->SetInputFile( inputFile );
		}
	}
	else
	{
		if ( m_pRenderer != wxXmlCueSheetRenderer::Null )
		{
			delete m_pRenderer;
		}
		m_pRenderer = new wxXmlCueSheetRenderer( m_cfg, inputFile );
		bShowInfo = true;
	}

	if ( bShowInfo )
	{
		wxLogInfo( _("Output file \u201C%s\u201D"), m_pRenderer->GetOutputFile() );
		if ( m_cfg.GenerateTags() )
		{
			wxLogInfo( _("Tags file \u201C%s\u201D"), m_pRenderer->GetTagsFile() );
		}
	}
	return *m_pRenderer;
}

bool wxMyApp::HasXmlRenderer() const
{
	return ( m_pRenderer != wxXmlCueSheetRenderer::Null );
}

wxXmlCueSheetRenderer& wxMyApp::GetXmlRenderer()
{
	wxASSERT( HasXmlRenderer() );
	return *m_pRenderer;
}
