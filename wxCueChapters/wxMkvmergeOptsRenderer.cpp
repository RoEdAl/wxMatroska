/*
   wxMkvmergeOptsRenderer.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include <wxCueFile/wxTextOutputStreamOnString.h>
#include <wxCueFile/wxTextInputStreamOnString.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>
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

void wxMkvmergeOptsRenderer::write_log_attachments( const wxArrayFileName& logFiles )
{
	size_t nAttachments = logFiles.GetCount();

	switch ( nAttachments )
	{
		case 0:
		return;

		case 1:
		*m_os <<
		wxT( "# EAC log" ) << endl <<
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

void wxMkvmergeOptsRenderer::write_eac_attachments( const wxInputFile& inputFile, const wxCueSheet& cueSheet )
{
	switch ( m_cfg.GetCueSheetAttachMode() )
	{
		case wxConfiguration::CUESHEET_ATTACH_SOURCE:
		write_source_eac_attachments( inputFile, cueSheet.GetContent() );
		break;

		case wxConfiguration::CUESHEET_ATTACH_DECODED:
		write_decoded_eac_attachments( inputFile, cueSheet.GetContent() );
		break;

		case wxConfiguration::CUESHEET_ATTACH_RENDERED:
		write_rendered_eac_attachments( inputFile, cueSheet );
		break;
	}
}

void wxMkvmergeOptsRenderer::write_source_eac_attachments( const wxInputFile& WXUNUSED( inputFile ), const wxArrayCueSheetContent& contents )
{
	size_t nContents	   = contents.Count();
	size_t nSourceContents = 0;

	for ( size_t i = 0; i < nContents; i++ )
	{
		if ( contents[ i ].HasSource() )
		{
			nSourceContents += 1;
		}
	}

	switch ( nSourceContents )
	{
		case 0:
		return;

		case 1:
		for ( size_t i = 0; i < nContents; i++ )
		{
			if ( !contents[ i ].HasSource() )
			{
				continue;
			}

			*m_os <<
			wxT( "# cuesheet" ) << endl <<
			wxT( "--attachment-name" ) << endl <<
			wxT( "cuesheet.cue" ) << endl <<
			wxT( "--attachment-description" ) << endl <<
			contents[ i ].GetSource().GetFullName() << endl <<
			wxT( "--attach-file" ) << endl <<
			mkvmerge_escape( contents[ i ].GetSource().GetFullPath() ) << endl;
		}

		break;

		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		*m_os << wxT( "# cuesheets" ) << endl;
		for ( size_t i = 0, nCounter = 1; i < nContents; i++ )
		{
			if ( !contents[ i ].HasSource() )
			{
				continue;
			}

			*m_os <<
			wxT( "--attachment-name" ) << endl <<
			wxString::Format( wxT( "cuesheet%d.log" ), nCounter ) << endl <<
			wxT( "--attachment-description" ) << endl <<
			contents[ i ].GetSource().GetFullName() << endl <<
			wxT( "--attach-file" ) << endl <<
			mkvmerge_escape( contents[ i ].GetSource().GetFullPath() ) << endl;

			nCounter += 1;
		}

		break;

		default:
		*m_os << wxT( "# cuesheets" ) << endl;
		for ( size_t i = 0, nCounter = 1; i < nContents; i++ )
		{
			if ( !contents[ i ].HasSource() )
			{
				continue;
			}

			*m_os <<
			wxT( "--attachment-name" ) << endl <<
			wxString::Format( wxT( "cuesheet%02d.log" ), nCounter ) << endl <<
			wxT( "--attachment-description" ) << endl <<
			contents[ i ].GetSource().GetFullName() << endl <<
			wxT( "--attach-file" ) << endl <<
			mkvmerge_escape( contents[ i ].GetSource().GetFullPath() ) << endl;

			nCounter += 1;
		}

		break;
	}
}

bool wxMkvmergeOptsRenderer::save_cuesheet( const wxInputFile& inputFile, const wxString& sPostfix, const wxString& sContent, wxFileName& fn )
{
	wxFileName cueSheet;

	if ( !m_cfg.GetOutputCueSheetFile( inputFile, sPostfix, cueSheet ) )
	{
		return false;
	}

	wxString		   sCueSeetPath( cueSheet.GetFullPath() );
	wxFileOutputStream os( cueSheet.GetFullPath() );
	if ( os.IsOk() )
	{
		wxLogInfo( _( "Creating cue sheet file \u201C%s\u201D" ), cueSheet.GetFullName() );
		wxSharedPtr<wxTextOutputStream> pStream( m_cfg.GetOutputTextStream( os ) );
		save_string_to_stream( *pStream, sContent );
		fn = cueSheet;
		return true;
	}
	else
	{
		wxLogError( _( "Fail to create cue sheet file \u201C%s\u201D" ), cueSheet.GetFullName() );
		return false;
	}
}

bool wxMkvmergeOptsRenderer::render_cuesheet( const wxInputFile& inputFile, const wxString& sPostfix, const wxCueSheet& cueSheet, wxFileName& fn )
{
	wxTextOutputStreamOnString tos;
	wxTextCueSheetRenderer	   renderer( &( tos.GetStream() ) );

	if ( renderer.Render( cueSheet ) )
	{
		return save_cuesheet( inputFile, sPostfix, tos.GetString(), fn );
	}
	else
	{
		wxLogError( wxT( "Fail to render cue sheet %s" ), sPostfix );
		return false;
	}
}

void wxMkvmergeOptsRenderer::write_decoded_eac_attachments( const wxInputFile& inputFile, const wxArrayCueSheetContent& contents )
{
	size_t	   nContents = contents.Count();
	wxFileName cueSheetPath;

	switch ( nContents )
	{
		case 0:
		return;

		case 1:
		if ( save_cuesheet( inputFile, wxT( "embedded" ), contents[ 0 ].GetValue(), cueSheetPath ) )
		{
			*m_os <<
			wxT( "# cuesheet" ) << endl <<
			wxT( "--attachment-name" ) << endl <<
			wxT( "cuesheet.cue" ) << endl <<
			wxT( "--attach-file" ) << endl <<
			mkvmerge_escape( cueSheetPath.GetFullPath() ) << endl;
		}

		break;

		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		*m_os << wxT( "# cuesheets" ) << endl;
		for ( size_t i = 0; i < nContents; i++ )
		{
			if ( save_cuesheet( inputFile, wxString::Format( wxT( "embedded%d" ), i + 1 ), contents[ i ].GetValue(), cueSheetPath ) )
			{
				*m_os <<
				wxT( "--attachment-name" ) << endl <<
				wxString::Format( wxT( "cuesheet%d.log" ), i + 1 ) << endl <<
				wxT( "--attach-file" ) << endl <<
				mkvmerge_escape( cueSheetPath.GetFullPath() ) << endl;
			}
		}

		break;

		default:
		*m_os << wxT( "# cuesheets" ) << endl;
		for ( size_t i = 0; i < nContents; i++ )
		{
			if ( save_cuesheet( inputFile, wxString::Format( wxT( "embedded%02d" ), i + 1 ), contents[ i ].GetValue(), cueSheetPath ) )
			{
				*m_os <<
				wxT( "--attachment-name" ) << endl <<
				wxString::Format( wxT( "cuesheet%d.log" ), i + 1 ) << endl <<
				wxT( "--attach-file" ) << endl <<
				mkvmerge_escape( cueSheetPath.GetFullPath() ) << endl;
			}
		}

		break;
	}
}

void wxMkvmergeOptsRenderer::write_rendered_eac_attachments( const wxInputFile& inputFile, const wxCueSheet& cueSheet )
{
	wxFileName cueSheetPath;

	if ( render_cuesheet( inputFile, wxT( "rendered" ), cueSheet, cueSheetPath ) )
	{
		*m_os <<
		wxT( "# cuesheet" ) << endl <<
		wxT( "--attachment-name" ) << endl <<
		wxT( "cuesheet.cue" ) << endl <<
		wxT( "--attachment-description" ) << endl <<
		wxT( "Rendered cue sheet" ) << endl <<
		wxT( "--attach-file" ) << endl <<
		mkvmerge_escape( cueSheetPath.GetFullPath() ) << endl;
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
			wxT( "--no-chapters" ) << endl;

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
		write_log_attachments( cueSheet.GetLog() );
	}

	write_eac_attachments( inputFile, cueSheet );

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
		save_string_to_stream( *pStream, m_os.GetString() );
		return true;
	}
	else
	{
		wxLogError( _( "Fail to save options to \u201C%s\u201D" ), m_sMatroskaOptsFile );
		return false;
	}
}

void wxMkvmergeOptsRenderer::save_string_to_stream( wxTextOutputStream& stream, const wxString& s )
{
	wxTextInputStreamOnString tis( s );
	wxString				  sLine;

	while ( !tis.Eof() )
	{
		sLine = ( *tis ).ReadLine();
		if ( sLine.IsEmpty() )
		{
			stream << endl;
		}
		else
		{
			stream << sLine << endl;
		}
	}

	stream.Flush();
}