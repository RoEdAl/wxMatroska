/*
 * wxApp.cpp
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
const wxChar wxMyApp::APP_VERSION[]		  = wxT( "0.95" );

// ===============================================================================

wxIMPLEMENT_APP_CONSOLE( wxMyApp );

wxMyApp::wxMyApp( void )
{}

void wxMyApp::InfoVersion( wxMessageOutput& out )
{
	out.Printf( _( "Application version: %s" ), APP_VERSION );
	out.Printf( _( "Author: %s" ), APP_AUTHOR );
	out.Output( _( "License: Simplified BSD License - http://www.opensource.org/licenses/bsd-license.php" ) );
	out.Printf( _( "wxWidgets version: %d.%d.%d. Copyright \u00A9 1992-2008 Julian Smart, Robert Roebling, Vadim Zeitlin and other members of the wxWidgets team" ), wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER );
	out.Output( wxCueSheetReader::GetTagLibVersion() );
	out.Printf( _( "Operating system: %s" ), wxPlatformInfo::Get().GetOperatingSystemDescription() );
}

void wxMyApp::InfoUsage( wxMessageOutput& out )
{
	out.Output( _( "Input file format specification:" ) );
	out.Output( _( "Input file may be a wildcard:" ) );
	out.Output( _( "\t*.cue" ) );
	out.Output( _( "When -ec is used input file may be a path to media file with embedded cue sheet:" ) );
	out.Output( _( "\t*.flac test.wv" ) );
	out.Output( _( "To read embedded cue sheet MediaInfo library is used." ) );
	out.Printf( _( "You may also specify data files after cue file using %c as separator." ), wxInputFile::SEPARATOR );
	out.Printf( _( "\t\"test.cue%ctest.flac\"" ), wxInputFile::SEPARATOR );
	out.Output( _( "This allow you to override data file specification in cue sheet file." ) );
}

void wxMyApp::InfoFormatDescription( wxMessageOutput& out )
{
	out.Output( _( "Formating directives:" ) );

	out.Output( _( "\t%da% - disc arranger" ) );
	out.Output( _( "\t%dc% - disc composer" ) );
	out.Output( _( "\t%dp% - disc performer" ) );
	out.Output( _( "\t%ds% - disc songwriter" ) );
	out.Output( _( "\t%dt% - disc title" ) );

	out.Output( _( "\t%n% - track number" ) );
	out.Output( _( "\t%ta% - track arranger" ) );
	out.Output( _( "\t%tc% - track composer" ) );
	out.Output( _( "\t%tp% - track performer" ) );
	out.Output( _( "\t%ts% - track songwriter" ) );
	out.Output( _( "\t%tt% - track title" ) );

	out.Output( _( "\t%aa% - track or disc arranger" ) );
	out.Output( _( "\t%ac% - track or disc composer" ) );
	out.Output( _( "\t%ap% - track or disc performer" ) );
	out.Output( _( "\t%as% - track or disc songwriter" ) );
	out.Output( _( "\t%at% - track or disc title" ) );
}

void wxMyApp::OnInitCmdLine( wxCmdLineParser& cmdline )
{
	MyAppConsole::OnInitCmdLine( cmdline );
	m_cfg.AddCmdLineParams( cmdline );
	cmdline.SetLogo( _( "This application converts cue sheet files to Matroska XML chapter files in a more advanced way than standard Matroska tools." ) );
}

bool wxMyApp::OnCmdLineParsed( wxCmdLineParser& cmdline )
{
	if ( !MyAppConsole::OnCmdLineParsed( cmdline ) )
	{
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

	wxDateTime dt( wxDateTime::Now() );
	srand( dt.GetTicks() );

	if ( !CheckLicense() )
	{
		wxLogError( _( "Cannot find or load license file" ) );
		return false;
	}

	if ( !MyAppConsole::OnInit() )
	{
		return false;
	}

	return true;
}

int wxMyApp::AppendCueSheet( wxCueSheet& cueSheet )
{
	wxASSERT( m_cfg.GetMerge() );

	if ( !( cueSheet.HasDuration() || cueSheet.CalculateDuration( m_cfg.GetAlternateExtensions() ) ) )
	{
		wxLogError( _( "Fail to calculate duration of cue sheet" ) );
		return 1;
	}

	wxCueSheet& mergedCueSheet = GetMergedCueSheet();
	mergedCueSheet.Append( cueSheet );
	return 0;
}

int wxMyApp::ConvertCueSheet( const wxInputFile& inputFile, wxCueSheet& cueSheet )
{
	if ( cueSheet.GetDataFilesCount() > 1u )
	{
		if ( !m_cfg.GetUseDataFiles() )
		{
			wxLogError( _( "Cue sheet has more than one data file." ) );
			wxLogError( _( "Please use -df (or --use-data-files) option." ) );
			return 1;
		}
	}

	if ( m_cfg.GetUseDataFiles() )
	{
		if ( !( cueSheet.HasDuration() || cueSheet.CalculateDuration( m_cfg.GetAlternateExtensions() ) ) )
		{
			wxLogError( _( "Fail to calculate duration of cue sheet" ) );
			return 1;
		}
	}

	switch( m_cfg.GetRenderMode() )
	{
		case wxConfiguration::RENDER_CUE_SHEET:
		{
			wxString sOutputFile( m_cfg.GetOutputFile( inputFile ) );
			wxLogInfo( _( "Saving cue scheet to \u201C%s\u201D" ), sOutputFile );
			wxFileOutputStream fos( sOutputFile );

			if ( !fos.IsOk() )
			{
				wxLogError( _( "Fail to open \u201C%s\u201D" ), sOutputFile );
				return 1;
			}

			wxSharedPtr< wxTextOutputStream > pTos( m_cfg.GetOutputTextStream( fos ) );
			wxTextCueSheetRenderer			  renderer( pTos.get() );

			if ( !renderer.Render( cueSheet ) )
			{
				return 1;
			}
		}
		break;

		case wxConfiguration::RENDER_MATROSKA_CHAPTERS:
		{
			wxLogInfo( _( "Converting cue scheet to XML format" ) );
			wxSharedPtr< wxXmlCueSheetRenderer > pXmlRenderer = GetXmlRenderer(
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
		break;

		case wxConfiguration::RENDER_WAV2IMG_CUE_POINTS:
		wxLogError( _("Currentrly this rendering mode is not implemented") );
		return 1;
	}

	return 0;
}

int wxMyApp::ProcessCueFile( const wxInputFile& inputFile, const wxTagSynonimsCollection& discSynonims, const wxTagSynonimsCollection& trackSynonims )
{
	wxCueSheetReader reader;

	reader
	.CorrectQuotationMarks( m_cfg.CorrectQuotationMarks(), m_cfg.GetLang() )
	.SetAlternateExt( m_cfg.GetAlternateExtensions() )
	.SetReadFlags( m_cfg.GetReadFlags() );

	wxString sInputFile( inputFile.GetInputFile().GetFullPath() );

	wxLogMessage( _( "Processing \u201C%s\u201D" ), sInputFile );

	if ( m_cfg.IsEmbedded() )
	{
		wxLogInfo( _( "Reading cue sheet from media file" ) );

		if ( !reader.ReadEmbeddedCueSheet( sInputFile ) )
		{
			wxLogError( _( "Fail to read embedded sue sheet from \u201C%s\u201D or parse error" ), sInputFile );
			return 1;
		}
	}
	else
	{
		wxLogInfo( _( "Reading cue sheet from text file" ) );

		if ( !reader.ReadCueSheet( sInputFile, m_cfg.UseMLang() ) )
		{
			wxLogError( _( "Fail to read or parse input cue file \u201C%s\u201D" ), sInputFile );
			return 1;
		}
	}

	wxCueSheet cueSheet( reader.GetCueSheet() );

	if ( inputFile.HasDataFiles() )
	{
		// obsolete code
		wxArrayDataFile dataFiles;
		inputFile.GetDataFiles( dataFiles, wxDataFile::WAVE );
		cueSheet.SetDataFiles( dataFiles );
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
	switch( m_cfg.GetInfoSubject() )
	{
		case wxConfiguration::INFO_VERSION:
		InfoVersion( *wxMessageOutput::Get() );
		return 0;

		case wxConfiguration::INFO_USAGE:
		InfoUsage( *wxMessageOutput::Get() );
		return 0;

		case wxConfiguration::INFO_FORMATTING_DIRECTIVES:
		InfoFormatDescription( *wxMessageOutput::Get() );
		return 0;

		case wxConfiguration::INFO_LICENSE:
		ShowLicense( *wxMessageOutput::Get() );
		return 0;
	}


	wxInputFile firstInputFile;
	bool		bFirst = true;

	wxTagSynonimsCollection discSynonims;
	wxTagSynonimsCollection trackSynonims;

	wxCueComponent::GetSynonims( discSynonims, false );
	wxCueComponent::GetSynonims( trackSynonims, true );

	int						res		  = 0;
	const wxArrayInputFile& inputFile = m_cfg.GetInputFiles();

	for ( size_t i = 0, nCount = inputFile.GetCount(); i < nCount; i++ )
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
	int res = MyAppConsole::OnExit();
	m_pMkvmergeOptsRenderer.reset();
	m_pMergedCueSheet.reset();
	return res;
}

wxSharedPtr< wxXmlCueSheetRenderer > wxMyApp::GetXmlRenderer( const wxInputFile& inputFile )
{
	wxSharedPtr< wxXmlCueSheetRenderer > pRes( wxXmlCueSheetRenderer::CreateObject( m_cfg, inputFile ) );
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

