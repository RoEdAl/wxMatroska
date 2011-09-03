/*
   wxApp.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxTagSynonims.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxCueFile/wxCueSheetRenderer.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>
#include "wxConfiguration.h"
#include "wxMkvmergeOptsRenderer.h"
#include "wxXmlCueSheetRenderer.h"
#include "wxApp.h"

// ===============================================================================

const wxChar wxMyApp::APP_NAME[]		  = wxT( "cue2mkc" );
const wxChar wxMyApp::APP_VERSION[]		  = wxT( "0.82" );
const wxChar wxMyApp::APP_VENDOR_NAME[]	  = wxT( "Edmunt Pienkowsky" );
const wxChar wxMyApp::APP_AUTHOR[]		  = wxT( "Edmunt Pienkowsky - roed@onet.eu" );
const wxChar wxMyApp::LICENSE_FILE_NAME[] = wxT( "license.txt" );

// ===============================================================================

static const size_t MAX_LICENSE_FILE_SIZE = 4 * 1024;

// ===============================================================================

wxIMPLEMENT_APP( wxMyApp );

wxMyApp::wxMyApp( void ):
	m_sSeparator( wxT( '=' ), 75 )
{}

void wxMyApp::AddSeparator( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( m_sSeparator );
}

void wxMyApp::AddVersionInfos( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( wxString::Format( _( "Application version: %s" ), APP_VERSION ) );
	cmdline.AddUsageText( wxString::Format( _( "Author: %s" ), APP_AUTHOR ) );
	cmdline.AddUsageText( _( "License: Simplified BSD License - http://www.opensource.org/licenses/bsd-license.php" ) );
	wxString sFlacVersion( FLAC__VERSION_STRING );
	wxString sWavpackVersion( wxString::FromUTF8( WavpackGetLibraryVersionString() ) );
	cmdline.AddUsageText( wxString::Format( _( "FLAC library version: %s. Copyright \u00A9 2000,2001,2002,2003,2004,2005,2006,2007,2008,2009  Josh Coalson" ), sFlacVersion ) );
	cmdline.AddUsageText( wxString::Format( _( "WAVPACK library version: %s. Copyright \u00A9 1998 - 2006 Conifer Software" ), sWavpackVersion ) );
	cmdline.AddUsageText( wxString::Format( _( "wxWidgets version: %d.%d.%d. Copyright \u00A9 1992-2008 Julian Smart, Robert Roebling, Vadim Zeitlin and other members of the wxWidgets team" ), wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER ) );
	cmdline.AddUsageText( wxString::Format( _( "Operating system: %s" ), wxPlatformInfo::Get().GetOperatingSystemDescription() ) );
}

void wxMyApp::AddInputFileFormatDescription( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( _( "Input file format specification:" ) );

	cmdline.AddUsageText( _( "Input file may be a wildcard:" ) );
	cmdline.AddUsageText( _( "\t*.cue" ) );
	cmdline.AddUsageText( _( "When -ec is used input file may be a path to media file with embedded cue sheet:" ) );
	cmdline.AddUsageText( _( "\t*.flac test.ape" ) );
	cmdline.AddUsageText( _( "To read embedded cue sheet MediaInfo library is used." ) );
	cmdline.AddUsageText( _( "For FLAC and Wavpack files the native libraries are used to read cuesheet and metadata." ) );
	cmdline.AddUsageText( wxString::Format( _( "You may also specify data files after cue file using %c as separator." ), wxInputFile::SEPARATOR ) );
	cmdline.AddUsageText( wxString::Format( _( "\t\"test.cue%ctest.flac\"" ), wxInputFile::SEPARATOR ) );
	cmdline.AddUsageText( _( "This allow you to override data file specification in cue sheet file." ) );
}

void wxMyApp::AddFormatDescription( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( _( "Formating directives:" ) );

	cmdline.AddUsageText( _( "\t%da% - disc arranger" ) );
	cmdline.AddUsageText( _( "\t%dc% - disc composer" ) );
	cmdline.AddUsageText( _( "\t%dp% - disc performer" ) );
	cmdline.AddUsageText( _( "\t%ds% - disc songwriter" ) );
	cmdline.AddUsageText( _( "\t%dt% - disc title" ) );

	cmdline.AddUsageText( _( "\t%n% - track number" ) );
	cmdline.AddUsageText( _( "\t%ta% - track arranger" ) );
	cmdline.AddUsageText( _( "\t%tc% - track composer" ) );
	cmdline.AddUsageText( _( "\t%tp% - track performer" ) );
	cmdline.AddUsageText( _( "\t%ts% - track songwriter" ) );
	cmdline.AddUsageText( _( "\t%tt% - track title" ) );

	cmdline.AddUsageText( _( "\t%aa% - track or disc arranger" ) );
	cmdline.AddUsageText( _( "\t%ac% - track or disc composer" ) );
	cmdline.AddUsageText( _( "\t%ap% - track or disc performer" ) );
	cmdline.AddUsageText( _( "\t%as% - track or disc songwriter" ) );
	cmdline.AddUsageText( _( "\t%at% - track or disc title" ) );
}

void wxMyApp::OnInitCmdLine( wxCmdLineParser& cmdline )
{
	wxAppConsole::OnInitCmdLine( cmdline );
	cmdline.AddSwitch( wxEmptyString, wxT( "license" ), _( "Show license" ), wxCMD_LINE_PARAM_OPTIONAL );
	wxConfiguration::AddCmdLineParams( cmdline );
	cmdline.SetLogo( _( "This application converts cue sheet files to Matroska XML chapter files in a more advanced way than standard Matroska tools." ) );
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

void wxMyApp::ShowLicense()
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
		wxPrintf( wxT( "%s\n" ), tis.ReadLine() );
	}
}

bool wxMyApp::OnCmdLineParsed( wxCmdLineParser& cmdline )
{
	if ( !wxAppConsole::OnCmdLineParsed( cmdline ) )
	{
		return false;
	}

	if ( cmdline.Found( wxT( "license" ) ) )
	{
		ShowLicense();
		return false;
	}

	if ( !m_cfg.Read( cmdline ) )
	{
		return false;
	}

	wxLogMessage( _( "%s ver. %s" ), GetAppDisplayName(), APP_VERSION );
	m_cfg.Dump();
	return true;
}

bool wxMyApp::OnInit()
{
	SetAppName( APP_NAME );
	SetVendorName( APP_VENDOR_NAME );
	SetVendorDisplayName( APP_AUTHOR );

	wxDateTime dt( wxDateTime::Now() );
	srand( dt.GetTicks() );

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

int wxMyApp::AppendCueSheet( wxCueSheet& cueSheet )
{
	wxASSERT( m_cfg.GetMerge() );

	if ( !cueSheet.HasDuration() )
	{
		if ( !cueSheet.CalculateDuration( m_cfg.GetAlternateExtensions() ) )
		{
			wxLogError( _( "Fail to calculate duration of cue sheet" ) );
			return 1;
		}
	}

	wxCueSheet& mergedCueSheet = GetMergedCueSheet();
	if ( mergedCueSheet.Append( cueSheet ) )
	{
		return 0;
	}
	else
	{
		wxLogError( _( "Fail to merge cue sheet" ) );
		return 1;
	}
}

int wxMyApp::ConvertCueSheet( const wxInputFile& inputFile,
	const wxCueSheet& cueSheet )
{
	if ( m_cfg.SaveCueSheet() )
	{
		wxString sOutputFile( m_cfg.GetOutputFile( inputFile ) );
		wxLogInfo( _( "Saving cue scheet to \u201C%s\u201D" ), sOutputFile );
		wxFileOutputStream fos( sOutputFile );
		if ( !fos.IsOk() )
		{
			wxLogError( _( "Fail to open \u201C%s\u201D" ), sOutputFile );
			return 1;
		}

		wxSharedPtr<wxTextOutputStream> pTos( m_cfg.GetOutputTextStream( fos ) );
		wxTextCueSheetRenderer			renderer( pTos.get() );
		if ( !renderer.Render( cueSheet ) )
		{
			return 1;
		}
	}
	else
	{
		wxLogInfo( _( "Converting cue scheet to XML format" ) );
		wxSharedPtr<wxXmlCueSheetRenderer> pXmlRenderer = GetXmlRenderer(
			inputFile );
		if ( pXmlRenderer->Render( cueSheet ) )
		{
			if ( m_cfg.GenerateMkvmergeOpts() )
			{
				wxMkvmergeOptsRenderer& optsRenderer = GetMkvmergeOptsRenderer();
				optsRenderer.RenderDisc( inputFile, cueSheet );
			}

			if ( !pXmlRenderer->SaveXmlDoc() )
			{
				return 1;
			}

			if ( m_cfg.GenerateMkvmergeOpts() )
			{
				wxMkvmergeOptsRenderer& optsRenderer = GetMkvmergeOptsRenderer( false );
				if ( !optsRenderer.Save() )
				{
					return 1;
				}

				if ( m_cfg.RunMkvmerge() )
				{
					if ( !RunMkvmerge( optsRenderer.GetMkvmergeOptsFile() ) )
					{
						return 1;
					}
				}
			}
		}
		else
		{
			wxLogError( _( "Fail to export cue sheet to Matroska chapters" ) );
			return 1;
		}
	}

	return 0;
}

int wxMyApp::ProcessCueFile( const wxInputFile& inputFile, const wxTagSynonimsCollection& discSynonims, const wxTagSynonimsCollection& trackSynonims )
{
	wxCueSheetReader reader;

	reader
	.CorrectQuotationMarks( m_cfg.CorrectQuotationMarks(), m_cfg.GetLang() )
	.SetParseComments( m_cfg.GenerateTagsFromComments() )
	.SetEllipsizeTags( m_cfg.EllipsizeTags() )
	.SetRemoveExtraSpaces( m_cfg.RemoveExtraSpaces() )
	.SetAlternateExt( m_cfg.GetAlternateExtensions() );

	wxString sInputFile( inputFile.GetInputFile().GetFullPath() );

	wxLogMessage( _( "Processing \u201C%s\u201D" ), sInputFile );

	if ( m_cfg.IsEmbedded() )
	{
		wxLogInfo( _( "Reading cue sheet from media file" ) );
		if ( !reader.ReadEmbeddedCueSheet( sInputFile, m_cfg.GetReadFlags() ) )
		{
			wxLogError( _( "Fail to read embedded sue sheet from \u201C%s\u201D or parse error" ), sInputFile );
			return 1;
		}
	}
	else
	{
		wxLogInfo( _( "Reading cue sheet from text file" ) );
		if ( !reader.ReadCueSheet( sInputFile, m_cfg.GetReadFlags(), m_cfg.UseMLang() ) )
		{
			wxLogError( _( "Fail to read or parse input cue file \u201C%s\u201D" ), sInputFile );
			return 1;
		}
	}

	wxCueSheet cueSheet( reader.GetCueSheet() );
	if ( inputFile.HasDataFiles() )
	{
		wxArrayDataFile dataFiles;
		inputFile.GetDataFiles( dataFiles, wxDataFile::WAVE );
		cueSheet.SetDataFiles( dataFiles );
	}
	else if ( m_cfg.IsEmbedded() )
	{
		wxDataFile dataFile( sInputFile, wxDataFile::WAVE );
		cueSheet.SetSingleDataFile( dataFile );
	}

	if ( m_cfg.GetUseDataFiles() )
	{
		if ( !cueSheet.CalculateDuration( m_cfg.GetAlternateExtensions() ) )
		{
			wxLogError( _( "Fail to calculate duration of cue sheet \u201C%s\u201D or parse error" ), sInputFile );
			if ( m_cfg.AbortOnError() )
			{
				return 1;
			}
		}
	}

	if ( m_cfg.GetMerge() )
	{
		return AppendCueSheet( cueSheet );
	}
	else
	{
		cueSheet.FindCommonTags( discSynonims, trackSynonims, false );
		return ConvertCueSheet( inputFile, cueSheet );
	}
}

int wxMyApp::OnRun()
{
	wxInputFile firstInputFile;
	bool		bFirst = true;

	wxTagSynonimsCollection discSynonims;
	wxTagSynonimsCollection trackSynonims;

	wxCueComponent::GetSynonims( discSynonims, false );
	wxCueComponent::GetSynonims( trackSynonims, true );

	int						res		  = 0;
	const wxArrayInputFile& inputFile = m_cfg.GetInputFiles();

	for ( size_t i = 0; i < inputFile.Count(); i++ )
	{
		wxFileName fn( inputFile[ i ].GetInputFile() );

		if ( !wxDir::Exists( fn.GetPath() ) )
		{
			wxLogMessage( _( "Directory \u201C%s\u201D doesn't exists" ), fn.GetPath() );
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
			wxLogError( _( "Cannot open directory \u201C%s\u201D" ), fn.GetPath() );
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
			wxInputFile singleFile( inputFile[ i ] );
			while ( true )
			{
				fn.SetFullName( sInputFile );
				singleFile.SetInputFile( fn );

				if ( bFirst )
				{
					firstInputFile = singleFile;
					bFirst		   = false;
				}

				res = ProcessCueFile( singleFile, discSynonims, trackSynonims );
				if ( ( res != 0 ) && ( m_cfg.AbortOnError() || m_cfg.GetMerge() ) )
				{
					break;
				}

				if ( !dir.GetNext( &sInputFile ) )
				{
					break;
				}
			}
		}
	}

	if ( m_cfg.GetMerge() && ( res == 0 ) )
	{
		wxASSERT( !bFirst );
		GetMergedCueSheet().FindCommonTags( discSynonims, trackSynonims, true );
		res = ConvertCueSheet( firstInputFile, GetMergedCueSheet() );
	}

	return ( m_cfg.AbortOnError() || m_cfg.GetMerge() ) ? res : 0;
}

int wxMyApp::OnExit()
{
	int res = wxAppConsole::OnExit();

	m_pMkvmergeOptsRenderer.reset();
	m_pMergedCueSheet.reset();
	CoUninitialize();
	wxLogMessage( _( "Done" ) );
	return res;
}

wxSharedPtr<wxXmlCueSheetRenderer> wxMyApp::GetXmlRenderer( const wxInputFile& inputFile )
{
	wxSharedPtr<wxXmlCueSheetRenderer> pRes( wxXmlCueSheetRenderer::CreateObject( m_cfg, inputFile ) );
	return pRes;
}

wxMkvmergeOptsRenderer& wxMyApp::GetMkvmergeOptsRenderer( bool bCreate )
{
	bool bShowInfo = false;

	if ( m_cfg.GetMerge() )
	{
		if ( !HasMkvmergeOptsRenderer() )
		{
			m_pMkvmergeOptsRenderer.reset( new wxMkvmergeOptsRenderer( m_cfg ) );
		}
	}
	else
	{
		if ( bCreate )
		{
			m_pMkvmergeOptsRenderer.reset( new wxMkvmergeOptsRenderer( m_cfg ) );
		}
		else
		{
			wxASSERT( HasMkvmergeOptsRenderer() );
		}
	}

	return *m_pMkvmergeOptsRenderer;
}

bool wxMyApp::HasMkvmergeOptsRenderer() const
{
	return m_pMkvmergeOptsRenderer;
}

bool wxMyApp::HasMergedCueSheet() const
{
	return m_pMergedCueSheet;
}

wxCueSheet& wxMyApp::GetMergedCueSheet()
{
	if ( !HasMergedCueSheet() )
	{
		wxLogDebug( wxT( "Creating empty cue sheet for merging" ) );
		m_pMergedCueSheet.reset( new wxCueSheet() );
	}

	wxASSERT( HasMergedCueSheet() );
	return *m_pMergedCueSheet;
}

bool wxMyApp::RunMkvmerge( const wxFileName& optionsFile )
{
	wxASSERT( m_cfg.GenerateMkvmergeOpts() );
	wxASSERT( m_cfg.RunMkvmerge() );

	wxString sCmdLine;
	if ( m_cfg.GetMkvmergeDir().IsOk() )
	{
		wxFileName mkvmerge( m_cfg.GetMkvmergeDir().GetFullPath(), wxT( "mkvmerge" ) );
		if ( wxLog::GetVerbose() )
		{
			sCmdLine.Printf( wxT( "\"%s\" \"@%s\"" ), mkvmerge.GetFullPath(), optionsFile.GetFullPath() );
		}
		else
		{
			sCmdLine.Printf( wxT( "\"%s\" --quiet \"@%s\"" ), mkvmerge.GetFullPath(), optionsFile.GetFullPath() );
		}
	}
	else
	{
		if ( wxLog::GetVerbose() )
		{
			sCmdLine.Printf( wxT( "mkvmerge \"@%s\"" ), optionsFile.GetFullPath() );
		}
		else
		{
			sCmdLine.Printf( wxT( "mkvmerge --quiet \"@%s\"" ), optionsFile.GetFullPath() );
		}
	}

	wxLogMessage( _( "Running mkvmerge with options file \u201C%s\u201D" ), optionsFile.GetFullName() );
	wxLogDebug( wxT( "Running commad: %s" ), sCmdLine );

	long nRes = 0;
	if ( m_cfg.UseFullPaths() )
	{
		nRes = wxExecute( sCmdLine, wxEXEC_SYNC | wxEXEC_NOEVENTS );
	}
	else
	{
		wxExecuteEnv env;
		env.cwd = optionsFile.GetPath();

		nRes = wxExecute( sCmdLine, wxEXEC_SYNC | wxEXEC_NOEVENTS, (wxProcess*)NULL, &env );
	}

	if ( nRes == -1 )
	{
		wxLogError( _( "Fail to execute mkvmerge tool" ) );
		return false;
	}
	else
	{
		if ( nRes <= 1 )
		{
			wxLogInfo( _( "mkvmerge exit code: %d" ), nRes );
			return true;
		}
		else
		{
			wxLogError( _( "mkvmerge exit code: %d" ), nRes );
			return false;
		}
	}
}

