/*
 * wxMkvmergeOptsRenderer.cpp
 */

#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxCueFile/wxStringProcessor.h>
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
				if ( count > 1000 ) return 4;
				else if ( count > 100 ) return 3;
				else if ( count > 10 ) return 2;
				else return 1;
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
				if ( !m_showFileNo ) return wxEmptyString;

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

					if ( nLen < m_nPadding ) return s.Prepend( wxString( m_nPadding - nLen, '0' ) );
					else return s;
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

	template<size_t N>
	void add_string(wxMkvmergeOptsRenderer::json& j, const char(&s)[N])
	{
		j.push_back(s);
	}

	void add_string(wxMkvmergeOptsRenderer::json& j, const wxString& s)
	{
		j.push_back(s.utf8_string());
	}
}

// ===============================================================================

wxMkvmergeOptsRenderer::wxMkvmergeOptsRenderer( const wxConfiguration& cfg ) :
	wxPrimitiveRenderer( cfg )
{}

const wxFileName& wxMkvmergeOptsRenderer::GetMkvmergeOptsFile() const
{
	return m_matroskaOptsFile;
}

void wxMkvmergeOptsRenderer::append_cover(wxMkvmergeOptsRenderer::json& opts, const wxCoverFile& coverFile )
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

	add_string( opts,  attachment::desc );
	add_string( opts,  sDescription );

	if ( coverFile.HasMimeType() )
	{
		add_string( opts,  attachment::mime );
		add_string( opts,  coverFile.GetMimeType() );
	}

	add_string( opts,  attachment::file );
	add_string( opts,  GetEscapedFile( coverFile.GetFileName() ) );
}

wxString wxMkvmergeOptsRenderer::GetEscapedFile( const wxFileName& fn )
{
	if ( m_cfg.UseFullPaths() ) return fn.GetFullPath();
	else return fn.GetFullName();
}

bool wxMkvmergeOptsRenderer::save_cover( const wxInputFile& inputFile, wxCoverFile& cover ) const
{
	if ( cover.HasFileName() ) return true;

	wxASSERT( cover.HasData() );

	wxFileName fn;

	if ( m_cfg.GetOutputFile( inputFile, "img", cover.GetExt(), fn ) ) return cover.Save( fn );
	else return false;
}

void wxMkvmergeOptsRenderer::write_cover_attachments(wxMkvmergeOptsRenderer::json& opts, const wxInputFile& inputFile, const wxArrayCoverFile& coverFiles )
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
				add_string( opts,  attachment::name );
				add_string( opts,  file_desc( fprefix::cover, coverFiles[ 0 ].GetFileName(), nAttachments ).ToString() );
				append_cover( opts, coverFiles[ 0 ] );
			}
			break;
		}

		default:
		{
			if ( save_cover( inputFile, coverFiles[ 0 ] ) )
			{
				add_string( opts,  attachment::name );
				add_string( opts,  file_desc( fprefix::cover, coverFiles[ 0 ].GetFileName(), nAttachments ).ToString() );
				append_cover( opts, coverFiles[ 0 ] );
			}

			// rest
			for ( size_t i = 1; i < nAttachments; ++i )
			{
				if ( save_cover( inputFile, coverFiles[ i ] ) )
				{
					add_string( opts,  attachment::name );
					add_string( opts,  file_desc( fprefix::cover, i + 1, coverFiles[ 0 ].GetFileName(), nAttachments ).ToString() );
					append_cover( opts, coverFiles[ i ] );
				}
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_cdtextfiles_attachments(wxMkvmergeOptsRenderer::json& opts, const wxArrayFileName& cdtFiles )
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
			add_string( opts,  attachment::name );
			add_string( opts,  file_desc( fprefix::cdtext, cdtFiles[ 0 ], nAttachments ).ToString() );
			add_string( opts,  attachment::desc );
			add_string( opts,  cdtFiles[ 0 ].GetFullName() );
			add_string( opts,  attachment::mime );
			add_string( opts,  mime::octet_stream );
			add_string( opts,  attachment::file );
			add_string( opts,  cdtFiles[ 0 ].GetFullPath() );
			break;
		}

		default:
		{
			for ( size_t i = 0; i < nAttachments; ++i )
			{
				add_string( opts,  attachment::name );
				add_string( opts,  file_desc( fprefix::cdtext, i + 1, cdtFiles[ i ], nAttachments ).ToString() );
				add_string( opts,  attachment::desc );
				add_string( opts,  cdtFiles[ i ].GetFullName() );
				add_string( opts,  attachment::mime );
				add_string( opts,  mime::octet_stream );
				add_string( opts,  attachment::file );
				add_string( opts,  cdtFiles[ i ].GetFullPath() );
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_log_attachments(wxMkvmergeOptsRenderer::json& opts, const wxArrayFileName& logFiles )
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
			add_string( opts,  attachment::name );
			add_string( opts,  file_desc( fprefix::eac, logFiles[ 0 ], nAttachments ).ToString() );
			add_string( opts,  attachment::desc );
			add_string( opts,  logFiles[ 0 ].GetFullName() );
			add_string( opts,  attachment::mime );
			add_string( opts,  mime::text_plain );
			add_string( opts,  attachment::file );
			add_string( opts,  logFiles[ 0 ].GetFullPath() );
			break;
		}

		default:
		{
			for ( size_t i = 0; i < nAttachments; ++i )
			{
				add_string( opts,  attachment::name );
				add_string( opts,  file_desc( fprefix::eac, i + 1, logFiles[ i ], nAttachments ).ToString() );
				add_string( opts,  attachment::desc );
				add_string( opts,  logFiles[ i ].GetFullName() );
				add_string( opts,  attachment::mime );
				add_string( opts,  mime::text_plain );
				add_string( opts,  attachment::file );
				add_string( opts,  logFiles[ i ].GetFullPath() );
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_accurip_log_attachments(wxMkvmergeOptsRenderer::json& opts, const wxArrayFileName& logFiles )
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
			add_string( opts,  attachment::name );
			add_string( opts,  file_desc( fprefix::accurip, logExt, nAttachments ).ToString() );
			add_string( opts,  attachment::desc );
			add_string( opts,  logFiles[ 0 ].GetFullName() );
			add_string( opts,  attachment::mime );
			add_string( opts,  mime::text_plain );
			add_string( opts,  attachment::file );
			add_string( opts,  logFiles[ 0 ].GetFullPath() );
			break;
		}

		default:
		{
			for ( size_t i = 0; i < nAttachments; ++i )
			{
				add_string( opts,  attachment::name );
				add_string( opts,  file_desc( fprefix::accurip, i + 1, logExt, nAttachments ).ToString() );
				add_string( opts,  attachment::desc );
				add_string( opts,  logFiles[ i ].GetFullName() );
				add_string( opts,  attachment::mime );
				add_string( opts,  mime::text_plain );
				add_string( opts,  attachment::file );
				add_string( opts,  logFiles[ i ].GetFullPath() );
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_eac_attachments(
	wxMkvmergeOptsRenderer::json& opts, const wxInputFile& inputFile, const wxCueSheet& cueSheet )
{
	switch ( m_cfg.GetCueSheetAttachMode() )
	{
		case wxConfiguration::CUESHEET_ATTACH_SOURCE:
		{
			write_source_eac_attachments( opts, inputFile, cueSheet.GetContents() );
			break;
		}

		case wxConfiguration::CUESHEET_ATTACH_DECODED:
		{
			write_decoded_eac_attachments( opts, inputFile, cueSheet.GetContents() );
			break;
		}

		case wxConfiguration::CUESHEET_ATTACH_RENDERED:
		{
			write_rendered_eac_attachments( opts, inputFile, cueSheet );
			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_source_eac_attachments(
	wxMkvmergeOptsRenderer::json& opts, const wxInputFile& WXUNUSED( inputFile ), const wxArrayCueSheetContent& contents )
{
	size_t nContents       = contents.GetCount();
	size_t nSourceContents = 0;

	for ( size_t i = 0; i < nContents; ++i )
	{
		const wxCueSheetContent& cnt = contents[ i ];

		if ( cnt.HasSource() && !cnt.IsEmbedded() ) nSourceContents += 1;
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

				if ( !( cnt.HasSource() && !cnt.IsEmbedded() ) ) continue;

				add_string( opts,  attachment::name );
				add_string( opts,  file_desc( fprefix::cuesheet, cnt.GetSource().GetFileName(), nSourceContents ).ToString() );
				add_string( opts,  attachment::desc );
				add_string( opts,  cnt.GetSource().GetFileName().GetFullName() );
				add_string( opts,  attachment::mime );
				add_string( opts,  mime::text_plain );
				add_string( opts,  attachment::file );
				add_string( opts,  cnt.GetSource().GetRealFileName().GetFullPath() );
			}

			break;
		}

		default:
		{
			for ( size_t i = 0, nCounter = 1; i < nContents; ++i )
			{
				const wxCueSheetContent& cnt = contents[ i ];

				if ( !( cnt.HasSource() && !cnt.IsEmbedded() ) ) continue;

				add_string( opts,  attachment::name );
				add_string( opts,  file_desc( fprefix::cuesheet, nCounter, cnt.GetSource().GetFileName(), nSourceContents ).ToString() );
				add_string( opts,  attachment::desc );
				add_string( opts,  cnt.GetSource().GetFileName().GetFullName() );
				add_string( opts,  attachment::mime );
				add_string( opts,  mime::text_plain );
				add_string( opts,  attachment::file );
				add_string( opts,  cnt.GetSource().GetRealFileName().GetFullPath() );

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
	if ( !m_cfg.GetOutputCueSheetFile( inputFile, sPostfix, cueSheet ) ) return false;

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

	if ( !wxTextCueSheetRenderer::ToString( *tos, cueSheet ) ) return false;

	return save_cuesheet( inputFile, sPostfix, tos.GetString(), fn );
}

void wxMkvmergeOptsRenderer::write_decoded_eac_attachments(wxMkvmergeOptsRenderer::json& opts,
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
				add_string( opts,  attachment::name );
				add_string( opts,  file_desc( fprefix::cuesheet, cueSheetPath, nContents ).ToString() );
				add_string( opts,  attachment::mime );
				add_string( opts,  mime::text_plain );
				add_string( opts,  attachment::file );
				add_string( opts,  cueSheetPath.GetFullPath() );
			}

			break;
		}

		default:
		{
			for ( size_t i = 0; i < nContents; ++i )
			{
				if ( save_cuesheet( inputFile, wxString::Format( "embedded%02" wxSizeTFmtSpec "u", i + 1 ), contents[ i ].GetValue(), cueSheetPath ) )
				{
					add_string( opts,  attachment::name );
					add_string( opts,  file_desc( fprefix::cuesheet, i + 1, cueSheetPath, nContents ).ToString() );
					add_string( opts,  attachment::mime );
					add_string( opts,  mime::text_plain );
					add_string( opts,  attachment::file );
					add_string( opts,  cueSheetPath.GetFullPath() );
				}
			}

			break;
		}
	}
}

void wxMkvmergeOptsRenderer::write_rendered_eac_attachments(wxMkvmergeOptsRenderer::json& opts,
		const wxInputFile& inputFile, const wxCueSheet& cueSheet )
{
	wxFileName cueSheetPath;

	if ( render_cuesheet( inputFile, "rendered", cueSheet, cueSheetPath ) )
	{
		add_string( opts,  attachment::name );
		add_string( opts,  file_desc( fprefix::cuesheet, cueSheetPath, 1 ).ToString() );
		add_string( opts,  attachment::desc );
		add_string( opts,  "Rendered cue sheet" );
		add_string( opts,  attachment::mime );
		add_string( opts,  mime::text_plain );
		add_string( opts,  attachment::file );
		add_string( opts,  cueSheetPath.GetFullPath() );
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

wxString wxMkvmergeOptsRenderer::get_track_name( const wxCueSheet& cueSheet ) const
{
	wxScopedPtr<wxStringProcessor> stringProcessor(m_cfg.CreateStringProcessor());
	wxString matroskaNameFormat = m_cfg.GetMatroskaNameFormat();

	(*stringProcessor)(matroskaNameFormat);
	return cueSheet.Format( m_cfg.GetTagSources(), matroskaNameFormat );
}

void wxMkvmergeOptsRenderer::RenderDisc( const wxInputFile& inputFile,
		const wxCueSheet& cueSheet )
{
	wxFileName    outputFile, tagsFile, matroskaFile;
	json opts;

	m_cfg.GetOutputFile( inputFile, outputFile, tagsFile );
	m_cfg.GetOutputMatroskaFile( inputFile, matroskaFile, m_matroskaOptsFile );

	const wxString trackName = get_track_name( cueSheet );

	// pre
	add_string( opts,  "--deterministic" );
	add_string( opts,  wxString(wxMyApp::APP_NAME) );
	add_string( opts,  "--disable-track-statistics-tags" );
	add_string( opts,  "--no-date" );
	add_string( opts,  "-o" );
	add_string( opts,  GetEscapedFile( matroskaFile ) );
	add_string( opts,  "--language" );
	add_string( opts,  wxString::Format( "0:%s", m_cfg.GetLang() ) );
	add_string( opts,  "--default-track" );
	add_string( opts,  "0:yes" );
	add_string( opts,  "--track-name" );
	add_string( opts,  wxString::Format( "0:%s", trackName ) );

	// tracks
	const wxArrayDataFile& dataFiles = cueSheet.GetDataFiles();
	bool                   bFirst    = true;

	for ( size_t nTracks = dataFiles.GetCount(), i = 0; i < nTracks; ++i )
	{
		add_string( opts,  "-a" );
		add_string( opts,  '0' );
		add_string( opts,  "-D" );
		add_string( opts,  "-S" );
		add_string( opts,  "-B" );
		add_string( opts,  "-T" );
		add_string( opts,  "-M" );
		add_string( opts,  "--no-global-tags" );
		add_string( opts,  "--no-chapters" );

		const wxDataFile& dataFile = dataFiles[ i ];
		wxASSERT( dataFile.HasRealFileName() );

		if ( !bFirst )
		{
			add_string( opts,  wxString::Format( "+%s", dataFile.GetRealFileName().GetFullPath() ) );
		}
		else
		{
			add_string( opts,  wxString::Format( "=%s", dataFile.GetRealFileName().GetFullPath() ) );
			bFirst = false;
		}
	}

	// cover - must be a first attachment
	if ( m_cfg.AttachCover() )
	{
		wxArrayCoverFile covers;
		cueSheet.GetSortedCovers( covers, m_cfg.GetImageHandler(), m_cfg.GetJpegImageQuality() );
		write_cover_attachments( opts, inputFile, covers );
	}

	write_cdtextfiles_attachments( opts, cueSheet.GetCdTextFiles() );

	// log
	if ( m_cfg.AttachEacLog() ) write_log_attachments( opts, cueSheet.GetLogs() );

	write_eac_attachments( opts, inputFile, cueSheet );

	// accurip
	if ( m_cfg.AttachAccurateRipLog() ) write_accurip_log_attachments( opts, cueSheet.GetAccurateRipLogs() );

	// post
	if ( !m_cfg.IsUnkLang() )
	{
		add_string( opts,  "--default-language" );
		add_string( opts,  m_cfg.GetLang() );
	}
	add_string( opts,  "--title" );
	add_string( opts,  trackName );
	add_string( opts,  "--chapters" );
	add_string( opts,  GetEscapedFile( outputFile ) );

	if ( m_cfg.GenerateTags() )
	{
		add_string( opts,  "--global-tags" );
		add_string( opts,  GetEscapedFile( tagsFile ) );
	}

	if ( dataFiles.GetCount() > 1u )
	{
		add_string( opts,  "--append-to" );
		add_string( opts,  get_mapping_str( cueSheet ) );
	}

	const wxString j = wxString::FromUTF8Unchecked(opts.dump(2));
	*m_os << j << endl;
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

