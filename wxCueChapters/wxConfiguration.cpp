/*
   wxConfiguration.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include "wxConfiguration.h"
#include "wxApp.h"

// ===============================================================================

const wxChar wxConfiguration::CUE_SHEET_EXT[]		  = wxT( "cue" );
const wxChar wxConfiguration::MATROSKA_CHAPTERS_EXT[] = wxT( "mkc.xml" );
const wxChar wxConfiguration::MATROSKA_TAGS_EXT[]	  = wxT( "mkt.xml" );
const wxChar wxConfiguration::MATROSKA_OPTS_EXT[]	  = wxT( "opt.txt" );
const wxChar wxConfiguration::MATROSKA_AUDIO_EXT[]	  = wxT( "mka" );
const wxChar wxConfiguration::CUESHEET_EXT[]		  = wxT( "cue" );
const wxChar wxConfiguration::TRACK_NAME_FORMAT[]	  = wxT( "%dp% - %dt% - %tt%" );
const wxChar wxConfiguration::MATROSKA_NAME_FORMAT[]  = wxT( "%dp% - %dt%" );
const size_t wxConfiguration::MAX_EXT_LEN			  = 20;
const wxChar wxConfiguration::LANG_FILE_URL[]		  = wxT( "http://www.loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt" );
const wxChar wxConfiguration::LANG_FILE_NAME[]		  = wxT( "ISO-639-2_utf-8.txt" );

// ===============================================================================

const wxConfiguration::CuesheetAttachModeName wxConfiguration::AttachModeNames[] =
{
	{ CUESHEET_ATTACH_NONE, wxT( "none" ) },
	{ CUESHEET_ATTACH_SOURCE, wxT( "source" ) },
	{ CUESHEET_ATTACH_DECODED, wxT( "decoded" ) },
	{ CUESHEET_ATTACH_RENDERED, wxT( "rendered" ) }
};

const size_t wxConfiguration::AttachModeNamesSize = WXSIZEOF( wxConfiguration::AttachModeNames );

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxConfiguration, wxObject );

// ===============================================================================

wxString wxConfiguration::GetFileEncodingStr( wxConfiguration::FILE_ENCODING eFileEncoding )
{
	wxString s;

	switch ( eFileEncoding )
	{
		case ENCODING_LOCAL:
		s = wxT( "LOCAL" );
		break;

		case ENCODING_UTF8:
		s = wxT( "UTF-8" );
		break;

		case ENCODING_UTF8_WITH_BOM:
		s = wxT( "UTF-8 (BOM)" );
		break;

		case ENCODING_UTF16:
		s = wxT( "UTF-16" );
		break;

		case ENCODING_UTF16_WITH_BOM:
		s = wxT( "UTF-16 (BOM)" );
		break;

		default:
		s.Printf( wxT( "UNKNOWN %d" ), eFileEncoding );
		break;
	}
	return s;
}

bool wxConfiguration::GetFileEncodingFromStr( const wxString& sFileEncoding, wxConfiguration::FILE_ENCODING& eFileEncoding )
{
	if (
		sFileEncoding.CmpNoCase( wxT( "local" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxT( "default" ) ) == 0
		)
	{
		eFileEncoding = ENCODING_LOCAL;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( wxT( "utf8" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxT( "utf-8" ) ) == 0
		)
	{
		eFileEncoding = ENCODING_UTF8;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( wxT( "utf8_bom" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxT( "utf8bom" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxT( "utf-8_bom" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxT( "utf-8-bom" ) ) == 0
		)
	{
		eFileEncoding = ENCODING_UTF8_WITH_BOM;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( wxT( "utf16" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxT( "utf-16" ) ) == 0
		)
	{
		eFileEncoding = ENCODING_UTF16;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( wxT( "utf16_bom" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxT( "utf16bom" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxT( "utf-16_bom" ) ) == 0 ||
		sFileEncoding.CmpNoCase( wxT( "utf-16-bom" ) ) == 0
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

wxString wxConfiguration::GetCueSheetAttachModeStr( wxConfiguration::CUESHEET_ATTACH_MODE eCsAttachMode )
{
	wxString s;
	bool	 bRes = false;

	for ( size_t i = 0; i < AttachModeNamesSize && !bRes; i++ )
	{
		if ( AttachModeNames[ i ].eMode == eCsAttachMode )
		{
			s = AttachModeNames[ i ].pszName;
			s.UpperCase();
			bRes = true;
		}
	}

	if ( !bRes )
	{
		s.Printf( wxT( "UNKNOWN <%d>" ), eCsAttachMode );
	}

	return s;
}

bool wxConfiguration::GetCueSheetAttachModeFromStr( const wxString& sCsAttachMode, wxConfiguration::CUESHEET_ATTACH_MODE& eCsAttachMode, bool& bDefault )
{
	if ( sCsAttachMode.CmpNoCase( wxT( "default" ) ) == 0 )
	{
		bDefault = true;
		return true;
	}
	else
	{
		bDefault = false;
		for ( size_t i = 0; i < AttachModeNamesSize; i++ )
		{
			if ( sCsAttachMode.CmpNoCase( AttachModeNames[ i ].pszName ) )
			{
				eCsAttachMode = AttachModeNames[ i ].eMode;
				return true;
			}
		}

		return false;
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
		wxLogDebug( wxT( "Cannot open language file \u201C%s\u201D" ), fn.GetFullPath() );
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

		wxStringTokenizer tokenizer( sLine, wxT( "|" ) );
		if ( tokenizer.HasMoreTokens() )
		{
			wxString sLang( tokenizer.GetNextToken() );
			if ( sLang.IsEmpty() || ( sLang.Length() > 3 ) )
			{
				wxLogDebug( wxT( "Skipping language %s" ), sLang );
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
	m_bChapterTimeEnd( true ),
	m_bUnknownChapterTimeEndToNextChapter( false ),
	m_nChapterOffset( 150 ),
	m_bUseDataFiles( false ),
	m_sAlternateExtensions( wxEmptyString ),
	m_sLang( wxT( "unk" ) ),
	m_sTrackNameFormat( TRACK_NAME_FORMAT ),
	m_sMatroskaNameFormat( MATROSKA_NAME_FORMAT ),
	m_bEmbedded( false ),
	m_bCorrectQuotationMarks( true ),
	m_bSaveCueSheet( false ),
	m_eCueSheetFileEncoding( ENCODING_LOCAL ),
	m_bGenerateTags( false ),
	m_bGenerateMkvmergeOpts( false ),
	m_bGenerateEditionUID( false ),
	m_bGenerateTagsFromComments( true ),
	m_bRunMkvmerge( true ),
	m_bTrackOneIndexOne( true ),
	m_bAbortOnError( true ),
	m_bHiddenIndexes( false ),
	m_sCueSheetExt( CUE_SHEET_EXT ),
	m_sMatroskaChaptersXmlExt( MATROSKA_CHAPTERS_EXT ),
	m_sMatroskaTagsXmlExt( MATROSKA_TAGS_EXT ),
	m_sMatroskaOptsExt( MATROSKA_OPTS_EXT ),
	m_bMerge( false ),
	m_nReadFlags( wxCueSheetReader::EC_MEDIA_READ_TAGS | wxCueSheetReader::EC_FLAC_READ_TAG_FIRST_THEN_COMMENT | wxCueSheetReader::EC_FIND_COVER | wxCueSheetReader::EC_FIND_LOG ),
	m_bUseMLang( true ),
	m_bUseFullPaths( false ),
	m_bEllipsizeTags( true ),
	m_eCsAttachMode( CUESHEET_ATTACH_NONE ),
	m_bRemoveExtraSpaces( true )
{
	ReadLanguagesStrings( m_asLang );
}

void wxConfiguration::AddCmdLineParams( wxCmdLineParser& cmdLine )
{
	cmdLine.AddOption( wxT( "o" ), wxT( "output" ), _( "Output Matroska chapter file or cue sheet file (see -c option)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT( "od" ), wxT( "output-directory" ), _( "Output directory (default: input directory)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT( "ce" ), wxT( "chapter-time-end" ), _( "Calculate end time of chapters if possible (default: on)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxT( "cn" ), wxT( "unknown-chapter-end-to-next-track" ), _( "If track's end time is unknown set it to next track position using frame offset (default: off)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxT( "fo" ), wxT( "frame-offset" ), _( "Offset in frames to use with -uc option (default: 150)" ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT( "df" ), wxT( "use-data-files" ), _( "Use data file(s) to calculate end time of chapters (default: off)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxT( "e" ), wxT( "alternate-extensions" ), _( "Comma-separated list of alternate extensions of data files (default: none)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT( "f" ), wxT( "track-title-format" ), wxString::Format( _( "Track title format (default: %s)" ), TRACK_NAME_FORMAT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT( "l" ), wxT( "language" ), _( "Set language of chapter's tilte (default: unk)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT( "ec" ), wxT( "embedded-cue" ), _( "Try to read embedded cue sheet (requires MediaInfo library)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxT( "m" ), wxT( "save-matroska-chapters" ), _( "Save Matroska chapter file (default) or cue sheet file" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxT( "t" ), wxT( "generate-tags" ), _( "Generate tags file (default: no)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxT( "k" ), wxT( "generate-mkvmerge-options" ), _( "Generate file with mkvmerge options (default: no)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxT( "run-mkvmerge" ), _( "Run mkvmerge tool after generation of options file (default: yes)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxT( "eu" ), wxT( "generate-edition-uid" ), _( "Generate edition UID (default: no)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxT( "tc" ), wxT( "generate-tags-from-comments" ), _( "Try to parse tags from cue sheet comments (default: yes)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxT( "oce" ), wxT( "cue-sheet-encoding" ), _( "Output cue sheet file encoding - possible values are local (default), utf8, utf8_bom, utf16, utf16_bom" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT( "a" ), wxT( "abort-on-error" ), _( "Abort when conversion errors occurs (default)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxT( "j" ), wxT( "merge" ), _( "Merge cue sheets (default: no)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxT( "mf" ), wxT( "matroska-title-format" ), wxString::Format( _( "Mtroska container's title format (default: %s)" ), MATROSKA_NAME_FORMAT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT( "cq" ), wxT( "correct-quotation-marks" ), _( "Correct \"simple 'quotation' marks\" to \u201Cenglish \u2018quotation\u2019 marks\u201D inside strings (default: on)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxT( "et" ), wxT( "ellipsize-tags" ), wxString::Format( _( "Ellipsize tags - convert last three dots to '%c' (default: on)" ), wxEllipsizer::ELLIPSIS ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxT( "rs" ), wxT( "remove-extra-spaces" ), _( "Remove extra spaces from tags (default: yes)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	// embedded mode flags
	cmdLine.AddSwitch( wxEmptyString, wxT( "single-media-file" ), _( "Embedded mode flag. Assume input as single media file without cuesheet (default: no)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxT( "flac-read-none" ), _( "Embedded mode flag - FLAC cuesheet read mode. Do not read cuesheet from FLAC container" ), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT( "flac-read-cuesheet-tag-first" ), _( "Embedded mode flag - FLAC cuesheet read mode. Try to read embedded cuesheet from FLAC container - first try read CUESHEET tag then try CUESHEET comment (default)" ), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT( "flac-read-vorbis-comment-first" ), _( "Embedded mode flag - FLAC cuesheet read mode. Try to read embedded cuesheet from FLAC container - first try read CUESHEET comment then try CUESHEET tag" ), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT( "flac-read-cuesheet-tag-only" ), _( "Embedded mode flag - FLAC cuesheet read mode. Try to read embedded cuesheet from FLAC container - try read CUESHEET tag only" ), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT( "flac-read-vorbis-comment-only" ), _( "Embedded mode flag - FLAC cuesheet read mode. Try to read embedded cuesheet from FLAC container - try read CUESHEET comment only" ), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT( "read-media-tags" ), _( "Embedded mode flag. Read tags from media file (default: yes)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	// advanced options
	cmdLine.AddSwitch( wxEmptyString, wxT( "use-mlang" ), _( "Use MLang library (default)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxT( "t1i0" ), wxT( "track-01-index-00" ), _( "For first track use index 00 as beginning of track (by default index 01 is used)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxT( "hidden-indexes" ), _( "Convert indexes to hidden (sub)chapters (default no)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxT( "attach-eac-log" ), _( "Attach EAC log file to mkvmerge options file (default: yes)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxT( "attach-cover" ), _( "Attach cover image (cover.*;front.*;album.*) to mkvmerge options file (default: yes)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxEmptyString, wxT( "cue-sheet-attach-mode" ), _( "Mode of attaching cue sheet to mkvmerge options file - possible values are none (default), source, decoded, rendered and default" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT( "use-full-paths" ), _( "Use full paths in mkvmerge options file (default: no)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( wxEmptyString, wxT( "mkvmerge-directory" ), _( "Location of mkvmerge tool" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	// tags usage
	cmdLine.AddSwitch( wxEmptyString, wxT( "use-cdtext-tags" ), _( "Use CD-TEXT tags (default)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxT( "use-cue-comments-tags" ), _( "Use tags from cuesheet comments (default)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( wxEmptyString, wxT( "use-media-tags" ), _( "Use tags from media file (default)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	// extensions
	cmdLine.AddOption( wxEmptyString, wxT( "cue-sheet-file-extension" ), wxString::Format( _( "Cue sheet file extension (default: %s)" ), CUE_SHEET_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, wxT( "matroska-chapters-file-extension" ), wxString::Format( _( "Matroska chapters XML file extension (default: %s)" ), MATROSKA_CHAPTERS_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, wxT( "matroska-tags-file-extension" ), wxString::Format( _( "Matroska tags XML file extension (default: %s)" ), MATROSKA_TAGS_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, wxT( "mkvmerge-options-file-extension" ), wxString::Format( _( "File extension of mkvmerge options file (default: %s)" ), MATROSKA_OPTS_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

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

bool wxConfiguration::ReadNegatableSwitchValue( const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal )
{
	wxCmdLineSwitchState state = cmdLine.FoundSwitch( name );
	bool				 res   = true;

	switch ( state )
	{
		case wxCMD_SWITCH_ON:
		switchVal = true;
		break;

		case wxCMD_SWITCH_OFF:
		switchVal = false;
		break;

		default:
		res = false;
		break;
	}
	return res;
}

bool wxConfiguration::Read( const wxCmdLineParser& cmdLine )
{
	bool	 bRes = true;
	wxString s;
	long	 v;
	bool	 switchVal;

	ReadNegatableSwitchValue( cmdLine, wxT( "ce" ), m_bChapterTimeEnd );
	ReadNegatableSwitchValue( cmdLine, wxT( "cn" ), m_bUnknownChapterTimeEndToNextChapter );

	if ( cmdLine.Found( wxT( "fo" ), &v ) )
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

	ReadNegatableSwitchValue( cmdLine, wxT( "df" ), m_bUseDataFiles );
	ReadNegatableSwitchValue( cmdLine, wxT( "ec" ), m_bEmbedded );
	ReadNegatableSwitchValue( cmdLine, wxT( "cq" ), m_bCorrectQuotationMarks );
	ReadNegatableSwitchValue( cmdLine, wxT( "et" ), m_bEllipsizeTags );
	ReadNegatableSwitchValue( cmdLine, wxT( "m" ), m_bSaveCueSheet );

	if ( ReadNegatableSwitchValue( cmdLine, wxT( "t1i0" ), m_bTrackOneIndexOne ) )
	{
		m_bTrackOneIndexOne = !m_bTrackOneIndexOne;
	}

	ReadNegatableSwitchValue( cmdLine, wxT( "a" ), m_bAbortOnError );
	ReadNegatableSwitchValue( cmdLine, wxT( "t" ), m_bGenerateTags );
	ReadNegatableSwitchValue( cmdLine, wxT( "k" ), m_bGenerateMkvmergeOpts );
	ReadNegatableSwitchValue( cmdLine, wxT( "hidden-indexes" ), m_bHiddenIndexes );
	ReadNegatableSwitchValue( cmdLine, wxT( "run-mkvmerge" ), m_bRunMkvmerge );
	ReadNegatableSwitchValue( cmdLine, wxT( "use-full-paths" ), m_bUseFullPaths );

	if ( cmdLine.Found( wxT( "oce" ), &s ) )
	{
		if ( !GetFileEncodingFromStr( s, m_eCueSheetFileEncoding ) )
		{
			wxLogWarning( _( "Wrong cue sheet file encoding %s" ), s );
			bRes = false;
		}
	}

	ReadNegatableSwitchValue( cmdLine, wxT( "eu" ), m_bGenerateEditionUID );
	ReadNegatableSwitchValue( cmdLine, wxT( "tc" ), m_bGenerateTagsFromComments );
	ReadNegatableSwitchValue( cmdLine, wxT( "j" ), m_bMerge );

	if ( cmdLine.Found( wxT( "cue-sheet-file-extension" ), &s ) )
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

	if ( cmdLine.Found( wxT( "matroska-chapters-file-extension" ), &s ) )
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

	if ( cmdLine.Found( wxT( "matroska-tags-file-extension" ), &s ) )
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

	if ( cmdLine.Found( wxT( "mkvmerge-options-file-extension" ), &s ) )
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

	if ( cmdLine.Found( wxT( "e" ), &s ) )
	{
		m_sAlternateExtensions = s;
	}

	if ( cmdLine.Found( wxT( "f" ), &s ) )
	{
		m_sTrackNameFormat = s;
	}

	if ( cmdLine.Found( wxT( "mf" ), &s ) )
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

	if ( cmdLine.Found( wxT( "l" ), &s ) )
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

	if ( cmdLine.Found( wxT( "o" ), &s ) )
	{
		m_outputFile.Assign( s );
		if ( !m_outputFile.MakeAbsolute() )
		{
			wxLogInfo( _( "Fail to normalize path \u201C%s\u201D" ), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT( "od" ), &s ) )
	{
		m_outputFile.AssignDir( s );
		if ( !m_outputFile.MakeAbsolute() )
		{
			wxLogInfo( _( "Fail to normalize path \u201C%s\u201D" ), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT( "mkvmerge-directory" ), &s ) )
	{
		m_mkvmergeDir.AssignDir( s );
		if ( !m_mkvmergeDir.MakeAbsolute() )
		{
			wxLogInfo( _( "Fail to normalize path \u201C%s\u201D" ), s );
			bRes = false;
		}
	}

	if ( ReadNegatableSwitchValue( cmdLine, wxT( "use-cdtext-tags" ), switchVal ) )
	{
		if ( switchVal )
		{
			RemoveIgnoredTagSource( wxCueTag::TAG_CD_TEXT );
		}
		else
		{
			AddIgnoredTagSource( wxCueTag::TAG_CD_TEXT );
		}
	}

	if ( ReadNegatableSwitchValue( cmdLine, wxT( "use-cue-comments-tags" ), switchVal ) )
	{
		if ( switchVal )
		{
			RemoveIgnoredTagSource( wxCueTag::TAG_CUE_COMMENT );
		}
		else
		{
			AddIgnoredTagSource( wxCueTag::TAG_CUE_COMMENT );
		}
	}

	if ( ReadNegatableSwitchValue( cmdLine, wxT( "use-media-tags" ), switchVal ) )
	{
		if ( switchVal )
		{
			RemoveIgnoredTagSource( wxCueTag::TAG_MEDIA_METADATA );
		}
		else
		{
			AddIgnoredTagSource( wxCueTag::TAG_MEDIA_METADATA );
		}
	}

	if ( ReadNegatableSwitchValue( cmdLine, wxT( "single-media-file" ), switchVal ) )
	{
		m_nReadFlags &= ~wxCueSheetReader::EC_SINGLE_MEDIA_FILE;
		m_nReadFlags |= switchVal ? wxCueSheetReader::EC_SINGLE_MEDIA_FILE : 0u;
	}

	if ( cmdLine.Found( wxT( "flac-read-none" ) ) )
	{
		m_nReadFlags &= ~wxCueSheetReader::EC_FLAC_READ_MASK;
		m_nReadFlags |= wxCueSheetReader::EC_FLAC_READ_NONE;
	}

	if ( cmdLine.Found( wxT( "flac-read-cuesheet-tag-first" ) ) )
	{
		m_nReadFlags &= ~wxCueSheetReader::EC_FLAC_READ_MASK;
		m_nReadFlags |= wxCueSheetReader::EC_FLAC_READ_TAG_FIRST_THEN_COMMENT;
	}

	if ( cmdLine.Found( wxT( "flac-read-vorbis-comment-first" ) ) )
	{
		m_nReadFlags &= ~wxCueSheetReader::EC_FLAC_READ_MASK;
		m_nReadFlags |= wxCueSheetReader::EC_FLAC_READ_COMMENT_FIRST_THEN_TAG;
	}

	if ( cmdLine.Found( wxT( "flac-read-cuesheet-tag-only" ) ) )
	{
		m_nReadFlags &= ~wxCueSheetReader::EC_FLAC_READ_MASK;
		m_nReadFlags |= wxCueSheetReader::EC_FLAC_READ_TAG_ONLY;
	}

	if ( cmdLine.Found( wxT( "flac-read-vorbis-comment-only" ) ) )
	{
		m_nReadFlags &= ~wxCueSheetReader::EC_FLAC_READ_MASK;
		m_nReadFlags |= wxCueSheetReader::EC_FLAC_READ_COMMENT_ONLY;
	}

	if ( ReadNegatableSwitchValue( cmdLine, wxT( "read-media-tags" ), switchVal ) )
	{
		if ( switchVal )
		{
			m_nReadFlags &= ~wxCueSheetReader::EC_MEDIA_READ_TAGS;
			m_nReadFlags |= wxCueSheetReader::EC_MEDIA_READ_TAGS;
		}
		else
		{
			m_nReadFlags &= ~wxCueSheetReader::EC_MEDIA_READ_TAGS;
			m_nReadFlags |= 0;
		}
	}

	if ( ReadNegatableSwitchValue( cmdLine, wxT( "attach-eac-log" ), switchVal ) )
	{
		if ( switchVal )
		{
			m_nReadFlags &= ~wxCueSheetReader::EC_FIND_LOG;
			m_nReadFlags |= wxCueSheetReader::EC_FIND_LOG;
		}
		else
		{
			m_nReadFlags &= ~wxCueSheetReader::EC_FIND_LOG;
			m_nReadFlags |= 0;
		}
	}

	if ( ReadNegatableSwitchValue( cmdLine, wxT( "attach-cover" ), switchVal ) )
	{
		if ( switchVal )
		{
			m_nReadFlags &= ~wxCueSheetReader::EC_FIND_COVER;
			m_nReadFlags |= wxCueSheetReader::EC_FIND_COVER;
		}
		else
		{
			m_nReadFlags &= ~wxCueSheetReader::EC_FIND_COVER;
			m_nReadFlags |= 0;
		}
	}

	// MLang
	ReadNegatableSwitchValue( cmdLine, wxT( "use-mlang" ), m_bUseMLang );
	ReadNegatableSwitchValue( cmdLine, wxT( "rs" ), m_bRemoveExtraSpaces );

	if ( cmdLine.Found( wxT( "cue-sheet-attach-mode" ), &s ) )
	{
		bool bDefault;
		if ( GetCueSheetAttachModeFromStr( s, m_eCsAttachMode, bDefault ) )
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

wxString wxConfiguration::BoolToStr( bool b )
{
	return b ? wxT( "yes" ) : wxT( "no" );
}

wxString wxConfiguration::BoolToIdx( bool b )
{
	return b ? wxT( "01" ) : wxT( "00" );
}

void wxConfiguration::AddFlag( wxArrayString& as, wxCueSheetReader::ReadFlags flags, wxCueSheetReader::ReadFlags mask, const wxString& sText )
{
	if ( wxCueSheetReader::TestReadFlags( flags, mask ) )
	{
		as.Add( sText );
	}
}

wxString wxConfiguration::GetReadFlagsDesc( wxCueSheetReader::ReadFlags flags )
{
	wxArrayString as;

	wxString s;

	switch ( flags & wxCueSheetReader::EC_FLAC_READ_MASK )
	{
		case wxCueSheetReader::EC_FLAC_READ_NONE:
		s = wxT( "flac-read-none" );
		break;

		case wxCueSheetReader::EC_FLAC_READ_TAG_FIRST_THEN_COMMENT:
		s = wxT( "flac-read-cuesheet-tag-first" );
		break;

		case wxCueSheetReader::EC_FLAC_READ_COMMENT_FIRST_THEN_TAG:
		s = wxT( "flac-read-vorbis-comment-first" );
		break;

		case wxCueSheetReader::EC_FLAC_READ_COMMENT_ONLY:
		s = wxT( "flac-read-cuesheet-tag-only" );
		break;

		case wxCueSheetReader::EC_FLAC_READ_TAG_ONLY:
		s = wxT( "flac-read-vorbis-comment-only" );
		break;

		default:
		s.Printf( wxT( "flac-read-<%08x>" ), ( flags & wxCueSheetReader::EC_FLAC_READ_MASK ) );
		break;
	}
	as.Add( s );

	AddFlag( as, flags, wxCueSheetReader::EC_SINGLE_MEDIA_FILE, wxT( "single-media-file" ) );
	AddFlag( as, flags, wxCueSheetReader::EC_MEDIA_READ_TAGS, wxT( "media-tags" ) );
	AddFlag( as, flags, wxCueSheetReader::EC_FIND_COVER, wxT( "find-cover" ) );
	AddFlag( as, flags, wxCueSheetReader::EC_FIND_LOG, wxT( "find-log" ) );

	s.Empty();
	size_t nItems = as.GetCount();
	for ( size_t i = 0; i < nItems; i++ )
	{
		s += as[ i ];
		s += wxT( ',' );
	}

	return s.RemoveLast();
}

wxString wxConfiguration::GetTagSourcesNames( const wxArrayTagSource& sources )
{
	wxString s;
	size_t	 nSources = sources.GetCount();

	wxASSERT( nSources > 0 );
	for ( size_t i = 0; i < nSources; i++ )
	{
		s += wxCueTag::SourceToString( sources[ i ] );
		s += wxT( ',' );
	}

	return s.RemoveLast();
}

void wxConfiguration::FillArray( wxArrayString& as ) const
{
	as.Add( wxString::Format( wxT( "Save cue sheet: %s" ), BoolToStr( m_bSaveCueSheet ) ) );
	as.Add( wxString::Format( wxT( "Generate tags file: %s" ), BoolToStr( m_bGenerateTags ) ) );
	as.Add( wxString::Format( wxT( "Generate mkvmerge options file: %s" ), BoolToStr( m_bGenerateMkvmergeOpts ) ) );
	if ( m_bGenerateMkvmergeOpts )
	{
		as.Add( wxString::Format( wxT( "Run mkvmerge: %s" ), BoolToStr( m_bRunMkvmerge ) ) );
		if ( m_mkvmergeDir.IsOk() )
		{
			as.Add( wxString::Format( wxT( "mkvmerge dir: %s" ), m_mkvmergeDir.GetFullPath() ) );
		}

		as.Add( wxString::Format( wxT( "Generate full paths: %s" ), BoolToStr( m_bUseFullPaths ) ) );
		as.Add( wxString::Format( wxT( "Cue sheet attach mode: %s" ), GetCueSheetAttachModeStr( m_eCsAttachMode ) ) );
	}

	as.Add( wxString::Format( wxT( "Generate edition UID: %s" ), BoolToStr( m_bGenerateEditionUID ) ) );
	as.Add( wxString::Format( wxT( "Generate tags from comments: %s" ), BoolToStr( m_bGenerateTagsFromComments ) ) );
	if ( !m_aeIgnoredSources.IsEmpty() )
	{
		as.Add( wxString::Format( wxT( "Ignored tag sources: %s" ), GetTagSourcesNames( m_aeIgnoredSources ) ) );
	}

	as.Add( wxString::Format( wxT( "Output cue sheet file encoding: %s" ), GetFileEncodingStr( m_eCueSheetFileEncoding ) ) );
	as.Add( wxString::Format( wxT( "Calculate end time of chapters: %s" ), BoolToStr( m_bChapterTimeEnd ) ) );
	as.Add( wxString::Format( wxT( "Read embedded cue sheet: %s" ), BoolToStr( m_bEmbedded ) ) );
	as.Add( wxString::Format( wxT( "Use data files to calculate end time of chapters: %s" ), BoolToStr( m_bUseDataFiles ) ) );

	as.Add( wxString::Format( wxT( "Alternate extensions: %s" ), m_sAlternateExtensions ) );
	as.Add( wxString::Format( wxT( "Set chapter's end time to beginning of next chapter if track's end time cannot be calculated: %s" ), BoolToStr( m_bUnknownChapterTimeEndToNextChapter ) ) );
	as.Add( wxString::Format( wxT( "Chapter offset (frames): %d" ), m_nChapterOffset ) );
	as.Add( wxString::Format( wxT( "Track name format: %s" ), m_sTrackNameFormat ) );
	as.Add( wxString::Format( wxT( "Matroska container name format: %s" ), m_sMatroskaNameFormat ) );
	as.Add( wxString::Format( wxT( "Chapter string language: %s" ), m_sLang ) );
	as.Add( wxString::Format( wxT( "For track 01 assume index %s as beginning of track" ), BoolToIdx( m_bTrackOneIndexOne ) ) );
	as.Add( wxString::Format( wxT( "Merge mode: %s" ), BoolToStr( m_bMerge ) ) );
	as.Add( wxString::Format( wxT( "Convert indexes to hidden subchapters: %s" ), BoolToStr( m_bHiddenIndexes ) ) );
	as.Add( wxString::Format( wxT( "Default cue sheet file extension: %s" ), m_sCueSheetExt ) );
	as.Add( wxString::Format( wxT( "Matroska chapters XML file extension: %s" ), m_sMatroskaChaptersXmlExt ) );
	as.Add( wxString::Format( wxT( "Matroska tags XML file extension: %s" ), m_sMatroskaTagsXmlExt ) );
	as.Add( wxString::Format( wxT( "mkvmerge options file extension: %s" ), m_sMatroskaOptsExt ) );
	as.Add( wxString::Format( wxT( "Correct \"simple 'quotation' marks\" inside strings: %s" ), BoolToStr( m_bCorrectQuotationMarks ) ) );
	as.Add( wxString::Format( wxT( "Ellipsize tags: %s" ), BoolToStr( m_bEllipsizeTags ) ) );
	as.Add( wxString::Format( wxT( "Remove extra spaces from tags: %s" ), BoolToStr( m_bRemoveExtraSpaces ) ) );
	as.Add( wxString::Format( wxT( "Read flags: %s" ), GetReadFlagsDesc( m_nReadFlags ) ) );
	as.Add( wxString::Format( wxT( "Use MLang library: %s" ), BoolToStr( m_bUseMLang ) ) );
}

void wxConfiguration::Dump() const
{
	if ( wxLog::IsLevelEnabled( wxLOG_Info, wxLOG_COMPONENT ) && wxLog::GetVerbose() )
	{
		wxString	  sSeparator( wxT( '=' ), 65 );
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

	sInit.Printf( wxT( "This file was created by %s tool" ), wxGetApp().GetAppDisplayName() );
	wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString, sInit );
	pNode->AddChild( pComment );

	wxArrayString as;
	wxDateTime	  dtNow( wxDateTime::Now() );

	as.Add( wxString::Format( wxT( "Application version: %s" ), wxGetApp().APP_VERSION ) );
	as.Add( wxString::Format( wxT( "Application vendor: %s" ), wxGetApp().GetVendorDisplayName() ) );
	as.Add( wxString::Format( wxT( "Creation time: %s %s" ), dtNow.FormatISODate(), dtNow.FormatISOTime() ) );
	as.Add( wxString::Format( wxT( "Output file: \u201C%s\u201D" ), outputFile.GetFullName() ) );

	FillArray( as );

	size_t strings = as.GetCount();
	for ( size_t i = 0; i < strings; i++ )
	{
		wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString, as[ i ] );
		pNode->AddChild( pComment );
	}
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

wxString wxConfiguration::GetOutputFile( const wxInputFile& _inputFile ) const
{
	wxFileName inputFile( _inputFile.GetInputFile() );

	if ( !inputFile.IsOk() )
	{
		return wxEmptyString;
	}

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetExt( m_bSaveCueSheet ? m_sCueSheetExt : m_sMatroskaChaptersXmlExt );
	}
	else
	{
		if ( m_outputFile.IsDir() )
		{
			inputFile.SetPath( m_outputFile.GetPath() );
			inputFile.SetExt( m_bSaveCueSheet ? m_sCueSheetExt : m_sMatroskaChaptersXmlExt );
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
		inputFile.SetExt( m_bSaveCueSheet ? m_sCueSheetExt : m_sMatroskaChaptersXmlExt );
		outputFile = inputFile;

		if ( !m_bSaveCueSheet && m_bGenerateTags )
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
			inputFile.SetExt( m_bSaveCueSheet ? m_sCueSheetExt : m_sMatroskaChaptersXmlExt );
		}
		else
		{
			inputFile = m_outputFile;
		}

		outputFile = inputFile;
		if ( !m_bSaveCueSheet && m_bGenerateTags )
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

	if ( !( !m_bSaveCueSheet && m_bGenerateMkvmergeOpts ) )
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

	if ( !( !m_bSaveCueSheet && m_bGenerateMkvmergeOpts ) )
	{
		return false;
	}

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetName( wxString::Format( wxT( "%s.%s" ), inputFile.GetName(), sPostFix ) );
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

		inputFile.SetName( wxString::Format( wxT( "%s.%s" ), inputFile.GetName(), sPostFix ) );
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

bool wxConfiguration::SaveCueSheet() const
{
	return m_bSaveCueSheet;
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

bool wxConfiguration::GenerateTagsFromComments() const
{
	return m_bGenerateTagsFromComments;
}

wxConfiguration::FILE_ENCODING wxConfiguration::GetCueSheetFileEncoding() const
{
	return m_eCueSheetFileEncoding;
}

wxSharedPtr<wxTextOutputStream> wxConfiguration::GetOutputTextStream( wxOutputStream& os ) const
{
	wxSharedPtr<wxTextOutputStream> pRes;
	switch ( m_eCueSheetFileEncoding )
	{
		case ENCODING_UTF8:
		pRes = wxTextOutputStreamWithBOMFactory::CreateUTF8( os, wxEOL_NATIVE, false, m_bUseMLang );
		break;

		case ENCODING_UTF8_WITH_BOM:
		pRes = wxTextOutputStreamWithBOMFactory::CreateUTF8( os, wxEOL_NATIVE, true, m_bUseMLang );
		break;

		case ENCODING_UTF16:
		pRes = wxTextOutputStreamWithBOMFactory::CreateUTF16( os, wxEOL_NATIVE, false, m_bUseMLang );
		break;

		case ENCODING_UTF16_WITH_BOM:
		pRes = wxTextOutputStreamWithBOMFactory::CreateUTF16( os, wxEOL_NATIVE, true, m_bUseMLang );
		break;

		default:
		pRes = new wxTextOutputStream( os, wxEOL_NATIVE, wxConvLocal );
		break;
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

void wxConfiguration::AddIgnoredTagSource( wxCueTag::TAG_SOURCE eSource )
{
	if ( m_aeIgnoredSources.Index( eSource ) == wxNOT_FOUND )
	{
		m_aeIgnoredSources.Add( eSource );
	}
}

void wxConfiguration::RemoveIgnoredTagSource( wxCueTag::TAG_SOURCE eSource )
{
	int nIdx = m_aeIgnoredSources.Index( eSource );

	if ( nIdx != wxNOT_FOUND )
	{
		m_aeIgnoredSources.RemoveAt( nIdx );
	}
}

bool wxConfiguration::ShouldIgnoreTag( const wxCueTag& tag ) const
{
	return ( m_aeIgnoredSources.Index( tag.GetSource() ) != wxNOT_FOUND );
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

bool wxConfiguration::EllipsizeTags() const
{
	return m_bEllipsizeTags;
}

bool wxConfiguration::AttachEacLog() const
{
	return ( m_nReadFlags & wxCueSheetReader::EC_FIND_LOG ) != 0;
}

bool wxConfiguration::AttachCover() const
{
	return ( m_nReadFlags & wxCueSheetReader::EC_FIND_COVER ) != 0;
}

bool wxConfiguration::RemoveExtraSpaces() const
{
	return m_bRemoveExtraSpaces;
}

wxConfiguration::CUESHEET_ATTACH_MODE wxConfiguration::GetCueSheetAttachMode() const
{
	return m_eCsAttachMode;
}

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayInputFile );

