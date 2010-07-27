/*
	wxConfiguration.cpp
*/

#include "StdWx.h"
#include "wxConfiguration.h"

const wxChar wxConfiguration::CUE_SHEET_EXT[] = wxT("cue");
const wxChar wxConfiguration::MATROSKA_CHAPTERS_EXT[] = wxT("mkc.xml");
static const size_t MAX_EXT_LEN = 20;

IMPLEMENT_CLASS( wxConfiguration, wxObject )

wxConfiguration::wxConfiguration(void)
	:m_bChapterTimeEnd(true),
	 m_bUnknownChapterTimeEndToNextChapter(false),
	 m_nChapterOffset(150),
	 m_bUseDataFiles(false),
	 m_sAlternateExtensions( wxEmptyString ),
	 m_sLang( wxT("eng") ),
	 m_sTrackNameFormat( wxT("%dp% - %dt% - %tt%") ),
	 m_bEmbedded( false ),
	 m_bPolishQuotationMarks(true),
	 m_bSaveCueSheet(false),
	 m_bTrackOneIndexOne(true),
	 m_bAbortOnError(true),
	 m_bRoundDownToFullFrames(false),
	 m_sCueSheetExt(CUE_SHEET_EXT),
	 m_sMatroskaChaptersXmlExt(MATROSKA_CHAPTERS_EXT)
{
}

wxConfiguration::~wxConfiguration(void)
{
}

void wxConfiguration::AddCmdLineParams( wxCmdLineParser& cmdLine )
{
	cmdLine.AddOption( wxT("o"), wxT("output"), _("Output Matroska chapter file or cue sheet file (see -c option), it may be also directory path"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("ce"), wxT("chapter-time-end"), _("Calculate end time of chapters if possible (default: on)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nce"), wxT("no-chapter-time-end"), _("Do not calculate end time of chapters"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("uc"), wxT("unknown-chapter-end-to-next-track"), _("If track's end time is unknown set it to next track position using frame offset (default: off)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nuc"), wxT("no-unknown-chapter-end-to-next-track"), _("Do not set end time of track if unknown"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("fo"), wxT("frame-offset"), _("Offset in frames to use with -uc option (default: 150)"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("df"), wxT("use-data-files"), _("Use data file(s) to calculate end time of chapters (default: on)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("ndf"), wxT("dont-use-data-files"), _("Use data file(s) to calculate end time of chapters (requires MediaInfo library)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("e"), wxT("alternate-extensions"), _("Comma-separated list of alternate extensions of data files (default: none)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("f"), wxT("single-data-file"), _("Sets single data file to cue sheet (default: none)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("tf"), wxT("track-title-format"), _("Track title format (default: %dp% - %dt% - %tt%)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("l"), wxT("language"), _("Set language of chapter tilte (default: eng)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("ec"), wxT("embedded-cue"), _("Try to read embedded cue sheet (requires MediaInfo library)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nec"), wxT("no-embedded-cue"), _("Try to read embedded cue sheet"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("c"), wxT("save-cue-sheet"), _("Save cue sheet instead of Matroska chapter file. This switch allows to extract embedded cue sheet when used with ec option."), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("m"), wxT("save-matroska-chapters"), _("Save Matroska chapter file (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("t1i1"), wxT("track-01-index-01"), _("For first track assume index 01 as beginning of track (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("t1i0"), wxT("track-01-index-00"), _("For first track assume index 00 as beginning of track"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("a"), wxT("abort-on-error"), _("Abort when conversion errors occurs (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("na"), wxT("dont-abort-on-error"), _("Do not abort when conversion errors occurs"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("r"), wxT("round-down-to-full-frames"), _("Round down track end time to full frames (default: no)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nr"), wxT("dont-round-down-to-full-frames"), _("Do not round down track end time to full frames"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("hi"), wxT("hidden-indexes"), _("Convert indexes to hidden chapters"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nhi"), wxT("no-hidden-indexes"), _("Convert indexes to normal (non-hidden) chapters (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("dce"), wxT("default-cue-sheet-file-extension"), wxString::Format( _("Default cue sheet file extension (default: %s)"), CUE_SHEET_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("dme"), wxT("default-matroska-chapters-file-extension"), wxString::Format( _("Default matroska chapters XML file extension (default: %s)"), MATROSKA_CHAPTERS_EXT) , wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("pqm"), wxT("polish-quotation-marks"), _("Convert \"simple 'quotation' marks\" to \u201Epolish \u201Aquotation\u2019 marks\u201D inside strings (default: on)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("npqm"), wxT("no-polish-quotation-marks"), _("Don't convert simple quotation marks to polish quotation marks inside strings"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddParam( _("<cue sheet>"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE );
}

static bool check_ext( const wxString& sExt )
{
	return !sExt.IsEmpty() && (sExt.Length() < MAX_EXT_LEN);
}

bool wxConfiguration::Read( const wxCmdLineParser& cmdLine )
{
	bool bRes = true;

	if ( cmdLine.Found( wxT("ce") ) ) m_bChapterTimeEnd = true;
	if ( cmdLine.Found( wxT("nce") ) ) m_bChapterTimeEnd = false;

	if ( cmdLine.Found( wxT("uc") ) ) m_bUnknownChapterTimeEndToNextChapter = true;
	if ( cmdLine.Found( wxT("nuc") ) ) m_bUnknownChapterTimeEndToNextChapter = false;

	wxString s;
	if ( cmdLine.Found( wxT("fo"), &s ) )
	{
		if ( !s.ToULong( &m_nChapterOffset ) )
		{
			wxLogWarning( _("Unknown frame offset - %s"), s.GetData() );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("df") ) ) m_bUseDataFiles = true;
	if ( cmdLine.Found( wxT("ndf") ) ) m_bUseDataFiles = false;

	if ( cmdLine.Found( wxT("ec") ) ) m_bEmbedded = true;
	if ( cmdLine.Found( wxT("nec") ) ) m_bEmbedded = false;

	if ( cmdLine.Found( wxT("pqm") ) ) m_bPolishQuotationMarks = true;
	if ( cmdLine.Found( wxT("npqm") ) ) m_bPolishQuotationMarks = false;

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

	if ( cmdLine.Found( wxT("dce"), &s ) )
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

	if ( cmdLine.Found( wxT("dme"), &s ) )
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

	if ( cmdLine.Found( wxT("e"), &s ) )
	{
		m_sAlternateExtensions = s;
	}

	if ( cmdLine.Found( wxT("tf"), &s ) )
	{
		m_sTrackNameFormat = s;
	}

	if ( cmdLine.GetParamCount() > 0 )
	{
		for( size_t i=0; i<cmdLine.GetParamCount(); i++ )
		{
			m_inputFile.Add( cmdLine.GetParam( i ) );
		}
	}
	else
	{
		wxLogWarning( _("Input file not specified") );
		bRes = false;
	}

	if ( cmdLine.Found( wxT("l"), &s ) )
	{
		if ( ( s.Length() == 3 ) && s.IsAscii() )
		{
			m_sLang = s.Lower();
		}
		else
		{
			wxLogWarning( _("Invalid laguage - %s"), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("o"), &s ) )
	{
		m_outputFile.Assign( s );
	}

	if ( cmdLine.Found( wxT("f"), &s ) )
	{
		m_singleDataFile.Assign( s );
	}

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

static wxString GetFileName( const wxString& sFileName )
{
	wxFileName fn( sFileName );
	return fn.GetFullName();
}

wxXmlNode* wxConfiguration::BuildXmlComments( const wxString& sInputFile, const wxString& sOutputFile, wxXmlNode*& pLast ) const
{
	wxString sInit( wxT("This chapter file was created by cue2mkc tool") );
	wxXmlNode* pComment = new wxXmlNode( (wxXmlNode*)NULL, wxXML_COMMENT_NODE, wxEmptyString, sInit );

	wxArrayString as;
	wxDateTime dtNow( wxDateTime::Now() );

	as.Add( wxString::Format( wxT("Creation time: %s %s"), dtNow.FormatISODate(), dtNow.FormatISOTime() ) );
	as.Add( wxString::Format( wxT("CUE file: %s"), GetFileName(sInputFile) ) );
	as.Add( wxString::Format( wxT("Output file: %s"), GetFileName(sOutputFile) ) );
	as.Add( wxString::Format( wxT("Save cue sheet: %s"), BoolToStr(m_bSaveCueSheet) ) );
	as.Add( wxString::Format( wxT("Calculate end time of chapters: %s"), BoolToStr(m_bChapterTimeEnd) ) );
	as.Add( wxString::Format( wxT("Read embedded cue sheet: %s"), BoolToStr(m_bEmbedded) ) );
	as.Add( wxString::Format( wxT("Use data files to calculate end time of chapters: %s"), BoolToStr(m_bUseDataFiles) ) );

	wxString sSingleDataFile;
	if ( m_singleDataFile.IsOk() )
	{
		sSingleDataFile = m_singleDataFile.GetFullPath();
	}
	as.Add( wxString::Format( wxT("Single data file: %s"), sSingleDataFile ) );
	as.Add( wxString::Format( wxT("Alternate extensions: %s"), m_sAlternateExtensions ) );
	as.Add( wxString::Format( wxT("Set chapter's end time to beginning of next chapter if track's end time cannot be calculated: %s"), BoolToStr(m_bUnknownChapterTimeEndToNextChapter) ) );
	as.Add( wxString::Format( wxT("Chapter offset (frames): %d"), m_nChapterOffset ) );
	as.Add( wxString::Format( wxT("Track name format: %s"), m_sTrackNameFormat ) );
	as.Add( wxString::Format( wxT("Chapter string language: %s"), m_sLang ) );
	as.Add( wxString::Format( wxT("For track 01 assume index %s as beginning of track"), BoolToIdx(m_bTrackOneIndexOne) ) );
	as.Add( wxString::Format( wxT("Round down track end time to full frames: %s"), BoolToStr(m_bRoundDownToFullFrames) ) );
	as.Add( wxString::Format( wxT("Convert indexes to hidden subchapters: %s"), BoolToStr(m_bHiddenIndexes) ) );
	as.Add( wxString::Format( wxT("Default cue sheet file extension: %s"), m_sCueSheetExt ) );
	as.Add( wxString::Format( wxT("Default Matroska chapters XML file extension: %s"), m_sMatroskaChaptersXmlExt ) );
	as.Add( wxString::Format( wxT("Convert \"simple 'quotation' marks\" to \u201Epolish \u201Aquotation\u2019 marks\u201D inside strings: %s"), BoolToStr(m_bPolishQuotationMarks) ) );

	wxXmlNode* pNext = pComment;
	size_t strings = as.GetCount();
	for( size_t i=0; i<strings; i++ )
	{
		wxXmlNode* pComment = new wxXmlNode( (wxXmlNode*)NULL, wxXML_COMMENT_NODE, wxEmptyString, as[i] );
		pNext->SetNext( pComment );
		pNext = pComment;
	}

	pLast = pNext;
	return pComment;
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

const wxFileName& wxConfiguration::GetSingleDataFile() const
{
	return m_singleDataFile;
}

bool wxConfiguration::HasSingleDataFile() const
{
	return m_singleDataFile.IsOk();
}

const wxString& wxConfiguration::GetTrackNameFormat() const
{
	return m_sTrackNameFormat;
}

const wxString& wxConfiguration::GetLang() const
{
	return m_sLang;
}

const wxArrayString& wxConfiguration::GetInputFiles() const
{
	return m_inputFile;
}

wxString wxConfiguration::GetOutputFile( const wxString& sInputFile ) const
{
	wxFileName inputFile( sInputFile );
	if ( !inputFile.IsOk() ) return wxEmptyString;

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetExt( m_bSaveCueSheet? m_sCueSheetExt : m_sMatroskaChaptersXmlExt );
	}
	else
	{
		if ( m_outputFile.DirExists() )
		{
			inputFile.SetPath( m_outputFile.GetFullPath() );
			inputFile.SetExt( m_bSaveCueSheet? m_sCueSheetExt : m_sMatroskaChaptersXmlExt );
		}
		else
		{
			inputFile = m_outputFile;
		}
	}
	return inputFile.GetFullPath();
}

bool wxConfiguration::IsEmbedded() const
{
	return m_bEmbedded;
}

bool wxConfiguration::UsePolishQuotationMarks() const
{
	return m_bPolishQuotationMarks;
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
