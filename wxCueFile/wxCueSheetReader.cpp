/*
	wxCueSheetReader.cpp
*/

#include "StdWx.h"
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxCueFile/wxMediaInfo.h>
#include "wxFlacMetaDataReader.h"
#include "wxWavpackTagReader.h"
#include <wxEncodingDetection/wxEncodingDetection.h>

wxIMPLEMENT_DYNAMIC_CLASS( wxCueSheetReader, wxObject )

wxCueSheetReader::PARSE_STRUCT wxCueSheetReader::parseArray[] = {
	{  wxT("REM"), &wxCueSheetReader::ParseComment },
	{  wxT("INDEX"), &wxCueSheetReader::ParseIndex },
	{  wxT("PREGAP"), &wxCueSheetReader::ParsePreGap },
	{  wxT("POSTGAP"), &wxCueSheetReader::ParsePostGap },
	{  wxT("FILE"), &wxCueSheetReader::ParseFile },
	{  wxT("FLAGS"), &wxCueSheetReader::ParseFlags },
	{  wxT("TRACK"), &wxCueSheetReader::ParseTrack },
	{  wxT("CATALOG"), &wxCueSheetReader::ParseCatalog },
	{  wxT("CDTEXTFILE"), &wxCueSheetReader::ParseCdTextFile }
};

size_t wxCueSheetReader::parseArraySize = sizeof(wxCueSheetReader::parseArray)/sizeof(PARSE_STRUCT);

static const wxChar* INFOS[] = {
	wxT("AudioCount"),
	wxT("CUESHEET"),
	wxT("cuesheet")
};

static const size_t INFOS_SIZE = sizeof(INFOS)/sizeof(const wxChar*);

static const wxChar* AUDIO_INFOS[] = {
	wxT("Format"),
};

static const size_t AUDIO_INFOS_SIZE = sizeof(AUDIO_INFOS)/sizeof(const wxChar*);

wxString wxCueSheetReader::GetKeywordsRegExp()
{
	wxString sKeywordsRegExp(wxCueComponent::GetKeywordsRegExp());
	wxString s;
	s.Printf( wxT("\\A\\s*%s\\s+(\\S.*\\S)\\s*\\Z"), sKeywordsRegExp );
	return s;
}

wxString wxCueSheetReader::GetDataModeRegExp()
{
	wxString sDataModeRegExp( wxTrack::GetDataModeRegExp() );
	wxString s;
	s.Printf( wxT("\\A(\\d{1,2})(?:\\s+%s){0,1}\\Z"), sDataModeRegExp );
	return s;
}

wxString wxCueSheetReader::GetCdTextInfoRegExp()
{
	wxString sRegExp(wxCueComponent::GetCdTextInfoRegExp());
	wxString s;
	s.Printf( wxT("\\A\\s*%s\\s+(\\S.*\\S)\\s*\\Z"), sRegExp );
	return s;
}

wxString wxCueSheetReader::GetDataFileRegExp()
{
	wxString sRegExp(wxDataFile::GetFileTypeRegExp());
	wxString s;
	s.Printf( wxT("\\A((?:\\\".*\\\")|(?:\\'.*\\'))(?:\\s+%s){0,1}\\Z"), sRegExp );
	return s;
}

wxCueSheetReader::wxCueSheetReader(void)
	:m_reKeywords( GetKeywordsRegExp(), wxRE_ADVANCED ),
	 m_reCdTextInfo( GetCdTextInfoRegExp(), wxRE_ADVANCED ),
	 m_reEmpty( wxT("\\A\\s*\\Z"), wxRE_ADVANCED ),
	 m_reIndex( wxT("\\A\\s*(\\d{1,2})\\s+(\\S.*\\S)\\Z"), wxRE_ADVANCED ),
	 m_reMsf( wxT("\\A(\\d{1,4}):(\\d{1,2}):(\\d{1,2})\\Z"), wxRE_ADVANCED ),
	 m_reQuotesEx( wxT("\\'(([^\\'\\u201E\\\u201D]|\\\')*)\\'(?![[:alnum:]])"), wxRE_ADVANCED ),
	 m_reFlags( wxT("\\s+"), wxRE_ADVANCED ),
	 m_reDataMode( GetDataModeRegExp(), wxRE_ADVANCED ),
	 m_reDataFile( GetDataFileRegExp(), wxRE_ADVANCED ),
	 m_reCatalog( wxT("\\d{13}"), wxRE_ADVANCED|wxRE_NOSUB ),
	 m_reIsrc( wxT("([[:upper:]]{2}|00)-{0,1}[[:upper:][:digit:]]{3}-{0,1}[[:digit:]]{5}"), wxRE_ADVANCED|wxRE_NOSUB ),
	 m_reTrackComment( wxT("cue[[.hyphen.][.underscore.][.low-line.]]track([[:digit:]]{1,2})[[.underscore.][.low-line.]]([[:alpha:][.hyphen.][.underscore.][.low-line.][.space.]]+)"), wxRE_ADVANCED|wxRE_ICASE ),
	 m_bErrorsAsWarnings( true ),
	 m_bParseComments( true )
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

	m_unquoter.SetLang( wxT("unk") );
}

const wxCueSheet& wxCueSheetReader::GetCueSheet() const
{
	return m_cueSheet;
}

const wxArrayString& wxCueSheetReader::GetCueLines() const
{
	return m_cueLines;
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

void wxCueSheetReader::CorrectQuotationMarks( bool bCorrectQuotationMarks, const wxString& sLang )
{
	m_unquoter.SetLang( bCorrectQuotationMarks? sLang : wxEmptyString );
}

bool wxCueSheetReader::ReadCueSheet(const wxString& sCueFile, bool bUseMLang )
{
	wxString sCPDescription;
	wxSharedPtr<wxMBConv> pConv( wxEncodingDetection::GetFileEncoding( sCueFile, bUseMLang, sCPDescription ) );
	if ( pConv )
	{
		wxLogInfo( _("Detected encoding of file \u201C%s\u201D file is \u201C%s\u201D"), sCueFile, sCPDescription );
		return ReadCueSheet( sCueFile, *pConv ); 
	}
	else
	{
		return false;
	}
}

bool wxCueSheetReader::ReadCueSheet(const wxString& sCueFile, wxMBConv& conv)
{
	m_cueFileName.Assign( sCueFile );
	m_cueFileName.MakeAbsolute();

	if ( !m_cueFileName.FileExists() || m_cueFileName.IsDir() )
	{
		wxLogError( _("Invalid path to CUE file \u201C%s\u201D."), sCueFile );
		return false;
	}

	wxFileInputStream fis( m_cueFileName.GetFullPath() );
	if ( !fis.IsOk() ) {
		wxLogError( _("Unable to open CUE file \u201C%s\u201D."), sCueFile );
		return false;
	}

	return internalReadCueSheet( fis, conv ) && ParseCue();
}

bool wxCueSheetReader::ReadCueSheet( wxInputStream& stream )
{
	m_cueFileName.Clear();
	return ReadCueSheet( stream, wxConvLocal );
}

bool wxCueSheetReader::ReadCueSheet( wxInputStream& stream, wxMBConv& conv )
{
	return internalReadCueSheet( stream, conv ) && ParseCue();
}

void wxCueSheetReader::ProcessMediaInfoCueSheet( wxString& sCueSheet )
{
	m_unquoter.GetReDoubleQuotes().ReplaceAll( &sCueSheet, wxUnquoter::POLISH_DOUBLE_QUOTES );
	m_reQuotesEx.ReplaceAll( &sCueSheet, wxUnquoter::POLISH_SINGLE_QUOTES );

	// slash -> division slash inside quotes

	// double quotes
	wxRegEx re( wxT("(\\u201E.*?)/([^\\u201E]*?\\u201D)"), wxRE_ADVANCED|wxRE_NEWLINE );
	wxASSERT( re.IsValid() );
	re.ReplaceAll( &sCueSheet, wxT("\\1\u2215\\2") ); // division slash

	// single quotes
	re.Compile( wxT("(\\u201A.*?)/([^\\u201A]*?\\u2019)"), wxRE_ADVANCED|wxRE_NEWLINE );
	wxASSERT( re.IsValid() );
	re.ReplaceAll( &sCueSheet, wxT("\\1\u2215\\2") ); // division slash

	// rest slashes to newline
	sCueSheet.Replace( wxT("/"), wxT("\n") );

	// backs from division slash to slash
	sCueSheet.Replace( wxT("\u2215"), wxT("/") );

	// back to normal quotes

	// double quotes
	re.Compile( wxT("\\u201E([^\\u201E\\u201D\\u201A\\u2019]*)\\u201D"), wxRE_ADVANCED|wxRE_NEWLINE );
	wxASSERT( re.IsValid() );
	re.ReplaceAll( &sCueSheet, wxT("\"\\1\"") );

	// single quotes
	re.Compile( wxT("\\u201A([^\\u201A\\u2019]*)\\u2019"), wxRE_ADVANCED|wxRE_NEWLINE );
	re.ReplaceAll( &sCueSheet, wxT("'\\1'") );
}

bool wxCueSheetReader::ReadCueSheetFromVorbisComment( const wxFlacMetaDataReader& flacReader, bool bUseComments )
{
	if ( !flacReader.HasVorbisComment() )
	{
		wxLogWarning( _("Cannot find Vorbis comments inside FLAC file") );
		return false;
	}

	wxString sCueSheet( flacReader.GetCueSheetFromVorbisComment() );
	if ( sCueSheet.IsEmpty() )
	{
		wxLogWarning( _("Cannot find CUESHEET comment") );
		return false;
	}

	wxStringInputStream is( sCueSheet );
	bool res = ReadCueSheet( is, wxConvUTF8 );
	if ( res )
	{
		m_cueSheet.SetSingleDataFile( flacReader.GetFlacFile() );
	}

	if ( res && bUseComments )
	{
		res = AppendFlacComments( flacReader, false );
	}

	return res;
}

bool wxCueSheetReader::ReadCueSheetFromCueSheetTag( const wxFlacMetaDataReader& flacReader, bool bUseComments )
{
	if ( !flacReader.HasCueSheet() )
	{
		wxLogWarning( _("Cannot find CueSheet tag inside FLAC file") );
		return false;
	}

	const FLAC::Metadata::CueSheet& cueSheet = flacReader.GetCueSheet();

	m_cueSheet.Clear();
	wxString sCatalog( cueSheet.get_media_catalog_number() );
	if ( !sCatalog.IsEmpty() )
	{
		m_cueSheet.SetCatalog( sCatalog );
	}

	for( unsigned int i=0; i<cueSheet.get_num_tracks(); i++ )
	{
		const FLAC::Metadata::CueSheet::Track flacTrack = cueSheet.get_track(i);
		wxTrack track( flacTrack.get_number() );
		wxString sIsrc( flacTrack.get_isrc() );
		if ( !sIsrc.IsEmpty() )
		{
			track.AddCdTextInfo( wxCueTag::Name::ISRC, sIsrc );
		}
		if ( flacTrack.get_pre_emphasis() )
		{
			track.AddFlag( wxTrack::PRE );
		}
		unsigned int numIndicies = flacTrack.get_num_indices();
		for( unsigned int j=0; j<numIndicies; j++ )
		{
			::FLAC__StreamMetadata_CueSheet_Index flacIdx = flacTrack.get_index( j );

			wxIndex idx( flacIdx.number, flacTrack.get_offset() + flacIdx.offset );
			track.AddIndex( idx );
		}

		m_cueSheet.AddTrack( track );
	}
	m_cueSheet.SetSingleDataFile( flacReader.GetFlacFile() );

	if ( bUseComments && flacReader.HasVorbisComment() )
	{
		if ( !AppendFlacComments( flacReader, false ) )
		{
			return false;
		}
	}

	return true;
}

bool wxCueSheetReader::ReadEmbeddedInFlacCueSheet( const wxString& sMediaFile, int nMode )
{
	wxFlacMetaDataReader flacReader;
	if ( !flacReader.ReadMetadata( sMediaFile ) )
	{
		return false;
	}

	bool bReadTags = ((nMode & EC_MEDIA_READ_TAGS) != 0);

	switch( nMode & EC_FLAC_READ_MASK )
	{
		case EC_FLAC_READ_COMMENT_ONLY:
		return ReadCueSheetFromVorbisComment( flacReader, bReadTags );

		case EC_FLAC_READ_TAG_ONLY:
		return ReadCueSheetFromCueSheetTag( flacReader, bReadTags );

		case EC_FLAC_READ_TAG_FIRST_THEN_COMMENT:
		return ReadCueSheetFromCueSheetTag( flacReader, bReadTags ) || ReadCueSheetFromVorbisComment( flacReader, bReadTags );

		case EC_FLAC_READ_COMMENT_FIRST_THEN_TAG:
		return ReadCueSheetFromVorbisComment( flacReader, bReadTags ) || ReadCueSheetFromCueSheetTag( flacReader, bReadTags );

		default:
		return false;
	}

	return true;
}

void wxCueSheetReader::AppendComments( wxArrayCueTag& comments, bool singleMediaFile )
{
	size_t nComments = comments.GetCount();

	for( size_t i=0; i<nComments; i++ )
	{
		wxCueTag& comment = comments[i];

		if ( comment.GetName().CmpNoCase( wxCueTag::Name::CUESHEET ) == 0 ) continue;

		comment.RemoveTrailingSpaces( m_spacesRemover );

		if ( singleMediaFile )
		{ // just add to first track
			wxASSERT( m_cueSheet.HasTrack( 1 ) );
			wxTrack& firstTrack = m_cueSheet.GetTrackByNumber( 1 );
			firstTrack.AddTag( comment );
		}
		else
		{
			if ( comment.GetName().CmpNoCase( wxCueTag::Name::TOTALTRACKS ) == 0 ) continue;

			if ( m_reTrackComment.Matches( comment.GetName() ) )
			{
				wxString sTagNumber( m_reTrackComment.GetMatch( comment.GetName(), 1 ) );
				wxString sTagName( m_reTrackComment.GetMatch( comment.GetName(), 2 ) );
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
						wxLogInfo( _("Skipping track comment %s - track %d not found"), comment.GetName(), trackNumber );
					}
				}
				else
				{
					wxLogDebug( wxT("Invalid track comment regular expression") );
				}
			}
			else
			{
				m_cueSheet.AddTag( comment );
			}
		}
	}
}

bool wxCueSheetReader::AppendFlacComments( const wxFlacMetaDataReader& flacReader, bool singleMediaFile )
{
	wxArrayCueTag comments;
	flacReader.ReadVorbisComments( comments );
	AppendComments( comments, singleMediaFile );
	return true;
}

bool wxCueSheetReader::ReadEmbeddedInWavpackTags( const wxString& sMediaFile, bool singleMediaFile )
{
	wxArrayCueTag comments;
	if ( wxWavpackTagReader::ReadTags( sMediaFile, comments ) )
	{
		AppendComments( comments, singleMediaFile );
		return true;
	}
	else
	{
		return false;
	}
}

bool wxCueSheetReader::ReadEmbeddedInWavpackCueSheet( const wxString& sMediaFile, int nMode )
{
	wxString sCueSheet;
	if ( !wxWavpackTagReader::ReadCueSheetTag( sMediaFile, sCueSheet ) )
	{
		return false;
	}

	wxStringInputStream is( sCueSheet );
	bool res = ReadCueSheet( is, wxConvUTF8 );
	if ( res )
	{
		m_cueSheet.SetSingleDataFile( sMediaFile );
	}

	if ( res )
	{
		bool bReadTags = ((nMode & EC_MEDIA_READ_TAGS) != 0);
		if ( bReadTags )
		{
			res = ReadEmbeddedInWavpackTags( sMediaFile, false );
		}
	}

	return res;
}

bool wxCueSheetReader::ReadEmbeddedCueSheet( const wxString& sMediaFile, int nMode )
{
	// using MediaInfo to get basic information about media
	wxMediaInfo dll;
	if ( !dll.Load() )
	{
		wxLogError( _("Fail to load MediaInfo library") );
		return false;
	}

	wxArrayString as1;
	wxArrayString as2;

	void* handle = dll.MediaInfoNew();
	size_t res = dll.MediaInfoOpen( handle, sMediaFile );
	if ( res == 0 )
	{
		wxLogError( _("MediaInfo - fail to open file \u201C%s\u201D"), sMediaFile );
		dll.MediaInfoDelete( handle );
		dll.Unload();
		return false;
	}

	for( size_t i=0; i<INFOS_SIZE; i++ )
	{
		wxString s1( 
			dll.MediaInfoGet( 
				handle,
				wxMediaInfo::MediaInfo_Stream_General,
				0,
				INFOS[i]
			)
		);

		wxString s2( 
			dll.MediaInfoGet( 
				handle,
				wxMediaInfo::MediaInfo_Stream_General,
				0,
				INFOS[i],
				wxMediaInfo::MediaInfo_Info_Measure
			)
		);

		as1.Add( s1 );
		as2.Add( s2 );
	}

	for( size_t i=0; i<AUDIO_INFOS_SIZE; i++ )
	{
		wxString s1( 
			dll.MediaInfoGet( 
				handle,
				wxMediaInfo::MediaInfo_Stream_Audio,
				0,
				AUDIO_INFOS[i]
			)
		);

		wxString s2( 
			dll.MediaInfoGet( 
				handle,
				wxMediaInfo::MediaInfo_Stream_Audio,
				0,
				AUDIO_INFOS[i],
				wxMediaInfo::MediaInfo_Info_Measure
			)
		);

		as1.Add( s1 );
		as2.Add( s2 );
	}

	dll.MediaInfoClose( handle );
	dll.MediaInfoDelete( handle );
	dll.Unload();

	bool check = true;
	bool singleMediaFile = ((nMode & EC_SINGLE_MEDIA_FILE) != 0);
	MEDIA_TYPE eMediaType = MEDIA_TYPE_UNKNOWN;
	unsigned long u;
	wxString sCueSheet;
	bool bCueSheet = false;

	for( size_t i=0; i<(INFOS_SIZE+AUDIO_INFOS_SIZE); i++ )
	{
		switch( i )
		{
			case 0: // count of audio streams
			if ( !as1[i].ToULong( &u ) || !(u > 0) )
			{
				wxLogWarning( _("MediaInfo - cannot find audio stream") );
				check = false;
			}
			break;

			case 1: // cue sheet #1
			if ( !bCueSheet )
			{
				if ( !as1[i].IsEmpty() )
				{
					sCueSheet = as1[i];
					bCueSheet = true;
					ProcessMediaInfoCueSheet( sCueSheet );
				}
			}
			break;

			case 2: // cue sheet #2
			if ( !bCueSheet )
			{
				if ( !as1[i].IsEmpty() )
				{
					sCueSheet = as1[i];
					bCueSheet = true;
					ProcessMediaInfoCueSheet( sCueSheet );
				}
			}
			break;

			case 3: // format
			if ( as1[i].CmpNoCase( wxT("FLAC") ) == 0 )
			{
				eMediaType = MEDIA_TYPE_FLAC;
			}
			else if ( as1[i].CmpNoCase( wxT("WAVPACK") ) == 0 )
			{
				eMediaType = MEDIA_TYPE_WAVPACK;
			}
			break;
		}
	}

	if ( !singleMediaFile )
	{
		if ( !bCueSheet )
		{
			wxLogWarning( _("MediaInfo - no cue sheet") );
			check = false;
		}
	}

	if ( !check ) return false;

	if ( singleMediaFile )
	{
		bool res = true;
		BuildFromSingleMediaFile( sMediaFile );
		bool bReadTags = (( nMode & EC_MEDIA_READ_TAGS ) != 0);
		if ( bReadTags )
		{
			switch( eMediaType )
			{
				case MEDIA_TYPE_FLAC:
				{
					wxFlacMetaDataReader flacReader;
					if ( flacReader.ReadMetadata( sMediaFile ) && flacReader.HasVorbisComment() )
					{
						res = AppendFlacComments( flacReader, true );
					}
				}
				break;

				case MEDIA_TYPE_WAVPACK:
				{
					res = ReadEmbeddedInWavpackTags( sMediaFile, true );
				}
				break;
			}
		}
		return res;
	}
	else
	{
		switch( eMediaType )
		{
			case MEDIA_TYPE_FLAC:
			return ReadEmbeddedInFlacCueSheet( sMediaFile, nMode );

			case MEDIA_TYPE_WAVPACK:
			return ReadEmbeddedInWavpackCueSheet( sMediaFile, nMode );

			default:
			{
				wxStringInputStream is( sCueSheet );
				return ReadCueSheet( is, wxConvUTF8 );
			}
		}
	}
}

void wxCueSheetReader::BuildFromSingleMediaFile( const wxString& sMediaFile )
{ // one track, one index
	m_cueSheet.Clear();
	wxTrack singleTrack( 1 );
	wxIndex singleIdx( 1, wxULL(0) );
	singleTrack.AddIndex( singleIdx );
	m_cueSheet.AddTrack( singleTrack );
	m_cueSheet.SetSingleDataFile( sMediaFile );
}

bool wxCueSheetReader::internalReadCueSheet(wxInputStream &stream, wxMBConv& conv )
{
	wxTextInputStream tis( stream, wxT(" \t"), conv );
	m_cueSheet.Clear();
	m_cueLines.Clear();
	while( !stream.Eof() )
	{
		wxString sLine( tis.ReadLine() );
		m_cueLines.Add( sLine );
	}
	return true;
}

bool wxCueSheetReader::IsTrack() const
{
	return ( m_cueSheet.GetTracks().Count() > 0 );
}

wxTrack& wxCueSheetReader::GetLastTrack()
{
	wxASSERT( IsTrack() );
	return m_cueSheet.GetLastTrack();
}

const wxTrack& wxCueSheetReader::GetLastTrack() const
{
	wxASSERT( IsTrack() );
	return m_cueSheet.GetLastTrack();
}


void wxCueSheetReader::AddError0( const wxString& sMsg )
{
	m_errors.Add( sMsg );
}

void wxCueSheetReader::AddError( const wxChar* pszFormat, ... )
{
	va_list argptr;
	va_start(argptr, pszFormat);
	wxString s;
	s.PrintfV( pszFormat, argptr );
	AddError0( s );
	va_end( argptr );
}

void wxCueSheetReader::DumpErrors( size_t nLine ) const
{
	if ( m_errors.Count() > 0 )
	{
		for( wxArrayString::const_iterator i=m_errors.begin(); i != m_errors.end(); i++ )
		{
			if ( m_bErrorsAsWarnings )
			{
				wxLogWarning( _("Line %d: %s"), nLine, i->GetData() );
			}
			else
			{
				wxLogError( _("Line %d: %s"), nLine, i->GetData() );
			}
		}
	}
}

bool wxCueSheetReader::CheckEntryType( wxCueComponent::ENTRY_TYPE et ) const
{
	if ( IsTrack() )
	{
		return GetLastTrack().CheckEntryType( et );
	}
	else
	{
		return m_cueSheet.CheckEntryType( et );
	}
}

wxString wxCueSheetReader::Unquote( const wxString& qs )
{
	return m_unquoter.UnquoteAndCorrect( qs );
}

void wxCueSheetReader::ParseLine( size_t WXUNUSED(nLine), const wxString& sToken, const wxString& sRest )
{
	for( size_t i=0; i<parseArraySize; i++ )
	{
		if ( sToken.CmpNoCase( parseArray[i].token ) == 0 )
		{
			wxCueComponent::ENTRY_TYPE et;
			wxCueComponent::GetEntryType( parseArray[i].token, et );
			m_errors.Clear();
			if ( CheckEntryType( et ) )
			{
				PARSE_METHOD method = parseArray[i].method;
				(this->*method)( sToken, sRest );
			}
			else
			{
				AddError( _("Keyword %s is not allowed here"), sToken );
			}
			return;
		}
	}

	m_errors.Clear();
	AddError( _("Unknown token %s"), sToken );
}

void wxCueSheetReader::ParseCdTextInfo( size_t WXUNUSED(nLine), const wxString& sToken, const wxString& sBody )
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
	if ( sToken.CmpNoCase( wxT("ISRC") ) == 0 )
	{
		if ( !m_reIsrc.Matches( sBody ) )
		{
			AddError0( _("Invalid ISRC code") );
			add = false;
		}
	}

	if ( add )
	{
		if ( !AddCdTextInfo( sToken, s ) )
		{
			AddError( _("Keyword %s is not allowed here"), sToken );
		}
	}
}

void wxCueSheetReader::ParseGarbage( const wxString& sLine )
{
	if ( IsTrack() )
	{
		GetLastTrack().ParseGarbage( sLine );
	}
	else
	{
		m_cueSheet.ParseGarbage( sLine );
	}
}

void wxCueSheetReader::ParseComment( const wxString& WXUNUSED(sToken), const wxString& sComment )
{
	if ( IsTrack() )
	{
		GetLastTrack().ParseComment( sComment, m_bParseComments );
	}
	else
	{
		m_cueSheet.ParseComment( sComment, m_bParseComments );
	}
}

bool wxCueSheetReader::ParseCue()
{
	m_dataFile.Clear();

	bool res = true;
	size_t nLine = 1;
	for( wxArrayString::const_iterator i = m_cueLines.begin(); i != m_cueLines.end(); i++,nLine++ )
    {
		if ( m_reEmpty.Matches( *i ) )
		{
			wxLogDebug( wxT("Skipping empty line %d."), nLine );
			continue;
		}

		if ( m_reKeywords.Matches( *i ) )
		{
			wxASSERT( m_reKeywords.GetMatchCount() == 3 );
			wxString sToken = m_reKeywords.GetMatch( *i, 1 );
			wxString sRest = m_reKeywords.GetMatch( *i, 2 );
			m_errors.Clear();
			ParseLine( nLine, sToken, sRest );
			if ( m_errors.Count() > 0 ) 
			{
				DumpErrors( nLine );
				ParseGarbage( *i );
				res = false;
			}
		}
		else if ( m_reCdTextInfo.Matches( *i ) )
		{
			wxASSERT( m_reCdTextInfo.GetMatchCount() == 3 );
			wxString sToken = m_reCdTextInfo.GetMatch( *i, 1 );
			wxString sRest = m_reCdTextInfo.GetMatch( *i, 2 );
			m_errors.Clear();
			ParseCdTextInfo( nLine, sToken, sRest );
			if ( m_errors.Count() > 0 ) 
			{
				DumpErrors( nLine );
				ParseGarbage( *i );
				res = false;
			}
		}
		else
		{
			wxLogWarning( _("Incorrect line %d: %s"), nLine, i->GetData() );
			ParseGarbage( *i );
			res = false;
		}
    }

	m_cueSheet.SortTracks();
	return res;
}

bool wxCueSheetReader::AddCdTextInfo( const wxString& sToken, const wxString& sBody )
{
	if ( IsTrack() )
	{
		return GetLastTrack().AddCdTextInfo( sToken, m_spacesRemover.Remove( sBody ) );
	}
	else
	{
		return m_cueSheet.AddCdTextInfo( sToken, m_spacesRemover.Remove( sBody ) );
	}
}

bool wxCueSheetReader::ParseMsf(const wxString& sBody, wxIndex& idx, bool bPrePost )
{
	bool res = true;
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
		res = idx.Assign( 0, min, sec, frames ).IsValid(bPrePost);
	}

	return res;
}

void wxCueSheetReader::ParsePreGap( const wxString& WXUNUSED(sToken), const wxString& sBody )
{
	wxIndex idx;
	if ( ParseMsf( sBody, idx, true ) )
	{
		GetLastTrack().SetPreGap( idx );
	}
	else
	{
		AddError0( _("Invalid index specification") );
	}
}

void wxCueSheetReader::ParsePostGap( const wxString& WXUNUSED(sToken), const wxString& sBody )
{
	wxIndex idx;
	if ( ParseMsf( sBody, idx, true ) )
	{
		GetLastTrack().SetPostGap( idx );
	}
	else
	{
		AddError0( _("Invalid index specification") );
	}
}

void wxCueSheetReader::ParseIndex( const wxString& WXUNUSED(sToken), const wxString& sBody )
{
	if ( m_reIndex.Matches( sBody ) )
	{
		unsigned long number;
		if ( !m_reIndex.GetMatch( sBody, 1 ).ToULong( &number ) )
		{
			AddError0( _("Invalid index specification") );
		}
		else
		{
			wxString sMsf = m_reIndex.GetMatch( sBody, 2 );
			wxIndex idx;
			if ( ParseMsf( sMsf, idx ) )
			{
				idx.SetNumber( number );
				if ( idx.IsValid() )
				{
					GetLastTrack().AddIndex( idx );
				}
				else
				{
					AddError0( _("Invalid index specification") );
				}
			}
			else
			{
				AddError0( _("Invalid index specification") );
			}
		}
	}
	else
	{
		AddError0( _("Invalid index specification") );
	}
}


void wxCueSheetReader::ParseFile( const wxString& WXUNUSED(sToken), const wxString& sBody )
{
	if ( m_reDataFile.Matches( sBody ) )
	{
		wxString sFile( m_reDataFile.GetMatch( sBody, 1 ) );
		wxString sType( m_reDataFile.GetMatch( sBody, 2 ) );

		wxDataFile::FileType ftype = wxDataFile::BINARY;
		if ( !sType.IsEmpty() )
		{
			wxDataFile::StringToFileType( sType, ftype );
		}

		wxFileName fn( Unquote( sFile ) );
		if ( m_cueFileName.IsOk() )
		{
			fn.SetPath( m_cueFileName.GetPath() );
		}

		m_dataFile.Assign( fn, ftype );
	}
}

void wxCueSheetReader::ParseFlags( const wxString& WXUNUSED(sToken), const wxString& sBody )
{
	wxString sFlags( sBody );
	m_reFlags.ReplaceAll( &sFlags, wxT('|') );
	wxStringTokenizer tokenizer( sFlags, wxT("|") );
	while( tokenizer.HasMoreTokens() )
	{
		wxString sFlag( tokenizer.GetNextToken() );
		if ( !GetLastTrack().AddFlag( sFlag ) )
		{
			AddError( _("Invalid flag %s"), sFlag );
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
			AddError( _("Invalid track number %s"), m_reDataMode.GetMatch( sBody, 1 ) );
		}
		else
		{
			wxString sMode( m_reDataMode.GetMatch( sBody, 2 ) );
			wxTrack newTrack( trackNo );
			if ( newTrack.IsValid() && newTrack.SetMode( sMode ) )
			{
				if ( !m_dataFile.IsEmpty() )
				{
					newTrack.SetDataFile( m_dataFile );
					m_dataFile.Clear();
				}
				m_cueSheet.AddTrack( newTrack );
			}
			else
			{
				AddError0( _("Invalid track specification") );
			}
		}
	}
	else
	{
		AddError0( _("Invalid track specification") );
	}
}

void wxCueSheetReader::ParseCatalog( const wxString& WXUNUSED(sToken), const wxString& sBody )
{
	if ( m_reCatalog.Matches( sBody ) )
	{
		m_cueSheet.SetCatalog( sBody );
	}
	else
	{
		AddError0( _("Invalid catalog number") );
	}
}

void wxCueSheetReader::ParseCdTextFile( const wxString& WXUNUSED(sToken), const wxString& sBody )
{
	m_cueSheet.SetCdTextFile( Unquote( sBody ) );
}
