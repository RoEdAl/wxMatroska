/*
   wxMkvmergeOptsRenderer.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include "wxConfiguration.h"
#include "wxInputFile.h"
#include "wxMkvmergeOptsRenderer.h"
#include "wxApp.h"

// ===============================================================================

wxMkvmergeOptsRenderer* const wxMkvmergeOptsRenderer::Null = ( wxMkvmergeOptsRenderer* const )NULL;

const wxChar wxMkvmergeOptsRenderer::LOG_EXT[] = wxT( "log" );

// ===============================================================================

wxMkvmergeOptsRenderer::wxMkvmergeOptsRenderer( const wxConfiguration& cfg )
	:m_cfg( cfg )
{}

const wxString& wxMkvmergeOptsRenderer::GetMkvmergeOptsFile() const
{
	return m_sMatroskaOptsFile;
}

wxString wxMkvmergeOptsRenderer::mkvmerge_escape( const wxString& s )
{
	wxString sRes( s );
	sRes.Replace( wxT( '\\' ), wxT( "\\\\" ) );
	sRes.Replace( wxT( ' ' ), wxT( "\\s" ) );
	sRes.Replace( wxT( '\"' ), wxT( "\\2" ) );
	sRes.Replace( wxT( ':' ), wxT( "\\c" ) );
	sRes.Replace( wxT( '#' ), wxT( "\\h" ) );
	return sRes;
}

void wxMkvmergeOptsRenderer::write_as( wxTextOutputStream& stream, const wxArrayString& as )
{
	for ( wxArrayString::const_iterator i = as.begin() ; i != as.end() ; i++ )
	{
		stream << *i << endl;
	}
}

bool wxMkvmergeOptsRenderer::GetLogFile( const wxFileName& inputFile, wxFileName& logFile )
{
	wxASSERT( inputFile.IsOk() );
	logFile = inputFile;
	logFile.SetExt( LOG_EXT );
	wxASSERT( logFile.IsOk() );
	return logFile.FileExists();
}

void wxMkvmergeOptsRenderer::write_attachments( wxTextOutputStream& stream )
{
	size_t nAttachments = m_logFiles.GetCount();

	switch ( nAttachments )
	{
		case 0:
		return;

		case 1:
		stream <<
		wxT( "# EAC log)" ) << endl <<
		wxT( "--attachment-name" ) << endl <<
		wxT( "eac.log" ) << endl <<
		wxT( "--attachment-description" ) << endl <<
		m_logFiles[ 0 ].GetFullName() << endl <<
		wxT( "--attach-file" ) << endl <<
		mkvmerge_escape( m_logFiles[ 0 ].GetFullPath() ) << endl;
		break;

		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		stream << wxT( "# EAC logs" ) << endl;
		for ( size_t i = 0 ; i < nAttachments ; i++ )
		{
			stream <<
			wxT( "--attachment-name" ) << endl <<
			wxString::Format( wxT( "eac%d.log" ), i + 1 ) << endl <<
			wxT( "--attachment-description" ) << endl <<
			m_logFiles[ i ].GetFullName() << endl <<
			wxT( "--attach-file" ) << endl <<
			mkvmerge_escape( m_logFiles[ i ].GetFullPath() ) << endl;
		}

		break;

		default:
		stream << wxT( "# EAC logs" ) << endl;
		for ( size_t i = 0 ; i < nAttachments ; i++ )
		{
			stream <<
			wxT( "--attachment-name" ) << endl <<
			wxString::Format( wxT( "eac%02d.log" ), i + 1 ) << endl <<
			wxT( "--attachment-description" ) << endl <<
			m_logFiles[ i ].GetFullName() << endl <<
			wxT( "--attach-file" ) << endl <<
			mkvmerge_escape( m_logFiles[ i ].GetFullPath() ) << endl;
		}

		break;
	}
}

void wxMkvmergeOptsRenderer::RenderDisc( const wxInputFile& inputFile, const wxCueSheet& cueSheet )
{
	wxString sOutputFile, sTagsFile, sMatroskaFile;

	m_cfg.GetOutputFile( inputFile, sOutputFile, sTagsFile );
	m_cfg.GetOutputMatroskaFile( inputFile, sMatroskaFile, m_sMatroskaOptsFile );

	// pre
	wxDateTime dtNow( wxDateTime::Now() );

	m_asMmcPre.Empty();
	m_asMmcPre.Add( wxString::Format( wxT( "# This file was created by %s tool" ), wxGetApp().GetAppDisplayName() ) );
	m_asMmcPre.Add( wxString::Format( wxT( "# Application version: %s" ), wxGetApp().APP_VERSION ) );
	m_asMmcPre.Add( wxString::Format( wxT( "# Application vendor: %s" ), wxGetApp().GetVendorDisplayName() ) );
	m_asMmcPre.Add( wxString::Format( wxT( "# Creation time: %s %s" ), dtNow.FormatISODate(), dtNow.FormatISOTime() ) );
	m_asMmcPre.Add( wxT( "# Output file" ) );
	m_asMmcPre.Add( wxT( "-o" ) );
	if ( m_cfg.UseFullPaths() )
	{
		m_asMmcPre.Add( mkvmerge_escape( sMatroskaFile ) );
	}
	else
	{
		m_asMmcPre.Add( mkvmerge_escape( wxMyApp::GetFileName( sMatroskaFile ) ) );
	}

	m_asMmcPre.Add( wxT( "--language" ) );
	m_asMmcPre.Add( wxString::Format( wxT( "0:%s" ), m_cfg.GetLang() ) );
	m_asMmcPre.Add( wxT( "--default-track" ) );
	m_asMmcPre.Add( wxT( "0:yes" ) );
	m_asMmcPre.Add( wxT( "--track-name" ) );
	m_asMmcPre.Add( wxString::Format( wxT( "0:%s" ), cueSheet.Format( m_cfg.GetMatroskaNameFormat() ) ) );
	m_asMmcPre.Add( wxT( "# Input file(s)" ) );

	// tracks
	const wxArrayTrack& tracks = cueSheet.GetTracks();
	size_t nTracks			   = tracks.Count();
	for ( size_t i = 0 ; i < nTracks ; i++ )
	{
		if ( tracks[ i ].HasDataFile() )
		{
			bool bNext = !m_asMmcInputFiles.IsEmpty();

			m_asMmcInputFiles.Add( wxT( "-a" ) );
			m_asMmcInputFiles.Add( wxT( "0" ) );
			m_asMmcInputFiles.Add( wxT( "-D" ) );
			m_asMmcInputFiles.Add( wxT( "-S" ) );
			m_asMmcInputFiles.Add( wxT( "-T" ) );
			m_asMmcInputFiles.Add( wxT( "--no-global-tags" ) );
			m_asMmcInputFiles.Add( wxT( "--no-chapters" ) );
			if ( bNext )
			{
				wxString sLine( tracks[ i ].GetDataFile().GetFullPath() );
				sLine.Prepend( wxT( '+' ) );
				m_asMmcInputFiles.Add( mkvmerge_escape( sLine ) );
			}
			else
			{
				m_asMmcInputFiles.Add( mkvmerge_escape( tracks[ i ].GetDataFile().GetFullPath() ) );
			}
		}
	}

	// log
	if ( m_cfg.AttachEacLog() )
	{
		wxFileName logFile;
		if ( GetLogFile( inputFile.GetInputFile(), logFile ) )
		{
			wxLogInfo( _T( "Found EAC log file \u201C%s\u201D" ), logFile.GetFullPath() );
			m_logFiles.Add( logFile );
		}
	}

	// poost
	m_asMmcPost.Empty();
	m_asMmcPost.Add( wxT( "# General options" ) );
	m_asMmcPost.Add( wxT( "--default-language" ) );
	m_asMmcPost.Add( m_cfg.GetLang() );
	m_asMmcPost.Add( wxT( "--title" ) );
	m_asMmcPost.Add( cueSheet.Format( m_cfg.GetMatroskaNameFormat() ) );
	m_asMmcPost.Add( wxT( "--chapters" ) );
	if ( m_cfg.UseFullPaths() )
	{
		m_asMmcPost.Add( mkvmerge_escape( sOutputFile ) );
	}
	else
	{
		m_asMmcPost.Add( mkvmerge_escape( wxMyApp::GetFileName( sOutputFile ) ) );
	}

	if ( m_cfg.GenerateTags() )
	{
		m_asMmcPost.Add( wxT( "--global-tags" ) );
		if ( m_cfg.UseFullPaths() )
		{
			m_asMmcPost.Add( mkvmerge_escape( sTagsFile ) );
		}
		else
		{
			m_asMmcPost.Add( mkvmerge_escape( wxMyApp::GetFileName( sTagsFile ) ) );
		}
	}
}

bool wxMkvmergeOptsRenderer::Save()
{
	wxASSERT( m_cfg.GenerateMkvmergeOpts() );
	wxFileOutputStream os( m_sMatroskaOptsFile );
	if ( os.IsOk() )
	{
		wxLogInfo( _T( "Creating mkvmerge options file \u201C%s\u201D" ), wxMyApp::GetFileName( m_sMatroskaOptsFile ) );
		wxSharedPtr< wxTextOutputStream > pStream( wxTextOutputStreamWithBOMFactory::CreateUTF8( os, wxEOL_NATIVE, true, m_cfg.UseMLang() ) );
		write_as( *pStream, m_asMmcPre );
		write_as( *pStream, m_asMmcInputFiles );
		write_attachments( *pStream );
		write_as( *pStream, m_asMmcPost );
		pStream->Flush();
		return true;
	}
	else
	{
		wxLogError( _( "Fail to save options to \u201C%s\u201D" ), m_sMatroskaOptsFile );
		return false;
	}
}