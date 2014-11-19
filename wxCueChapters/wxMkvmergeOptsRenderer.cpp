/*
 * wxMkvmergeOptsRenderer.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#include <wxEncodingDetection/wxTextInputStreamOnString.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>
#include "wxConfiguration.h"
#include "wxInputFile.h"
#include "wxPrimitiveRenderer.h"
#include "wxMkvmergeOptsRenderer.h"
#include "wxApp.h"

// ===============================================================================

wxMkvmergeOptsRenderer::wxMkvmergeOptsRenderer( const wxConfiguration& cfg ):
	wxPrimitiveRenderer( cfg )
{}

const wxFileName& wxMkvmergeOptsRenderer::GetMkvmergeOptsFile() const
{
	return m_matroskaOptsFile;
}

wxString wxMkvmergeOptsRenderer::mkvmerge_escape( const wxString& s )
{
	wxString sRes( s );

	sRes.Replace( wxS( '\\' ), wxS( "\\\\" ) );
	sRes.Replace( wxS( ' ' ), wxS( "\\s" ) );
	sRes.Replace( wxS( '\"' ), wxS( "\\2" ) );
	sRes.Replace( wxS( ':' ), wxS( "\\c" ) );
	sRes.Replace( wxS( '#' ), wxS( "\\h" ) );
	return sRes;
}

wxTextOutputStream& wxMkvmergeOptsRenderer::append_cover( wxTextOutputStream& str, const wxCoverFile& coverFile )
{
    str <<
        wxS( "--attachment-description" ) << endl <<
        (coverFile.HasDescription() ? coverFile.GetDescription() : coverFile.GetFileName().GetFullName()) << endl;

    if (coverFile.HasMimeType())
    {
        str <<
            wxS( "--attachment-mime-type" ) << endl <<
            coverFile.GetMimeType() << endl;
    }

    str << 
        wxS( "--attach-file" ) << endl <<
        mkvmerge_escape( coverFile.GetFileName( ) ) << endl;

    return str;
}

wxString wxMkvmergeOptsRenderer::mkvmerge_escape( const wxFileName& fn )
{
	return mkvmerge_escape( fn.GetFullPath() );
}

wxString wxMkvmergeOptsRenderer::mkvmerge_escape( const wxDataFile& df )
{
	wxASSERT( df.HasRealFileName() );
	return mkvmerge_escape( df.GetRealFileName() );
}

wxString wxMkvmergeOptsRenderer::GetEscapedFile( const wxFileName& fn )
{
	if ( m_cfg.UseFullPaths() )
	{
		return mkvmerge_escape( fn.GetFullPath() );
	}
	else
	{
		return mkvmerge_escape( fn.GetFullName() );
	}
}

bool wxMkvmergeOptsRenderer::save_cover( const wxInputFile& inputFile, wxCoverFile& cover ) const
{
    if (cover.HasFileName()) return true;

    wxASSERT( cover.HasData() );

    wxFileName fn;
    if (m_cfg.GetOutputFile( inputFile, wxS( "img" ), cover.GetExt(), fn ))
    {
        return cover.Save( fn );
    }
    else
    {
        return false;
    }
}

void wxMkvmergeOptsRenderer::write_cover_attachments( const wxInputFile& inputFile, const wxArrayCoverFile& coverFiles ) const
{
	size_t nAttachments = coverFiles.GetCount();

	switch ( nAttachments )
	{
		case 0:
		{
			wxLogWarning( _( "Cover(s) not found." ) );
			*m_os << wxS( "# Cover(s) not found" ) << endl;
			return;
		}

		case 1:
		{
            *m_os << wxS( "# cover" ) << endl;

            if (save_cover( inputFile, coverFiles[0] ))
            {
                *m_os <<
                    wxS( "--attachment-name" ) << endl <<
                    wxS( "cover." ) << coverFiles[0].GetFileName( ).GetExt( ).Lower( ) << endl;
                append_cover( *m_os, coverFiles[0] );
            }
			break;
		}

		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		{
            *m_os << wxS( "# covers (" ) << nAttachments << wxS( ')' ) << endl;

            if (save_cover( inputFile, coverFiles[0] ))
            {
                *m_os << 
                    // first attachment
                    wxS( "--attachment-name" ) << endl <<
                    wxS( "cover." ) << coverFiles[0].GetFileName().GetExt().Lower() << endl;

                append_cover( *m_os, coverFiles[0] );
            }

			// rest
			for ( size_t i = 1; i < nAttachments; i++ )
			{
                if (save_cover( inputFile, coverFiles[i] ))
                {
                    *m_os <<
                        wxS( "--attachment-name" ) << endl <<
                        wxString::Format( wxS( "cover%d." ), i + 1 ) << coverFiles[i].GetFileName().GetExt().Lower() << endl;

                    append_cover( *m_os, coverFiles[i] );
                }
			}

			break;
		}

		default:
        {
             *m_os << wxS( "# covers (" ) << nAttachments << wxS( ')' ) << endl;

            if (save_cover( inputFile, coverFiles[0] ))
            {
                *m_os <<
                    // first attachment
                    wxS( "--attachment-name" ) << endl <<
                    wxS( "cover." ) << coverFiles[0].GetFileName().GetExt().Lower() << endl;

                append_cover( *m_os, coverFiles[0] );
            }

			// rest
			for ( size_t i = 1; i < nAttachments; i++ )
			{
                if (save_cover( inputFile, coverFiles[i] ))
                {
                    *m_os <<
                        wxS( "--attachment-name" ) << endl <<
                        wxString::Format( wxS( "cover%02d." ), i + 1 ) << coverFiles[i].GetFileName().GetExt().Lower() << endl;

                    append_cover( *m_os, coverFiles[i] );
                }
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_cdtextfiles_attachments( const wxArrayFileName& cdtFiles )
{
	size_t nAttachments = cdtFiles.GetCount();

	switch ( nAttachments )
	{
		case 0:
		{
			*m_os << wxS( "# CDTEXT files not attached" ) << endl;
			return;
		}

		case 1:
		{
			*m_os <<
			wxS( "# CDTEXT file" ) << endl <<
			wxS( "--attachment-name" ) << endl <<
			wxS( "cdtext." ) << cdtFiles[ 0 ].GetExt().Lower() << endl <<
			wxS( "--attachment-description" ) << endl <<
			cdtFiles[ 0 ].GetFullName() << endl <<
			wxS( "--attachment-mime-type" ) << endl <<
			wxS( "application/octet-stream" ) << endl <<
			wxS( "--attach-file" ) << endl <<
			mkvmerge_escape( cdtFiles[ 0 ] ) << endl;
			break;
		}

		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		{
			*m_os << wxS( "# CDTEXT (" ) << nAttachments << wxS( ')' ) << endl;

			for ( size_t i = 0; i < nAttachments; i++ )
			{
				*m_os <<
				wxS( "--attachment-name" ) << endl <<
				wxString::Format( wxS( "cdtext%d." ), i + 1 ) << cdtFiles[ i ].GetExt().Lower() << endl <<
				wxS( "--attachment-description" ) << endl <<
				cdtFiles[ i ].GetFullName() << endl <<
				wxS( "--attachment-mime-type" ) << endl <<
				wxS( "application/octet-stream" ) << endl <<
				wxS( "--attach-file" ) << endl <<
				mkvmerge_escape( cdtFiles[ i ] ) << endl;
			}

			break;
		}

		default:
		{
			*m_os << wxS( "# covers (" ) << nAttachments << wxS( ')' ) << endl;
			
			for ( size_t i = 0; i < nAttachments; i++ )
			{
				*m_os <<
				wxS( "--attachment-name" ) << endl <<
				wxString::Format( wxS( "cdtext%02d." ), i + 1 ) << cdtFiles[ i ].GetExt().Lower() << endl <<
				wxS( "--attachment-description" ) << endl <<
				cdtFiles[ i ].GetFullName() << endl <<
				wxS( "--attachment-mime-type" ) << endl <<
				wxS( "application/octet-stream" ) << endl <<
				wxS( "--attach-file" ) << endl <<
				mkvmerge_escape( cdtFiles[ i ] ) << endl;
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_log_attachments( const wxArrayFileName& logFiles )
{
	size_t nAttachments = logFiles.GetCount();

	switch ( nAttachments )
	{
		case 0:
		{
			wxLogWarning( _( "EAC log(s) not found." ) );
			*m_os << wxS( "# EAC log(s) not found" ) << endl;
			return;
		}

		case 1:
		{
			*m_os <<
			wxS( "# EAC log" ) << endl <<
			wxS( "--attachment-name" ) << endl <<
			wxS( "eac.log" ) << endl <<
			wxS( "--attachment-description" ) << endl <<
			logFiles[ 0 ].GetFullName() << endl <<
			wxS( "--attachment-mime-type" ) << endl <<
			wxS( "text/plain" ) << endl <<
			wxS( "--attach-file" ) << endl <<
			mkvmerge_escape( logFiles[ 0 ] ) << endl;
			break;
		}

		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		{
			*m_os << wxS( "# EAC logs (" ) << nAttachments << wxS( ')' ) << endl;
			for ( size_t i = 0; i < nAttachments; i++ )
			{
				*m_os <<
				wxS( "--attachment-name" ) << endl <<
				wxString::Format( wxS( "eac%d.log" ), i + 1 ) << endl <<
				wxS( "--attachment-description" ) << endl <<
				logFiles[ i ].GetFullName() << endl <<
				wxS( "--attachment-mime-type" ) << endl <<
				wxS( "text/plain" ) << endl <<
				wxS( "--attach-file" ) << endl <<
				mkvmerge_escape( logFiles[ i ] ) << endl;
			}

			break;
		}

		default:
		{
			*m_os << wxS( "# EAC logs (" ) << nAttachments << wxS( ')' ) << endl;
			for ( size_t i = 0; i < nAttachments; i++ )
			{
				*m_os <<
				wxS( "--attachment-name" ) << endl <<
				wxString::Format( wxS( "eac%02d.log" ), i + 1 ) << endl <<
				wxS( "--attachment-description" ) << endl <<
				logFiles[ i ].GetFullName() << endl <<
				wxS( "--attachment-mime-type" ) << endl <<
				wxS( "text/plain" ) << endl <<
				wxS( "--attach-file" ) << endl <<
				mkvmerge_escape( logFiles[ i ] ) << endl;
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_eac_attachments(
		const wxInputFile& inputFile, const wxCueSheet& cueSheet )
{
	switch ( m_cfg.GetCueSheetAttachMode() )
	{
		case wxConfiguration::CUESHEET_ATTACH_SOURCE:
		{
			write_source_eac_attachments( inputFile, cueSheet.GetContents() );
			break;
		}

		case wxConfiguration::CUESHEET_ATTACH_DECODED:
		{
			write_decoded_eac_attachments( inputFile, cueSheet.GetContents() );
			break;
		}

		case wxConfiguration::CUESHEET_ATTACH_RENDERED:
		{
			write_rendered_eac_attachments( inputFile, cueSheet );
			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_source_eac_attachments(
		const wxInputFile& WXUNUSED( inputFile ), const wxArrayCueSheetContent& contents )
{
	size_t nContents	   = contents.GetCount();
	size_t nSourceContents = 0;

	for ( size_t i = 0; i < nContents; i++ )
	{
		const wxCueSheetContent cnt = contents[ i ];

		if ( cnt.HasSource() && !cnt.IsEmbedded() )
		{
			nSourceContents += 1;
		}
	}

	switch ( nSourceContents )
	{
		case 0:
		{
			*m_os << wxS( "# no source cuesheets" ) << endl;
			return;
		}

		case 1:
		{
			for ( size_t i = 0; i < nContents; i++ )
			{
				const wxCueSheetContent cnt = contents[ i ];

				if ( !( cnt.HasSource() && !cnt.IsEmbedded() ) )
				{
					continue;
				}

				*m_os <<
				wxS( "# cuesheet" ) << endl <<
				wxS( "--attachment-name" ) << endl <<
				wxS( "cuesheet.cue" ) << endl <<
				wxS( "--attachment-description" ) << endl <<
				cnt.GetSource().GetFileName().GetFullName() << endl <<
				wxS( "--attachment-mime-type" ) << endl <<
				wxS( "text/plain" ) << endl <<
				wxS( "--attach-file" ) << endl <<
				mkvmerge_escape( cnt.GetSource() ) << endl;
			}

			break;
		}

		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		{
			*m_os << wxS( "# cuesheets (" ) << nSourceContents << wxS( ')' ) << endl;
			for ( size_t i = 0, nCounter = 1; i < nContents; i++ )
			{
				const wxCueSheetContent cnt = contents[ i ];

				if ( !( cnt.HasSource() && !cnt.IsEmbedded() ) )
				{
					continue;
				}

				*m_os <<
				wxS( "--attachment-name" ) << endl <<
				wxString::Format( wxS( "cuesheet%d.cue" ), nCounter ) << endl <<
				wxS( "--attachment-description" ) << endl <<
				cnt.GetSource().GetFileName().GetFullName() << endl <<
				wxS( "--attachment-mime-type" ) << endl <<
				wxS( "text/plain" ) << endl <<
				wxS( "--attach-file" ) << endl <<
				mkvmerge_escape( cnt.GetSource() ) << endl;

				nCounter += 1;
			}

			break;
		}

		default:
		{
			*m_os << wxS( "# cuesheets (" ) << nSourceContents << wxS( ')' ) << endl;
			for ( size_t i = 0, nCounter = 1; i < nContents; i++ )
			{
				const wxCueSheetContent cnt = contents[ i ];

				if ( !( cnt.HasSource() && !cnt.IsEmbedded() ) )
				{
					continue;
				}

				*m_os <<
				wxS( "--attachment-name" ) << endl <<
				wxString::Format( wxS( "cuesheet%02d.cue" ), nCounter ) << endl <<
				wxS( "--attachment-description" ) << endl <<
				cnt.GetSource().GetFileName().GetFullName() << endl <<
				wxS( "--attachment-mime-type" ) << endl <<
				wxS( "text/plain" ) << endl <<
				wxS( "--attach-file" ) << endl <<
				mkvmerge_escape( cnt.GetSource() ) << endl;

				nCounter += 1;
			}

			break;
		}
	}
}

bool wxMkvmergeOptsRenderer::save_cuesheet( const wxInputFile& inputFile,
		const wxString& sPostfix, const wxString& sContent,
		wxFileName& cueSheet ) const
{
	if ( !m_cfg.GetOutputCueSheetFile( inputFile, sPostfix, cueSheet ) )
	{
		return false;
	}

	wxFileOutputStream os( cueSheet.GetFullPath() );

	if ( os.IsOk() )
	{
		wxLogInfo( _( "Creating cue sheet file \u201C%s\u201D" ), cueSheet.GetFullName() );
		wxSharedPtr< wxTextOutputStream > pStream( m_cfg.GetOutputTextStream( os ) );
		wxTextOutputStreamOnString::SaveTo( *pStream, sContent );
		return true;
	}
	else
	{
		wxLogError( _( "Fail to create cue sheet file \u201C%s\u201D" ), cueSheet.GetFullName() );
		return false;
	}
}

bool wxMkvmergeOptsRenderer::render_cuesheet( const wxInputFile& inputFile,
		const wxString& sPostfix, const wxCueSheet& cueSheet,
		wxFileName& fn )
{
	wxTextOutputStreamOnString tos;
	if ( !wxTextCueSheetRenderer::ToString( *tos, cueSheet ) )
	{
		return false;
	}

	return save_cuesheet( inputFile, sPostfix, tos.GetString(), fn );
}

void wxMkvmergeOptsRenderer::write_decoded_eac_attachments( const wxInputFile& inputFile,
		const wxArrayCueSheetContent& contents )
{
	size_t	   nContents = contents.GetCount();
	wxFileName cueSheetPath;

	switch ( nContents )
	{
		case 0:
		{
			*m_os << wxS( "# no decoded cuesheets" ) << endl;
			return;
		}

		case 1:
		{
			if ( save_cuesheet( inputFile, wxS( "embedded" ), contents[ 0 ].GetValue(), cueSheetPath ) )
			{
				*m_os <<
				wxS( "# cuesheet" ) << endl <<
				wxS( "--attachment-name" ) << endl <<
				wxS( "cuesheet.cue" ) << endl <<
				wxS( "--attachment-mime-type" ) << endl <<
				wxS( "text/plain" ) << endl <<
				wxS( "--attach-file" ) << endl <<
				mkvmerge_escape( cueSheetPath ) << endl;
			}

			break;
		}

		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		{
			*m_os << wxS( "# cuesheets" ) << endl;
			for ( size_t i = 0; i < nContents; i++ )
			{
				if ( save_cuesheet( inputFile, wxString::Format( wxS( "embedded%d" ), i + 1 ), contents[ i ].GetValue(), cueSheetPath ) )
				{
					*m_os <<
					wxS( "--attachment-name" ) << endl <<
					wxString::Format( wxS( "cuesheet%d.cue" ), i + 1 ) << endl <<
					wxS( "--attachment-mime-type" ) << endl <<
					wxS( "text/plain" ) << endl <<
					wxS( "--attach-file" ) << endl <<
					mkvmerge_escape( cueSheetPath ) << endl;
				}
			}

			break;
		}

		default:
		{
			*m_os << wxS( "# cuesheets" ) << endl;
			for ( size_t i = 0; i < nContents; i++ )
			{
				if ( save_cuesheet( inputFile, wxString::Format( wxS( "embedded%02d" ), i + 1 ), contents[ i ].GetValue(), cueSheetPath ) )
				{
					*m_os <<
					wxS( "--attachment-name" ) << endl <<
					wxString::Format( wxS( "cuesheet%d.cue" ), i + 1 ) << endl <<
					wxS( "--attachment-mime-type" ) << endl <<
					wxS( "text/plain" ) << endl <<
					wxS( "--attach-file" ) << endl <<
					mkvmerge_escape( cueSheetPath ) << endl;
				}
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_rendered_eac_attachments( const wxInputFile& inputFile, const wxCueSheet& cueSheet )
{
	wxFileName cueSheetPath;

	if ( render_cuesheet( inputFile, wxS( "rendered" ), cueSheet, cueSheetPath ) )
	{
		*m_os <<
		wxS( "# cuesheet" ) << endl <<
		wxS( "--attachment-name" ) << endl <<
		wxS( "cuesheet.cue" ) << endl <<
		wxS( "--attachment-description" ) << endl <<
		wxS( "Rendered cue sheet" ) << endl <<
		wxS( "--attachment-mime-type" ) << endl <<
		wxS( "text/plain" ) << endl <<
		wxS( "--attach-file" ) << endl <<
		mkvmerge_escape( cueSheetPath ) << endl;
	}
}

wxString wxMkvmergeOptsRenderer::get_mapping_str( const wxCueSheet& cueSheet )
{
	wxASSERT( cueSheet.GetDataFilesCount() > 1u );
	wxString sRes;

	for ( size_t i = 1u, nCount = cueSheet.GetDataFilesCount(); i < nCount; i++ )
	{
		sRes += wxString::Format( wxS( "%d:0:%d:0," ), i, i - 1 );
	}

	return sRes.RemoveLast();
}

void wxMkvmergeOptsRenderer::RenderDisc( const wxInputFile& inputFile,
		const wxCueSheet& cueSheet )
{
	wxFileName outputFile, tagsFile, matroskaFile;

	m_cfg.GetOutputFile( inputFile, outputFile, tagsFile );
	m_cfg.GetOutputMatroskaFile( inputFile, matroskaFile, m_matroskaOptsFile );

	// pre
	wxDateTime dtNow( wxDateTime::Now() );

	*m_os <<
	wxS( "# This file was created by " ) << wxGetApp().GetAppDisplayName() << wxS( " tool" ) << endl <<
	wxS( "# Application version: " ) << wxGetApp().APP_VERSION << endl <<
	wxS( "# Application vendor: " ) << wxGetApp().GetVendorDisplayName() << endl <<
	wxS( "# Creation time: " ) << dtNow.FormatISODate() << wxS( ' ' ) << dtNow.FormatISOTime() << endl <<
	wxS( "# mkvmerge's UI language" ) << endl <<
	wxS( "--ui-language" ) << endl <<
	wxS( "en" ) << endl <<
	wxS( "# Output file" ) << endl <<
	wxS( "-o" ) << endl << GetEscapedFile( matroskaFile ) << endl <<
	wxS( "--language" ) << endl << wxS( "0:" ) << m_cfg.GetLang() << endl <<
	wxS( "--default-track" ) << endl << wxS( "0:yes" ) << endl <<
	wxS( "--track-name" ) << endl << wxS( "0:" ) << cueSheet.Format( m_cfg.GetMatroskaNameFormat() ) << endl <<
	wxS( "# Input file(s)" ) << endl;

	// tracks
	const wxArrayDataFile& dataFiles = cueSheet.GetDataFiles();
	bool				   bFirst	 = true;

	for ( size_t nTracks = dataFiles.GetCount(), i = 0; i < nTracks; i++ )
	{
		*m_os <<
		wxS( "-a" ) << endl << wxS( "0" ) << endl <<
		wxS( "-D" ) << endl <<
		wxS( "-S" ) << endl <<
		wxS( "-T" ) << endl <<
		wxS( "--no-global-tags" ) << endl <<
		wxS( "--no-chapters" ) << endl;

		const wxDataFile& dataFile = dataFiles[ i ];
		wxASSERT( dataFile.HasRealFileName() );

		if ( !bFirst )
		{
			*m_os <<
			wxS( '+' ) << endl <<
			mkvmerge_escape( dataFile ) << endl;
		}
		else
		{
			*m_os <<
			wxS( '=' ) << endl <<
			mkvmerge_escape( dataFile ) << endl;

			bFirst = false;
		}
	}

	// cover - must be a first attachment
	if ( m_cfg.AttachCover() )
	{
		write_cover_attachments( inputFile, cueSheet.GetCovers() );
	}

	write_cdtextfiles_attachments( cueSheet.GetCdTextFiles() );

	// log
	if ( m_cfg.AttachEacLog() )
	{
		write_log_attachments( cueSheet.GetLogs() );
	}

	write_eac_attachments( inputFile, cueSheet );

	// post
	*m_os <<
	wxS( "# General options" ) << endl <<
	wxS( "--default-language" ) << endl << m_cfg.GetLang() << endl <<
	wxS( "--title" ) << endl << cueSheet.Format( m_cfg.GetMatroskaNameFormat() ) << endl <<
	wxS( "--chapters" ) << endl << GetEscapedFile( outputFile ) << endl;

	if ( m_cfg.GenerateTags() )
	{
		*m_os << wxS( "--global-tags" ) << endl << GetEscapedFile( tagsFile ) << endl;
	}

	if ( dataFiles.GetCount() > 1u )
	{
		*m_os << wxS( "--append-to" ) << endl << get_mapping_str( cueSheet ) << endl;
	}
}

bool wxMkvmergeOptsRenderer::Save()
{
	wxASSERT( m_cfg.GenerateMkvmergeOpts() );
	wxFileOutputStream os( m_matroskaOptsFile.GetFullPath() );

	if ( os.IsOk() )
	{
		wxLogInfo( _( "Creating mkvmerge options file \u201C%s\u201D" ), m_matroskaOptsFile.GetFullName() );
		wxSharedPtr< wxTextOutputStream > pStream( m_cfg.GetOutputTextStream( os ) );
		m_os.SaveTo( *pStream );
		return true;
	}
	else
	{
		wxLogError( _( "Fail to save options to \u201C%s\u201D" ), m_matroskaOptsFile.GetFullName() );
		return false;
	}
}

