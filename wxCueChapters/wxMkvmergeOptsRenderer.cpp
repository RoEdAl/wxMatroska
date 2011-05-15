/*
   wxMkvmergeOptsRenderer.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include <wxCueFile/wxTextInputStreamOnString.h>
#include <wxCueFile/wxTextOutputStreamOnString.h>
#include "wxConfiguration.h"
#include "wxInputFile.h"
#include "wxMkvmergeOptsRenderer.h"
#include "wxApp.h"

// ===============================================================================

wxMkvmergeOptsRenderer::wxMkvmergeOptsRenderer( const wxConfiguration& cfg ):
	m_cfg( cfg )
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

void wxMkvmergeOptsRenderer::write_attachments( const wxArrayFileName& logFiles )
{
	size_t nAttachments = logFiles.GetCount();

	switch ( nAttachments )
	{
		case 0:
		return;

		case 1:
		*m_os <<
		wxT( "# EAC log)" ) << endl <<
		wxT( "--attachment-name" ) << endl <<
		wxT( "eac.log" ) << endl <<
		wxT( "--attachment-description" ) << endl <<
		logFiles[ 0 ].GetFullName() << endl <<
		wxT( "--attach-file" ) << endl <<
		mkvmerge_escape( logFiles[ 0 ].GetFullPath() ) << endl;
		break;

		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		*m_os << wxT( "# EAC logs" ) << endl;
		for ( size_t i = 0; i < nAttachments; i++ )
		{
			*m_os <<
			wxT( "--attachment-name" ) << endl <<
			wxString::Format( wxT( "eac%d.log" ), i + 1 ) << endl <<
			wxT( "--attachment-description" ) << endl <<
			logFiles[ i ].GetFullName() << endl <<
			wxT( "--attach-file" ) << endl <<
			mkvmerge_escape( logFiles[ i ].GetFullPath() ) << endl;
		}

		break;

		default:
		*m_os << wxT( "# EAC logs" ) << endl;
		for ( size_t i = 0; i < nAttachments; i++ )
		{
			*m_os <<
			wxT( "--attachment-name" ) << endl <<
			wxString::Format( wxT( "eac%02d.log" ), i + 1 ) << endl <<
			wxT( "--attachment-description" ) << endl <<
			logFiles[ i ].GetFullName() << endl <<
			wxT( "--attach-file" ) << endl <<
			mkvmerge_escape( logFiles[ i ].GetFullPath() ) << endl;
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

	*m_os << wxT( "# This file was created by " ) << wxGetApp().GetAppDisplayName() << wxT( " tool" ) << endl <<
	wxT( "# Application version: " ) << wxGetApp().APP_VERSION << endl <<
	wxT( "# Application vendor: " ) << wxGetApp().GetVendorDisplayName() << endl <<
	wxT( "# Creation time: " ) << dtNow.FormatISODate() << wxT( ' ' ) << dtNow.FormatISOTime() << endl <<
	wxT( "# Output file" ) << endl <<
	wxT( "-o" ) << endl;

	if ( m_cfg.UseFullPaths() )
	{
		*m_os << mkvmerge_escape( sMatroskaFile ) << endl;
	}
	else
	{
		*m_os << mkvmerge_escape( wxMyApp::GetFileName( sMatroskaFile ) ) << endl;
	}

	*m_os << wxT( "--language" ) << endl <<
	wxT( "0:" ) << m_cfg.GetLang() << endl <<
	wxT( "--default-track" ) << endl <<
	wxT( "0:yes" ) << endl <<
	wxT( "--track-name" ) << endl <<
	wxT( "0:" ) << cueSheet.Format( m_cfg.GetMatroskaNameFormat() ) << endl <<
	wxT( "# Input file(s)" ) << endl;

	// tracks
	const wxArrayTrack& tracks = cueSheet.GetTracks();
	bool				bFirst = true;
	for ( size_t nTracks = tracks.Count(), i = 0; i < nTracks; i++ )
	{
		if ( tracks[ i ].HasDataFile() )
		{
			*m_os << wxT( "-a" ) << endl <<
			wxT( "0" ) << endl <<
			wxT( "-D" ) << endl <<
			wxT( "-S" ) << endl <<
			wxT( "-T" ) << endl <<
			wxT( "--no-global-tags" ) << endl <<
			wxT( "--no-chapters" );

			if ( !bFirst )
			{
				*m_os << wxT( '+' ) << mkvmerge_escape( tracks[ i ].GetDataFile().GetFullPath() ) << endl;
			}
			else
			{
				*m_os << mkvmerge_escape( tracks[ i ].GetDataFile().GetFullPath() ) << endl;
				bFirst = false;
			}
		}
	}

	// log
	if ( m_cfg.AttachEacLog() )
	{
		write_attachments( cueSheet.GetLog() );
	}

	// poost
	*m_os << wxT( "# General options" ) << endl <<
	wxT( "--default-language" ) << endl <<
	m_cfg.GetLang() << endl <<
	wxT( "--title" ) << endl <<
	cueSheet.Format( m_cfg.GetMatroskaNameFormat() ) << endl <<
	wxT( "--chapters" ) << endl;

	if ( m_cfg.UseFullPaths() )
	{
		*m_os << mkvmerge_escape( sOutputFile ) << endl;
	}
	else
	{
		*m_os << mkvmerge_escape( wxMyApp::GetFileName( sOutputFile ) ) << endl;
	}

	if ( m_cfg.GenerateTags() )
	{
		*m_os << wxT( "--global-tags" ) << endl;
		if ( m_cfg.UseFullPaths() )
		{
			*m_os << mkvmerge_escape( sTagsFile ) << endl;
		}
		else
		{
			*m_os << mkvmerge_escape( wxMyApp::GetFileName( sTagsFile ) ) << endl;
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
		wxSharedPtr<wxTextOutputStream> pStream( wxTextOutputStreamWithBOMFactory::CreateUTF8( os, wxEOL_NATIVE, true, m_cfg.UseMLang() ) );
		wxTextInputStreamOnString		tis( m_os.GetString() );
		while ( !tis.Eof() )
		{
			*pStream << ( *tis ).ReadLine() << endl;
		}

		pStream->Flush();
		return true;
	}
	else
	{
		wxLogError( _( "Fail to save options to \u201C%s\u201D" ), m_sMatroskaOptsFile );
		return false;
	}
}