/*
 * wxCueSheetReader.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxTextOutputStreamOnString.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheetContent.h>
#include <wxCueFile/wxCueSheetReader.h>
#include "wxFlacMetaDataReader.h"
#include "wxWavpackTagReader.h"
#include <wxEncodingDetection/wxEncodingDetection.h>
#include <wxCueFile/wxTextInputStreamOnString.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxCueSheetReader, wxObject );

// ===============================================================================

const wxChar wxCueSheetReader::LOG_EXT[]  = wxT( "log" );
const wxChar wxCueSheetReader::LOG_MASK[] = wxT( "*.log" );

// ===============================================================================

const wxCueSheetReader::PARSE_STRUCT wxCueSheetReader::parseArray[] =
{
	{ wxT( "REM" ), &wxCueSheetReader::ParseComment },
	{ wxT( "INDEX" ), &wxCueSheetReader::ParseIndex },
	{ wxT( "PREGAP" ), &wxCueSheetReader::ParsePreGap },
	{ wxT( "POSTGAP" ), &wxCueSheetReader::ParsePostGap },
	{ wxT( "FILE" ), &wxCueSheetReader::ParseFile },
	{ wxT( "FLAGS" ), &wxCueSheetReader::ParseFlags },
	{ wxT( "TRACK" ), &wxCueSheetReader::ParseTrack },
	{ wxT( "CATALOG" ), &wxCueSheetReader::ParseCatalog },
	{ wxT( "CDTEXTFILE" ), &wxCueSheetReader::ParseCdTextFile }
};

const size_t wxCueSheetReader::parseArraySize = WXSIZEOF( wxCueSheetReader::parseArray );

// ===============================================================================

const wxChar* const wxCueSheetReader::CoverNames[] =
{
	wxT( "cover" ),
	wxT( "front" ),
	wxT( "folder" ),
	wxT( "picture" )
};

const size_t wxCueSheetReader::CoverNamesSize = WXSIZEOF( wxCueSheetReader::CoverNames );

// ===============================================================================

const wxChar* const wxCueSheetReader::CoverExts[] =
{
	wxT( "jpg" ),
	wxT( "jpeg" ),
	wxT( "png" )
};

const size_t wxCueSheetReader::CoverExtsSize = WXSIZEOF( wxCueSheetReader::CoverExts );

// ===============================================================================

wxString wxCueSheetReader::GetKeywordsRegExp()
{
	wxString sKeywordsRegExp( wxCueComponent::GetKeywordsRegExp() );
	wxString s;

	s.Printf( wxT( "\\A\\s*%s\\s+(\\S.*\\S)\\s*\\Z" ), sKeywordsRegExp );
	return s;
}

wxString wxCueSheetReader::GetDataModeRegExp()
{
	wxString sDataModeRegExp( wxTrack::GetDataModeRegExp() );
	wxString s;

	s.Printf( wxT( "\\A(\\d{1,2})(?:\\s+%s){0,1}\\Z" ), sDataModeRegExp );
	return s;
}

wxString wxCueSheetReader::GetCdTextInfoRegExp()
{
	wxString sRegExp( wxCueComponent::GetCdTextInfoRegExp() );
	wxString s;

	s.Printf( wxT( "\\A\\s*%s\\s+(\\S.*\\S)\\s*\\Z" ), sRegExp );
	return s;
}

wxString wxCueSheetReader::GetDataFileRegExp()
{
	wxString sRegExp( wxDataFile::GetFileTypeRegExp() );
	wxString s;

	s.Printf( wxT( "\\A((?:\\\".*\\\")|(?:\\'.*\\'))(?:\\s+%s){0,1}\\Z" ), sRegExp );
	return s;
}

bool wxCueSheetReader::GetLogFile( const wxFileName& inputFile, bool bAnyLog, wxFileName& logFile )
{
	wxASSERT( inputFile.IsOk() );

	if ( bAnyLog )	// any lof file in the same directory
	{
		wxFileName sourceDirFn( inputFile );

		sourceDirFn.SetName( wxEmptyString );
		sourceDirFn.ClearExt();
		wxASSERT( sourceDirFn.IsDirReadable() );

		wxDir sourceDir( sourceDirFn.GetPath() );

		if ( !sourceDir.IsOpened() )
		{
			wxLogError( _( "Fail to open directory \u201C%s\u201D" ), sourceDirFn.GetPath() );
			return false;
		}

		wxString sFileName;

		if ( sourceDir.GetFirst( &sFileName, LOG_MASK, wxDIR_FILES ) )
		{
			while ( true )
			{
				wxFileName fileName( sourceDir.GetName(), sFileName );
				logFile = fileName;
				return true;
			}
		}

		return false;
	}
	else
	{
		logFile = inputFile;
		logFile.SetExt( LOG_EXT );
		wxASSERT( logFile.IsOk() );
		return logFile.IsFileReadable();
	}
}

bool wxCueSheetReader::IsCoverFile( const wxFileName& fileName )
{
	for ( size_t i = 0; i < CoverExtsSize; i++ )
	{
		if ( fileName.GetExt().CmpNoCase( CoverExts[ i ] ) == 0 )
		{
			return true;
		}
	}

	return false;
}

bool wxCueSheetReader::GetCoverFile( const wxDir& sourceDir, const wxString& sFileNameBase, wxFileName& coverFile )
{
	wxASSERT( sourceDir.IsOpened() );

	wxString sFileName;
	size_t	 nCounter = 0;
	wxString sFileSpec( wxString::Format( wxT( "%s.*" ), sFileNameBase ) );

	if ( sourceDir.GetFirst( &sFileName, sFileSpec, wxDIR_FILES ) )
	{
		while ( true )
		{
			wxFileName fileName( sourceDir.GetName(), sFileName );

			if ( IsCoverFile( fileName ) )
			{
				coverFile = fileName;
				return true;
			}

			if ( !sourceDir.GetNext( &sFileName ) )
			{
				break;
			}
		}
	}

	return false;
}

bool wxCueSheetReader::GetCoverFile( const wxFileName& inputFile, wxFileName& coverFile )
{
	wxFileName sourceDirFn( inputFile );

	sourceDirFn.SetName( wxEmptyString );
	sourceDirFn.ClearExt();
	wxASSERT( sourceDirFn.IsDirReadable() );

	wxDir sourceDir( sourceDirFn.GetPath() );

	if ( !sourceDir.IsOpened() )
	{
		wxLogError( _( "Fail to open directory \u201C%s\u201D" ), sourceDirFn.GetPath() );
		return false;
	}

	for ( size_t i = 0; i < CoverNamesSize; i++ )
	{
		if ( GetCoverFile( sourceDir, CoverNames[ i ], coverFile ) )
		{
			return true;
		}
	}

	return false;
}

wxString wxCueSheetReader::GetOneTrackCue()
{
	wxTextOutputStreamOnString tos;

	( *tos ) <<
	wxT( "REM one-track CUE sheet" ) << endl <<
	wxT( "FILE \"%source%\" WAVE" ) << endl <<
	wxT( "\tTRACK 01 AUDIO" ) << endl <<
	wxT( "\tINDEX 01 00:00:00" ) << endl;
	( *tos ).Flush();
	return tos.GetString();
}

bool wxCueSheetReader::TestReadFlags( ReadFlags nFlags, ReadFlags nMask )
{
	return ( nFlags & nMask ) == nMask;
}

wxCueSheetReader::wxCueSheetReader( void ):
	m_reKeywords( GetKeywordsRegExp(), wxRE_ADVANCED ),
	m_reCdTextInfo( GetCdTextInfoRegExp(), wxRE_ADVANCED ),
	m_reEmpty( wxT( "\\A\\s*\\Z" ), wxRE_ADVANCED ),
	m_reIndex( wxT( "\\A\\s*(\\d{1,2})\\s+(\\S.*\\S)\\Z" ), wxRE_ADVANCED ),
	m_reMsf( wxT( "\\A(\\d{1,4}):(\\d{1,2}):(\\d{1,2})\\Z" ), wxRE_ADVANCED ),
	m_reQuotesEx( wxUnquoter::RE_SINGLE_QUOTES_EX, wxRE_ADVANCED ),
	m_reFlags( wxT( "\\s+" ), wxRE_ADVANCED ),
	m_reDataMode( GetDataModeRegExp(), wxRE_ADVANCED ),
	m_reDataFile( GetDataFileRegExp(), wxRE_ADVANCED ),
	m_reCatalog( wxT( "\\d{13}" ), wxRE_ADVANCED | wxRE_NOSUB ),
	m_reIsrc( wxT( "([[:upper:]]{2}|00)-{0,1}[[:upper:][:digit:]]{3}-{0,1}[[:digit:]]{5}" ), wxRE_ADVANCED | wxRE_NOSUB ),
	m_reTrackComment( wxT( "cue[[.hyphen.][.underscore.][.low-line.]]track([[:digit:]]{1,2})[[.underscore.][.low-line.]]([[:alpha:][.hyphen.][.underscore.][.low-line.][.space.]]+)" ), wxRE_ADVANCED | wxRE_ICASE ),
	m_reCommentMeta( wxT( "\\A([[.quotation-mark.]]{0,1})([[:upper:][.hyphen.][.underscore.][:space:][.low-line.]]+)\\1[[:space:]]+([^[:space:]].+)\\Z" ), wxRE_ADVANCED ),
	m_bErrorsAsWarnings( true ),
	m_bParseComments( true ),
	m_bEllipsizeTags( true ),
	m_bRemoveExtraSpaces( false ),
	m_sOneTrackCue( GetOneTrackCue() )
{
	wxASSERT( m_reKeywords.IsValid() );
	wxASSERT( m_reCdTextInfo.IsValid() );
	wxASSERT( m_reEmpty.IsValid() );
	wxASSERT( m_reIndex.IsValid() );
	wxASSERT( m_reMsf.IsValid() );
	wxASSERT( m_reQuotesEx.IsValid() );
	wxASSERT( m_reFlags.IsValid() );
	wxASSERT( m_reDataMode.IsValid() );
	wxASSERT( m_reDataFile.IsValid() );
	wxASSERT( m_reCatalog.IsValid() );
	wxASSERT( m_reIsrc.IsValid() );
	wxASSERT( m_reTrackComment.IsValid() );
	wxASSERT( m_reCommentMeta.IsValid() );
}

const wxCueSheet& wxCueSheetReader::GetCueSheet() const
{
	return m_cueSheet;
}

bool wxCueSheetReader::ErrorsAsWarnings() const
{
	return m_bErrorsAsWarnings;
}

wxCueSheetReader& wxCueSheetReader::SetErrorsAsWarnings( bool bErrorsAsWarnings )
{
	m_bErrorsAsWarnings = bErrorsAsWarnings;
	return *this;
}

bool wxCueSheetReader::ParseComments() const
{
	return m_bParseComments;
}

wxCueSheetReader& wxCueSheetReader::SetParseComments( bool bParseComments )
{
	m_bParseComments = bParseComments;
	return *this;
}

bool wxCueSheetReader::EllipsizeTags() const
{
	return m_bEllipsizeTags;
}

wxCueSheetReader& wxCueSheetReader::SetEllipsizeTags( bool bEllipsizeTags )
{
	m_bEllipsizeTags = bEllipsizeTags;
	return *this;
}

bool wxCueSheetReader::RemoveExtraSpaces() const
{
	return m_bRemoveExtraSpaces;
}

wxCueSheetReader& wxCueSheetReader::SetRemoveExtraSpaces( bool bRemoveExtraSpaces )
{
	m_bRemoveExtraSpaces = bRemoveExtraSpaces;
	return *this;
}

wxCueSheetReader& wxCueSheetReader::CorrectQuotationMarks( bool bCorrectQuotationMarks, const wxString& sLang )
{
	m_unquoter.SetLang( bCorrectQuotationMarks ? sLang : wxEmptyString );
	return *this;
}

const wxString& wxCueSheetReader::GetAlternateExt() const
{
	return m_sAlternateExt;
}

wxCueSheetReader& wxCueSheetReader::SetAlternateExt( const wxString& sAlternateExt )
{
	m_sAlternateExt = sAlternateExt;
	return *this;
}

bool wxCueSheetReader::FindLog( const wxCueSheetContent& content, ReadFlags nMode )
{
	wxASSERT( TestReadFlags( nMode, EC_FIND_LOG ) );
	wxFileName logFile;

	if ( GetLogFile( content.GetSource(), TestReadFlags( nMode, EC_SINGLE_MEDIA_FILE ), logFile ) )
	{
		m_cueSheet.AddLog( logFile );
		return true;
	}
	else
	{
		return false;
	}
}

bool wxCueSheetReader::FindCover( const wxCueSheetContent& content, ReadFlags nMode )
{
	wxASSERT( TestReadFlags( nMode, EC_FIND_COVER ) );
	wxFileName coverFile;

	if ( GetCoverFile( content.GetSource(), coverFile ) )
	{
		m_cueSheet.AddCover( coverFile );
		return true;
	}
	else
	{
		return false;
	}
}

bool wxCueSheetReader::ReadCueSheet( const wxString& sCueFile, ReadFlags nMode, bool bUseMLang )
{
	wxString							   sCPDescription;
	wxEncodingDetection::wxMBConvSharedPtr pConv( wxEncodingDetection::GetFileEncoding( sCueFile, bUseMLang, sCPDescription ) );

	if ( pConv )
	{
		wxLogInfo( _( "Detected encoding of file \u201C%s\u201D file is \u201C%s\u201D" ), sCueFile, sCPDescription );
		return ReadCueSheet( sCueFile, *pConv, nMode );
	}
	else
	{
		return false;
	}
}

bool wxCueSheetReader::ReadCueSheet( const wxString& sCueFile, wxMBConv& conv, ReadFlags nMode )
{
	wxFileName cueFileName( sCueFile );

	cueFileName.MakeAbsolute();

	if ( !cueFileName.IsFileReadable() || cueFileName.IsDir() )
	{
		wxLogError( _( "Invalid path to CUE file \u201C%s\u201D" ), sCueFile );
		return false;
	}

	wxFileInputStream fis( cueFileName.GetFullPath() );

	if ( !fis.IsOk() )
	{
		wxLogError( _( "Unable to open CUE file \u201C%s\u201D" ), cueFileName.GetFullPath() );
		return false;
	}

	return ParseCue( wxCueSheetContent( internalReadCueSheet( fis, conv ), cueFileName, false ), nMode );
}

bool wxCueSheetReader::ReadCueSheet( wxInputStream& stream, ReadFlags nMode )
{
	return ReadCueSheet( stream, wxConvLocal, nMode );
}

bool wxCueSheetReader::ReadCueSheet( wxInputStream& stream, wxMBConv& conv, ReadFlags nMode )
{
	return ParseCue( wxCueSheetContent( internalReadCueSheet( stream, conv ) ), nMode );
}

void wxCueSheetReader::ProcessMediaInfoCueSheet( wxString& sCueSheet )
{
	m_unquoter.GetReDoubleQuotes().ReplaceAll( &sCueSheet, wxUnquoter::POLISH_DOUBLE_QUOTES );
	m_reQuotesEx.ReplaceAll( &sCueSheet, wxUnquoter::POLISH_SINGLE_QUOTES );

	// slash -> division slash inside quotes

	// double quotes
	wxRegEx re( wxT( "(\\u201E.*?)/([^\\u201E]*?\\u201D)" ), wxRE_ADVANCED | wxRE_NEWLINE );
	wxASSERT( re.IsValid() );
	re.ReplaceAll( &sCueSheet, wxT( "\\1\u2215\\2" ) );	// division slash

	// single quotes
	re.Compile( wxT( "(\\u201A.*?)/([^\\u201A]*?\\u2019)" ), wxRE_ADVANCED | wxRE_NEWLINE );
	wxASSERT( re.IsValid() );
	re.ReplaceAll( &sCueSheet, wxT( "\\1\u2215\\2" ) );	// division slash

	// rest slashes to newline
	sCueSheet.Replace( wxT( "/" ), wxT( "\n" ) );

	// backs from division slash to slash
	sCueSheet.Replace( wxT( "\u2215" ), wxT( "/" ) );

	// back to normal quotes

	// double quotes
	re.Compile( wxT( "\\u201E([^\\u201E\\u201D\\u201A\\u2019]*)\\u201D" ), wxRE_ADVANCED | wxRE_NEWLINE );
	wxASSERT( re.IsValid() );
	re.ReplaceAll( &sCueSheet, wxT( "\"\\1\"" ) );

	// single quotes
	re.Compile( wxT( "\\u201A([^\\u201A\\u2019]*)\\u2019" ), wxRE_ADVANCED | wxRE_NEWLINE );
	re.ReplaceAll( &sCueSheet, wxT( "'\\1'" ) );
}

bool wxCueSheetReader::ReadCueSheetFromVorbisComment( const wxDataFile& mediaFile, const wxFlacMetaDataReader& flacReader, ReadFlags nMode )
{
	if ( !flacReader.HasVorbisComment() )
	{
		wxLogWarning( _( "Cannot find Vorbis comments inside FLAC file" ) );
		return false;
	}

	wxString sCueSheet;

	if ( !flacReader.GetCueSheetFromVorbisComment( sCueSheet ) )
	{
		wxLogWarning( _( "Cannot find CUESHEET comment" ) );
		return false;
	}

	bool res = ParseCue( wxCueSheetContent( sCueSheet, mediaFile.GetRealFileName(), true ), ( nMode & ~EC_MEDIA_READ_TAGS ) );

	if ( res )
	{
		m_cueSheet.SetSingleDataFile( mediaFile );
	}

	if ( res && TestReadFlags( nMode, EC_MEDIA_READ_TAGS ) )
	{
		res = AppendFlacTags( flacReader, false );
	}

	return res;
}

bool wxCueSheetReader::ReadCueSheetFromCueSheetTag( const wxDataFile& mediaFile, const wxFlacMetaDataReader& flacReader, ReadFlags nMode )
{
	if ( !flacReader.HasCueSheet() )
	{
		wxLogWarning( _( "Cannot find CueSheet tag inside FLAC file" ) );
		return false;
	}

	const FLAC::Metadata::CueSheet& cueSheet = flacReader.GetCueSheet();

	m_cueSheet.Clear();
	wxString sCatalog( cueSheet.get_media_catalog_number() );

	if ( !sCatalog.IsEmpty() )
	{
		m_cueSheet.AddCatalog( sCatalog );
	}

	for ( unsigned int i = 0; i < cueSheet.get_num_tracks(); i++ )
	{
		const FLAC::Metadata::CueSheet::Track flacTrack = cueSheet.get_track( i );
		wxTrack								  track( flacTrack.get_number() );
		wxString							  sIsrc( flacTrack.get_isrc() );

		if ( !sIsrc.IsEmpty() )
		{
			track.AddCdTextInfoTag( wxCueTag::Name::ISRC, sIsrc );
		}

		if ( flacTrack.get_pre_emphasis() )
		{
			track.AddFlag( wxTrack::PRE );
		}

		unsigned int numIndicies = flacTrack.get_num_indices();
		for ( unsigned int j = 0; j < numIndicies; j++ )
		{
			::FLAC__StreamMetadata_CueSheet_Index flacIdx = flacTrack.get_index( j );
			wxIndex idx( flacIdx.number, flacTrack.get_offset() + flacIdx.offset );
			track.AddIndex( idx );
		}

		m_cueSheet.AddTrack( track );
	}

	m_cueSheet.SetSingleDataFile( mediaFile );
	wxCueSheetContent content( wxTextCueSheetRenderer::ToString( m_cueSheet ), flacReader.GetFlacFile(), true );
	m_cueSheetContent = content;

	if ( TestReadFlags( nMode, EC_MEDIA_READ_TAGS ) && flacReader.HasVorbisComment() )
	{
		if ( !AppendFlacTags( flacReader, false ) )
		{
			return false;
		}
	}

	if ( TestReadFlags( nMode, EC_FIND_LOG ) )
	{
		FindLog( content, nMode );
	}

	if ( TestReadFlags( nMode, EC_FIND_COVER ) )
	{
		FindCover( content, nMode );
	}

	return true;
}

bool wxCueSheetReader::ReadEmbeddedInFlacCueSheet( const wxDataFile& mediaFile, ReadFlags nMode )
{
	wxFlacMetaDataReader flacReader;
	wxString			 sMediaFile( mediaFile.GetRealFileName().GetFullPath() );

	if ( !flacReader.ReadMetadata( sMediaFile ) )
	{
		return false;
	}

	switch ( nMode & EC_FLAC_READ_MASK )
	{
		case EC_FLAC_READ_COMMENT_ONLY:
		{
			return ReadCueSheetFromVorbisComment( mediaFile, flacReader, nMode );
		}

		case EC_FLAC_READ_TAG_ONLY:
		{
			return ReadCueSheetFromCueSheetTag( mediaFile, flacReader, nMode );
		}

		case EC_FLAC_READ_TAG_FIRST_THEN_COMMENT:
		{
			return ReadCueSheetFromCueSheetTag( mediaFile, flacReader, nMode ) || ReadCueSheetFromVorbisComment( mediaFile, flacReader, nMode );
		}

		case EC_FLAC_READ_COMMENT_FIRST_THEN_TAG:
		{
			return ReadCueSheetFromVorbisComment( mediaFile, flacReader, nMode ) || ReadCueSheetFromCueSheetTag( mediaFile, flacReader, nMode );
		}

		default:
		return false;
	}

	return true;
}

void wxCueSheetReader::AppendTags( const wxArrayCueTag& comments, bool bSingleMediaFile )
{
	for ( size_t i = 0, nComments = comments.GetCount(); i < nComments; i++ )
	{
		wxCueTag comment( comments[ i ] );

		if ( comment == wxCueTag::Name::CUESHEET )
		{
			continue;
		}

		comment.Unquote( m_unquoter );
		comment.RemoveTrailingSpaces( m_trailingSpacesRemover );

		if ( m_bRemoveExtraSpaces )
		{
			comment.RemoveExtraSpaces( m_reduntantSpacesRemover );
		}

		if ( m_bEllipsizeTags )
		{
			comment.Ellipsize( m_ellipsizer );
		}

		if ( bSingleMediaFile )	// just add to first track
		{
			wxASSERT( m_cueSheet.HasTrack( 1 ) );
			wxTrack& firstTrack = m_cueSheet.GetTrackByNumber( 1 );
			firstTrack.AddTag( comment );
		}
		else
		{
			if ( comment == wxCueTag::Name::TOTALTRACKS )
			{
				continue;
			}

			if ( m_reTrackComment.Matches( comment.GetName() ) )
			{
				wxString	  sTagNumber( m_reTrackComment.GetMatch( comment.GetName(), 1 ) );
				wxString	  sTagName( m_reTrackComment.GetMatch( comment.GetName(), 2 ) );
				unsigned long trackNumber;

				if ( sTagNumber.ToULong( &trackNumber ) )
				{
					if ( m_cueSheet.HasTrack( trackNumber ) )
					{
						wxTrack& track = m_cueSheet.GetTrackByNumber( trackNumber );
						wxCueTag trackTag( comment.GetSource(), sTagName, comment.GetValue() );
						track.AddTag( trackTag );
					}
					else
					{
						wxLogInfo( _( "Skipping track comment %s - track %d not found" ), comment.GetName(), trackNumber );
					}
				}
				else
				{
					wxLogDebug( wxT( "Invalid track comment regular expression" ) );
				}
			}
			else
			{
				m_cueSheet.AddTag( comment );
			}
		}
	}
}

void wxCueSheetReader::AppendTags( const wxArrayCueTag& tags, size_t nTrackFrom, size_t nTrackTo )
{
	for ( size_t i = 0, nTags = tags.GetCount(); i < nTags; i++ )
	{
		wxCueTag tag( tags[ i ] );

		if ( tag == wxCueTag::Name::CUESHEET )
		{
			continue;
		}

		if ( tag == wxCueTag::Name::TOTALTRACKS )
		{
			continue;
		}

		tag.Unquote( m_unquoter );
		tag.RemoveTrailingSpaces( m_trailingSpacesRemover );

		if ( m_bRemoveExtraSpaces )
		{
			tag.RemoveExtraSpaces( m_reduntantSpacesRemover );
		}

		if ( m_bEllipsizeTags )
		{
			tag.Ellipsize( m_ellipsizer );
		}

		if ( m_reTrackComment.Matches( tag.GetName() ) )
		{
			wxString	  sTagNumber( m_reTrackComment.GetMatch( tag.GetName(), 1 ) );
			wxString	  sTagName( m_reTrackComment.GetMatch( tag.GetName(), 2 ) );
			unsigned long trackNumber;

			if ( sTagNumber.ToULong( &trackNumber ) )
			{
				if ( m_cueSheet.HasTrack( trackNumber ) )
				{
					wxTrack& track = m_cueSheet.GetTrackByNumber( trackNumber );
					track.AddTag( wxCueTag( tag.GetSource(), sTagName, tag.GetValue() ) );
				}
				else
				{
					wxLogInfo( _( "Skipping track comment %s - track %d not found" ), tag.GetName(), trackNumber );
				}
			}
			else
			{
				wxLogDebug( wxT( "Invalid track comment regular expression" ) );
			}
		}
		else
		{
			for ( size_t j = nTrackFrom; j <= nTrackTo; j++ )
			{
				m_cueSheet.GetTrack( j ).AddTag( tag );
			}
		}
	}
}

bool wxCueSheetReader::AppendFlacTags( const wxFlacMetaDataReader& flacReader, bool bSingleMediaFile )
{
	wxArrayCueTag tags;

	flacReader.ReadVorbisComments( tags );
	AppendTags( tags, bSingleMediaFile );
	return true;
}

bool wxCueSheetReader::AppendFlacTags( const wxFlacMetaDataReader& flacReader, size_t nTrackFrom, size_t nTrackTo )
{
	wxArrayCueTag tags;

	flacReader.ReadVorbisComments( tags );
	AppendTags( tags, nTrackFrom, nTrackTo );
	return true;
}

bool wxCueSheetReader::ReadWavpackTags( const wxString& sMediaFile, bool bSingleMediaFile )
{
	wxArrayCueTag comments;

	if ( wxWavpackTagReader::ReadTags( sMediaFile, comments ) )
	{
		AppendTags( comments, bSingleMediaFile );
		return true;
	}
	else
	{
		return false;
	}
}

bool wxCueSheetReader::ReadWavpackTags( const wxString& sMediaFile, size_t nTrackFrom, size_t nTrackTo )
{
	wxArrayCueTag comments;

	if ( wxWavpackTagReader::ReadTags( sMediaFile, comments ) )
	{
		AppendTags( comments, nTrackFrom, nTrackTo );
		return true;
	}
	else
	{
		return false;
	}
}

bool wxCueSheetReader::ReadEmbeddedInWavpackCueSheet( const wxDataFile& mediaFile, ReadFlags nMode )
{
	wxString sCueSheet;
	wxString sMediaFile( mediaFile.GetRealFileName().GetFullPath() );

	if ( !wxWavpackTagReader::ReadCueSheetTag( sMediaFile, sCueSheet ) )
	{
		return false;
	}

	bool res = ParseCue( wxCueSheetContent( sCueSheet, sMediaFile, true ), ( nMode & ~EC_MEDIA_READ_TAGS ) );

	if ( res )
	{
		wxASSERT( m_cueSheet.HasSingleDataFile() );
		m_cueSheet.SetSingleDataFile( mediaFile );
	}

	if ( res )
	{
		if ( TestReadFlags( nMode, EC_MEDIA_READ_TAGS ) )
		{
			res = ReadWavpackTags( sMediaFile, false );
		}
	}

	return res;
}

bool wxCueSheetReader::ReadEmbeddedCueSheet( const wxString& sMediaFile, ReadFlags nMode )
{
	wxDataFile dataFile( sMediaFile, wxDataFile::WAVE );

	if ( !dataFile.GetInfo() )
	{
		return false;
	}

	bool bSingleMediaFile = TestReadFlags( nMode, EC_SINGLE_MEDIA_FILE );
	bool bCueSheet		  = dataFile.HasCueSheet();

	if ( !bSingleMediaFile )
	{
		if ( !bCueSheet )
		{
			wxLogWarning( _( "MediaInfo - no cue sheet" ) );
			return false;
		}
	}

	bool bRes = false;

	if ( bSingleMediaFile )
	{
		bRes = BuildFromSingleMediaFile( sMediaFile, nMode );
	}
	else
	{
		switch ( dataFile.GetMediaType() )
		{
			case wxDataFile::MEDIA_TYPE_FLAC:
			{
				bRes = ReadEmbeddedInFlacCueSheet( dataFile, nMode );
				break;
			}

			case wxDataFile::MEDIA_TYPE_WAVPACK:
			{
				bRes = ReadEmbeddedInWavpackCueSheet( dataFile, nMode );
				break;
			}

			default:
			{
				wxString sCueSheet( dataFile.GetCueSheet() );
				ProcessMediaInfoCueSheet( sCueSheet );
				bRes = ParseCue( wxCueSheetContent( sCueSheet, sMediaFile, true ), nMode );
				break;
			}
		}
	}

	return bRes;
}

/* one track, one index*/
bool wxCueSheetReader::BuildFromSingleMediaFile( const wxString& sMediaFile, ReadFlags nMode )
{
	m_cueSheet.Clear();
	wxString sOneTrackCue( m_sOneTrackCue );
	size_t	 nRepl = sOneTrackCue.Replace( wxT( "%source%" ), sMediaFile );
	wxASSERT( nRepl > 0 );

	if ( ParseCue( wxCueSheetContent( sOneTrackCue, sMediaFile, true ), nMode ) )
	{
		wxASSERT( m_cueSheet.GetTracksCount() == 1u );
		wxASSERT( m_cueSheet.HasSingleDataFile() );
		return true;
	}
	else
	{
		return false;
	}
}

wxString wxCueSheetReader::internalReadCueSheet( wxInputStream& stream, wxMBConv& conv )
{
	wxTextInputStream		   tis( stream, wxT( " \t" ), conv );
	wxTextOutputStreamOnString tos;

	m_cueSheet.Clear();

	while ( !stream.Eof() )
	{
		*tos << tis.ReadLine() << endl;
	}

	return tos.GetString();
}

void wxCueSheetReader::AddError0( const wxString& sMsg )
{
	m_errors.Add( sMsg );
}

void wxCueSheetReader::AddError( const wxChar* pszFormat, ... )
{
	va_list argptr;

	va_start( argptr, pszFormat );
	wxString s;
	s.PrintfV( pszFormat, argptr );
	AddError0( s );
	va_end( argptr );
}

void wxCueSheetReader::DumpErrors( size_t nLine ) const
{
	if ( !m_errors.IsEmpty() )
	{
		for ( wxArrayString::const_iterator i = m_errors.begin();
			  i != m_errors.end(); i++ )
		{
			if ( m_bErrorsAsWarnings )
			{
				wxLogWarning( _( "Line %d: %s" ), nLine, i->GetData() );
			}
			else
			{
				wxLogError( _( "Line %d: %s" ), nLine, i->GetData() );
			}
		}
	}
}

bool wxCueSheetReader::CheckEntryType( wxCueComponent::ENTRY_TYPE et ) const
{
	if ( m_cueSheet.HasTracks() )
	{
		return m_cueSheet.GetLastTrack().CheckEntryType( et );
	}
	else
	{
		return m_cueSheet.CheckEntryType( et );
	}
}

wxString wxCueSheetReader::Unquote( const wxString& qs )
{
	return wxCueTag::UnEscape( m_unquoter.UnquoteAndCorrect( qs ) );
}

void wxCueSheetReader::ParseLine( size_t WXUNUSED( nLine ), const wxString& sToken, const wxString& sRest )
{
	for ( size_t i = 0; i < parseArraySize; i++ )
	{
		if ( sToken.CmpNoCase( parseArray[ i ].token ) == 0 )
		{
			wxCueComponent::ENTRY_TYPE et;
			wxCueComponent::GetEntryType( parseArray[ i ].token, et );
			m_errors.Clear();

			if ( CheckEntryType( et ) )
			{
				PARSE_METHOD method = parseArray[ i ].method;
				( this->*method )( sToken, sRest );
			}
			else
			{
				AddError( _( "Keyword %s is not allowed here" ), sToken );
			}

			return;
		}
	}

	m_errors.Clear();
	AddError( _( "Unknown token %s" ), sToken );
}

void wxCueSheetReader::ParseCdTextInfo( size_t WXUNUSED( nLine ), const wxString& sToken, const wxString& sBody )
{
	wxCueComponent::ENTRY_FORMAT fmt;

	wxCueComponent::GetCdTextInfoFormat( sToken, fmt );
	wxString s;

	if ( fmt == wxCueComponent::CHARACTER )
	{
		s = Unquote( sBody );
	}
	else
	{
		s = sBody;
	}

	bool add = true;

	if ( sToken.CmpNoCase( wxT( "ISRC" ) ) == 0 )
	{
		if ( !m_reIsrc.Matches( sBody ) )
		{
			AddError0( _( "Invalid ISRC code" ) );
			add = false;
		}
	}

	if ( add )
	{
		if ( !AddCdTextInfo( sToken, s ) )
		{
			AddError( _( "Keyword %s is not allowed here" ), sToken );
		}
	}
}

void wxCueSheetReader::ParseGarbage( const wxString& sLine )
{
	if ( m_cueSheet.HasTracks() )
	{
		m_cueSheet.GetLastTrack().AddGarbage( sLine );
	}
	else
	{
		m_cueSheet.AddGarbage( sLine );
	}
}

void wxCueSheetReader::ParseComment( wxCueComponent& component, const wxString& sComment )
{
	if ( !m_bParseComments )
	{
		component.AddComment( sComment );
		return;
	}

	if ( m_reCommentMeta.Matches( sComment ) )
	{
		component.AddTag( wxCueTag::TAG_CUE_COMMENT, m_reCommentMeta.GetMatch( sComment, 2 ), m_genericUnquoter.Unquote( m_reCommentMeta.GetMatch( sComment, 3 ) ) );
	}
	else
	{
		component.AddComment( sComment );
	}
}

void wxCueSheetReader::ParseComment( const wxString& WXUNUSED( sToken ), const wxString& sComment )
{
	if ( m_cueSheet.HasTracks() )
	{
		ParseComment( m_cueSheet.GetLastTrack(), sComment );
	}
	else
	{
		ParseComment( m_cueSheet, sComment );
	}
}

bool wxCueSheetReader::ParseCue( const wxCueSheetContent& content, ReadFlags nMode )
{
	m_cueSheetContent = content;

	size_t	 nLine = 1;
	wxString sLine;

	wxTextInputStreamOnString tis( content.GetValue() );
	while ( !tis.Eof() )
	{
		sLine = ( *tis ).ReadLine();

		if ( m_reEmpty.Matches( sLine ) )
		{
			wxLogDebug( wxT( "Skipping empty line %d" ), nLine );
			continue;
		}

		if ( !ParseCueLine( sLine, nLine ) )
		{
			return false;
		}

		nLine += 1;
	}

	m_cueSheet.SortTracks();
	m_cueSheet.AddContent( content );

	if ( TestReadFlags( nMode, EC_MEDIA_READ_TAGS ) )
	{
		ReadTagsFromRelatedFiles();
	}

	if ( content.HasSource() )
	{
		if ( TestReadFlags( nMode, EC_FIND_LOG ) )
		{
			FindLog( content, nMode );
		}

		if ( TestReadFlags( nMode, EC_FIND_COVER ) )
		{
			FindCover( content, nMode );
		}
	}

	return true;
}

bool wxCueSheetReader::ParseCueLine( const wxString& sLine, size_t nLine )
{
	bool res = true;

	if ( m_reKeywords.Matches( sLine ) )
	{
		wxASSERT( m_reKeywords.GetMatchCount() == 3 );
		wxString sToken = m_reKeywords.GetMatch( sLine, 1 );
		wxString sRest	= m_reKeywords.GetMatch( sLine, 2 );
		m_errors.Clear();
		ParseLine( nLine, sToken, sRest );

		if ( m_errors.GetCount() > 0 )
		{
			DumpErrors( nLine );
			ParseGarbage( sLine );
			res = false;
		}
	}
	else if ( m_reCdTextInfo.Matches( sLine ) )
	{
		wxASSERT( m_reCdTextInfo.GetMatchCount() == 3 );
		wxString sToken = m_reCdTextInfo.GetMatch( sLine, 1 );
		wxString sRest	= m_reCdTextInfo.GetMatch( sLine, 2 );
		m_errors.Clear();
		ParseCdTextInfo( nLine, sToken, sRest );

		if ( m_errors.GetCount() > 0 )
		{
			DumpErrors( nLine );
			ParseGarbage( sLine );
			res = false;
		}
	}
	else
	{
		wxLogWarning( _( "Incorrect line %d: %s" ), nLine, sLine );
		ParseGarbage( sLine );
		res = false;
	}

	return res;
}

bool wxCueSheetReader::AddCdTextInfo( const wxString& sToken, const wxString& sBody )
{
	wxString sModifiedBody( m_trailingSpacesRemover.Remove( sBody ) );

	if ( m_bEllipsizeTags )
	{
		sModifiedBody = m_ellipsizer.Ellipsize( sModifiedBody );
	}

	if ( m_bRemoveExtraSpaces )
	{
		m_reduntantSpacesRemover.Remove( sModifiedBody );
	}

	if ( m_cueSheet.HasTracks() )
	{
		return m_cueSheet.GetLastTrack().AddCdTextInfoTag( sToken, sModifiedBody );
	}
	else
	{
		return m_cueSheet.AddCdTextInfoTag( sToken, sModifiedBody );
	}
}

bool wxCueSheetReader::ParseMsf( const wxString& sBody, wxIndex& idx, bool bPrePost )
{
	bool		  res = true;
	unsigned long min, sec, frames;

	if ( m_reMsf.Matches( sBody ) )
	{
		if ( !m_reMsf.GetMatch( sBody, 1 ).ToULong( &min ) )
		{
			res = false;
		}

		if ( !m_reMsf.GetMatch( sBody, 2 ).ToULong( &sec ) )
		{
			res = false;
		}

		if ( !m_reMsf.GetMatch( sBody, 3 ).ToULong( &frames ) )
		{
			res = false;
		}
	}
	else
	{
		res = false;
	}

	if ( res )
	{
		res = idx.Assign( 0, min, sec, frames ).IsValid( bPrePost );
	}

	return res;
}

void wxCueSheetReader::ParsePreGap( const wxString& WXUNUSED( sToken ), const wxString& sBody )
{
	wxIndex idx;

	if ( ParseMsf( sBody, idx, true ) )
	{
		if ( m_cueSheet.GetTracksCount() > 1u )
		{
			idx.SetDataFileIdx( m_cueSheet.GetBeforeLastTrack().GetMaxDataFileIdx( false ) );
			m_cueSheet.GetLastTrack().SetPreGap( idx );
		}
		else
		{
			AddError0( _( "Fail to add pre-gap - no previous track" ) );
		}
	}
	else
	{
		AddError0( _( "Invalid index specification" ) );
	}
}

void wxCueSheetReader::ParsePostGap( const wxString& WXUNUSED( sToken ), const wxString& sBody )
{
	wxIndex idx;

	if ( ParseMsf( sBody, idx, true ) )
	{
		if ( m_cueSheet.HasDataFiles() )
		{
			idx.SetDataFileIdx( m_cueSheet.GetLastDataFileIdx() );
			m_cueSheet.GetLastTrack().SetPostGap( idx );
		}
		else
		{
			AddError0( _( "Cannot add post-gap - no data files" ) );
		}
	}
	else
	{
		AddError0( _( "Invalid index specification" ) );
	}
}

void wxCueSheetReader::ParseIndex( const wxString& WXUNUSED( sToken ), const wxString& sBody )
{
	if ( m_reIndex.Matches( sBody ) )
	{
		unsigned long number;

		if ( !m_reIndex.GetMatch( sBody, 1 ).ToULong( &number ) )
		{
			AddError0( _( "Invalid index specification" ) );
		}
		else
		{
			wxString sMsf = m_reIndex.GetMatch( sBody, 2 );
			wxIndex	 idx;

			if ( ParseMsf( sMsf, idx ) )
			{
				idx.SetNumber( number );

				if ( idx.IsValid() )
				{
					if ( m_cueSheet.HasDataFiles() )
					{
						idx.SetDataFileIdx( m_cueSheet.GetLastDataFileIdx() );
						m_cueSheet.GetLastTrack().AddIndex( idx );
					}
					else
					{
						AddError0( _( "Fail to add index - no data file" ) );
					}
				}
				else
				{
					AddError0( _( "Invalid index specification" ) );
				}
			}
			else
			{
				AddError0( _( "Invalid index specification" ) );
			}
		}
	}
	else
	{
		AddError0( _( "Invalid index specification" ) );
	}
}

void wxCueSheetReader::ParseFile( const wxString& WXUNUSED( sToken ), const wxString& sBody )
{
	if ( m_reDataFile.Matches( sBody ) )
	{
		wxString sFile( m_reDataFile.GetMatch( sBody, 1 ) );
		wxString sType( m_reDataFile.GetMatch( sBody, 2 ) );

		wxDataFile::FileType ftype = wxDataFile::BINARY;

		if ( !sType.IsEmpty() )
		{
			wxDataFile::FromString( sType, ftype );
		}

		wxFileName fn( Unquote( sFile ) );

		if ( m_cueSheetContent.HasSource() )
		{
			fn.SetPath( m_cueSheetContent.GetSource().GetPath() );
		}

		m_cueSheet.AddDataFile( wxDataFile( fn, ftype ) );
	}
}

void wxCueSheetReader::ParseFlags( const wxString& WXUNUSED( sToken ), const wxString& sBody )
{
	wxString sFlags( sBody );

	m_reFlags.ReplaceAll( &sFlags, wxT( '|' ) );
	wxStringTokenizer tokenizer( sFlags, wxT( "|" ) );
	while ( tokenizer.HasMoreTokens() )
	{
		wxString sFlag( tokenizer.GetNextToken() );

		if ( !m_cueSheet.GetLastTrack().AddFlag( sFlag ) )
		{
			AddError( _( "Invalid flag %s" ), sFlag );
		}
	}
}

void wxCueSheetReader::ParseTrack( const wxString& sToken, const wxString& sBody )
{
	if ( m_reDataMode.Matches( sBody ) )
	{
		unsigned long trackNo;

		if ( !m_reDataMode.GetMatch( sBody, 1 ).ToULong( &trackNo ) )
		{
			AddError( _( "Invalid track number %s" ),
					m_reDataMode.GetMatch( sBody, 1 ) );
		}
		else
		{
			wxString sMode( m_reDataMode.GetMatch( sBody, 2 ) );
			wxTrack	 newTrack( trackNo );

			if ( newTrack.IsValid() && newTrack.SetMode( sMode ) )
			{
				m_cueSheet.AddTrack( newTrack );
			}
			else
			{
				AddError0( _( "Invalid track specification" ) );
			}
		}
	}
	else
	{
		AddError0( _( "Invalid track specification" ) );
	}
}

void wxCueSheetReader::ParseCatalog( const wxString& WXUNUSED( sToken ), const wxString& sBody )
{
	if ( m_reCatalog.Matches( sBody ) )
	{
		m_cueSheet.AddCatalog( sBody );
	}
	else
	{
		AddError0( _( "Invalid catalog number" ) );
	}
}

void wxCueSheetReader::ParseCdTextFile( const wxString& WXUNUSED( sToken ), const wxString& sBody )
{
	m_cueSheet.AddCdTextFile( Unquote( sBody ) );
}

bool wxCueSheetReader::ReadTagsFromRelatedFiles()
{
	if ( !m_cueSheet.CalculateDuration( m_sAlternateExt ) )
	{
		return false;
	}

	const wxArrayDataFile& dataFiles = m_cueSheet.GetDataFiles();
	size_t				   nTrackFrom, nTrackTo;
	bool				   bRes = true;

	for ( size_t i = 0, nCount = dataFiles.GetCount(); i < nCount; i++ )
	{
		wxASSERT( dataFiles[ i ].HasRealFileName() );

		if ( m_cueSheet.GetRelatedTracks( i, nTrackFrom, nTrackTo ) )
		{
			if ( !ReadTagsFromMediaFile( dataFiles[ i ], nTrackFrom, nTrackTo ) )
			{
				bRes = false;
			}
		}
		else
		{
			wxLogWarning( _( "Data file %s is not related" ), dataFiles[ i ].GetRealFileName().GetName() );
		}
	}

	return bRes;
}

bool wxCueSheetReader::ReadTagsFromMediaFile( const wxDataFile& dataFile, size_t nTrackFrom, size_t nTrackTo )
{
	bool bRes = false;

	switch ( dataFile.GetMediaType() )
	{
		case wxDataFile::MEDIA_TYPE_FLAC:
		{
			wxFlacMetaDataReader flacReader;

			if ( flacReader.ReadMetadata( dataFile.GetRealFileName().GetFullPath() ) && flacReader.HasVorbisComment() )
			{
				bRes = AppendFlacTags( flacReader, nTrackFrom, nTrackTo );
			}

			break;
		}

		case wxDataFile::MEDIA_TYPE_WAVPACK:
		{
			bRes = ReadWavpackTags( dataFile.GetRealFileName().GetFullPath(), nTrackFrom, nTrackTo );
			break;
		}

		default:
		{
			bRes = false;
			wxLogWarning( _( "Cannot read metadata from %s" ), dataFile.GetMIFormat() );
			break;
		}
	}

	return bRes;
}

