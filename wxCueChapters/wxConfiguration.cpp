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

const wxChar wxConfiguration::CUE_SHEET_EXT[] = wxT("cue");
const wxChar wxConfiguration::MATROSKA_CHAPTERS_EXT[] = wxT("mkc.xml");
const wxChar wxConfiguration::MATROSKA_TAGS_EXT[] = wxT("mkt.xml");
const wxChar wxConfiguration::MATROSKA_OPTS_EXT[] = wxT("opt.txt");
const wxChar wxConfiguration::MATROSKA_AUDIO_EXT[] = wxT("mka");

const wxChar wxConfiguration::TRACK_NAME_FORMAT[] = wxT("%dp% - %dt% - %tt%");
const wxChar wxConfiguration::MATROSKA_NAME_FORMAT[] = wxT("%dp% - %dt%");

static const size_t MAX_EXT_LEN = 20;
static const wxChar LANG_FILE_URL[] = wxT("http://www.loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt");
static const wxChar LANG_FILE_NAME[] = wxT("ISO-639-2_utf-8.txt");

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxConfiguration, wxObject )

// ===============================================================================

wxString wxConfiguration::GetFileEncodingStr( wxConfiguration::FILE_ENCODING eFileEncoding )
{
	wxString s;
	switch( eFileEncoding )
	{
		case ENCODING_LOCAL:
		s = wxT("LOCAL");
		break;

		case ENCODING_UTF8:
		s = wxT("UTF-8");
		break;

		case ENCODING_UTF8_WITH_BOM:
		s = wxT("UTF-8 (BOM)");
		break;

		case ENCODING_UTF16:
		s = wxT("UTF-16");
		break;

		case ENCODING_UTF16_WITH_BOM:
		s = wxT("UTF-16 (BOM)");
		break;

		default:
		s.Printf( wxT("UNKNOWN %d"), eFileEncoding );
		break;
	}

	return s;
}

bool wxConfiguration::GetFileEncodingFromStr( const wxString& sFileEncoding, wxConfiguration::FILE_ENCODING& eFileEncoding )
{
	if (
		sFileEncoding.CmpNoCase( wxT("local") ) == 0 ||
		sFileEncoding.CmpNoCase( wxT("default") ) == 0
	)
	{
		eFileEncoding = ENCODING_LOCAL;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( wxT("utf8") ) == 0 ||
		sFileEncoding.CmpNoCase( wxT("utf-8") ) == 0
	)
	{
		eFileEncoding = ENCODING_UTF8;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( wxT("utf8_bom") ) == 0 ||
		sFileEncoding.CmpNoCase( wxT("utf-8_bom") ) == 0 ||
		sFileEncoding.CmpNoCase( wxT("utf-8-bom") ) == 0 
	)
	{
		eFileEncoding = ENCODING_UTF8_WITH_BOM;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( wxT("utf16") ) == 0 ||
		sFileEncoding.CmpNoCase( wxT("utf-16") ) == 0
	)
	{
		eFileEncoding = ENCODING_UTF16;
		return true;
	}
	else if (
		sFileEncoding.CmpNoCase( wxT("utf16_bom") ) == 0 ||
		sFileEncoding.CmpNoCase( wxT("utf-16_bom") ) == 0 ||
		sFileEncoding.CmpNoCase( wxT("utf-16-bom") ) == 0 
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

bool wxConfiguration::ReadLanguagesStrings( wxSortedArrayString& as )
{
	const wxStandardPaths& paths = wxStandardPaths::Get();
	wxFileName fn( paths.GetExecutablePath() );
	fn.SetFullName( LANG_FILE_NAME );
	if ( !fn.FileExists() )
	{
		wxLogInfo( _("Cannot find language file \u201C%s\u201D"), fn.GetFullPath() );
		wxLogInfo( _("You can find this file at \u201C%s\u201D"), LANG_FILE_URL );
		return false;
	}

	wxFileInputStream fis( fn.GetFullPath() );
	if ( !fis.IsOk() )
	{
		wxLogDebug( wxT("Cannot open language file \u201C%s\u201D"), fn.GetFullPath() );
		return false;
	}

	as.Clear();
	wxTextInputStream tis( fis );
	size_t n = 0;
	while( !fis.Eof() )
	{
		wxString sLine( tis.ReadLine() );
		if ( sLine.IsEmpty() ) continue;

		wxStringTokenizer tokenizer( sLine, wxT("|") );
		if ( tokenizer.HasMoreTokens() )
		{
			wxString sLang( tokenizer.GetNextToken() );
			if ( sLang.IsEmpty() || (sLang.Length() > 3) )
			{
				wxLogDebug( wxT("Skipping language %s"), sLang );
			}
			else
			{
				as.Add( sLang );
			}
		}

		if ( n++ > 5000 )
		{
			wxLogError( _("Too many languages. File \u201C%s\u201D is corrupt"), fn.GetFullName() );
			as.Clear();
			return false;
		}
	}
	return true;
}

wxConfiguration::wxConfiguration(void)
	:m_bChapterTimeEnd(true),
	 m_bUnknownChapterTimeEndToNextChapter(false),
	 m_nChapterOffset(150),
	 m_bUseDataFiles(false),
	 m_sAlternateExtensions( wxEmptyString ),
	 m_sLang( wxT("unk") ),
	 m_sTrackNameFormat( TRACK_NAME_FORMAT ),
	 m_sMatroskaNameFormat( MATROSKA_NAME_FORMAT ),
	 m_bEmbedded(false),
	 m_bCorrectQuotationMarks(true),
	 m_bSaveCueSheet(false),
	 m_eCueSheetFileEncoding(ENCODING_LOCAL),
	 m_bGenerateTags(false),
	 m_bGenerateMkvmergeOpts(false),
	 m_bGenerateEditionUID(false),
	 m_bGenerateTagsFromComments(true),
	 m_bRunMkvmerge(true),
	 m_bTrackOneIndexOne(true),
	 m_bAbortOnError(true),
	 m_bRoundDownToFullFrames(false),
	 m_sCueSheetExt(CUE_SHEET_EXT),
	 m_sMatroskaChaptersXmlExt(MATROSKA_CHAPTERS_EXT),
	 m_sMatroskaTagsXmlExt(MATROSKA_TAGS_EXT),
	 m_sMatroskaOptsExt(MATROSKA_OPTS_EXT),
	 m_bMerge(false),
	 m_nEmbeddedModeFlags(wxCueSheetReader::EC_MEDIA_READ_TAGS|wxCueSheetReader::EC_FLAC_READ_TAG_FIRST_THEN_COMMENT),
	 m_bUseMLang(true),
	 m_bFullPaths(false),
	 m_bEllipsizeTags(true),
	 m_bAttachEacLog(true)
{
	ReadLanguagesStrings( m_asLang );
}

wxConfiguration::~wxConfiguration(void)
{
}

void wxConfiguration::AddCmdLineParams( wxCmdLineParser& cmdLine )
{
	cmdLine.AddOption( wxT("o"), wxT("output"), _("Output Matroska chapter file or cue sheet file (see -c option)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("od"), wxT("output-directory"), _("Output directory (default: input directory)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("ce"), wxT("chapter-time-end"), _("Calculate end time of chapters if possible (default: on)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nce"), wxT("no-chapter-time-end"), _("Do not calculate end time of chapters"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("uc"), wxT("unknown-chapter-end-to-next-track"), _("If track's end time is unknown set it to next track position using frame offset (default: off)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nuc"), wxT("no-unknown-chapter-end-to-next-track"), _("Do not set end time of track if unknown"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("fo"), wxT("frame-offset"), _("Offset in frames to use with -uc option (default: 150)"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("df"), wxT("use-data-files"), _("Use data file(s) to calculate end time of chapters (default: off)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("ndf"), wxT("dont-use-data-files"), _("Don't use data file(s) to calculate end time of chapters (requires MediaInfo library)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("e"), wxT("alternate-extensions"), _("Comma-separated list of alternate extensions of data files (default: none)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("f"), wxT("single-data-file"), _("Sets single data file to cue sheet (default: none)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("tf"), wxT("track-title-format"), wxString::Format( _("Track title format (default: %s)"), TRACK_NAME_FORMAT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("l"), wxT("language"), _("Set language of chapter's tilte (default: unk)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("ec"), wxT("embedded-cue"), _("Try to read embedded cue sheet (requires MediaInfo library)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nec"), wxT("no-embedded-cue"), _("Try to read embedded cue sheet"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("c"), wxT("save-cue-sheet"), _("Save cue sheet instead of Matroska chapter file. This switch allows to extract embedded cue sheet when used with ec option."), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("m"), wxT("save-matroska-chapters"), _("Save Matroska chapter file (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("t"), wxT("generate-tags"), _("Generate tags file (default: no)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nt"), wxT("dont-generate-tags"), _("Do not generate tags file"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("of"), wxT("generate-mkvmerge-options"), _("Generate file with mkvmerge options (default: no)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("run-mkvmerge"), _("Run mkvmerge tool after generation of options file (default: yes)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("dont-run-mkvmerge"), _("Run mkvmerge tool after generation of options file (default: no)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, wxT("mkvmerge-directory"), _("Location of mkvmerge tool"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nof"), wxT("dont-generate-mkvmerge-options"), _("Don't generate file with mkvmerge options"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("eu"), wxT("generate-edition-uid"), _("Generate edition UID (default: no)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("neu"), wxT("dont-generate-edition-uid"), _("Do not generate edition UID"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("tc"), wxT("generate-tags-from-comments"), _("Try to parse tags from cue sheet comments (default: yes)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("ntc"), wxT("dont-generate-tags-from-comments"), _("Do not try to parse tags from cue sheet comments"), wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( wxEmptyString, wxT("ignore-cdtext-tags"), _("Ignore all CD-TEXT tags"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("use-cdtext-tags"), _("Use CD-TEXT tags (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("ignore-cue-comments-tags"), _("Ignore all tags from cuesheet comments"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("use-cue-comments-tags"), _("Use tags from cuesheet comments (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("ignore-media-tags"), _("Ignore all tagsfrom media file"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("use-media-tags"), _("Use tags from media file (default)"), wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( wxT("oce"), wxT("cue-sheet-encoding"), _("Output cue sheet file encoding - possible values are local (default), utf8, utf8_bom, utf16, utf16_bom"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( wxT("t1i1"), wxT("track-01-index-01"), _("For first track assume index 01 as beginning of track (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("t1i0"), wxT("track-01-index-00"), _("For first track assume index 00 as beginning of track"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("a"), wxT("abort-on-error"), _("Abort when conversion errors occurs (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("na"), wxT("dont-abort-on-error"), _("Do not abort when conversion errors occurs"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("r"), wxT("round-down-to-full-frames"), _("Round down track end time to full frames (default: no)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nr"), wxT("dont-round-down-to-full-frames"), _("Do not round down track end time to full frames"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("hi"), wxT("hidden-indexes"), _("Convert indexes to hidden (sub)chapters"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("merge"), _("Merge cue sheets (default: no)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("dont-merge"), _("Do not merge cue sheets"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nhi"), wxT("no-hidden-indexes"), _("Convert indexes to normal (non-hidden) (sub)chapters (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, wxT("cue-sheet-file-extension"), wxString::Format( _("Cue sheet file extension (default: %s)"), CUE_SHEET_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, wxT("matroska-chapters-file-extension"), wxString::Format( _("Matroska chapters XML file extension (default: %s)"), MATROSKA_CHAPTERS_EXT) , wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, wxT("matroska-tags-file-extension"), wxString::Format( _("Matroska tags XML file extension (default: %s)"), MATROSKA_TAGS_EXT) , wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxEmptyString, wxT("mkvmerge-options-file-extension"), wxString::Format( _("File extension of mkvmerge options file (default: %s)"), MATROSKA_OPTS_EXT) , wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("mf"), wxT("matroska-title-format"), wxString::Format( _("Mtroska container's title format (default: %s)"), MATROSKA_NAME_FORMAT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("cq"), wxT("correct-quotation-marks"), _("Correct \"simple 'quotation' marks\" to \u201Cenglish \u2018quotation\u2019 marks\u201D inside strings (default: on)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("ncq"), wxT("dont-correct-quotation-marks"), _("Dont correct \"simple 'quotation' marks\" to \u201Cenglish \u2018quotation\u2019 marks\u201D inside strings"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("et"), wxT("ellipsize-tags"), wxString::Format( _("Ellipsize tags - convert last three dots to '%c' (default: on)"), wxEllipsizer::ELLIPSIS ), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("net"), wxT("dont-ellipsize-tags"), _("Do not ellipsize tags"), wxCMD_LINE_PARAM_OPTIONAL );

	// embedded mode flags
	cmdLine.AddSwitch( wxEmptyString, wxT("single-media-file"), _("Embedded mode flag. Assume input as single media file without cuesheet (default: no, opposite to media-file-with-embedded-cuesheet)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("media-file-with-embedded-cuesheet"), _("Embedded mode flag. Assume input file as media file with embedded cuesheet (default: yes, opposite to single-media-file)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("flac-read-none"), _("Embedded mode flag - FLAC cuesheet read mode. Do not read cuesheet from FLAC container"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("flac-read-cuesheet-tag-first"), _("Embedded mode flag - FLAC cuesheet read mode. Try to read embedded cuesheet from FLAC container - first try read CUESHEET tag then try CUESHEET comment (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("flac-read-vorbis-comment-first"), _("Embedded mode flag - FLAC cuesheet read mode. Try to read embedded cuesheet from FLAC container - first try read CUESHEET comment then try CUESHEET tag"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("flac-read-cuesheet-tag-only"), _("Embedded mode flag - FLAC cuesheet read mode. Try to read embedded cuesheet from FLAC container - try read CUESHEET tag only"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("flac-read-vorbis-comment-only"), _("Embedded mode flag - FLAC cuesheet read mode. Try to read embedded cuesheet from FLAC container - try read CUESHEET comment only"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("read-media-tags"), _("Embedded mode flag. Read tags from media file (default: yes)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("dont-read-media-tags"), _("Embedded mode flag. Don't read tags from media file"), wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( wxEmptyString, wxT("use-mlang"), _("Use MLang library (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("dont-use-mlang"), _("Don't use MLang library"), wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( wxEmptyString, wxT("full-paths"), _("Use full paths in mkvmerge options file (default: yes)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("no-full-paths"), _("Don't use full paths in mkvmerge options file"), wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( wxEmptyString, wxT("attach-eac-log"), _("Attach EAC log file to mkvmerge options file (default: yes)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("dont-attach-eac-log"), _("Don't attach EAC log file to mkvmerge options file"), wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddParam( _("<cue sheet>"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE|wxCMD_LINE_PARAM_OPTIONAL );
}

static bool check_ext( const wxString& sExt )
{
	return !sExt.IsEmpty() && (sExt.Length() < MAX_EXT_LEN);
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
		return (idx != wxNOT_FOUND);
	}
}

bool wxConfiguration::Read( const wxCmdLineParser& cmdLine )
{
	bool bRes = true;

	if ( cmdLine.Found( wxT("ce") ) ) m_bChapterTimeEnd = true;
	if ( cmdLine.Found( wxT("nce") ) ) m_bChapterTimeEnd = false;

	if ( cmdLine.Found( wxT("uc") ) ) m_bUnknownChapterTimeEndToNextChapter = true;
	if ( cmdLine.Found( wxT("nuc") ) ) m_bUnknownChapterTimeEndToNextChapter = false;

	wxString s;
	long v;
	if ( cmdLine.Found( wxT("fo"), &v ) )
	{
		if ( (v<0) || (v>10000) )
		{
			wxLogWarning( _("Wrong frame offset - %d"), v );
			bRes = false;
		}
		else
		{
			m_nChapterOffset = (unsigned long)v;
		}
	}

	if ( cmdLine.Found( wxT("df") ) ) m_bUseDataFiles = true;
	if ( cmdLine.Found( wxT("ndf") ) ) m_bUseDataFiles = false;

	if ( cmdLine.Found( wxT("ec") ) ) m_bEmbedded = true;
	if ( cmdLine.Found( wxT("nec") ) ) m_bEmbedded = false;

	if ( cmdLine.Found( wxT("cq") ) ) m_bCorrectQuotationMarks = true;
	if ( cmdLine.Found( wxT("ncq") ) ) m_bCorrectQuotationMarks = false;

	if ( cmdLine.Found( wxT("et") ) ) m_bEllipsizeTags = true;
	if ( cmdLine.Found( wxT("net") ) ) m_bEllipsizeTags = false;

	if ( cmdLine.Found( wxT("c") ) ) m_bSaveCueSheet = true;
	if ( cmdLine.Found( wxT("m") ) ) m_bSaveCueSheet = false;

	if ( cmdLine.Found( wxT("t1i1") ) ) m_bTrackOneIndexOne = true;
	if ( cmdLine.Found( wxT("t1i0") ) ) m_bTrackOneIndexOne = false;

	if ( cmdLine.Found( wxT("a") ) ) m_bAbortOnError = true;
	if ( cmdLine.Found( wxT("na") ) ) m_bAbortOnError = false;

	if ( cmdLine.Found( wxT("r") ) ) m_bRoundDownToFullFrames = true;
	if ( cmdLine.Found( wxT("nr") ) ) m_bRoundDownToFullFrames = false;

	if ( cmdLine.Found( wxT("hi") ) ) m_bHiddenIndexes = true;
	if ( cmdLine.Found( wxT("nhi") ) ) m_bHiddenIndexes = false;

	if ( cmdLine.Found( wxT("t") ) ) m_bGenerateTags = true;
	if ( cmdLine.Found( wxT("nt") ) ) m_bGenerateTags = false;

	if ( cmdLine.Found( wxT("of") ) ) m_bGenerateMkvmergeOpts = true;
	if ( cmdLine.Found( wxT("nof") ) ) m_bGenerateMkvmergeOpts = false;

	if ( cmdLine.Found( wxT("run-mkvmerge") ) ) m_bRunMkvmerge = true;
	if ( cmdLine.Found( wxT("dont-run-mkvmerge") ) ) m_bRunMkvmerge = false;

	if ( cmdLine.Found( wxT("full-paths") ) ) m_bFullPaths = true;
	if ( cmdLine.Found( wxT("no-full-paths") ) ) m_bFullPaths = false;

	if ( cmdLine.Found( wxT("oce"), &s ) )
	{
		if ( !GetFileEncodingFromStr( s, m_eCueSheetFileEncoding ) )
		{
			wxLogWarning( _("Wrong cue sheet file encoding %s"), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("eu") ) ) m_bGenerateEditionUID = true;
	if ( cmdLine.Found( wxT("neu") ) ) m_bGenerateEditionUID = false;

	if ( cmdLine.Found( wxT("tc") ) ) m_bGenerateTagsFromComments = true;
	if ( cmdLine.Found( wxT("ntc") ) ) m_bGenerateTagsFromComments = false;

	if ( cmdLine.Found( wxT("merge") ) ) m_bMerge = true;
	if ( cmdLine.Found( wxT("dont-merge") ) ) m_bMerge = false;

	if ( cmdLine.Found( wxT("cue-sheet-file-extension"), &s ) )
	{
		if ( check_ext( s ) )
		{
			m_sCueSheetExt = s;
		}
		else
		{
			wxLogWarning( _("Invalid cue sheet file extension %s"), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("matroska-chapters-file-extension"), &s ) )
	{
		if ( check_ext( s ) )
		{
			m_sMatroskaChaptersXmlExt = s;
		}
		else
		{
			wxLogWarning( _("Invalid Matroska chapters file extension %s"), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("matroska-tags-file-extension"), &s ) )
	{
		if ( check_ext( s ) )
		{
			m_sMatroskaTagsXmlExt = s;
		}
		else
		{
			wxLogWarning( _("Invalid Matroska tags file extension %s"), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("mkvmerge-options-file-extension"), &s ) )
	{
		if ( check_ext( s ) )
		{
			m_sMatroskaOptsExt = s;
		}
		else
		{
			wxLogWarning( _("Invalid options file extension %s"), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("e"), &s ) )
	{
		m_sAlternateExtensions = s;
	}

	if ( cmdLine.Found( wxT("tf"), &s ) )
	{
		m_sTrackNameFormat = s;
	}

	if ( cmdLine.Found( wxT("mf"), &s ) )
	{
		m_sMatroskaNameFormat = s;
	}

	if ( cmdLine.GetParamCount() > 0 )
	{
		for( size_t i=0; i<cmdLine.GetParamCount(); i++ )
		{
			wxInputFile inputFile( cmdLine.GetParam( i ) );
			if ( inputFile.IsOk() )
			{
				m_inputFile.Add( inputFile );
			}
			else
			{
				wxLogWarning( _("Invalid input file \u201C%s\u201D"), cmdLine.GetParam( i ) );
				bRes = false;
			}
		}
	}

	if ( cmdLine.Found( wxT("l"), &s ) )
	{
		if ( CheckLang( s ) )
		{
			m_sLang = s.Lower();
		}
		else
		{
			wxLogWarning( _("Invalid laguage %s"), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("o"), &s ) )
	{
		m_outputFile.Assign( s );
		if ( !m_outputFile.MakeAbsolute() )
		{
			wxLogInfo( _("Fail to normalize path \u201C%s\u201D"), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("od"), &s ) )
	{
		m_outputFile.AssignDir( s );
		if ( !m_outputFile.MakeAbsolute() )
		{
			wxLogInfo( _("Fail to normalize path \u201C%s\u201D"), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("mkvmerge-directory"), &s ) )
	{
		m_mkvmergeDir.AssignDir( s );
		if ( !m_mkvmergeDir.MakeAbsolute() )
		{
			wxLogInfo( _("Fail to normalize path \u201C%s\u201D"), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("ignore-cdtext-tags") ) )
	{
		AddTagSourceToIgnore( wxCueTag::TAG_CD_TEXT );
	}

	if ( cmdLine.Found( wxT("use-cdtext-tags") ) )
	{
		RemoveTagSourceToIgnore( wxCueTag::TAG_CD_TEXT );
	}

	if ( cmdLine.Found( wxT("ignore-cue-comments-tags") ) )
	{
		AddTagSourceToIgnore( wxCueTag::TAG_CUE_COMMENT );
	}

	if ( cmdLine.Found( wxT("use-cue-comments-tags") ) )
	{
		RemoveTagSourceToIgnore( wxCueTag::TAG_CUE_COMMENT );
	}

	if ( cmdLine.Found( wxT("ignore-media-tags") ) )
	{
		AddTagSourceToIgnore( wxCueTag::TAG_MEDIA_METADATA );
	}

	if ( cmdLine.Found( wxT("use-media-tags") ) )
	{
		RemoveTagSourceToIgnore( wxCueTag::TAG_MEDIA_METADATA );
	}

	if ( cmdLine.Found( wxT("single-media-file") ) )
	{
		m_nEmbeddedModeFlags &= ~wxCueSheetReader::EC_SINGLE_MEDIA_FILE;
		m_nEmbeddedModeFlags |= wxCueSheetReader::EC_SINGLE_MEDIA_FILE;
	}

	if ( cmdLine.Found( wxT("media-file-with-embedded-cuesheet") ) )
	{
		m_nEmbeddedModeFlags &= ~wxCueSheetReader::EC_SINGLE_MEDIA_FILE;
		m_nEmbeddedModeFlags |= 0;
	}

	if ( cmdLine.Found( wxT("flac-read-none") ) )
	{
		m_nEmbeddedModeFlags &= ~wxCueSheetReader::EC_FLAC_READ_MASK;
		m_nEmbeddedModeFlags |= wxCueSheetReader::EC_FLAC_READ_NONE;
	}

	if ( cmdLine.Found( wxT("flac-read-cuesheet-tag-first") ) )
	{
		m_nEmbeddedModeFlags &= ~wxCueSheetReader::EC_FLAC_READ_MASK;
		m_nEmbeddedModeFlags |= wxCueSheetReader::EC_FLAC_READ_TAG_FIRST_THEN_COMMENT;
	}

	if ( cmdLine.Found( wxT("flac-read-vorbis-comment-first") ) )
	{
		m_nEmbeddedModeFlags &= ~wxCueSheetReader::EC_FLAC_READ_MASK;
		m_nEmbeddedModeFlags |= wxCueSheetReader::EC_FLAC_READ_COMMENT_FIRST_THEN_TAG;
	}

	if ( cmdLine.Found( wxT("flac-read-cuesheet-tag-only") ) )
	{
		m_nEmbeddedModeFlags &= ~wxCueSheetReader::EC_FLAC_READ_MASK;
		m_nEmbeddedModeFlags |= wxCueSheetReader::EC_FLAC_READ_TAG_ONLY;
	}

	if ( cmdLine.Found( wxT("flac-read-vorbis-comment-only") ) )
	{
		m_nEmbeddedModeFlags &= ~wxCueSheetReader::EC_FLAC_READ_MASK;
		m_nEmbeddedModeFlags |= wxCueSheetReader::EC_FLAC_READ_COMMENT_ONLY;
	}

	if ( cmdLine.Found( wxT("read-media-tags") ) )
	{
		m_nEmbeddedModeFlags &= ~wxCueSheetReader::EC_MEDIA_READ_TAGS;
		m_nEmbeddedModeFlags |= wxCueSheetReader::EC_MEDIA_READ_TAGS;
	}

	if ( cmdLine.Found( wxT("dont-read-media-tags") ) )
	{
		m_nEmbeddedModeFlags &= ~wxCueSheetReader::EC_MEDIA_READ_TAGS;
		m_nEmbeddedModeFlags |= 0;
	}

	// MLang
	if ( cmdLine.Found( wxT("use-mlang") ) )
	{
		m_bUseMLang = true;
	}

	if ( cmdLine.Found( wxT("dont-use-mlang") ) )
	{
		m_bUseMLang = false;
	}

	if ( cmdLine.Found( wxT("attach-eac-log") ) ) m_bAttachEacLog = true;
	if ( cmdLine.Found( wxT("dont-attach-eac-log") ) ) m_bAttachEacLog = false;

	return bRes;
}

static wxString BoolToStr( bool b )
{
	return b? wxT("yes") : wxT("no");
}

static wxString BoolToIdx( bool b )
{
	return b? wxT("01") : wxT("00");
}

static wxString GetEmbeddedModeFlagsDesc( unsigned int flags )
{
	wxArrayString as;
	as.Add( ( ( flags & wxCueSheetReader::EC_SINGLE_MEDIA_FILE ) != 0 )? wxT("single-media-file") : wxT("media-file-with-embedded-cuesheet") );

	wxString s;
	switch( flags & wxCueSheetReader::EC_FLAC_READ_MASK )
	{
		case wxCueSheetReader::EC_FLAC_READ_NONE:
		s = wxT("flac-read-none");
		break;

		case wxCueSheetReader::EC_FLAC_READ_TAG_FIRST_THEN_COMMENT:
		s = wxT("flac-read-cuesheet-tag-first");
		break;

		case wxCueSheetReader::EC_FLAC_READ_COMMENT_FIRST_THEN_TAG:
		s = wxT("flac-read-vorbis-comment-first");
		break;

		case wxCueSheetReader::EC_FLAC_READ_COMMENT_ONLY:
		s = wxT("flac-read-cuesheet-tag-only");
		break;

		case wxCueSheetReader::EC_FLAC_READ_TAG_ONLY:
		s = wxT("flac-read-vorbis-comment-only");
		break;

		default:
		s = wxT("flac_read_???");
		break;
	}
	as.Add( s );

	as.Add( ( ( flags & wxCueSheetReader::EC_MEDIA_READ_TAGS ) != 0 )? wxT("read-media-tags") : wxT("dont-read-media-tags") );

	s.Empty();
	size_t nItems = as.GetCount();
	for( size_t i=0; i<nItems; i++ )
	{
		s += as[i];
		s += wxT(',');
	}

	return s.RemoveLast();
}

static wxString GetTagSourcesNames( const wxArrayTagSource& sources )
{
	wxString s;
	size_t nSources = sources.GetCount();
	wxASSERT( nSources > 0 );
	for( size_t i=0; i<nSources; i++ )
	{
		s += wxCueTag::SourceToString( sources[i] );
		s += wxT(',');
	}
	return s.RemoveLast();
}

void wxConfiguration::FillArray( wxArrayString& as ) const
{
	as.Add( wxString::Format( wxT("Save cue sheet: %s"), BoolToStr(m_bSaveCueSheet) ) );
	as.Add( wxString::Format( wxT("Generate tags file: %s"), BoolToStr(m_bGenerateTags) ) );
	as.Add( wxString::Format( wxT("Generate mkvmerge options file: %s"), BoolToStr(m_bGenerateMkvmergeOpts) ) );
	if ( m_bGenerateMkvmergeOpts )
	{
		as.Add( wxString::Format( wxT("Run mkvmerge: %s"), BoolToStr(m_bRunMkvmerge) ) );
		if ( m_mkvmergeDir.IsOk() )
		{
			as.Add( wxString::Format( wxT("mkvmerge dir: %s"), m_mkvmergeDir.GetFullPath() ) );
		}
		as.Add( wxString::Format( wxT("Generate full paths: %s"), BoolToStr(m_bFullPaths) ) );
		as.Add( wxString::Format( wxT("Attach EAC log: %s"), BoolToStr(m_bAttachEacLog) ) );
	}
	as.Add( wxString::Format( wxT("Generate edition UID: %s"), BoolToStr(m_bGenerateEditionUID) ) );
	as.Add( wxString::Format( wxT("Generate tags from comments: %s"), BoolToStr(m_bGenerateTagsFromComments) ) );
	if ( !m_aeIgnoredSources.IsEmpty() )
	{
		as.Add( wxString::Format( wxT("Ignored tag sources: %s"), GetTagSourcesNames(m_aeIgnoredSources) ) );
	}
	as.Add( wxString::Format( wxT("Output cue sheet file encoding: %s"), GetFileEncodingStr(m_eCueSheetFileEncoding) ) );
	as.Add( wxString::Format( wxT("Calculate end time of chapters: %s"), BoolToStr(m_bChapterTimeEnd) ) );
	as.Add( wxString::Format( wxT("Read embedded cue sheet: %s"), BoolToStr(m_bEmbedded) ) );
	as.Add( wxString::Format( wxT("Use data files to calculate end time of chapters: %s"), BoolToStr(m_bUseDataFiles) ) );

	as.Add( wxString::Format( wxT("Alternate extensions: %s"), m_sAlternateExtensions ) );
	as.Add( wxString::Format( wxT("Set chapter's end time to beginning of next chapter if track's end time cannot be calculated: %s"), BoolToStr(m_bUnknownChapterTimeEndToNextChapter) ) );
	as.Add( wxString::Format( wxT("Chapter offset (frames): %d"), m_nChapterOffset ) );
	as.Add( wxString::Format( wxT("Track name format: %s"), m_sTrackNameFormat ) );
	as.Add( wxString::Format( wxT("Matroska container name format: %s"), m_sMatroskaNameFormat ) );
	as.Add( wxString::Format( wxT("Chapter string language: %s"), m_sLang ) );
	as.Add( wxString::Format( wxT("For track 01 assume index %s as beginning of track"), BoolToIdx(m_bTrackOneIndexOne) ) );
	as.Add( wxString::Format( wxT("Round down track end time to full frames: %s"), BoolToStr(m_bRoundDownToFullFrames) ) );
	as.Add( wxString::Format( wxT("Merge mode: %s"), BoolToStr(m_bMerge) ) );
	as.Add( wxString::Format( wxT("Convert indexes to hidden subchapters: %s"), BoolToStr(m_bHiddenIndexes) ) );
	as.Add( wxString::Format( wxT("Default cue sheet file extension: %s"), m_sCueSheetExt ) );
	as.Add( wxString::Format( wxT("Matroska chapters XML file extension: %s"), m_sMatroskaChaptersXmlExt ) );
	as.Add( wxString::Format( wxT("Matroska tags XML file extension: %s"), m_sMatroskaTagsXmlExt ) );
	as.Add( wxString::Format( wxT("mkvmerge options file extension: %s"), m_sMatroskaOptsExt ) );
	as.Add( wxString::Format( wxT("Correct \"simple 'quotation' marks\" inside strings: %s"), BoolToStr(m_bCorrectQuotationMarks) ) );
	as.Add( wxString::Format( wxT("Ellipsize tags: %s"), BoolToStr(m_bEllipsizeTags) ) );
	if ( m_bEmbedded )
	{
		as.Add( wxString::Format( wxT("Embedded mode flags: %s"), GetEmbeddedModeFlagsDesc(m_nEmbeddedModeFlags) ) );
	}
	as.Add( wxString::Format( wxT("Use MLang library: %s"), BoolToStr(m_bUseMLang) ) );
}

void wxConfiguration::Dump() const
{
	if ( wxLog::IsLevelEnabled( wxLOG_Info, wxLOG_COMPONENT ) && wxLog::GetVerbose() )
	{
		wxString sSeparator( wxT('='), 65 );
		wxArrayString as;
		as.Add( sSeparator );
		as.Add( _("Configuration:") );
		FillArray( as );
		as.Add( wxString::Format( _("Output path: \u201C%s\u201D"), m_outputFile.GetFullPath() ) );
		as.Add( sSeparator );
		size_t strings = as.GetCount();
		wxDateTime dt( wxDateTime::Now() );
		wxLog* pLog = wxLog::GetActiveTarget();
		for( size_t i=0; i<strings; i++ )
		{
			pLog->OnLog( wxLOG_Info, as[i], dt.GetTicks() );
		}
	}
}

void wxConfiguration::BuildXmlComments( const wxString& sOutputFile, wxXmlNode* pNode ) const
{
	wxString sInit;
	sInit.Printf( wxT("This file was created by %s tool"), wxGetApp().GetAppDisplayName() );
	wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString, sInit );
	pNode->AddChild( pComment );

	wxArrayString as;
	wxDateTime dtNow( wxDateTime::Now() );

	as.Add( wxString::Format( wxT("Application version: %s"), wxGetApp().APP_VERSION ) );
	as.Add( wxString::Format( wxT("Application vendor: %s"), wxGetApp().GetVendorDisplayName() ) );
	as.Add( wxString::Format( wxT("Creation time: %s %s"), dtNow.FormatISODate(), dtNow.FormatISOTime() ) );
	as.Add( wxString::Format( wxT("Output file: \u201C%s\u201D"), wxMyApp::GetFileName(sOutputFile) ) );

	FillArray( as );

	size_t strings = as.GetCount();
	for( size_t i=0; i<strings; i++ )
	{
		wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString, as[i] );
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
	if ( !inputFile.IsOk() ) return wxEmptyString;

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetExt( m_bSaveCueSheet? m_sCueSheetExt : m_sMatroskaChaptersXmlExt );
	}
	else
	{
		if ( m_outputFile.IsDir() )
		{
			inputFile.SetPath( m_outputFile.GetPath() );
			inputFile.SetExt( m_bSaveCueSheet? m_sCueSheetExt : m_sMatroskaChaptersXmlExt );
		}
		else
		{
			inputFile = m_outputFile;
		}
	}
	return inputFile.GetFullPath();
}

void wxConfiguration::GetOutputFile( 
	const wxInputFile& _inputFile,
	wxString& sOutputFile, wxString& sTagsFile
) const
{
	sOutputFile.Empty();
	sTagsFile.Empty();

	wxFileName inputFile( _inputFile.GetInputFile() );
	if ( !inputFile.IsOk() ) return;

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetExt( m_bSaveCueSheet? m_sCueSheetExt : m_sMatroskaChaptersXmlExt );
		sOutputFile = inputFile.GetFullPath();

		if ( !m_bSaveCueSheet && m_bGenerateTags )
		{
			inputFile.SetExt( m_sMatroskaTagsXmlExt );
			sTagsFile = inputFile.GetFullPath();
		}
	}
	else
	{
		if ( m_outputFile.IsDir() )
		{
			inputFile.SetPath( m_outputFile.GetPath() );
			inputFile.SetExt( m_bSaveCueSheet? m_sCueSheetExt : m_sMatroskaChaptersXmlExt );
		}
		else
		{
			inputFile = m_outputFile;
		}

		sOutputFile = inputFile.GetFullPath();
		if ( !m_bSaveCueSheet && m_bGenerateTags )
		{
			inputFile.SetExt( m_sMatroskaTagsXmlExt );
			sTagsFile = inputFile.GetFullPath();
		}
	}
}

void wxConfiguration::GetOutputMatroskaFile( 
	const wxInputFile& _inputFile,
	wxString& sMatroskaFile, wxString& sOptionsFile
) const
{
	sMatroskaFile.Empty();
	sOptionsFile.Empty();

	wxFileName inputFile( _inputFile.GetInputFile() );
	if ( !inputFile.IsOk() ) return;
	if ( !(!m_bSaveCueSheet && m_bGenerateMkvmergeOpts) ) return;

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetExt( MATROSKA_AUDIO_EXT );
		sMatroskaFile = inputFile.GetFullPath();

		inputFile.SetExt( m_sMatroskaOptsExt );
		sOptionsFile = inputFile.GetFullPath();
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
		sMatroskaFile = inputFile.GetFullPath();

		inputFile.SetExt( m_sMatroskaOptsExt );
		sOptionsFile = inputFile.GetFullPath();
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

bool wxConfiguration::RoundDownToFullFrames() const
{
	return m_bRoundDownToFullFrames;
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

wxSharedPtr<wxTextOutputStream> wxConfiguration::GetOutputTextStream( wxOutputStream& os )
{
	wxSharedPtr<wxTextOutputStream> pRes;
	switch( m_eCueSheetFileEncoding )
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

unsigned int wxConfiguration::GetEmbeddedModeFlags() const
{
	return m_nEmbeddedModeFlags;
}

void wxConfiguration::AddTagSourceToIgnore( wxCueTag::TAG_SOURCE eSource )
{
	if (m_aeIgnoredSources.Index( eSource ) == wxNOT_FOUND)
	{
		m_aeIgnoredSources.Add( eSource );
	}
}

void wxConfiguration::RemoveTagSourceToIgnore( wxCueTag::TAG_SOURCE eSource )
{
	int nIdx = m_aeIgnoredSources.Index( eSource );
	if ( nIdx != wxNOT_FOUND )
	{
		m_aeIgnoredSources.RemoveAt( nIdx );
	}
}

bool wxConfiguration::ShouldIgnoreTag( const wxCueTag& tag ) const
{
	return (m_aeIgnoredSources.Index( tag.GetSource() ) != wxNOT_FOUND);
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
	return m_bFullPaths;
}

bool wxConfiguration::EllipsizeTags() const
{
	return m_bEllipsizeTags;
}

bool wxConfiguration::AttachEacLog() const
{
	return m_bAttachEacLog;
}

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayInputFile );