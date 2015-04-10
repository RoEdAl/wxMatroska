/*
 * wxCueSheetReader.cpp
 */

#include "StdWx.h"
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheetContent.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxEncodingDetection/wxEncodingDetection.h>
#include <wxEncodingDetection/wxTextInputStreamOnString.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxCueSheetReader, wxObject );

// ===============================================================================

const char wxCueSheetReader::LOG_EXT[]	= "log";
const char wxCueSheetReader::LOG_MASK[] = "*.log";

// ===============================================================================

const wxCueSheetReader::PARSE_STRUCT wxCueSheetReader::parseArray[] =
{
	{ "REM", &wxCueSheetReader::ParseComment },
	{ "INDEX", &wxCueSheetReader::ParseIndex },
	{ "PREGAP", &wxCueSheetReader::ParsePreGap },
	{ "POSTGAP", &wxCueSheetReader::ParsePostGap },
	{ "FILE", &wxCueSheetReader::ParseFile },
	{ "FLAGS", &wxCueSheetReader::ParseFlags },
	{ "TRACK", &wxCueSheetReader::ParseTrack },
	{ "CATALOG", &wxCueSheetReader::ParseCatalog },
	{ "CDTEXTFILE", &wxCueSheetReader::ParseCdTextFile }
};

// ===============================================================================

namespace
{
    template<size_t FMTS, typename G>
    inline wxString build_regex_str( const char( &fmt )[FMTS], G getter )
    {
        wxString se( getter() );
        wxString s;

        s.Printf( fmt, se );
        return s;
    }

    template<typename G>
    inline wxString build_component_regex_str( G getter )
    {
        return wxString::Format( wxCueComponent::REG_EXP_FMT, getter() );
    }

    struct reg_exp
    {
        static inline wxString keywords()
        {
            return build_component_regex_str( wxCueComponent::GetKeywordsRegExp );
        }

        static inline wxString data_mode()
        {
            return build_regex_str( "\\A(\\d{1,2})(?:\\s+%s){0,1}\\Z", wxTrack::GetDataModeRegExp );
        }

        static inline wxString cd_text_info()
        {
            return build_component_regex_str( wxCueComponent::GetCdTextInfoRegExp );
        }

        static inline wxString data_file()
        {
            return build_regex_str( "\\A((?:\\\".*\\\")|(?:\\'.*\\'))(?:\\s+%s){0,1}\\Z", wxDataFile::GetFileTypeRegExp );
        }
    };
}


bool wxCueSheetReader::GetLogFile( const wxFileName& inputFile, bool bAnyLog, wxFileName& logFile )
{
	wxASSERT( inputFile.IsOk() );

	if ( bAnyLog )	// any log file in the same directory
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

wxString wxCueSheetReader::GetOneTrackCue()
{
	wxTextOutputStreamOnString tos;

	( *tos ) <<
		"REM one-track CUE sheet" << endl <<
		"FILE \"%source%\" WAVE" << endl <<
		"\tTRACK 01 AUDIO" << endl <<
		"\tINDEX 01 00:00:00" << endl;
	tos->Flush();

	return tos.GetString();
}

bool wxCueSheetReader::TestReadFlags( ReadFlags nMask ) const
{
	return ( m_nReadFlags & nMask ) == nMask;
}

wxString wxCueSheetReader::GetTagLibVersion()
{
	return wxString::Format( wxS("TagLib version: %d.%d.%d. Copyright \u00A9 2002 - 2008 by Scott Wheeler"), TAGLIB_MAJOR_VERSION, TAGLIB_MINOR_VERSION, TAGLIB_PATCH_VERSION );
}

#ifdef __WXDEBUG__

void TagLibDebugListener::printMessage( const TagLib::String& msg )
{
	wxLogDebug( msg.toCWString() );
}
#endif

wxCueSheetReader::wxCueSheetReader( void ):
	m_reKeywords( reg_exp::keywords(), wxRE_ADVANCED ),
	m_reCdTextInfo( reg_exp::cd_text_info(), wxRE_ADVANCED ),
	m_reEmpty( "\\A\\s*\\Z", wxRE_ADVANCED ),
	m_reIndex( "\\A\\s*(\\d{1,2})\\s+(\\S.*\\S)\\Z", wxRE_ADVANCED ),
	m_reMsf( "\\A(\\d{1,4}):(\\d{1,2}):(\\d{1,2})\\Z", wxRE_ADVANCED ),
	m_reQuotesEx( wxUnquoter::RE_SINGLE_QUOTES_EX, wxRE_ADVANCED ),
	m_reFlags( "\\s+", wxRE_ADVANCED ),
	m_reDataMode( reg_exp::data_mode(), wxRE_ADVANCED ),
	m_reDataFile( reg_exp::data_file(), wxRE_ADVANCED ),
	m_reCatalog( "\\d{13}", wxRE_ADVANCED | wxRE_NOSUB ),
	m_reIsrc( "\\A([[:upper:]]{2}|00)-{0,1}[[:upper:][:digit:]]{3}-{0,1}[[:digit:]]{2}-{0,1}[[:digit:]]{5}\\Z", wxRE_ADVANCED | wxRE_NOSUB ),
	m_reTrackComment( "cue[[.hyphen.][.underscore.][.low-line.]]track([[:digit:]]{1,2})[[.underscore.][.low-line.]]([[:alpha:][.hyphen.][.underscore.][.low-line.][.space.]]+)", wxRE_ADVANCED | wxRE_ICASE ),
	m_reCommentMeta( "\\A([[.quotation-mark.]]{0,1})([[:upper:][.hyphen.][.underscore.][:space:][.low-line.]]+)\\1[[:space:]]+([^[:space:]].+)\\Z", wxRE_ADVANCED ),
	m_bErrorsAsWarnings( true ),
	m_nReadFlags( EC_PARSE_COMMENTS | EC_ELLIPSIZE_TAGS | EC_REMOVE_EXTRA_SPACES | EC_MEDIA_READ_TAGS | EC_FIND_COVER | EC_FIND_LOG | EC_CONVERT_UPPER_ROMAN_NUMERALS ),
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

#ifdef __WXDEBUG__
	TagLib::setDebugListener( &m_debugListener );
#endif
}

#ifdef __WXDEBUG__
wxCueSheetReader::~wxCueSheetReader()
{
	TagLib::setDebugListener( nullptr );
}
#endif

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

wxCueSheetReader::ReadFlags wxCueSheetReader::GetReadFlags() const
{
	return m_nReadFlags;
}

wxCueSheetReader& wxCueSheetReader::SetReadFlags( wxCueSheetReader::ReadFlags nReadFlags )
{
	m_nReadFlags = nReadFlags;
	return *this;
}

bool wxCueSheetReader::FindLog( const wxCueSheetContent& content )
{
	wxASSERT( TestReadFlags( EC_FIND_LOG ) );
	wxFileName logFile;

	if ( GetLogFile( content.GetSource().GetFileName(), TestReadFlags( EC_SINGLE_MEDIA_FILE ), logFile ) )
	{
		m_cueSheet.AddLog( logFile );
		return true;
	}
	else
	{
		return false;
	}
}

bool wxCueSheetReader::FindCover( const wxCueSheetContent& content )
{
	wxASSERT( TestReadFlags( EC_FIND_COVER ) );
	wxFileName coverFile;

	if ( wxCoverFile::Find( content.GetSource().GetFileName(), coverFile ) )
	{
		AddCover( coverFile );
		return true;
	}
	else
	{
		return false;
	}
}

bool wxCueSheetReader::ReadCueSheet( const wxString& sCueFile, bool bUseMLang )
{
	wxString							   sCPDescription;
	wxEncodingDetection::wxMBConvSharedPtr pConv( wxEncodingDetection::GetFileEncoding( sCueFile, bUseMLang, sCPDescription ) );

	if ( pConv )
	{
		wxLogInfo( _( "Detected encoding of file \u201C%s\u201D file is \u201C%s\u201D" ), sCueFile, sCPDescription );
		return ReadCueSheet( sCueFile, *pConv );
	}
	else
	{
		return false;
	}
}

bool wxCueSheetReader::ReadCueSheet( const wxString& sCueFile, wxMBConv& conv )
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

	wxDataFile dataFile( cueFileName );
	dataFile.FindFile();

	return ParseCue( wxCueSheetContent( internalReadCueSheet( fis, conv ), dataFile, false ) );
}

bool wxCueSheetReader::ReadCueSheet( wxInputStream& stream )
{
	return ReadCueSheet( stream, wxConvLocal );
}

bool wxCueSheetReader::ReadCueSheet( wxInputStream& stream, wxMBConv& conv )
{
	return ParseCue( wxCueSheetContent( internalReadCueSheet( stream, conv ) ) );
}

void wxCueSheetReader::AppendTags( const wxArrayCueTag& comments, bool bSingleMediaFile )
{
	for ( size_t i = 0, nComments = comments.GetCount(); i < nComments; ++i )
	{
		wxCueTag comment( comments[ i ] );

		if ( comment == wxCueTag::Name::CUESHEET )
		{
			continue;
		}

		comment.Unquote( m_unquoter );
        CorrectTag( comment );

		if ( bSingleMediaFile )	// just add to first track
		{
            if (comment == wxCueTag::Name::TRACKNUMBER)
            {
                continue;
            }

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
					wxLogDebug( "Invalid track comment regular expression" );
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
	for ( size_t i = 0, nTags = tags.GetCount(); i < nTags; ++i )
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

        if (tag == wxCueTag::Name::TRACKNUMBER)
        {
            continue;
        }

		tag.Unquote( m_unquoter );
        CorrectTag( tag );

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
				wxLogDebug( wxS( "Invalid track comment regular expression" ) );
			}
		}
		else
		{
			for ( size_t j = nTrackFrom; j <= nTrackTo; ++j )
			{
				m_cueSheet.GetTrack( j ).AddTag( tag );
			}
		}
	}
}

bool wxCueSheetReader::ReadCueSheetEx( const wxString& sFile, bool bUseMLang )
{
	wxDataFile dataFile( sFile, wxDataFile::WAVE );

	if ( dataFile.GetInfo( m_sAlternateExt ) )
	{	// embedded or single
		bool bSingleMediaFile = TestReadFlags( EC_SINGLE_MEDIA_FILE );
		bool bCueSheet		  = dataFile.HasCueSheet();

		if ( bSingleMediaFile )
		{
			return BuildFromSingleMediaFile( dataFile );
		}
		else
		{
			if ( bCueSheet )
			{
				return ParseCue( wxCueSheetContent( dataFile ) );
			}
			else
			{
				wxLogWarning( _( "Cannot find cue sheet" ) );
				return false;
			}
		}
	}
	else
	{	// assuming normal text file
		return ReadCueSheet( sFile, bUseMLang );
	}
}

/* one track, one index*/
bool wxCueSheetReader::BuildFromSingleMediaFile( const wxDataFile& mediaFile )
{
	m_cueSheet.Clear();
	wxString sOneTrackCue( m_sOneTrackCue );
	size_t	 nRepl = sOneTrackCue.Replace( "%source%", mediaFile.GetRealFileName().GetFullPath() );
	wxASSERT( nRepl > 0 );

	if ( ParseCue( wxCueSheetContent( sOneTrackCue, mediaFile, true ) ) )
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
	wxTextInputStream		   tis( stream, " \t", conv );
	wxTextOutputStreamOnString tos;

	m_cueSheet.Clear();

	wxString sLine;

	while ( !stream.Eof() )
	{
		sLine = tis.ReadLine();

		if ( sLine.IsEmpty() )
		{
			*tos << endl;
		}
		else
		{
			*tos << sLine << endl;
		}
	}

	return tos.GetString();
}

void wxCueSheetReader::AddError0( const wxString& sMsg )
{
	m_errors.Add( sMsg );
}

void wxCueSheetReader::AddError( const wxString& sFormat, ... )
{
	va_list argptr;
    wxString s;
    int nRes;

	va_start( argptr, sFormat );
	nRes = s.PrintfV( sFormat, argptr );
	va_end( argptr );

    if (nRes > 0) AddError0( s );
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
				wxLogWarning( _( "Line %" wxSizeTFmtSpec "d: %s" ), nLine, i->GetData() );
			}
			else
			{
				wxLogError( _( "Line %" wxSizeTFmtSpec "d: %s" ), nLine, i->GetData() );
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

template< size_t SIZE >
bool wxCueSheetReader::ParseLine( const wxString& sToken, const wxString& sRest, const PARSE_STRUCT(&pa)[ SIZE ] )
{
	for ( size_t i = 0; i < SIZE; ++i )
	{
		if ( sToken.CmpNoCase( pa[ i ].token ) == 0 )
		{
			wxCueComponent::ENTRY_TYPE et;
			wxCueComponent::GetEntryType( pa[ i ].token, et );
			m_errors.Clear();

			if ( CheckEntryType( et ) )
			{
				PARSE_METHOD method = pa[ i ].method;
				( this->*method )( sToken, sRest );
			}
			else
			{
				AddError( _( "Keyword %s is not allowed here" ), sToken );
			}

			return true;
		}
	}

	return false;
}

void wxCueSheetReader::ParseLine( size_t WXUNUSED( nLine ), const wxString& sToken, const wxString& sRest )
{
	if ( !ParseLine( sToken, sRest, parseArray ) )
	{
		m_errors.Clear();
		AddError( _( "Unknown token %s" ), sToken );
	}
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

	if ( sToken.CmpNoCase( wxCueTag::Name::ISRC ) == 0 )
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

void wxCueSheetReader::CorrectTag( wxCueTag& tag ) const
{
    tag.RemoveTrailingSpaces( m_trailingSpacesRemover );

    if (TestReadFlags( EC_REMOVE_EXTRA_SPACES ))
    {
        tag.RemoveExtraSpaces( m_reduntantSpacesRemover );
    }

    if (TestReadFlags( EC_ELLIPSIZE_TAGS ))
    {
        tag.Ellipsize( m_ellipsizer );
    }

    if (TestReadFlags( EC_CONVERT_UPPER_ROMAN_NUMERALS ))
    {
        tag.ConvertRomanNumerals( m_romanNumveralsConvUpper );
    }

    if (TestReadFlags( EC_CONVERT_LOWER_ROMAN_NUMERALS ))
    {
        tag.ConvertRomanNumerals( m_romanNumveralsConvLower );
    }

    if (TestReadFlags( EC_CORRECT_DASHES ))
    {
        tag.CorrectDashes( m_dashesCorrector );
    }
}

void wxCueSheetReader::CorrectString( wxString& str ) const
{
    str = m_trailingSpacesRemover.Remove( str );

    if (TestReadFlags( EC_REMOVE_EXTRA_SPACES ))
    {
        m_reduntantSpacesRemover.Remove( str );
    }

    if (TestReadFlags( EC_ELLIPSIZE_TAGS ))
    {
        str = m_ellipsizer.Ellipsize( str );
    }

    if (TestReadFlags( EC_CONVERT_UPPER_ROMAN_NUMERALS ))
    {
        str = m_romanNumveralsConvUpper.Convert( str );
    }

    if (TestReadFlags( EC_CONVERT_LOWER_ROMAN_NUMERALS ))
    {
        str = m_romanNumveralsConvLower.Convert( str );
    }

    if (TestReadFlags( EC_CORRECT_DASHES ))
    {
        m_dashesCorrector.Replace( str );
    }
}

void wxCueSheetReader::ParseComment( wxCueComponent& component, const wxString& sComment )
{
	if ( !TestReadFlags( EC_PARSE_COMMENTS ) )
	{
		component.AddComment( sComment );
		return;
	}

	if ( m_reCommentMeta.Matches( sComment ) )
	{
        wxCueTag comment( wxCueTag::TAG_CUE_COMMENT, m_reCommentMeta.GetMatch( sComment, 2 ), m_genericUnquoter.Unquote( m_reCommentMeta.GetMatch( sComment, 3 ) ) );
        CorrectTag( comment );
		component.AddTag( comment );
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

bool wxCueSheetReader::ParseCue( const wxCueSheetContent& content )
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
			wxLogDebug( "Skipping empty line %" wxSizeTFmtSpec "u", nLine );
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

	if ( TestReadFlags( EC_MEDIA_READ_TAGS ) )
	{
		ReadTagsFromRelatedFiles();
	}

	if ( content.HasSource() )
	{
		if ( TestReadFlags( EC_FIND_LOG ) )
		{
			FindLog( content );
		}

		if ( TestReadFlags( EC_FIND_COVER ) )
		{
			if ( TestReadFlags( EC_MEDIA_READ_TAGS ) )
			{
				FindCoversInRelatedFiles();
			}
			FindCover( content );
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
		wxLogWarning( _( "Incorrect line %" wxSizeTFmtSpec "d: %s" ), nLine, sLine );
		ParseGarbage( sLine );
		res = false;
	}

	return res;
}

bool wxCueSheetReader::AddCdTextInfo( const wxString& sToken, const wxString& sBody )
{
	wxString sModifiedBody( sBody );
    CorrectString( sModifiedBody );

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
			if ( m_cueSheetContent.IsEmbedded() )	// the only source
			{
				m_cueSheet.AddDataFile( m_cueSheetContent.GetSource() );
			}
			else
			{
				fn.SetPath( m_cueSheetContent.GetSource().GetFileName().GetPath() );
				m_cueSheet.AddDataFile( wxDataFile( fn, ftype ) );
			}
		}
		else
		{
			m_cueSheet.AddDataFile( wxDataFile( fn, ftype ) );
		}
	}
}

void wxCueSheetReader::ParseFlags( const wxString& WXUNUSED( sToken ), const wxString& sBody )
{
	wxString sFlags( sBody );

	m_reFlags.ReplaceAll( &sFlags, '|' );
	wxStringTokenizer tokenizer( sFlags, '|' );
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
	wxFileName cdTextFile( Unquote( sBody ) );

	if ( cdTextFile.GetPath().IsEmpty() && m_cueSheetContent.HasSource() )
	{
		cdTextFile.SetPath( m_cueSheetContent.GetSource().GetFileName().GetPath() );
	}

	m_cueSheet.AddCdTextFile( cdTextFile );
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

	for ( size_t i = 0, nCount = dataFiles.GetCount(); i < nCount; ++i )
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
			wxLogWarning( _( "Data file \u201C%s\u201D is not related to any track" ), dataFiles[ i ].GetRealFileName().GetName() );
		}
	}

	return bRes;
}

void wxCueSheetReader::FindCoversInRelatedFiles()
{
	if ( !m_cueSheet.CalculateDuration( m_sAlternateExt ) )
	{
		return;
	}

	const wxArrayDataFile& dataFiles = m_cueSheet.GetDataFiles();

	for ( size_t i = 0, nCount = dataFiles.GetCount(); i < nCount; ++i )
	{
		ExtractCoversFromDataFile( dataFiles[ i ] );
	}
}

bool wxCueSheetReader::ReadTagsFromMediaFile( const wxDataFile& _dataFile, size_t nTrackFrom, size_t nTrackTo )
{
	if ( _dataFile.HasRealFileName() )
	{
		AppendTags( _dataFile.GetTags(), nTrackFrom, nTrackTo );
		return true;
	}
	else
	{
		wxDataFile dataFile( _dataFile );

		if ( dataFile.GetInfo() )
		{
			AppendTags( dataFile.GetTags(), nTrackFrom, nTrackTo );
			return true;
		}
		else
		{
			wxLogError( _( "Cannot read metadata from \u201C%s\u201D" ), dataFile.GetFileName().GetFullName() );
			return false;
		}
	}
}

void wxCueSheetReader::ExtractCoversFromDataFile( const wxDataFile& dataFile )
{
	if ( !dataFile.HasRealFileName() ) { return; }

	wxArrayCoverFile covers;
	wxCoverFile::Extract( dataFile.GetRealFileName(), covers );
	AddCovers( covers );
}

void wxCueSheetReader::AddCover( const wxFileName& fn )
{
    m_cueSheet.AddCover( fn.GetFullPath() );
}

void wxCueSheetReader::AddCovers( const wxArrayCoverFile& covers )
{
    m_cueSheet.AddCovers( covers );
}
