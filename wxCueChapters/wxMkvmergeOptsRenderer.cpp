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

namespace
{
    struct attachment
    {
        static const char name[];
        static const char mime[];
        static const char desc[];
        static const char file[];
    };

    const char attachment::name[] = "--attachment-name";
    const char attachment::mime[] = "--attachment-mime-type";
    const char attachment::desc[] = "--attachment-description";
    const char attachment::file[] = "--attach-file";

    struct mime
    {
        static const char octet_stream[];
        static const char text_plain[];
    };

    const char mime::octet_stream[] = "application/octet-stream";
    const char mime::text_plain[] = "text/plain";
}

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

	sRes.Replace( '\\', "\\\\" );
	sRes.Replace( ' ', "\\s" );
	sRes.Replace( '\"', "\\2" );
	sRes.Replace( ':', "\\c" );
	sRes.Replace( '#', "\\h" );

	return sRes;
}

wxTextOutputStream& wxMkvmergeOptsRenderer::append_cover( wxTextOutputStream& str, const wxCoverFile& coverFile )
{
    wxString stype;
    wxCoverFile::GetStrFromType( coverFile.GetType(), stype );

    wxString sDescription;
    if (coverFile.HasDescription())
    {
        sDescription = coverFile.GetDescription();
        sDescription << " [" << stype << ']';
    }
    else 
    {
        sDescription = stype;
    }
    
    str << attachment::desc << endl << sDescription << endl;

    if (coverFile.HasMimeType())
    {
        str <<
            attachment::mime << endl <<
            coverFile.GetMimeType() << endl;
    }

    str << 
        attachment::file << endl <<
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
    if (m_cfg.GetOutputFile( inputFile, "img", cover.GetExt(), fn ))
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
			*m_os << "# Cover(s) not found" << endl;
			return;
		}

		case 1:
		{
            *m_os << "# cover" << endl;

            if (save_cover( inputFile, coverFiles[0] ))
            {
                *m_os <<
                    attachment::name << endl <<
                    "cover." << coverFiles[0].GetFileName( ).GetExt( ).Lower( ) << endl;
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
            *m_os << "# covers (" << nAttachments << ')' << endl;

            if (save_cover( inputFile, coverFiles[0] ))
            {
                *m_os << 
                    // first attachment
                    attachment::name << endl <<
                    wxS( "cover." ) << coverFiles[0].GetFileName().GetExt().Lower() << endl;

                append_cover( *m_os, coverFiles[0] );
            }

			// rest
			for ( size_t i = 1; i < nAttachments; i++ )
			{
                if (save_cover( inputFile, coverFiles[i] ))
                {
                    *m_os <<
                        attachment::name << endl <<
                        wxString::Format( "cover%d.", i + 1 ) << coverFiles[i].GetFileName().GetExt().Lower() << endl;

                    append_cover( *m_os, coverFiles[i] );
                }
			}

			break;
		}

		default:
        {
             *m_os << "# covers (" << nAttachments << ')' << endl;

            if (save_cover( inputFile, coverFiles[0] ))
            {
                *m_os <<
                    // first attachment
                    attachment::name << endl <<
                    "cover." << coverFiles[0].GetFileName().GetExt().Lower() << endl;

                append_cover( *m_os, coverFiles[0] );
            }

			// rest
			for ( size_t i = 1; i < nAttachments; i++ )
			{
                if (save_cover( inputFile, coverFiles[i] ))
                {
                    *m_os <<
                        attachment::name << endl <<
                        wxString::Format( "cover%02d.", i + 1 ) << coverFiles[i].GetFileName().GetExt().Lower() << endl;

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
			*m_os << wxS( "# no CDTEXT file(s)" ) << endl;
			return;
		}

		case 1:
		{
			*m_os <<
			"# CDTEXT file" << endl <<
			attachment::name << endl <<
			"cdtext." << cdtFiles[ 0 ].GetExt().Lower() << endl <<
			attachment::desc << endl <<
			cdtFiles[ 0 ].GetFullName() << endl <<
			attachment::mime << endl <<
			mime::octet_stream << endl <<
			attachment::file << endl <<
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
			*m_os << "# CDTEXT (" << nAttachments << ')' << endl;

			for ( size_t i = 0; i < nAttachments; i++ )
			{
				*m_os <<
				attachment::name << endl <<
				wxString::Format( "cdtext%d.", i + 1 ) << cdtFiles[ i ].GetExt().Lower() << endl <<
				attachment::desc << endl <<
				cdtFiles[ i ].GetFullName() << endl <<
				attachment::mime << endl <<
				mime::octet_stream << endl <<
				attachment::file << endl <<
				mkvmerge_escape( cdtFiles[ i ] ) << endl;
			}

			break;
		}

		default:
		{
			*m_os << "# covers (" << nAttachments << ')' << endl;
			
			for ( size_t i = 0; i < nAttachments; i++ )
			{
				*m_os <<
				attachment::name << endl <<
				wxString::Format( "cdtext%02d.", i + 1 ) << cdtFiles[ i ].GetExt().Lower() << endl <<
				attachment::desc << endl <<
				cdtFiles[ i ].GetFullName() << endl <<
				attachment::mime << endl <<
				mime::octet_stream << endl <<
				attachment::file << endl <<
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
			*m_os << "# no EAC log(s)" << endl;
			return;
		}

		case 1:
		{
			*m_os <<
			"# EAC log" << endl <<
			attachment::name << endl <<
			"eac.log" << endl <<
			attachment::desc << endl <<
			logFiles[ 0 ].GetFullName() << endl <<
			attachment::mime << endl <<
			mime::text_plain << endl <<
			attachment::file << endl <<
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
			*m_os << "# EAC logs (" << nAttachments << ')' << endl;

			for ( size_t i = 0; i < nAttachments; i++ )
			{
				*m_os <<
				attachment::name << endl <<
				wxString::Format( "eac%d.log", i + 1 ) << endl <<
				attachment::desc << endl <<
				logFiles[ i ].GetFullName() << endl <<
				attachment::mime << endl <<
				mime::text_plain << endl <<
				attachment::file << endl <<
				mkvmerge_escape( logFiles[ i ] ) << endl;
			}

			break;
		}

		default:
		{
			*m_os << "# EAC logs (" << nAttachments << ')' << endl;
			for ( size_t i = 0; i < nAttachments; i++ )
			{
				*m_os <<
				attachment::name << endl <<
				wxString::Format( "eac%02d.log", i + 1 ) << endl <<
				attachment::desc << endl <<
				logFiles[ i ].GetFullName() << endl <<
				attachment::mime << endl <<
				mime::text_plain << endl <<
				attachment::file << endl <<
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
			*m_os << "# no source cuesheets" << endl;
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
				"# cuesheet" << endl <<
				attachment::name << endl <<
				"cuesheet.cue" << endl <<
				attachment::desc << endl <<
				cnt.GetSource().GetFileName().GetFullName() << endl <<
				attachment::mime << endl <<
				mime::text_plain << endl <<
				attachment::file << endl <<
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
			*m_os << "# cuesheets (" << nSourceContents << ')' << endl;
			for ( size_t i = 0, nCounter = 1; i < nContents; i++ )
			{
				const wxCueSheetContent cnt = contents[ i ];

				if ( !( cnt.HasSource() && !cnt.IsEmbedded() ) )
				{
					continue;
				}

				*m_os <<
				attachment::name << endl <<
				wxString::Format( "cuesheet%d.cue", nCounter ) << endl <<
				attachment::desc << endl <<
				cnt.GetSource().GetFileName().GetFullName() << endl <<
				attachment::mime << endl <<
				mime::text_plain << endl <<
				attachment::file << endl <<
				mkvmerge_escape( cnt.GetSource() ) << endl;

				nCounter += 1;
			}

			break;
		}

		default:
		{
			*m_os << "# cuesheets (" << nSourceContents << ')' << endl;
			for ( size_t i = 0, nCounter = 1; i < nContents; i++ )
			{
				const wxCueSheetContent cnt = contents[ i ];

				if ( !( cnt.HasSource() && !cnt.IsEmbedded() ) )
				{
					continue;
				}

				*m_os <<
				attachment::name << endl <<
				wxString::Format( "cuesheet%02d.cue", nCounter ) << endl <<
				attachment::desc<< endl <<
				cnt.GetSource().GetFileName().GetFullName() << endl <<
				attachment::mime << endl <<
				mime::text_plain << endl <<
				attachment::file << endl <<
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
			*m_os << "# no decoded cuesheets" << endl;
			return;
		}

		case 1:
		{
			if ( save_cuesheet( inputFile, "embedded", contents[ 0 ].GetValue(), cueSheetPath ) )
			{
				*m_os <<
				"# cuesheet" << endl <<
				attachment::name << endl <<
				"cuesheet.cue" << endl <<
				attachment::mime << endl <<
				mime::text_plain << endl <<
				attachment::file << endl <<
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
			*m_os << "# cuesheets" << endl;
			for ( size_t i = 0; i < nContents; i++ )
			{
				if ( save_cuesheet( inputFile, wxString::Format( "embedded%d", i + 1 ), contents[ i ].GetValue(), cueSheetPath ) )
				{
					*m_os <<
					attachment::name << endl <<
					wxString::Format( "cuesheet%d.cue", i + 1 ) << endl <<
					attachment::mime << endl <<
					mime::text_plain << endl <<
					attachment::file << endl <<
					mkvmerge_escape( cueSheetPath ) << endl;
				}
			}

			break;
		}

		default:
		{
			*m_os << "# cuesheets" << endl;
			for ( size_t i = 0; i < nContents; i++ )
			{
				if ( save_cuesheet( inputFile, wxString::Format( "embedded%02d", i + 1 ), contents[ i ].GetValue(), cueSheetPath ) )
				{
					*m_os <<
					attachment::name << endl <<
					wxString::Format( "cuesheet%d.cue", i + 1 ) << endl <<
					attachment::mime << endl <<
					mime::text_plain << endl <<
					attachment::file << endl <<
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

	if ( render_cuesheet( inputFile, "rendered", cueSheet, cueSheetPath ) )
	{
		*m_os <<
		"# cuesheet" << endl <<
		attachment::name << endl <<
		"cuesheet.cue" << endl <<
		attachment::desc << endl <<
		"Rendered cue sheet" << endl <<
		attachment::mime << endl <<
		mime::text_plain << endl <<
		attachment::file << endl <<
		mkvmerge_escape( cueSheetPath ) << endl;
	}
}

wxString wxMkvmergeOptsRenderer::get_mapping_str( const wxCueSheet& cueSheet )
{
	wxASSERT( cueSheet.GetDataFilesCount() > 1u );
	wxString sRes;

	for ( size_t i = 1u, nCount = cueSheet.GetDataFilesCount(); i < nCount; i++ )
	{
		sRes += wxString::Format( "%d:0:%d:0,", i, i - 1 );
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
	"# This file was created by " << wxGetApp().GetAppDisplayName() << endl <<
	"# Application version: " << wxGetApp().APP_VERSION << endl <<
	"# Application vendor: " << wxGetApp().GetVendorDisplayName() << endl <<
	"# Creation time: " << dtNow.FormatISODate() << ' ' << dtNow.FormatISOTime() << endl <<
    '#' << endl <<
	"# mkvmerge's UI language" << endl <<
	"--ui-language" << endl <<
	"en" << endl <<
	"# Output file" << endl <<
	"-o" << endl << GetEscapedFile( matroskaFile ) << endl <<
	"--language" << endl << "0:" << m_cfg.GetLang() << endl <<
	"--default-track" << endl << "0:yes" << endl <<
	"--track-name" << endl << "0:"  << cueSheet.Format( m_cfg.GetMatroskaNameFormat() ) << endl <<
	"# Input file(s)" << endl;

	// tracks
	const wxArrayDataFile& dataFiles = cueSheet.GetDataFiles();
	bool				   bFirst	 = true;

	for ( size_t nTracks = dataFiles.GetCount(), i = 0; i < nTracks; i++ )
	{
		*m_os <<
		"-a" << endl << "0" << endl <<
		"-D" << endl <<
		"-S" << endl <<
		"-T" << endl <<
		"--no-global-tags" << endl <<
		"--no-chapters" << endl;

		const wxDataFile& dataFile = dataFiles[ i ];
		wxASSERT( dataFile.HasRealFileName() );

		if ( !bFirst )
		{
			*m_os <<
			'+' << endl <<
			mkvmerge_escape( dataFile ) << endl;
		}
		else
		{
			*m_os <<
			'=' << endl <<
			mkvmerge_escape( dataFile ) << endl;

			bFirst = false;
		}
	}

	// cover - must be a first attachment
	if ( m_cfg.AttachCover() )
	{
        wxArrayCoverFile covers;
        cueSheet.GetSortedCovers( covers );
		write_cover_attachments( inputFile, covers );
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
	"# General options" << endl <<
	"--default-language" << endl << m_cfg.GetLang() << endl <<
	"--title" << endl << cueSheet.Format( m_cfg.GetMatroskaNameFormat() ) << endl <<
	"--chapters" << endl << GetEscapedFile( outputFile ) << endl;

	if ( m_cfg.GenerateTags() )
	{
		*m_os << "--global-tags" << endl << GetEscapedFile( tagsFile ) << endl;
	}

	if ( dataFiles.GetCount() > 1u )
	{
		*m_os << "--append-to" << endl << get_mapping_str( cueSheet ) << endl;
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

