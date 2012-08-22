/*
 * wxConfiguration.cpp
 */

#include "StdWx.h"
#include <enum2str.h>
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include "wxConfiguration.h"
#include "wxApp.h"

// ===============================================================================

const wxChar wxConfiguration::CUE_SHEET_EXT[]		  = wxS( "cue" );
const wxChar wxConfiguration::MATROSKA_CHAPTERS_EXT[] = wxS( "mkc.xml" );
const wxChar wxConfiguration::MATROSKA_TAGS_EXT[]	  = wxS( "mkt.xml" );
const wxChar wxConfiguration::MATROSKA_OPTS_EXT[]	  = wxS( "opt.txt" );
const wxChar wxConfiguration::MATROSKA_AUDIO_EXT[]	  = wxS( "mka" );
const wxChar wxConfiguration::CUESHEET_EXT[]		  = wxS( "cue" );
const wxChar wxConfiguration::TRACK_NAME_FORMAT[]	  = wxS( "%dp% - %dt% - %tt%" );
const wxChar wxConfiguration::MATROSKA_NAME_FORMAT[]  = wxS( "%dp% - %dt%" );
const size_t wxConfiguration::MAX_EXT_LEN			  = 20;
const wxChar wxConfiguration::LANG_FILE_URL[]		  = wxS( "http://www.loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt" );
const wxChar wxConfiguration::LANG_FILE_NAME[]		  = wxS( "ISO-639-2_utf-8.txt" );

// ===============================================================================

const wxConfiguration::CuesheetAttachModeName wxConfiguration::AttachModeNames[] =
{
	{ CUESHEET_ATTACH_NONE, wxS( "none" ) },
	{ CUESHEET_ATTACH_SOURCE, wxS( "source" ) },
	{ CUESHEET_ATTACH_DECODED, wxS( "decoded" ) },
	{ CUESHEET_ATTACH_RENDERED, wxS( "rendered" ) }
};

// ===============================================================================

const wxConfiguration::RenderModeName wxConfiguration::RenderModeNames[] =
{
	{ RENDER_CUE_SHEET, wxS( "cuesheet" ) },
	{ RENDER_MATROSKA_CHAPTERS, wxS( "matroska" ) },
	{ RENDER_WAV2IMG_CUE_POINTS, wxS( "wav2img" ) }
};

wxString wxConfiguration::GetRenderingModes()
{
	return get_texts( RenderModeNames );
}

// ===============================================================================

const wxConfiguration::INFO_SUBJECT_DESC wxConfiguration::InfoSubjectDesc[] =
{
	{ INFO_VERSION, wxT( "version" ) },
	{ INFO_USAGE, wxT( "usage" ) },
	{ INFO_FORMATTING_DIRECTIVES, wxT( "formatting" ) },
	{ INFO_LICENSE, wxT( "license" ) }
};

// ===============================================================================

wxString wxConfiguration::GetInfoSubjectTexts()
{
	return get_texts( InfoSubjectDesc );
}

bool wxConfiguration::FromString( const wxString& s, wxConfiguration::INFO_SUBJECT& e )
{
	return from_string( s, e, InfoSubjectDesc );
}

wxString wxConfiguration::ToString( wxConfiguration::INFO_SUBJECT e )
{
	return to_string( e, InfoSubjectDesc );
}

wxString wxConfiguration::ToString( wxConfiguration::RENDER_MODE e )
{
	return to_string( e, RenderModeNames );
}

bool wxConfiguration::FromString( const wxString& s, wxConfiguration::RENDER_MODE& e )
{
	return from_string( s, e, RenderModeNames );
}

wxString wxConfiguration::ToString( wxConfiguration::FILE_ENCODING eFileEncoding )
{
	wxString s;

	switch ( eFileEncoding )
	{
		case ENCODING_LOCAL:
		{
			s = wxS( "LOCAL" );
			break;
		}

		case ENCODING_UTF8:
		{
			s = wxS( "UTF-8" );
			break;
		}

		case ENCODING_UTF8_WITH_BOM:
		{
			s = wxS( "UTF-8 (BOM)" );
			break;
		}

		case ENCODING_UTF16:
		{
			s = wxS( "UTF-16" );
			break;
		}

		case ENCODING_UTF16_WITH_BOM:
		{
			s = wxS( "UTF-16 (BOM)" );
			break;
		}

		default:
		{
			s.Printf( wxS( "UNKNOWN %d" ), eFileEncoding );
			break;
		}
	}

	return s;
}

bool wxConfiguration::FromString( const wxString& sFileEncoding, wxConfiguration::FILE_ENCODING& eFileEncoding )
{
	if (
		sFileEncoding.CmpNoCase( wxS( "local" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxS( "default" ) ) == 0
		)
	{
		eFileEncoding = ENCODING_LOCAL;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( wxS( "utf8" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxS( "utf-8" ) ) == 0
		)
	{
		eFileEncoding = ENCODING_UTF8;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( wxS( "utf8_bom" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxS( "utf8bom" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxS( "utf-8_bom" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxS( "utf-8-bom" ) ) == 0
		)
	{
		eFileEncoding = ENCODING_UTF8_WITH_BOM;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( wxS( "utf16" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxS( "utf-16" ) ) == 0
		)
	{
		eFileEncoding = ENCODING_UTF16;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( wxS( "utf16_bom" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxS( "utf16bom" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxS( "utf-16_bom" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxS( "utf-16-bom" ) ) == 0
		)
	{
		eFileEncoding = ENCODING_UTF16_WITH_BOM;
		return true;
	}
	else
	{
		return false;
	}
}

wxString wxConfiguration::ToString( wxConfiguration::CUESHEET_ATTACH_MODE e )
{
	return to_string( e, AttachModeNames );
}

bool wxConfiguration::FromString( const wxString& sCsAttachMode, wxConfiguration::CUESHEET_ATTACH_MODE& eCsAttachMode, bool& bDefault )
{
	if ( sCsAttachMode.CmpNoCase( wxS( "default" ) ) == 0 )
	{
		bDefault = true;
		return true;
	}
	else
	{
		bDefault = false;
		return from_string( sCsAttachMode, eCsAttachMode, AttachModeNames );
	}
}

bool wxConfiguration::ReadLanguagesStrings( wxSortedArrayString& as )
{
	const wxStandardPaths& paths = wxStandardPaths::Get();
	wxFileName			   fn( paths.GetExecutablePath() );

	fn.SetFullName( LANG_FILE_NAME );

	if ( !fn.IsFileReadable() )
	{
		wxLogInfo( _( "Cannot find language file \u201C%s\u201D" ), fn.GetFullPath() );
		wxLogInfo( _( "You can find this file at \u201C%s\u201D" ), LANG_FILE_URL );
		return false;
	}

	wxFileInputStream fis( fn.GetFullPath() );

	if ( !fis.IsOk() )
	{
		wxLogDebug( wxS( "Cannot open language file \u201C%s\u201D" ), fn.GetFullPath() );
		return false;
	}

	as.Clear();
	wxTextInputStream tis( fis );
	size_t			  n = 0;
	while ( !fis.Eof() )
	{
		wxString sLine( tis.ReadLine() );

		if ( sLine.IsEmpty() )
		{
			continue;
		}

		wxStringTokenizer tokenizer( sLine, wxS( "|" ) );

		if ( tokenizer.HasMoreTokens() )
		{
			wxString sLang( tokenizer.GetNextToken() );

			if ( sLang.IsEmpty() || ( sLang.Length() > 3 ) )
			{
				wxLogDebug( wxS( "Skipping language %s" ), sLang );
			}
			else
			{
				as.Add( sLang );
			}
		}

		if ( n++ > 5000 )
		{
			wxLogError( _( "Too many languages. File \u201C%s\u201D is corrupt" ), fn.GetFullName() );
			as.Clear();
			return false;
		}
	}

	return true;
}

wxConfiguration::wxConfiguration( void ):
	m_infoSubject( INFO_NONE ),
	m_bChapterTimeEnd( true ),
	m_bUnknownChapterTimeEndToNextChapter( false ),
	m_nChapterOffset( 150 ),
	m_bUseDataFiles( false ),
	m_sAlternateExtensions( wxEmptyString ),
	m_sLang( wxS( "unk" ) ),
	m_sTrackNameFormat( TRACK_NAME_FORMAT ),
	m_sMatroskaNameFormat( MATROSKA_NAME_FORMAT ),
	m_bEmbedded( false ),
	m_bCorrectQuotationMarks( true ),
	m_eRenderMode( RENDER_MATROSKA_CHAPTERS ),
	m_eCueSheetFileEncoding( ENCODING_LOCAL ),
	m_bGenerateTags( false ),
	m_bGenerateMkvmergeOpts( false ),
	m_bGenerateEditionUID( false ),
	m_bRunMkvmerge( true ),
	m_bTrackOneIndexOne( true ),
	m_bAbortOnError( true ),
	m_bHiddenIndexes( false ),
	m_sCueSheetExt( CUE_SHEET_EXT ),
	m_sMatroskaChaptersXmlExt( MATROSKA_CHAPTERS_EXT ),
	m_sMatroskaTagsXmlExt( MATROSKA_TAGS_EXT ),
	m_sMatroskaOptsExt( MATROSKA_OPTS_EXT ),
	m_bMerge( false ),
	m_nReadFlags( wxCueSheetReader::EC_PARSE_COMMENTS | wxCueSheetReader::EC_ELLIPSIZE_TAGS | wxCueSheetReader::EC_REMOVE_EXTRA_SPACES | wxCueSheetReader::EC_MEDIA_READ_TAGS | wxCueSheetReader::EC_FIND_COVER | wxCueSheetReader::EC_FIND_LOG ),
	m_bUseMLang( true ),
	m_bUseFullPaths( false ),
	m_eCsAttachMode( CUESHEET_ATTACH_NONE )
{
	ReadLanguagesStrings( m_asLang );
}

void wxConfiguration::AddCmdLineParams( wxCmdLineParser& cmdLine ) const
{
	cmdLine.AddOption( "i", "info", wxString::Format( _( "Display additional information about [%s]" ), GetInfoSubjectTexts() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( wxS( "o" ), wxS( "output" ), _( "Output Matroska chapter file or cue sheet file (see -c option)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxS( "od" ), wxS( "output-directory" ), _( "Output directory (default: input directory)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxS( "ce" ), wxS( "chapter-time-end" ), wxString::Format( _( "Calculate end time of chapters if possible (default: %s)" ), BoolToStr( m_bChapterTimeEnd ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxS( "cn" ), wxS( "unknown-chapter-end-to-next-track" ), wxString::Format( _( "If track's end time is unknown set it to next track position using frame offset (default: %s)" ), BoolToStr( m_bUnknownChapterTimeEndToNextChapter ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxS( "fo" ), wxS( "frame-offset" ), wxString::Format( _( "Offset in frames to use with -uc option (default: %u)" ), m_nChapterOffset ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxS( "df" ), wxS( "use-data-files" ), wxString::Format( _( "Use data file(s) to calculate end time of chapters (default: %s)" ), BoolToStr( m_bUseDataFiles ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxS( "e" ), wxS( "alternate-extensions" ), _( "Comma-separated list of alternate extensions of data files (default: none)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxS( "f" ), wxS( "track-title-format" ), wxString::Format( _( "Track title format (default: %s)" ), TRACK_NAME_FORMAT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxS( "l" ), wxS( "language" ), wxString::Format( _( "Set language of chapter's tilte (default: %s)" ), m_sLang ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxS( "ec" ), wxS( "embedded-cue" ), wxString::Format( _( "Try to read embedded cue sheet (requires MediaInfo library) (default: %s)" ), BoolToStr( m_bEmbedded ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxS( "m" ), wxS( "rendering-method" ), wxString::Format( _( "Rendering method [%s] (default: %s)" ), GetRenderingModes(), ToString( m_eRenderMode ) ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxS( "t" ), wxS( "generate-tags" ), wxString::Format( _( "Generate tags file (default: %s)" ), BoolToStr( m_bGenerateTags ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxS( "k" ), wxS( "generate-mkvmerge-options" ), wxString::Format( _( "Generate file with mkvmerge options (default: %s)" ), BoolToStr( m_bGenerateMkvmergeOpts ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxS( "run-mkvmerge" ), wxString::Format( _( "Run mkvmerge tool after generation of options file (default: %s)" ), BoolToStr( m_bRunMkvmerge ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxS( "eu" ), wxS( "generate-edition-uid" ), wxString::Format( _( "Generate edition UID (default: %s)" ), BoolToStr( m_bGenerateEditionUID ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxS( "oce" ), wxS( "cue-sheet-encoding" ), _( "Output cue sheet file encoding - possible values are local (default), utf8, utf8_bom, utf16, utf16_bom" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxS( "a" ), wxS( "abort-on-error" ), wxString::Format( _( "Abort when conversion errors occurs (default: %s)" ), BoolToStr( m_bAbortOnError ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxS( "j" ), wxS( "merge" ), wxString::Format( _( "Merge cue sheets (default: %s)" ), BoolToStr( m_bMerge ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxS( "mf" ), wxS( "matroska-title-format" ), wxString::Format( _( "Mtroska container's title format (default: %s)" ), MATROSKA_NAME_FORMAT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxS( "cq" ), wxS( "correct-quotation-marks" ), wxString::Format( _( "Correct \"simple 'quotation' marks\" to \u201Cenglish \u2018quotation\u2019 marks\u201D inside strings (default: %s)" ), BoolToStr( m_bCorrectQuotationMarks ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	// read flags
	cmdLine.AddSwitch( wxS( "et" ), wxS( "ellipsize-tags" ), wxString::Format( _( "Ellipsize tags - convert last three dots to '%c' (default: %s)" ), wxEllipsizer::ELLIPSIS, ReadFlagTestStr( wxCueSheetReader::EC_ELLIPSIZE_TAGS ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxS( "rs" ), wxS( "remove-extra-spaces" ), wxString::Format( _( "Remove extra spaces from tags (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_REMOVE_EXTRA_SPACES ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxS( "tc" ), wxS( "generate-tags-from-comments" ), wxString::Format( _( "Try to parse tags from cue sheet comments (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_PARSE_COMMENTS ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxS( "single-media-file" ), wxString::Format( _( "Embedded mode flag. Assume input as single media file without cuesheet (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_SINGLE_MEDIA_FILE ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxS( "read-media-tags" ), wxString::Format( _( "Embedded mode flag. Read tags from media file (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_MEDIA_READ_TAGS ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxS( "attach-eac-log" ), wxString::Format( _( "Attach EAC log file to mkvmerge options file (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_FIND_LOG ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxS( "attach-cover" ), wxString::Format( _( "Attach cover image (cover.*;front.*;album.*) to mkvmerge options file (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_FIND_COVER ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	// advanced options
	cmdLine.AddSwitch( wxEmptyString, wxS( "use-mlang" ), wxString::Format( _( "Use MLang library (default: %s)" ), BoolToStr( m_bUseMLang ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxS( "t1i0" ), wxS( "track-01-index-00" ), wxString::Format( _( "For first track use index 00 as beginning of track (default: %s)" ), BoolToStr( !m_bTrackOneIndexOne ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxS( "hidden-indexes" ), wxString::Format( _( "Convert indexes to hidden (sub)chapters (default %s)" ), BoolToStr( m_bHiddenIndexes ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxEmptyString, wxS( "cue-sheet-attach-mode" ), _( "Mode of attaching cue sheet to mkvmerge options file - possible values are none (default), source, decoded, rendered and default" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxS( "use-full-paths" ), wxString::Format( _( "Use full paths in mkvmerge options file (default: %s)" ), BoolToStr( m_bUseFullPaths ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxEmptyString, wxS( "mkvmerge-directory" ), _( "Location of mkvmerge tool" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	// tags usage
	cmdLine.AddSwitch( wxEmptyString, wxS( "use-cdtext-tags" ), wxString::Format( _( "Use CD-TEXT tags (default: %s)" ), TagSourcesTestStr( wxCueTag::TAG_CD_TEXT ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxS( "use-cue-comments-tags" ), wxString::Format( _( "Use tags from cuesheet comments (default: %s)" ), TagSourcesTestStr( wxCueTag::TAG_CUE_COMMENT ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxS( "use-media-tags" ), wxString::Format( _( "Use tags from media file (default: %s)" ), TagSourcesTestStr( wxCueTag::TAG_MEDIA_METADATA ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	// extensions
	cmdLine.AddOption( wxEmptyString, wxS( "cue-sheet-file-extension" ), wxString::Format( _( "Cue sheet file extension (default: %s)" ), CUE_SHEET_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, wxS( "matroska-chapters-file-extension" ), wxString::Format( _( "Matroska chapters XML file extension (default: %s)" ), MATROSKA_CHAPTERS_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, wxS( "matroska-tags-file-extension" ), wxString::Format( _( "Matroska tags XML file extension (default: %s)" ), MATROSKA_TAGS_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, wxS( "mkvmerge-options-file-extension" ), wxString::Format( _( "File extension of mkvmerge options file (default: %s)" ), MATROSKA_OPTS_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	// input files
	cmdLine.AddParam( _( "<cue sheet>" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL );
}

bool wxConfiguration::check_ext( const wxString& sExt )
{
	return !sExt.IsEmpty() && ( sExt.Length() < MAX_EXT_LEN );
}

bool wxConfiguration::CheckLang( const wxString& sLang ) const
{
	if ( m_asLang.IsEmpty() )
	{
		return !sLang.IsEmpty() && ( sLang.Length() <= 3 ) && sLang.IsAscii();
	}
	else
	{
		int idx = m_asLang.Index( sLang.Lower() );
		return ( idx != wxNOT_FOUND );
	}
}

bool wxConfiguration::ReadReadFlags( const wxCmdLineParser& cmdLine, const wxString& name, wxCueSheetReader::ReadFlags nReadFlags )
{
	wxCmdLineSwitchState state = cmdLine.FoundSwitch( name );
	bool				 bSwitchValue;

	if ( ReadNegatableSwitchValue( cmdLine, name, bSwitchValue ) )
	{
		m_nReadFlags &= ~nReadFlags;
		m_nReadFlags |= bSwitchValue ? nReadFlags : 0u;
		return true;
	}
	else
	{
		return false;
	}
}

bool wxConfiguration::ReadTagSources( const wxCmdLineParser& cmdLine, const wxString& name, wxCueTag::TagSources nTagSources )
{
	wxCmdLineSwitchState state = cmdLine.FoundSwitch( name );
	bool				 bSwitchValue;

	if ( ReadNegatableSwitchValue( cmdLine, name, bSwitchValue ) )
	{
		m_nTagSources &= ~nTagSources;
		m_nTagSources |= bSwitchValue ? nTagSources : 0u;
		return true;
	}
	else
	{
		return false;
	}
}

bool wxConfiguration::Read( const wxCmdLineParser& cmdLine )
{
	bool	 bRes = true;
	wxString s;
	long	 v;

	if ( cmdLine.Found( "i", &s ) )
	{
		if ( !FromString( s, m_infoSubject ) )
		{
			wxLogWarning( _( "Invalid info subject - %s" ), s );
			bRes = false;
		}
	}

	ReadNegatableSwitchValue( cmdLine, wxS( "ce" ), m_bChapterTimeEnd );
	ReadNegatableSwitchValue( cmdLine, wxS( "cn" ), m_bUnknownChapterTimeEndToNextChapter );

	if ( cmdLine.Found( wxS( "fo" ), &v ) )
	{
		if ( ( v < 0 ) || ( v > 10000 ) )
		{
			wxLogWarning( _( "Wrong frame offset - %d" ), v );
			bRes = false;
		}
		else
		{
			m_nChapterOffset = (unsigned long)v;
		}
	}

	ReadNegatableSwitchValue( cmdLine, wxS( "df" ), m_bUseDataFiles );
	ReadNegatableSwitchValue( cmdLine, wxS( "ec" ), m_bEmbedded );
	ReadNegatableSwitchValue( cmdLine, wxS( "cq" ), m_bCorrectQuotationMarks );

	if ( cmdLine.Found( wxS( "m" ), &s ) )
	{
		if ( !FromString( s, m_eRenderMode ) )
		{
			wxLogWarning( _( "Wrong rendering method - %s " ), s );
			bRes = false;
		}
	}

	ReadNegatableSwitchValueAndNegate( cmdLine, wxS( "t1i0" ), m_bTrackOneIndexOne );
	ReadNegatableSwitchValue( cmdLine, wxS( "a" ), m_bAbortOnError );
	ReadNegatableSwitchValue( cmdLine, wxS( "t" ), m_bGenerateTags );
	ReadNegatableSwitchValue( cmdLine, wxS( "k" ), m_bGenerateMkvmergeOpts );
	ReadNegatableSwitchValue( cmdLine, wxS( "hidden-indexes" ), m_bHiddenIndexes );
	ReadNegatableSwitchValue( cmdLine, wxS( "run-mkvmerge" ), m_bRunMkvmerge );
	ReadNegatableSwitchValue( cmdLine, wxS( "use-full-paths" ), m_bUseFullPaths );

	if ( cmdLine.Found( wxS( "oce" ), &s ) )
	{
		if ( !FromString( s, m_eCueSheetFileEncoding ) )
		{
			wxLogWarning( _( "Wrong cue sheet file encoding %s" ), s );
			bRes = false;
		}
	}

	ReadNegatableSwitchValue( cmdLine, wxS( "eu" ), m_bGenerateEditionUID );
	ReadNegatableSwitchValue( cmdLine, wxS( "j" ), m_bMerge );

	if ( cmdLine.Found( wxS( "cue-sheet-file-extension" ), &s ) )
	{
		if ( check_ext( s ) )
		{
			m_sCueSheetExt = s;
		}
		else
		{
			wxLogWarning( _( "Invalid cue sheet file extension %s" ), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxS( "matroska-chapters-file-extension" ), &s ) )
	{
		if ( check_ext( s ) )
		{
			m_sMatroskaChaptersXmlExt = s;
		}
		else
		{
			wxLogWarning( _( "Invalid Matroska chapters file extension %s" ), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxS( "matroska-tags-file-extension" ), &s ) )
	{
		if ( check_ext( s ) )
		{
			m_sMatroskaTagsXmlExt = s;
		}
		else
		{
			wxLogWarning( _( "Invalid Matroska tags file extension %s" ), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxS( "mkvmerge-options-file-extension" ), &s ) )
	{
		if ( check_ext( s ) )
		{
			m_sMatroskaOptsExt = s;
		}
		else
		{
			wxLogWarning( _( "Invalid options file extension %s" ), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxS( "e" ), &s ) )
	{
		m_sAlternateExtensions = s;
	}

	if ( cmdLine.Found( wxS( "f" ), &s ) )
	{
		m_sTrackNameFormat = s;
	}

	if ( cmdLine.Found( wxS( "mf" ), &s ) )
	{
		m_sMatroskaNameFormat = s;
	}

	if ( cmdLine.GetParamCount() > 0 )
	{
		for ( size_t i = 0; i < cmdLine.GetParamCount(); i++ )
		{
			wxInputFile inputFile( cmdLine.GetParam( i ) );

			if ( inputFile.IsOk() )
			{
				m_inputFile.Add( inputFile );
			}
			else
			{
				wxLogWarning( _( "Invalid input file \u201C%s\u201D" ), cmdLine.GetParam( i ) );
				bRes = false;
			}
		}
	}

	if ( cmdLine.Found( wxS( "l" ), &s ) )
	{
		if ( CheckLang( s ) )
		{
			m_sLang = s.Lower();
		}
		else
		{
			wxLogWarning( _( "Invalid laguage %s" ), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxS( "o" ), &s ) )
	{
		m_outputFile.Assign( s );

		if ( !m_outputFile.MakeAbsolute() )
		{
			wxLogInfo( _( "Fail to normalize path \u201C%s\u201D" ), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxS( "od" ), &s ) )
	{
		m_outputFile.AssignDir( s );

		if ( !m_outputFile.MakeAbsolute() )
		{
			wxLogInfo( _( "Fail to normalize path \u201C%s\u201D" ), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxS( "mkvmerge-directory" ), &s ) )
	{
		m_mkvmergeDir.AssignDir( s );

		if ( !m_mkvmergeDir.MakeAbsolute() )
		{
			wxLogInfo( _( "Fail to normalize path \u201C%s\u201D" ), s );
			bRes = false;
		}
	}

	ReadTagSources( cmdLine, wxS( "use-cdtext-tags" ), wxCueTag::TAG_CD_TEXT );
	ReadTagSources( cmdLine, wxS( "use-cue-comments-tags" ), wxCueTag::TAG_CUE_COMMENT );
	ReadTagSources( cmdLine, wxS( "use-media-tags" ), wxCueTag::TAG_MEDIA_METADATA );

	ReadReadFlags( cmdLine, wxS( "tc" ), wxCueSheetReader::EC_PARSE_COMMENTS );
	ReadReadFlags( cmdLine, wxS( "et" ), wxCueSheetReader::EC_ELLIPSIZE_TAGS );
	ReadReadFlags( cmdLine, wxS( "rs" ), wxCueSheetReader::EC_REMOVE_EXTRA_SPACES );
	ReadReadFlags( cmdLine, wxS( "single-media-file" ), wxCueSheetReader::EC_SINGLE_MEDIA_FILE );
	ReadReadFlags( cmdLine, wxS( "read-media-tags" ), wxCueSheetReader::EC_MEDIA_READ_TAGS );
	ReadReadFlags( cmdLine, wxS( "attach-eac-log" ), wxCueSheetReader::EC_FIND_LOG );
	ReadReadFlags( cmdLine, wxS( "attach-cover" ), wxCueSheetReader::EC_FIND_COVER );

	// MLang
	ReadNegatableSwitchValue( cmdLine, wxS( "use-mlang" ), m_bUseMLang );

	if ( cmdLine.Found( wxS( "cue-sheet-attach-mode" ), &s ) )
	{
		bool bDefault;

		if ( FromString( s, m_eCsAttachMode, bDefault ) )
		{
			if ( bDefault )
			{
				m_eCsAttachMode = m_bEmbedded ? CUESHEET_ATTACH_DECODED : CUESHEET_ATTACH_SOURCE;
			}
		}
		else
		{
			wxLogWarning( _( "Wrong cue sheet attaching mode %s" ), s );
			bRes = false;
		}
	}

	return bRes;
}

wxString wxConfiguration::BoolToIdx( bool b )
{
	return b ? wxS( "01" ) : wxS( "00" );
}

wxString wxConfiguration::ReadFlagTestStr( wxCueSheetReader::ReadFlags n ) const
{
	return BoolToStr( ( m_nReadFlags & n ) == n );
}

wxString wxConfiguration::TagSourcesTestStr( wxCueTag::TagSources n ) const
{
	return BoolToStr( ( m_nTagSources & n ) == n );
}

void wxConfiguration::AddFlag( wxArrayString& as, wxCueSheetReader::ReadFlags flags, wxCueSheetReader::ReadFlags mask, const wxString& sText )
{
	if ( ( flags & mask ) == mask )
	{
		as.Add( sText );
	}
}

wxString wxConfiguration::GetReadFlagsDesc( wxCueSheetReader::ReadFlags flags )
{
	wxArrayString as;

	AddFlag( as, flags, wxCueSheetReader::EC_PARSE_COMMENTS, wxS( "generate-tags-from-comments" ) );
	AddFlag( as, flags, wxCueSheetReader::EC_ELLIPSIZE_TAGS, wxS( "ellipsize-tags" ) );
	AddFlag( as, flags, wxCueSheetReader::EC_REMOVE_EXTRA_SPACES, wxS( "remove-extra-spaces" ) );
	AddFlag( as, flags, wxCueSheetReader::EC_SINGLE_MEDIA_FILE, wxS( "single-media-file" ) );
	AddFlag( as, flags, wxCueSheetReader::EC_MEDIA_READ_TAGS, wxS( "media-tags" ) );
	AddFlag( as, flags, wxCueSheetReader::EC_FIND_COVER, wxS( "find-cover" ) );
	AddFlag( as, flags, wxCueSheetReader::EC_FIND_LOG, wxS( "find-log" ) );

	wxString s;
	for ( size_t i = 0, nCount = as.GetCount(); i < nCount; i++ )
	{
		s += as[ i ];
		s += wxS( ',' );
	}

	return s.RemoveLast();
}

void wxConfiguration::FillArray( wxArrayString& as ) const
{
	as.Add( wxString::Format( wxS( "Rendering method: %s" ), ToString( m_eRenderMode ) ) );
	as.Add( wxString::Format( wxS( "Generate tags file: %s" ), BoolToStr( m_bGenerateTags ) ) );
	as.Add( wxString::Format( wxS( "Generate mkvmerge options file: %s" ), BoolToStr( m_bGenerateMkvmergeOpts ) ) );

	if ( m_bGenerateMkvmergeOpts )
	{
		as.Add( wxString::Format( wxS( "Run mkvmerge: %s" ), BoolToStr( m_bRunMkvmerge ) ) );

		if ( m_mkvmergeDir.IsOk() )
		{
			as.Add( wxString::Format( wxS( "mkvmerge dir: %s" ), m_mkvmergeDir.GetFullPath() ) );
		}

		as.Add( wxString::Format( wxS( "Generate full paths: %s" ), BoolToStr( m_bUseFullPaths ) ) );
		as.Add( wxString::Format( wxS( "Cue sheet attach mode: %s" ), ToString( m_eCsAttachMode ) ) );
	}

	as.Add( wxString::Format( wxS( "Generate edition UID: %s" ), BoolToStr( m_bGenerateEditionUID ) ) );
	as.Add( wxString::Format( wxS( "Tag sources: %s" ), wxCueTag::SourcesToString( m_nTagSources ) ) );
	as.Add( wxString::Format( wxS( "Output cue sheet file encoding: %s" ), ToString( m_eCueSheetFileEncoding ) ) );
	as.Add( wxString::Format( wxS( "Calculate end time of chapters: %s" ), BoolToStr( m_bChapterTimeEnd ) ) );
	as.Add( wxString::Format( wxS( "Read embedded cue sheet: %s" ), BoolToStr( m_bEmbedded ) ) );
	as.Add( wxString::Format( wxS( "Use data files to calculate end time of chapters: %s" ), BoolToStr( m_bUseDataFiles ) ) );

	as.Add( wxString::Format( wxS( "Alternate extensions: %s" ), m_sAlternateExtensions ) );
	as.Add( wxString::Format( wxS( "Set chapter's end time to beginning of next chapter if track's end time cannot be calculated: %s" ), BoolToStr( m_bUnknownChapterTimeEndToNextChapter ) ) );
	as.Add( wxString::Format( wxS( "Chapter offset (frames): %d" ), m_nChapterOffset ) );
	as.Add( wxString::Format( wxS( "Track name format: %s" ), m_sTrackNameFormat ) );
	as.Add( wxString::Format( wxS( "Matroska container name format: %s" ), m_sMatroskaNameFormat ) );
	as.Add( wxString::Format( wxS( "Chapter string language: %s" ), m_sLang ) );
	as.Add( wxString::Format( wxS( "For track 01 assume index %s as beginning of track" ), BoolToIdx( m_bTrackOneIndexOne ) ) );
	as.Add( wxString::Format( wxS( "Merge mode: %s" ), BoolToStr( m_bMerge ) ) );
	as.Add( wxString::Format( wxS( "Convert indexes to hidden subchapters: %s" ), BoolToStr( m_bHiddenIndexes ) ) );
	as.Add( wxString::Format( wxS( "Default cue sheet file extension: %s" ), m_sCueSheetExt ) );
	as.Add( wxString::Format( wxS( "Matroska chapters XML file extension: %s" ), m_sMatroskaChaptersXmlExt ) );
	as.Add( wxString::Format( wxS( "Matroska tags XML file extension: %s" ), m_sMatroskaTagsXmlExt ) );
	as.Add( wxString::Format( wxS( "mkvmerge options file extension: %s" ), m_sMatroskaOptsExt ) );
	as.Add( wxString::Format( wxS( "Correct \"simple 'quotation' marks\" inside strings: %s" ), BoolToStr( m_bCorrectQuotationMarks ) ) );
	as.Add( wxString::Format( wxS( "Read flags: %s" ), GetReadFlagsDesc( m_nReadFlags ) ) );
	as.Add( wxString::Format( wxS( "Use MLang library: %s" ), BoolToStr( m_bUseMLang ) ) );
}

void wxConfiguration::Dump() const
{
	if ( wxLog::IsLevelEnabled( wxLOG_Info, wxLOG_COMPONENT ) && wxLog::GetVerbose() )
	{
		wxString	  sSeparator( wxS( '=' ), 65 );
		wxArrayString as;
		as.Add( sSeparator );
		as.Add( _( "Configuration:" ) );
		FillArray( as );
		as.Add( wxString::Format( _( "Output path: \u201C%s\u201D" ), m_outputFile.GetFullPath() ) );
		as.Add( sSeparator );
		size_t	   strings = as.GetCount();
		wxDateTime dt( wxDateTime::Now() );
		wxLog*	   pLog = wxLog::GetActiveTarget();
		for ( size_t i = 0; i < strings; i++ )
		{
			pLog->OnLog( wxLOG_Info, as[ i ], dt.GetTicks() );
		}
	}
}

void wxConfiguration::BuildXmlComments( const wxFileName& outputFile, wxXmlNode* pNode ) const
{
	wxString sInit;

	sInit.Printf( wxS( "This file was created by %s tool" ), wxGetApp().GetAppDisplayName() );
	wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString, sInit );
	pNode->AddChild( pComment );

	wxArrayString as;
	wxDateTime	  dtNow( wxDateTime::Now() );

	as.Add( wxString::Format( wxS( "Application version: %s" ), wxGetApp().APP_VERSION ) );
	as.Add( wxString::Format( wxS( "Application vendor: %s" ), wxGetApp().GetVendorDisplayName() ) );
	as.Add( wxString::Format( wxS( "Creation time: %s %s" ), dtNow.FormatISODate(), dtNow.FormatISOTime() ) );
	as.Add( wxString::Format( wxS( "Output file: \u201C%s\u201D" ), outputFile.GetFullName() ) );

	FillArray( as );

	size_t strings = as.GetCount();
	for ( size_t i = 0; i < strings; i++ )
	{
		wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString, as[ i ] );
		pNode->AddChild( pComment );
	}
}

wxConfiguration::INFO_SUBJECT wxConfiguration::GetInfoSubject() const
{
	return m_infoSubject;
}

bool wxConfiguration::GetChapterTimeEnd() const
{
	return m_bChapterTimeEnd;
}

bool wxConfiguration::GetUnknownChapterTimeEndToNextChapter() const
{
	return m_bUnknownChapterTimeEndToNextChapter;
}

unsigned long wxConfiguration::GetChapterOffset() const
{
	return m_nChapterOffset;
}

bool wxConfiguration::GetUseDataFiles() const
{
	return m_bUseDataFiles;
}

const wxString& wxConfiguration::GetAlternateExtensions() const
{
	return m_sAlternateExtensions;
}

bool wxConfiguration::HasAlternateExtensions() const
{
	return !m_sAlternateExtensions.IsEmpty();
}

const wxString& wxConfiguration::GetTrackNameFormat() const
{
	return m_sTrackNameFormat;
}

const wxString& wxConfiguration::GetMatroskaNameFormat() const
{
	return m_sMatroskaNameFormat;
}

const wxString& wxConfiguration::GetLang() const
{
	return m_sLang;
}

const wxArrayInputFile& wxConfiguration::GetInputFiles() const
{
	return m_inputFile;
}

wxString wxConfiguration::GetExt() const
{
	wxString sExt;

	switch ( m_eRenderMode )
	{
		case RENDER_CUE_SHEET:
		{
			sExt = m_sCueSheetExt;
			break;
		}

		case RENDER_MATROSKA_CHAPTERS:
		{
			sExt = m_sMatroskaChaptersXmlExt;
			break;
		}

		case RENDER_WAV2IMG_CUE_POINTS:
		{
			sExt = wxS( "txt" );
			break;
		}

		default:
		{
			wxASSERT( false );
			sExt = wxS( "???" );
			break;
		}
	}

	return sExt;
}

wxString wxConfiguration::GetOutputFile( const wxInputFile& _inputFile ) const
{
	wxFileName inputFile( _inputFile.GetInputFile() );

	if ( !inputFile.IsOk() )
	{
		return wxEmptyString;
	}

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetExt( GetExt() );
	}
	else
	{
		if ( m_outputFile.IsDir() )
		{
			inputFile.SetPath( m_outputFile.GetPath() );
			inputFile.SetExt( GetExt() );
		}
		else
		{
			inputFile = m_outputFile;
		}
	}

	return inputFile.GetFullPath();
}

void wxConfiguration::GetOutputFile( const wxInputFile& _inputFile, wxFileName& outputFile, wxFileName& tagsFile ) const
{
	outputFile.Clear();
	tagsFile.Clear();

	wxFileName inputFile( _inputFile.GetInputFile() );

	if ( !inputFile.IsOk() )
	{
		return;
	}

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetExt( GetExt() );
		outputFile = inputFile;

		if ( ( m_eRenderMode == RENDER_MATROSKA_CHAPTERS ) && m_bGenerateTags )
		{
			inputFile.SetExt( m_sMatroskaTagsXmlExt );
			tagsFile = inputFile;
		}
	}
	else
	{
		if ( m_outputFile.IsDir() )
		{
			inputFile.SetPath( m_outputFile.GetPath() );
			inputFile.SetExt( GetExt() );
		}
		else
		{
			inputFile = m_outputFile;
		}

		outputFile = inputFile;

		if ( ( m_eRenderMode == RENDER_MATROSKA_CHAPTERS ) && m_bGenerateTags )
		{
			inputFile.SetExt( m_sMatroskaTagsXmlExt );
			tagsFile = inputFile;
		}
	}
}

void wxConfiguration::GetOutputMatroskaFile( const wxInputFile& _inputFile, wxFileName& matroskaFile, wxFileName& optionsFile ) const
{
	matroskaFile.Clear();
	optionsFile.Clear();

	wxFileName inputFile( _inputFile.GetInputFile() );

	if ( !inputFile.IsOk() )
	{
		return;
	}

	if ( m_eRenderMode != RENDER_MATROSKA_CHAPTERS )
	{
		return;
	}

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetExt( MATROSKA_AUDIO_EXT );
		matroskaFile = inputFile;

		inputFile.SetExt( m_sMatroskaOptsExt );
		optionsFile = inputFile;
	}
	else
	{
		if ( m_outputFile.IsDir() )
		{
			inputFile.SetPath( m_outputFile.GetPath() );
		}
		else
		{
			inputFile = m_outputFile;
		}

		inputFile.SetExt( MATROSKA_AUDIO_EXT );
		matroskaFile = inputFile;

		inputFile.SetExt( m_sMatroskaOptsExt );
		optionsFile = inputFile;
	}
}

bool wxConfiguration::GetOutputCueSheetFile( const wxInputFile& _inputFile, const wxString& sPostFix, wxFileName& cueFile ) const
{
	wxFileName inputFile( _inputFile.GetInputFile() );

	if ( !inputFile.IsOk() )
	{
		return false;
	}

	if ( m_eRenderMode != RENDER_CUE_SHEET )
	{
		return false;
	}

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetName( wxString::Format( wxS( "%s.%s" ), inputFile.GetName(), sPostFix ) );
		inputFile.SetExt( CUESHEET_EXT );
		inputFile.Normalize();
		cueFile = inputFile;
		return true;
	}
	else
	{
		if ( m_outputFile.IsDir() )
		{
			inputFile.SetPath( m_outputFile.GetPath() );
		}
		else
		{
			inputFile = m_outputFile;
		}

		inputFile.SetName( wxString::Format( wxS( "%s.%s" ), inputFile.GetName(), sPostFix ) );
		inputFile.SetExt( CUESHEET_EXT );
		inputFile.Normalize();
		cueFile = inputFile;
		return true;
	}
}

bool wxConfiguration::IsEmbedded() const
{
	return m_bEmbedded;
}

bool wxConfiguration::CorrectQuotationMarks() const
{
	return m_bCorrectQuotationMarks;
}

wxConfiguration::RENDER_MODE wxConfiguration::GetRenderMode() const
{
	return m_eRenderMode;
}

bool wxConfiguration::TrackOneIndexOne() const
{
	return m_bTrackOneIndexOne;
}

bool wxConfiguration::AbortOnError() const
{
	return m_bAbortOnError;
}

bool wxConfiguration::HiddenIndexes() const
{
	return m_bHiddenIndexes;
}

const wxString& wxConfiguration::CueSheetExt() const
{
	return m_sCueSheetExt;
}

const wxString& wxConfiguration::MatroskaChaptersXmlExt() const
{
	return m_sMatroskaChaptersXmlExt;
}

bool wxConfiguration::GenerateTags() const
{
	return m_bGenerateTags;
}

bool wxConfiguration::GenerateMkvmergeOpts() const
{
	return m_bGenerateMkvmergeOpts;
}

bool wxConfiguration::GenerateEditionUID() const
{
	return m_bGenerateEditionUID;
}

wxConfiguration::FILE_ENCODING wxConfiguration::GetCueSheetFileEncoding() const
{
	return m_eCueSheetFileEncoding;
}

wxSharedPtr< wxTextOutputStream > wxConfiguration::GetOutputTextStream( wxOutputStream& os ) const
{
	wxSharedPtr< wxTextOutputStream > pRes;
	switch ( m_eCueSheetFileEncoding )
	{
		case ENCODING_UTF8:
		{
			pRes = wxTextOutputStreamWithBOMFactory::CreateUTF8( os, wxEOL_NATIVE, false, m_bUseMLang );
			break;
		}

		case ENCODING_UTF8_WITH_BOM:
		{
			pRes = wxTextOutputStreamWithBOMFactory::CreateUTF8( os, wxEOL_NATIVE, true, m_bUseMLang );
			break;
		}

		case ENCODING_UTF16:
		{
			pRes = wxTextOutputStreamWithBOMFactory::CreateUTF16( os, wxEOL_NATIVE, false, m_bUseMLang );
			break;
		}

		case ENCODING_UTF16_WITH_BOM:
		{
			pRes = wxTextOutputStreamWithBOMFactory::CreateUTF16( os, wxEOL_NATIVE, true, m_bUseMLang );
			break;
		}

		default:
		{
			pRes = new wxTextOutputStream( os, wxEOL_NATIVE, wxConvLocal );
			break;
		}
	}

	return pRes;
}

bool wxConfiguration::GetMerge() const
{
	return m_bMerge;
}

wxCueSheetReader::ReadFlags wxConfiguration::GetReadFlags() const
{
	return m_nReadFlags;
}

wxCueTag::TagSources wxConfiguration::GetTagSources() const
{
	return m_nTagSources;
}

bool wxConfiguration::UseMLang() const
{
	return m_bUseMLang;
}

const wxString& wxConfiguration::MatroskaTagsXmlExt() const
{
	return m_sMatroskaTagsXmlExt;
}

const wxString& wxConfiguration::MatroskaOptsExt() const
{
	return m_sMatroskaOptsExt;
}

bool wxConfiguration::RunMkvmerge() const
{
	return m_bRunMkvmerge;
}

const wxFileName& wxConfiguration::GetMkvmergeDir() const
{
	return m_mkvmergeDir;
}

bool wxConfiguration::UseFullPaths() const
{
	return m_bUseFullPaths;
}

bool wxConfiguration::AttachEacLog() const
{
	return ( m_nReadFlags & wxCueSheetReader::EC_FIND_LOG ) != 0;
}

bool wxConfiguration::AttachCover() const
{
	return ( m_nReadFlags & wxCueSheetReader::EC_FIND_COVER ) != 0;
}

wxConfiguration::CUESHEET_ATTACH_MODE wxConfiguration::GetCueSheetAttachMode() const
{
	return m_eCsAttachMode;
}

#include <wx/arrimpl.cpp>	// this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayInputFile );

