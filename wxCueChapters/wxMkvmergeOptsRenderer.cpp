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
	const char mime::text_plain[]   = "text/plain";

	class file_desc
	{
		public:

			static size_t tc2padding( size_t count )
			{
				if ( count > 1000 ) { return 4; }
				else if ( count > 100 ) { return 3; }
				else if ( count > 10 ) { return 2; }
				else { return 1; }
			}

			file_desc() :
				m_showFileNo( false ), m_nPadding( -1 )
			{}

			file_desc( const char* filePrefix, size_t fileNo, const wxString& fileExt, size_t nTotalCount ) :
				m_filePrefix( filePrefix ), m_fileNo( fileNo ), m_fileExt( fileExt ), m_nPadding( tc2padding( nTotalCount ) ), m_showFileNo( true )
			{}

			file_desc( const char* filePrefix, size_t fileNo, const wxFileName& fn, size_t nTotalCount ) :
				m_filePrefix( filePrefix ), m_fileNo( fileNo ), m_fileExt( fn.GetExt().Lower() ), m_nPadding( tc2padding( nTotalCount ) ), m_showFileNo( true )
			{}

			file_desc( const char* filePrefix, const wxString& fileExt, size_t nTotalCount ) :
				m_filePrefix( filePrefix ), m_showFileNo( false ), m_fileExt( fileExt ), m_nPadding( tc2padding( nTotalCount ) )
			{}

			file_desc( const char* filePrefix, const wxFileName& fn, size_t nTotalCount ) :
				m_filePrefix( filePrefix ), m_showFileNo( false ), m_fileExt( fn.GetExt().Lower() ), m_nPadding( tc2padding( nTotalCount ) )
			{}

			file_desc( const file_desc& fd ) :
				m_filePrefix( fd.m_filePrefix ), m_fileNo( fd.m_fileNo ), m_showFileNo( fd.m_showFileNo ), m_fileExt( fd.m_fileExt ), m_nPadding( fd.m_nPadding )
			{}

		protected:

			wxString getPrefix() const
			{
				return m_filePrefix;
			}

			wxString getFileNo() const
			{
				if ( !m_showFileNo )
				{
					return wxEmptyString;
				}

				if ( m_nPadding == 0 )
				{
					wxString s;
					s.Printf( "%" wxSizeTFmtSpec "u", m_fileNo );
					return s;
				}
				else
				{
					wxString s;
					s.Printf( "%" wxSizeTFmtSpec "u", m_fileNo );

					size_t nLen = s.Length();

					if ( nLen < m_nPadding )
					{
						return s.Prepend( wxString( m_nPadding - nLen, '0' ) );
					}
					else
					{
						return s;
					}
				}
			}

			wxString getExt() const
			{
				return wxString::Format( ".%s", m_fileExt );
			}

		public:

			wxString ToString() const
			{
				return getPrefix() + getFileNo() + getExt();
			}

		protected:

			const char* m_filePrefix;
			size_t m_fileNo;
			bool m_showFileNo;
			wxString m_fileExt;
			size_t m_nPadding;
	};

	struct fprefix
	{
		static const char cover[];
		static const char cdtext[];
		static const char eac[];
		static const char accurip[];
		static const char cuesheet[];
	};

	const char fprefix::cover[]    = "cover";
	const char fprefix::cdtext[]   = "cdtext";
	const char fprefix::eac[]      = "eac";
	const char fprefix::accurip[]  = "accurip";
	const char fprefix::cuesheet[] = "cuesheet";
}

// ===============================================================================

wxMkvmergeOptsRenderer::wxMkvmergeOptsRenderer( const wxConfiguration& cfg ) :
	wxPrimitiveRenderer( cfg )
{}

const wxFileName& wxMkvmergeOptsRenderer::GetMkvmergeOptsFile() const
{
	return m_matroskaOptsFile;
}

void wxMkvmergeOptsRenderer::append_cover( wxArrayString& sa, const wxCoverFile& coverFile )
{
	wxString stype;

	wxCoverFile::GetStrFromType( coverFile.GetType(), stype );

	wxString sDescription;

	if ( coverFile.HasDescription() )
	{
		sDescription = coverFile.GetDescription();
		sDescription << " [" << stype << ']';
	}
	else
	{
		sDescription = stype;
	}

	sa.Add( attachment::desc );
	sa.Add( sDescription );

	if ( coverFile.HasMimeType() )
	{
		sa.Add( attachment::mime );
		sa.Add( coverFile.GetMimeType() );
	}

	sa.Add( attachment::file );
	sa.Add( GetEscapedFile( coverFile.GetFileName() ) );
}

wxString wxMkvmergeOptsRenderer::GetEscapedFile( const wxFileName& fn )
{
	if ( m_cfg.UseFullPaths() )
	{
		return fn.GetFullPath();
	}
	else
	{
		return fn.GetFullName();
	}
}

bool wxMkvmergeOptsRenderer::save_cover( const wxInputFile& inputFile, wxCoverFile& cover ) const
{
	if ( cover.HasFileName() ) { return true; }

	wxASSERT( cover.HasData() );

	wxFileName fn;

	if ( m_cfg.GetOutputFile( inputFile, "img", cover.GetExt(), fn ) )
	{
		return cover.Save( fn );
	}
	else
	{
		return false;
	}
}

void wxMkvmergeOptsRenderer::write_cover_attachments( wxArrayString& as, const wxInputFile& inputFile, const wxArrayCoverFile& coverFiles )
{
	size_t nAttachments = coverFiles.GetCount();

	switch ( nAttachments )
	{
		case 0:
		{
			break;
		}

		case 1:
		{
			if ( save_cover( inputFile, coverFiles[ 0 ] ) )
			{
				as.Add( attachment::name );
				as.Add( file_desc( fprefix::cover, coverFiles[ 0 ].GetFileName(), nAttachments ).ToString() );
				append_cover( as, coverFiles[ 0 ] );
			}
			break;
		}

		default:
		{
			if ( save_cover( inputFile, coverFiles[ 0 ] ) )
			{
				as.Add( attachment::name );
				as.Add( file_desc( fprefix::cover, coverFiles[ 0 ].GetFileName(), nAttachments ).ToString() );
				append_cover( as, coverFiles[ 0 ] );
			}

			// rest
			for ( size_t i = 1; i < nAttachments; ++i )
			{
				if ( save_cover( inputFile, coverFiles[ i ] ) )
				{
					as.Add( attachment::name );
					as.Add( file_desc( fprefix::cover, i + 1, coverFiles[ 0 ].GetFileName(), nAttachments ).ToString() );
					append_cover( as, coverFiles[ i ] );
				}
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_cdtextfiles_attachments( wxArrayString& sa, const wxArrayFileName& cdtFiles )
{
	size_t nAttachments = cdtFiles.GetCount();

	switch ( nAttachments )
	{
		case 0:
		{
			break;
		}

		case 1:
		{
			sa.Add( attachment::name );
			sa.Add( file_desc( fprefix::cdtext, cdtFiles[ 0 ], nAttachments ).ToString() );
			sa.Add( attachment::desc );
			sa.Add( cdtFiles[ 0 ].GetFullName() );
			sa.Add( attachment::mime );
			sa.Add( mime::octet_stream );
			sa.Add( attachment::file );
			sa.Add( cdtFiles[ 0 ].GetFullPath() );
			break;
		}

		default:
		{
			for ( size_t i = 0; i < nAttachments; ++i )
			{
				sa.Add( attachment::name );
				sa.Add( file_desc( fprefix::cdtext, i + 1, cdtFiles[ i ], nAttachments ).ToString() );
				sa.Add( attachment::desc );
				sa.Add( cdtFiles[ i ].GetFullName() );
				sa.Add( attachment::mime );
				sa.Add( mime::octet_stream );
				sa.Add( attachment::file );
				sa.Add( cdtFiles[ i ].GetFullPath() );
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_log_attachments( wxArrayString& sa, const wxArrayFileName& logFiles )
{
	size_t nAttachments = logFiles.GetCount();

	switch ( nAttachments )
	{
		case 0:
		{
			break;
		}

		case 1:
		{
			sa.Add( attachment::name );
			sa.Add( file_desc( fprefix::eac, logFiles[ 0 ], nAttachments ).ToString() );
			sa.Add( attachment::desc );
			sa.Add( logFiles[ 0 ].GetFullName() );
			sa.Add( attachment::mime );
			sa.Add( mime::text_plain );
			sa.Add( attachment::file );
			sa.Add( logFiles[ 0 ].GetFullPath() );
			break;
		}

		default:
		{
			for ( size_t i = 0; i < nAttachments; ++i )
			{
				sa.Add( attachment::name );
				sa.Add( file_desc( fprefix::eac, i + 1, logFiles[ i ], nAttachments ).ToString() );
				sa.Add( attachment::desc );
				sa.Add( logFiles[ i ].GetFullName() );
				sa.Add( attachment::mime );
				sa.Add( mime::text_plain );
				sa.Add( attachment::file );
				sa.Add( logFiles[ i ].GetFullPath() );
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_accurip_log_attachments( wxArrayString& sa, const wxArrayFileName& logFiles )
{
	size_t         nAttachments = logFiles.GetCount();
	const wxString logExt( "log" );

	switch ( nAttachments )
	{
		case 0:
		{
			break;
		}

		case 1:
		{
			sa.Add( attachment::name );
			sa.Add( file_desc( fprefix::accurip, logExt, nAttachments ).ToString() );
			sa.Add( attachment::desc );
			sa.Add( logFiles[ 0 ].GetFullName() );
			sa.Add( attachment::mime );
			sa.Add( mime::text_plain );
			sa.Add( attachment::file );
			sa.Add( logFiles[ 0 ].GetFullPath() );
			break;
		}

		default:
		{
			for ( size_t i = 0; i < nAttachments; ++i )
			{
				sa.Add( attachment::name );
				sa.Add( file_desc( fprefix::accurip, i + 1, logExt, nAttachments ).ToString() );
				sa.Add( attachment::desc );
				sa.Add( logFiles[ i ].GetFullName() );
				sa.Add( attachment::mime );
				sa.Add( mime::text_plain );
				sa.Add( attachment::file );
				sa.Add( logFiles[ i ].GetFullPath() );
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_eac_attachments(
		wxArrayString& sa, const wxInputFile& inputFile, const wxCueSheet& cueSheet )
{
	switch ( m_cfg.GetCueSheetAttachMode() )
	{
		case wxConfiguration::CUESHEET_ATTACH_SOURCE:
		{
			write_source_eac_attachments( sa, inputFile, cueSheet.GetContents() );
			break;
		}

		case wxConfiguration::CUESHEET_ATTACH_DECODED:
		{
			write_decoded_eac_attachments( sa, inputFile, cueSheet.GetContents() );
			break;
		}

		case wxConfiguration::CUESHEET_ATTACH_RENDERED:
		{
			write_rendered_eac_attachments( sa, inputFile, cueSheet );
			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_source_eac_attachments(
		wxArrayString& sa, const wxInputFile& WXUNUSED( inputFile ), const wxArrayCueSheetContent& contents )
{
	size_t nContents       = contents.GetCount();
	size_t nSourceContents = 0;

	for ( size_t i = 0; i < nContents; ++i )
	{
		const wxCueSheetContent& cnt = contents[ i ];

		if ( cnt.HasSource() && !cnt.IsEmbedded() )
		{
			nSourceContents += 1;
		}
	}

	switch ( nSourceContents )
	{
		case 0:
		{
			break;
		}

		case 1:
		{
			for ( size_t i = 0; i < nContents; ++i )
			{
				const wxCueSheetContent& cnt = contents[ i ];

				if ( !( cnt.HasSource() && !cnt.IsEmbedded() ) )
				{
					continue;
				}

				sa.Add( attachment::name );
				sa.Add( file_desc( fprefix::cuesheet, cnt.GetSource().GetFileName(), nSourceContents ).ToString() );
				sa.Add( attachment::desc );
				sa.Add( cnt.GetSource().GetFileName().GetFullName() );
				sa.Add( attachment::mime );
				sa.Add( mime::text_plain );
				sa.Add( attachment::file );
				sa.Add( cnt.GetSource().GetRealFileName().GetFullPath() );
			}

			break;
		}

		default:
		{
			for ( size_t i = 0, nCounter = 1; i < nContents; ++i )
			{
				const wxCueSheetContent& cnt = contents[ i ];

				if ( !( cnt.HasSource() && !cnt.IsEmbedded() ) )
				{
					continue;
				}

				sa.Add( attachment::name );
				sa.Add( file_desc( fprefix::cuesheet, nCounter, cnt.GetSource().GetFileName(), nSourceContents ).ToString() );
				sa.Add( attachment::desc );
				sa.Add( cnt.GetSource().GetFileName().GetFullName() );
				sa.Add( attachment::mime );
				sa.Add( mime::text_plain );
				sa.Add( attachment::file );
				sa.Add( cnt.GetSource().GetRealFileName().GetFullPath() );

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

void wxMkvmergeOptsRenderer::write_decoded_eac_attachments( wxArrayString& sa,
		const wxInputFile& inputFile, const wxArrayCueSheetContent& contents )
{
	size_t     nContents = contents.GetCount();
	wxFileName cueSheetPath;

	switch ( nContents )
	{
		case 0:
		{
			break;
		}

		case 1:
		{
			if ( save_cuesheet( inputFile, "embedded", contents[ 0 ].GetValue(), cueSheetPath ) )
			{
				sa.Add( attachment::name );
				sa.Add( file_desc( fprefix::cuesheet, cueSheetPath, nContents ).ToString() );
				sa.Add( attachment::mime );
				sa.Add( mime::text_plain );
				sa.Add( attachment::file );
				sa.Add( cueSheetPath.GetFullPath() );
			}

			break;
		}

		default:
		{
			for ( size_t i = 0; i < nContents; ++i )
			{
				if ( save_cuesheet( inputFile, wxString::Format( "embedded%02" wxSizeTFmtSpec "u", i + 1 ), contents[ i ].GetValue(), cueSheetPath ) )
				{
					sa.Add( attachment::name );
					sa.Add( file_desc( fprefix::cuesheet, i + 1, cueSheetPath, nContents ).ToString() );
					sa.Add( attachment::mime );
					sa.Add( mime::text_plain );
					sa.Add( attachment::file );
					sa.Add( cueSheetPath.GetFullPath() );
				}
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_rendered_eac_attachments( wxArrayString& sa,
		const wxInputFile& inputFile, const wxCueSheet& cueSheet )
{
	wxFileName cueSheetPath;

	if ( render_cuesheet( inputFile, "rendered", cueSheet, cueSheetPath ) )
	{
		sa.Add( attachment::name );
		sa.Add( file_desc( fprefix::cuesheet, cueSheetPath, 1 ).ToString() );
		sa.Add( attachment::desc );
		sa.Add( "Rendered cue sheet" );
		sa.Add( attachment::mime );
		sa.Add( mime::text_plain );
		sa.Add( attachment::file );
		sa.Add( cueSheetPath.GetFullPath() );
	}
}

wxString wxMkvmergeOptsRenderer::get_mapping_str( const wxCueSheet& cueSheet )
{
	wxASSERT( cueSheet.GetDataFilesCount() > 1u );
	wxString sRes;

	for ( size_t i = 1u, nCount = cueSheet.GetDataFilesCount(); i < nCount; ++i )
	{
		sRes += wxString::Format( "%" wxSizeTFmtSpec "u:0:%" wxSizeTFmtSpec "u:0,", i, i - 1 );
	}

	return sRes.RemoveLast();
}

namespace
{
	wxString json_escape( const wxString& s )
	{
		wxString res( s );

		res.Replace( '\t', "\\t" );
		res.Replace( '\\', "\\\\" );
		res.Replace( '\"', "\\\"" );
		return res;
	}

	void dump_string_array( wxTextOutputStream& str, const wxArrayString& sa )
	{
		size_t nSize = sa.GetCount();

		for ( size_t i = 0; i < nSize; ++i )
		{
			str << '\"' << json_escape( sa[ i ] ) << '\"';

			if ( i < ( nSize - 1 ) ) { str << ','; }
			str << endl;
		}
	}
}

void wxMkvmergeOptsRenderer::RenderDisc( const wxInputFile& inputFile,
		const wxCueSheet& cueSheet )
{
	wxFileName    outputFile, tagsFile, matroskaFile;
	wxArrayString sa;

	m_cfg.GetOutputFile( inputFile, outputFile, tagsFile );
	m_cfg.GetOutputMatroskaFile( inputFile, matroskaFile, m_matroskaOptsFile );

	// pre
	sa.Add( "--ui-language" );
	sa.Add( "en" );
	sa.Add( "--disable-track-statistics-tags" );
	sa.Add( "-o" );
	sa.Add( GetEscapedFile( matroskaFile ) );
	sa.Add( "--language" );
	sa.Add( wxString::Format( "0:%s", m_cfg.GetLang() ) );
	sa.Add( "--default-track" );
	sa.Add( "0:yes" );
	sa.Add( "--track-name" );
	sa.Add( wxString::Format( "0:%s", cueSheet.Format( m_cfg.GetTagSources(), m_cfg.GetMatroskaNameFormat() ) ) );

	// tracks
	const wxArrayDataFile& dataFiles = cueSheet.GetDataFiles();
	bool                   bFirst    = true;

	for ( size_t nTracks = dataFiles.GetCount(), i = 0; i < nTracks; ++i )
	{
		sa.Add( "-a" );
		sa.Add( '0' );
		sa.Add( "-D" );
		sa.Add( "-S" );
		sa.Add( "-T" );
		sa.Add( "--no-global-tags" );
		sa.Add( "--no-chapters" );

		const wxDataFile& dataFile = dataFiles[ i ];
		wxASSERT( dataFile.HasRealFileName() );

		if ( !bFirst )
		{
			sa.Add( wxString::Format( "+%s", dataFile.GetRealFileName().GetFullPath() ) );
		}
		else
		{
			sa.Add( wxString::Format( "=%s", dataFile.GetRealFileName().GetFullPath() ) );
			bFirst = false;
		}
	}

	// cover - must be a first attachment
	if ( m_cfg.AttachCover() )
	{
		wxArrayCoverFile covers;
		cueSheet.GetSortedCovers( covers, m_cfg.GetImageHandler(), m_cfg.GetJpegImageQuality() );
		write_cover_attachments( sa, inputFile, covers );
	}

	write_cdtextfiles_attachments( sa, cueSheet.GetCdTextFiles() );

	// log
	if ( m_cfg.AttachEacLog() )
	{
		write_log_attachments( sa, cueSheet.GetLogs() );
	}

	write_eac_attachments( sa, inputFile, cueSheet );

	// accurip
	if ( m_cfg.AttachAccurateRipLog() )
	{
		write_accurip_log_attachments( sa, cueSheet.GetAccurateRipLogs() );
	}

	// post
	sa.Add( "--default-language" );
	sa.Add( m_cfg.GetLang() );
	sa.Add( "--title" );
	sa.Add( cueSheet.Format( m_cfg.GetTagSources(), m_cfg.GetMatroskaNameFormat() ) );
	sa.Add( "--chapters" );
	sa.Add( GetEscapedFile( outputFile ) );

	if ( m_cfg.GenerateTags() )
	{
		sa.Add( "--global-tags" );
		sa.Add( GetEscapedFile( tagsFile ) );
	}

	if ( dataFiles.GetCount() > 1u )
	{
		sa.Add( "--append-to" );
		sa.Add( get_mapping_str( cueSheet ) );
	}

	*m_os << '[' << endl;
	dump_string_array( *m_os, sa );
	*m_os << ']' << endl;
}

bool wxMkvmergeOptsRenderer::Save()
{
	wxASSERT( m_cfg.GenerateMkvmergeOpts() );
	wxFileOutputStream os( m_matroskaOptsFile.GetFullPath() );

	if ( os.IsOk() )
	{
		wxLogInfo( _( "Creating mkvmerge options file \u201C%s\u201D" ), m_matroskaOptsFile.GetFullName() );
		wxTextOutputStream stream( os, wxEOL_NATIVE, wxConvUTF8 );
		m_os.SaveTo( stream );
		return true;
	}
	else
	{
		wxLogError( _( "Fail to save options to \u201C%s\u201D" ), m_matroskaOptsFile.GetFullName() );
		return false;
	}
}

