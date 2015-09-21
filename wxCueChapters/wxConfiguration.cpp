/*
 * wxConfiguration.cpp
 */

#include "StdWx.h"
#include <enum2str.h>
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxEncodingDetection/wxEncodingDetection.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include "wxConfiguration.h"
#include "wxApp.h"

// ===============================================================================

const char	 wxConfiguration::CUE_SHEET_EXT[]		  = "cue";
const char	 wxConfiguration::MATROSKA_CHAPTERS_EXT[] = "mkc.xml";
const char	 wxConfiguration::MATROSKA_TAGS_EXT[]	  = "mkt.xml";
const char	 wxConfiguration::MATROSKA_OPTS_EXT[]	  = "opt.txt";
const char	 wxConfiguration::MATROSKA_AUDIO_EXT[]	  = "mka";
const char	 wxConfiguration::CUESHEET_EXT[]		  = "cue";

// en dash - U+2013, thin space - U+2009
const wxChar	 wxConfiguration::TRACK_NAME_FORMAT[]	  = wxS("%dp%\u2009\u2013\u2009%dt%\u2009\u2013\u2009%tt%");
const wxChar	 wxConfiguration::MATROSKA_NAME_FORMAT[]  = wxS("%dp%\u2009\u2013\u2009%dt%");

const size_t wxConfiguration::MAX_EXT_LEN			  = 20;
const char	 wxConfiguration::LANG_FILE_URL[]		  = "http://www.loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt";
const char	 wxConfiguration::LANG_FILE_NAME[]		  = "ISO-639-2_utf-8.txt";
const char	 wxConfiguration::LANG_UND[]			  = "und";

// ===============================================================================

const wxConfiguration::CuesheetAttachModeName wxConfiguration::AttachModeNames[] =
{
	{ CUESHEET_ATTACH_NONE, "none" },
	{ CUESHEET_ATTACH_SOURCE, "source" },
	{ CUESHEET_ATTACH_DECODED, "decoded" },
	{ CUESHEET_ATTACH_RENDERED, "rendered" }
};

// ===============================================================================

const wxConfiguration::RenderModeName wxConfiguration::RenderModeNames[] =
{
	{ RENDER_CUE_SHEET, "cuesheet" },
	{ RENDER_MATROSKA_CHAPTERS, "matroska" },
	{ RENDER_WAV2IMG_CUE_POINTS, "wav2img" }
};

wxString wxConfiguration::GetRenderingModes()
{
	return get_texts( RenderModeNames );
}

// ===============================================================================

const wxConfiguration::INFO_SUBJECT_DESC wxConfiguration::InfoSubjectDesc[] =
{
	{ INFO_VERSION, "version" },
	{ INFO_USAGE, "usage" },
	{ INFO_FORMATTING_DIRECTIVES, "formatting" },
	{ INFO_LICENSE, "license" }
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
			s = "LOCAL";
			break;
		}

		case ENCODING_UTF8:
		{
			s = "UTF-8";
			break;
		}

		case ENCODING_UTF8_WITH_BOM:
		{
			s = "UTF-8 (BOM)";
			break;
		}

		case ENCODING_UTF16_LE:
		{
			s = "UTF-16LE";
			break;
		}

		case ENCODING_UTF16_LE_WITH_BOM:
		{
			s = "UTF-16LE (BOM)";
			break;
		}

		case ENCODING_UTF16_BE:
		{
			s = "UTF-16BE";
			break;
		}

		case ENCODING_UTF16_BE_WITH_BOM:
		{
			s = "UTF-16BE (BOM)";
			break;
		}

		default:
		{
			s.Printf( "UNKNOWN %d", eFileEncoding );
			break;
		}
	}

	return s;
}

bool wxConfiguration::FromString( const wxString& sFileEncoding_, wxConfiguration::FILE_ENCODING& eFileEncoding )
{
	wxString sFileEncoding( sFileEncoding_ );

	sFileEncoding.Replace( '-', wxEmptyString );
	sFileEncoding.Replace( '_', wxEmptyString );

	if (
		sFileEncoding.CmpNoCase( "local" ) == 0 ||
		sFileEncoding.CmpNoCase( "default" ) == 0
		)
	{
		eFileEncoding = ENCODING_LOCAL;
		return true;
	}
	else if ( sFileEncoding.CmpNoCase( "utf8" ) == 0 )
	{
		eFileEncoding = ENCODING_UTF8;
		return true;
	}
	else if ( sFileEncoding.CmpNoCase( "utf8bom" ) == 0 )
	{
		eFileEncoding = ENCODING_UTF8_WITH_BOM;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( "utf16" ) == 0 ||
		sFileEncoding.CmpNoCase( "utf16le" ) == 0
		)
	{
		eFileEncoding = ENCODING_UTF16_LE;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( "utf16bom" ) == 0 ||
		sFileEncoding.CmpNoCase( "utf16lebom" ) == 0
		)
	{
		eFileEncoding = ENCODING_UTF16_LE_WITH_BOM;
		return true;
	}
	else if ( sFileEncoding.CmpNoCase( "utf16be" ) == 0 )
	{
		eFileEncoding = ENCODING_UTF16_BE;
		return true;
	}
	else if ( sFileEncoding.CmpNoCase( "utf16bebom" ) == 0 )
	{
		eFileEncoding = ENCODING_UTF16_BE_WITH_BOM;
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
	if ( sCsAttachMode.CmpNoCase( "default" ) == 0 )
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

		wxStringTokenizer tokenizer( sLine, '|' );

		if ( tokenizer.HasMoreTokens() )
		{
			wxString sLang( tokenizer.GetNextToken() );

			if ( sLang.IsEmpty() || ( sLang.Length() > 3 ) )
			{
				wxLogDebug( "Skipping language %s", sLang );
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
	m_sLang( LANG_UND ),
	m_sTrackNameFormat( TRACK_NAME_FORMAT ),
	m_sMatroskaNameFormat( MATROSKA_NAME_FORMAT ),
	m_bCorrectQuotationMarks( true ),
	m_eRenderMode( RENDER_MATROSKA_CHAPTERS ),
	m_eFileEncoding( ENCODING_UTF8_WITH_BOM ),
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
    m_nReadFlags( wxCueSheetReader::EC_PARSE_COMMENTS | wxCueSheetReader::EC_ELLIPSIZE_TAGS | wxCueSheetReader::EC_REMOVE_EXTRA_SPACES | wxCueSheetReader::EC_MEDIA_READ_TAGS | wxCueSheetReader::EC_FIND_COVER | wxCueSheetReader::EC_FIND_LOG | wxCueSheetReader::EC_FIND_ACCURIP_LOG | wxCueSheetReader::EC_CONVERT_COVER_TO_JPEG | wxCueSheetReader::EC_CORRECT_DASHES ),
	m_bUseMLang( true ),
	m_bUseFullPaths( false ),
	m_eCsAttachMode( CUESHEET_ATTACH_NONE ),
    m_bRenderArtistForTrack( false ),
    m_nJpegImageQuality( 75 ),
    m_imageHandler( nullptr )
{
	ReadLanguagesStrings( m_asLang );
}

void wxConfiguration::AddCmdLineParams( wxCmdLineParser& cmdLine ) const
{
	cmdLine.AddOption( "i", "info", wxString::Format( _( "Display additional information about [%s]" ), GetInfoSubjectTexts() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( "o", "output", _( "Output Matroska chapter file or cue sheet file (see -c option)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "od", "output-directory", _( "Output directory (default: input directory)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( "ce", "chapter-time-end", wxString::Format( _( "Calculate end time of chapters if possible (default: %s)" ), BoolToStr( m_bChapterTimeEnd ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( "cn", "unknown-chapter-end-to-next-track", wxString::Format( _( "If track's end time is unknown set it to next track position using frame offset (default: %s)" ), BoolToStr( m_bUnknownChapterTimeEndToNextChapter ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "fo", "frame-offset", wxString::Format( _( "Offset in frames to use with -uc option (default: %u)" ), m_nChapterOffset ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( "df", "use-data-files", wxString::Format( _( "Use data file(s) to calculate end time of chapters (default: %s)" ), BoolToStr( m_bUseDataFiles ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "x", "alternate-extensions", _( "Comma-separated list of alternate extensions of data files (default: none)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "f", "track-title-format", wxString::Format( _( "Track title format (default: %s)" ), TRACK_NAME_FORMAT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "l", "language", wxString::Format( _( "Set language of chapter's tilte (default: %s)" ), m_sLang ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "m", "rendering-method", wxString::Format( _( "Rendering method [%s] (default: %s)" ), GetRenderingModes(), ToString( m_eRenderMode ) ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( "t", "generate-tags", wxString::Format( _( "Generate tags file (default: %s)" ), BoolToStr( m_bGenerateTags ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( "k", "generate-mkvmerge-options", wxString::Format( _( "Generate file with mkvmerge options (default: %s)" ), BoolToStr( m_bGenerateMkvmergeOpts ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, "run-mkvmerge", wxString::Format( _( "Run mkvmerge tool after generation of options file (default: %s)" ), BoolToStr( m_bRunMkvmerge ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( "eu", "generate-edition-uid", wxString::Format( _( "Generate edition UID (default: %s)" ), BoolToStr( m_bGenerateEditionUID ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "e", "output-encoding", _( "Output cue sheet file encoding [local|utf8|utf8_bom|utf16le|utf16le_bom|utf16be|utf16be_bom] (default: utf8_bom)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( "a", "abort-on-error", wxString::Format( _( "Abort when conversion errors occurs (default: %s)" ), BoolToStr( m_bAbortOnError ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( "j", "merge", wxString::Format( _( "Merge cue sheets (default: %s)" ), BoolToStr( m_bMerge ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "mf", "matroska-title-format", wxString::Format( _( "Mtroska container's title format (default: %s)" ), MATROSKA_NAME_FORMAT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( "cq", "correct-quotation-marks", wxString::Format( _( "Correct \"simple 'quotation' marks\" to \u201Cenglish \u2018quotation\u2019 marks\u201D inside strings (default: %s)" ), BoolToStr( m_bCorrectQuotationMarks ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	// read flags
	cmdLine.AddSwitch( "et", "ellipsize-tags", wxString::Format( _( "Ellipsize tags - convert last three dots to '%c' (default: %s)" ), wxEllipsizer::ELLIPSIS, ReadFlagTestStr( wxCueSheetReader::EC_ELLIPSIZE_TAGS ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( "rs", "remove-extra-spaces", wxString::Format( _( "Remove extra spaces from tags (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_REMOVE_EXTRA_SPACES ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( "tc", "generate-tags-from-comments", wxString::Format( _( "Try to parse tags from cue sheet comments (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_PARSE_COMMENTS ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, "single-media-file", wxString::Format( _( "Embedded mode flag. Assume input as single media file without cuesheet (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_SINGLE_MEDIA_FILE ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, "read-media-tags", wxString::Format( _( "Embedded mode flag. Read tags from media file (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_MEDIA_READ_TAGS ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, "attach-eac-log", wxString::Format( _( "Attach EAC log file to mkvmerge options file (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_FIND_LOG ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, "attach-cover", wxString::Format( _( "Attach cover image (cover.*;front.*;album.*) to mkvmerge options file (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_FIND_COVER ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
    cmdLine.AddSwitch( wxEmptyString, "attach-accurip-log", wxString::Format( _( "Attach AccurateRip log (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_FIND_ACCURIP_LOG ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
    cmdLine.AddSwitch( "ru", "upper-roman-numerals", wxString::Format( _( "Convert roman numerals - upper case (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_CONVERT_UPPER_ROMAN_NUMERALS ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
    cmdLine.AddSwitch( "rl", "lower-roman-numerals", wxString::Format( _( "Convert roman numerals - lower case (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_CONVERT_LOWER_ROMAN_NUMERALS ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
    cmdLine.AddSwitch( wxEmptyString, "correct-dashes", wxString::Format( _( "Correct dashes (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_CORRECT_DASHES ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	// advanced options
	cmdLine.AddSwitch( wxEmptyString, "use-mlang", wxString::Format( _( "Use MLang library (default: %s)" ), BoolToStr( m_bUseMLang ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( "t1i0", "track-01-index-00", wxString::Format( _( "For first track use index 00 as beginning of track (default: %s)" ), BoolToStr( !m_bTrackOneIndexOne ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, "hidden-indexes", wxString::Format( _( "Convert indexes to hidden (sub)chapters (default %s)" ), BoolToStr( m_bHiddenIndexes ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxEmptyString, "cue-sheet-attach-mode", _( "Mode of attaching cue sheet to mkvmerge options file - possible values are none (default), source, decoded, rendered and default" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, "use-full-paths", wxString::Format( _( "Use full paths in mkvmerge options file (default: %s)" ), BoolToStr( m_bUseFullPaths ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
    cmdLine.AddSwitch( "ra", "render-artist-for-track", wxString::Format( _( "Render artist for track (default: %s)" ), BoolToStr( m_bRenderArtistForTrack ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	// tags usage
	cmdLine.AddSwitch( wxEmptyString, "use-cdtext-tags", wxString::Format( _( "Use CD-TEXT tags (default: %s)" ), TagSourcesTestStr( wxCueTag::TAG_CD_TEXT ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, "use-cue-comments-tags", wxString::Format( _( "Use tags from cuesheet comments (default: %s)" ), TagSourcesTestStr( wxCueTag::TAG_CUE_COMMENT ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, "use-media-tags", wxString::Format( _( "Use tags from media file (default: %s)" ), TagSourcesTestStr( wxCueTag::TAG_MEDIA_METADATA ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	// extensions
	cmdLine.AddOption( wxEmptyString, "cue-sheet-file-extension", wxString::Format( _( "Cue sheet file extension (default: %s)" ), CUE_SHEET_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, "matroska-chapters-file-extension", wxString::Format( _( "Matroska chapters XML file extension (default: %s)" ), MATROSKA_CHAPTERS_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, "matroska-tags-file-extension", wxString::Format( _( "Matroska tags XML file extension (default: %s)" ), MATROSKA_TAGS_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, "mkvmerge-options-file-extension", wxString::Format( _( "File extension of mkvmerge options file (default: %s)" ), MATROSKA_OPTS_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

    cmdLine.AddSwitch( wxEmptyString, "convert-cover-to-jpeg", wxString::Format( _( "Convert cover files to JPEG (default: %s)" ), ReadFlagTestStr( wxCueSheetReader::EC_CONVERT_COVER_TO_JPEG ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
    cmdLine.AddOption( wxEmptyString, "jpeg-image-quality", wxString::Format( _( "JPEG image quality (default %d)" ), m_nJpegImageQuality ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

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

	ReadNegatableSwitchValue( cmdLine, "ce", m_bChapterTimeEnd );
	ReadNegatableSwitchValue( cmdLine, "cn", m_bUnknownChapterTimeEndToNextChapter );

	if ( cmdLine.Found( "fo", &v ) )
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

	ReadNegatableSwitchValue( cmdLine, "df", m_bUseDataFiles );
	ReadNegatableSwitchValue( cmdLine, "cq", m_bCorrectQuotationMarks );

	if ( cmdLine.Found( "m", &s ) )
	{
		if ( !FromString( s, m_eRenderMode ) )
		{
			wxLogWarning( _( "Wrong rendering method - %s " ), s );
			bRes = false;
		}
	}

	ReadNegatableSwitchValueAndNegate( cmdLine, "t1i0", m_bTrackOneIndexOne );
	ReadNegatableSwitchValue( cmdLine, "a", m_bAbortOnError );
	ReadNegatableSwitchValue( cmdLine, "t", m_bGenerateTags );
	ReadNegatableSwitchValue( cmdLine, "k", m_bGenerateMkvmergeOpts );
	ReadNegatableSwitchValue( cmdLine, "hidden-indexes", m_bHiddenIndexes );
	ReadNegatableSwitchValue( cmdLine, "run-mkvmerge", m_bRunMkvmerge );
	ReadNegatableSwitchValue( cmdLine, "use-full-paths", m_bUseFullPaths );
    ReadNegatableSwitchValue( cmdLine, "ra", m_bRenderArtistForTrack );

	if ( cmdLine.Found( "e", &s ) )
	{
		if ( !FromString( s, m_eFileEncoding ) )
		{
			wxLogWarning( _( "Wrong output encoding %s" ), s );
			bRes = false;
		}
	}

	ReadNegatableSwitchValue( cmdLine, "eu", m_bGenerateEditionUID );
	ReadNegatableSwitchValue( cmdLine, "j", m_bMerge );

	if ( cmdLine.Found( "cue-sheet-file-extension", &s ) )
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

	if ( cmdLine.Found( "matroska-chapters-file-extension", &s ) )
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

	if ( cmdLine.Found( "matroska-tags-file-extension", &s ) )
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

	if ( cmdLine.Found( "mkvmerge-options-file-extension", &s ) )
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

	if ( cmdLine.Found( "x", &s ) )
	{
		m_sAlternateExtensions = s;
	}

	if ( cmdLine.Found( "f", &s ) )
	{
		m_sTrackNameFormat = s;
	}

	if ( cmdLine.Found( "mf", &s ) )
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

	if ( cmdLine.Found( "l", &s ) )
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

	if ( cmdLine.Found( "o", &s ) )
	{
		m_outputFile.Assign( s );

		if ( !m_outputFile.MakeAbsolute() )
		{
			wxLogInfo( _( "Fail to normalize path \u201C%s\u201D" ), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( "od", &s ) )
	{
		m_outputFile.AssignDir( s );

		if ( !m_outputFile.MakeAbsolute() )
		{
			wxLogInfo( _( "Fail to normalize path \u201C%s\u201D" ), s );
			bRes = false;
		}
	}

	ReadTagSources( cmdLine, "use-cdtext-tags", wxCueTag::TAG_CD_TEXT );
	ReadTagSources( cmdLine, "use-cue-comments-tags", wxCueTag::TAG_CUE_COMMENT );
	ReadTagSources( cmdLine, "use-media-tags", wxCueTag::TAG_MEDIA_METADATA );

	ReadReadFlags( cmdLine, "tc", wxCueSheetReader::EC_PARSE_COMMENTS );
	ReadReadFlags( cmdLine, "et", wxCueSheetReader::EC_ELLIPSIZE_TAGS );
	ReadReadFlags( cmdLine, "rs", wxCueSheetReader::EC_REMOVE_EXTRA_SPACES );
	ReadReadFlags( cmdLine, "single-media-file", wxCueSheetReader::EC_SINGLE_MEDIA_FILE );
	ReadReadFlags( cmdLine, "read-media-tags", wxCueSheetReader::EC_MEDIA_READ_TAGS );
	ReadReadFlags( cmdLine, "attach-eac-log", wxCueSheetReader::EC_FIND_LOG );
    ReadReadFlags( cmdLine, "attach-accurip-log", wxCueSheetReader::EC_FIND_ACCURIP_LOG );
	ReadReadFlags( cmdLine, "attach-cover", wxCueSheetReader::EC_FIND_COVER );
    ReadReadFlags( cmdLine, "ru", wxCueSheetReader::EC_CONVERT_UPPER_ROMAN_NUMERALS );
    ReadReadFlags( cmdLine, "rl", wxCueSheetReader::EC_CONVERT_LOWER_ROMAN_NUMERALS );
    ReadReadFlags( cmdLine, "correct-dashes", wxCueSheetReader::EC_CORRECT_DASHES );

	// MLang
	ReadNegatableSwitchValue( cmdLine, "use-mlang", m_bUseMLang );

	if ( cmdLine.Found( "cue-sheet-attach-mode", &s ) )
	{
		bool bDefault;

		if ( FromString( s, m_eCsAttachMode, bDefault ) )
		{
			if ( bDefault )
			{
				m_eCsAttachMode = CUESHEET_ATTACH_SOURCE;
			}
		}
		else
		{
			wxLogWarning( _( "Wrong cue sheet attaching mode %s" ), s );
			bRes = false;
		}
	}

    ReadReadFlags( cmdLine, "convert-cover-to-jpeg", wxCueSheetReader::EC_CONVERT_COVER_TO_JPEG );

    if (cmdLine.Found( "jpeg-image-quality", &v ))
    {
        if ((v < 0) || (v > 1000))
        {
            wxLogWarning( _( "Wrong JPEG image quality - %d" ), v );
            bRes = false;
        }
        else
        {
            m_nJpegImageQuality = (unsigned long)v;
        }
    }

	return bRes;
}

bool wxConfiguration::InitJpegHandler()
{
    if (ConvertCoversToJpeg())
    {
        m_imageHandler = wxImage::FindHandler( wxBITMAP_TYPE_JPEG );
        return m_imageHandler != nullptr;
    }

    return true;
}

wxString wxConfiguration::BoolToIdx( bool b )
{
	return b ? "01" : "00";
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

	AddFlag( as, flags, wxCueSheetReader::EC_PARSE_COMMENTS, "generate-tags-from-comments" );
	AddFlag( as, flags, wxCueSheetReader::EC_ELLIPSIZE_TAGS, "ellipsize-tags" );
	AddFlag( as, flags, wxCueSheetReader::EC_REMOVE_EXTRA_SPACES, "remove-extra-spaces" );
	AddFlag( as, flags, wxCueSheetReader::EC_SINGLE_MEDIA_FILE, "single-media-file" );
	AddFlag( as, flags, wxCueSheetReader::EC_MEDIA_READ_TAGS, "media-tags" );
	AddFlag( as, flags, wxCueSheetReader::EC_FIND_COVER, "find-cover" );
	AddFlag( as, flags, wxCueSheetReader::EC_FIND_LOG, "find-log" );
    AddFlag( as, flags, wxCueSheetReader::EC_FIND_ACCURIP_LOG, "find-accurip-log" );
    AddFlag( as, flags, wxCueSheetReader::EC_CONVERT_UPPER_ROMAN_NUMERALS, "upper-roman-numerals" );
    AddFlag( as, flags, wxCueSheetReader::EC_CONVERT_UPPER_ROMAN_NUMERALS, "lower-roman-numerals" );
    AddFlag( as, flags, wxCueSheetReader::EC_CONVERT_COVER_TO_JPEG, "convert-cover-to-jpeg" );
    AddFlag( as, flags, wxCueSheetReader::EC_CORRECT_DASHES, "correct-dashes" );

	wxString s;
	for ( size_t i = 0, nCount = as.GetCount(); i < nCount; ++i )
	{
		s << as[ i ] << ',';
	}

	return s.RemoveLast();
}

void wxConfiguration::FillArray( wxArrayString& as ) const
{
	as.Add( wxString::Format( "Rendering method: %s", ToString( m_eRenderMode ) ) );
	as.Add( wxString::Format( "Generate tags file: %s", BoolToStr( m_bGenerateTags ) ) );
	as.Add( wxString::Format( "Generate mkvmerge options file: %s", BoolToStr( m_bGenerateMkvmergeOpts ) ) );
    as.Add( wxString::Format( "Run mkvmerge: %s", BoolToStr( m_bRunMkvmerge ) ) );
	as.Add( wxString::Format( "Generate edition UID: %s", BoolToStr( m_bGenerateEditionUID ) ) );
	as.Add( wxString::Format( "Tag sources: %s", wxCueTag::SourcesToString( m_nTagSources ) ) );
	as.Add( wxString::Format( "Output file encoding: %s", ToString( m_eFileEncoding ) ) );
	as.Add( wxString::Format( "Calculate end time of chapters: %s", BoolToStr( m_bChapterTimeEnd ) ) );
	as.Add( wxString::Format( "Use data files to calculate end time of chapters: %s", BoolToStr( m_bUseDataFiles ) ) );

	as.Add( wxString::Format( "Alternate extensions: %s", m_sAlternateExtensions ) );
	as.Add( wxString::Format( "Set chapter's end time to beginning of next chapter if track's end time cannot be calculated: %s", BoolToStr( m_bUnknownChapterTimeEndToNextChapter ) ) );
	as.Add( wxString::Format( "Chapter offset (frames): %d", m_nChapterOffset ) );
	as.Add( wxString::Format( "Track name format: %s", m_sTrackNameFormat ) );
	as.Add( wxString::Format( "Matroska container name format: %s", m_sMatroskaNameFormat ) );
	as.Add( wxString::Format( "Chapter string language: %s", m_sLang ) );
	as.Add( wxString::Format( "For track 01 assume index %s as beginning of track", BoolToIdx( m_bTrackOneIndexOne ) ) );
	as.Add( wxString::Format( "Merge mode: %s", BoolToStr( m_bMerge ) ) );
	as.Add( wxString::Format( "Convert indexes to hidden subchapters: %s", BoolToStr( m_bHiddenIndexes ) ) );
	as.Add( wxString::Format( "Default cue sheet file extension: %s", m_sCueSheetExt ) );
	as.Add( wxString::Format( "Matroska chapters XML file extension: %s", m_sMatroskaChaptersXmlExt ) );
	as.Add( wxString::Format( "Matroska tags XML file extension: %s", m_sMatroskaTagsXmlExt ) );
	as.Add( wxString::Format( "mkvmerge options file extension: %s", m_sMatroskaOptsExt ) );
	as.Add( wxString::Format( "Correct \"simple 'quotation' marks\" inside strings: %s", BoolToStr( m_bCorrectQuotationMarks ) ) );
    as.Add( wxString::Format( "Render artist for tracks: %s", BoolToStr( m_bRenderArtistForTrack ) ) );
	as.Add( wxString::Format( "Read flags: %s", GetReadFlagsDesc( m_nReadFlags ) ) );
	as.Add( wxString::Format( "Use MLang library: %s", BoolToStr( m_bUseMLang ) ) );
    as.Add( wxString::Format( "JPEG image quality: %d", m_nJpegImageQuality ) );
}

void wxConfiguration::Dump() const
{
	if ( wxLog::IsLevelEnabled( wxLOG_Info, wxLOG_COMPONENT ) && wxLog::GetVerbose() )
	{
		wxString	  sSeparator( '=', 65 );
		wxArrayString as;
		as.Add( sSeparator );
		as.Add( _( "Configuration:" ) );
		FillArray( as );
		as.Add( wxString::Format( _( "Output path: \u201C%s\u201D" ), m_outputFile.GetFullPath() ) );
		as.Add( sSeparator );
		size_t	   strings = as.GetCount();
		wxDateTime dt( wxDateTime::Now() );
		wxLog*	   pLog = wxLog::GetActiveTarget();
		for ( size_t i = 0; i < strings; ++i )
		{
			pLog->OnLog( wxLOG_Info, as[ i ], dt.GetTicks() );
		}
	}
}

void wxConfiguration::BuildXmlComments( const wxFileName& outputFile, wxXmlNode* pNode ) const
{
	wxString sInit;

	sInit.Printf( "This file was created by %s", wxGetApp().GetAppDisplayName() );
	wxXmlNode* pComment = new wxXmlNode( nullptr, wxXML_COMMENT_NODE, wxEmptyString, sInit );
	pNode->AddChild( pComment );

	wxArrayString as;
	wxDateTime	  dtNow( wxDateTime::Now() );

	as.Add( wxString::Format( "Application version: %s", wxGetApp().APP_VERSION ) );
	as.Add( wxString::Format( "Application vendor: %s", wxGetApp().GetVendorDisplayName() ) );
	as.Add( wxString::Format( "Creation time: %s %s", dtNow.FormatISODate(), dtNow.FormatISOTime() ) );
	as.Add( wxString::Format( "Output file: \u201C%s\u201D", outputFile.GetFullName() ) );

	FillArray( as );

	size_t strings = as.GetCount();
	for ( size_t i = 0; i < strings; ++i )
	{
		wxXmlNode* pComment = new wxXmlNode( nullptr, wxXML_COMMENT_NODE, wxEmptyString, as[ i ] );
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

bool wxConfiguration::RenderArtistForTrack() const
{
    return m_bRenderArtistForTrack;
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
			sExt = "txt";
			break;
		}

		default:
		{
			wxASSERT( false );
			sExt = "???";
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

bool wxConfiguration::GetOutputFile( const wxInputFile& _inputFile, const wxString& sPostFix, const wxString& sExt, wxFileName& fn ) const
{
	wxFileName inputFile( _inputFile.GetInputFile() );

	if ( !inputFile.IsOk() )
	{
		return false;
	}

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetName( wxString::Format( "%s.%s", inputFile.GetName(), sPostFix ) );
		inputFile.SetExt( sExt );
		inputFile.Normalize();
		fn = inputFile;
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

		inputFile.SetName( wxString::Format( "%s.%s", inputFile.GetName(), sPostFix ) );
		inputFile.SetExt( sExt );
		inputFile.Normalize();
		fn = inputFile;
		return true;
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

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetName( wxString::Format( "%s.%s", inputFile.GetName(), sPostFix ) );
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

		inputFile.SetName( wxString::Format( "%s.%s", inputFile.GetName(), sPostFix ) );
		inputFile.SetExt( CUESHEET_EXT );
		inputFile.Normalize();
		cueFile = inputFile;
		return true;
	}
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

wxConfiguration::FILE_ENCODING wxConfiguration::GetFileEncoding() const
{
	return m_eFileEncoding;
}

wxString wxConfiguration::GetXmlFileEncoding() const
{
	switch ( m_eFileEncoding )
	{
		case ENCODING_UTF8:
		case ENCODING_UTF8_WITH_BOM:
		{
			return "UTF-8";
		}

		case ENCODING_UTF16_LE:
		case ENCODING_UTF16_LE_WITH_BOM:
		{
			return "UTF-16LE";
		}

		case ENCODING_UTF16_BE:
		case ENCODING_UTF16_BE_WITH_BOM:
		{
			return "UTF-16BE";
		}

		default:
		return "UTF-8";
	}
}

namespace
{
    void enc_2_cp( wxConfiguration::FILE_ENCODING enc, wxUint32& nCodePage, bool& bBom )
    {
        bBom = false;

        switch (enc)
        {
            case wxConfiguration::ENCODING_UTF8_WITH_BOM:
            {
                bBom = true;
            }

            case wxConfiguration::ENCODING_UTF8:
            {
                nCodePage = wxEncodingDetection::CP::UTF8;
                break;
            }

            case wxConfiguration::ENCODING_UTF16_LE_WITH_BOM:
            {
                 bBom = true;
            }

            case wxConfiguration::ENCODING_UTF16_LE:
            {
                nCodePage = wxEncodingDetection::CP::UTF16_LE;
                break;
            }

            case wxConfiguration::ENCODING_UTF16_BE_WITH_BOM:
            {
                bBom = true;
            }

            case wxConfiguration::ENCODING_UTF16_BE:
            {
                nCodePage = wxEncodingDetection::CP::UTF16_BE;
                break;
            }

            default:
            {
                nCodePage = wxEncodingDetection::GetDefaultEncoding();
                break;
            }
        }
    }
}

wxSharedPtr< wxTextOutputStream > wxConfiguration::GetOutputTextStream( wxOutputStream& os ) const
{
	wxSharedPtr< wxTextOutputStream > pRes;
	wxUint32						  nCodePage;
	bool							  bBom;
	enc_2_cp( m_eFileEncoding, nCodePage, bBom );

	return wxTextOutputStreamWithBOMFactory::Create( os, wxEOL_NATIVE, bBom, nCodePage, m_bUseMLang );
}

wxSharedPtr< wxMBConv > wxConfiguration::GetXmlEncoding() const
{
	wxString sDescription;

	wxSharedPtr< wxMBConv > pRes;
	wxUint32				nCodePage;
	bool					bBom;
	enc_2_cp( m_eFileEncoding, nCodePage, bBom );

	pRes = wxEncodingDetection::GetStandardMBConv( nCodePage, m_bUseMLang, sDescription );

	if ( !pRes )
	{
		pRes = wxEncodingDetection::GetDefaultEncoding( m_bUseMLang, sDescription );
	}

	return pRes;
}

bool wxConfiguration::MergeMode() const
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

bool wxConfiguration::UseFullPaths() const
{
	return m_bUseFullPaths;
}

bool wxConfiguration::AttachEacLog() const
{
	return (m_nReadFlags& wxCueSheetReader::EC_FIND_LOG) != 0;
}

bool wxConfiguration::AttachAccurateRipLog( ) const
{
    return (m_nReadFlags& wxCueSheetReader::EC_FIND_ACCURIP_LOG) != 0;
}

bool wxConfiguration::AttachCover() const
{
	return (m_nReadFlags& wxCueSheetReader::EC_FIND_COVER) != 0;
}

bool wxConfiguration::ConvertCoversToJpeg( ) const
{
    return (m_nReadFlags& wxCueSheetReader::EC_CONVERT_COVER_TO_JPEG) != 0;
}

int wxConfiguration::GetJpegImageQuality() const
{
    return m_nJpegImageQuality;
}

wxImageHandler* const wxConfiguration::GetImageHandler() const
{
    return m_imageHandler;
}

wxConfiguration::CUESHEET_ATTACH_MODE wxConfiguration::GetCueSheetAttachMode() const
{
	return m_eCsAttachMode;
}

#include <wx/arrimpl.cpp>	// this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayInputFile );

