/*
	wxConfiguration.cpp
*/

#include "StdWx.h"
#include "wxConfiguration.h"
#include "wxApp.h"

const wxChar wxConfiguration::CUE_SHEET_EXT[] = wxT("cue");
const wxChar wxConfiguration::MATROSKA_CHAPTERS_EXT[] = wxT("mkc.xml");
const wxChar wxConfiguration::MATROSKA_TAGS_EXT[] = wxT("mkt.xml");

static const size_t MAX_EXT_LEN = 20;
static const wxChar LANG_FILE_URL[] = wxT("http://www.loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt");
static const wxChar LANG_FILE_NAME[] = wxT("ISO-639-2_utf-8.txt");

IMPLEMENT_CLASS( wxConfiguration, wxObject )

bool wxConfiguration::ReadLanguagesStrings( wxSortedArrayString& as )
{
	const wxStandardPaths& paths = wxStandardPaths::Get();
	wxFileName fn( paths.GetExecutablePath() );
	fn.SetFullName( LANG_FILE_NAME );
	if ( !fn.FileExists() )
	{
		wxLogInfo( wxT("Cannot find language file \u201C%s\u201D."), fn.GetFullPath() );
		wxLogInfo( wxT("You can find this file at \u201C%s\u201D."), LANG_FILE_URL );
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
				wxLogDebug( _("Skipping language %s."), sLang );
			}
			else
			{
				as.Add( sLang );
			}
		}

		if ( n++ > 5000 )
		{
			wxLogError( _("Too many languages. File \u201C%s\u201D is corrupt."), fn.GetFullName() );
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
	 m_sLang( wxT("eng") ),
	 m_sTrackNameFormat( wxT("%dp% - %dt% - %tt%") ),
	 m_bEmbedded(false),
	 m_bCorrectQuotationMarks(true),
	 m_bSaveCueSheet(false),
	 m_bCueSheetFileUtf8Encoding(false),
	 m_bGenerateTags(false),
	 m_bGenerateEditionUID(false),
	 m_bGenerateTagsFromComments(true),
	 m_bTrackOneIndexOne(true),
	 m_bAbortOnError(true),
	 m_bRoundDownToFullFrames(false),
	 m_sCueSheetExt(CUE_SHEET_EXT),
	 m_sMatroskaChaptersXmlExt(MATROSKA_CHAPTERS_EXT),
	 m_sMatroskaTagsXmlExt(MATROSKA_TAGS_EXT)
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
	cmdLine.AddOption( wxT("tf"), wxT("track-title-format"), _("Track title format (default: %dp% - %dt% - %tt%)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("l"), wxT("language"), _("Set language of chapter's tilte (default: eng)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("ec"), wxT("embedded-cue"), _("Try to read embedded cue sheet (requires MediaInfo library)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nec"), wxT("no-embedded-cue"), _("Try to read embedded cue sheet"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("c"), wxT("save-cue-sheet"), _("Save cue sheet instead of Matroska chapter file. This switch allows to extract embedded cue sheet when used with ec option."), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("m"), wxT("save-matroska-chapters"), _("Save Matroska chapter file (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("t"), wxT("generate-tags"), _("Generate tags file (default: no)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nt"), wxT("dont-generate-tags"), _("Do not generate tags file"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("eu"), wxT("generate-edition_uid"), _("Generate edition UID (default: no)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("neu"), wxT("dont-generate-edition_uid"), _("Do not generate edition UID"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("tc"), wxT("generate-tags-from-comments"), _("Try to parse tags from cue sheet comments (default: yes)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("ntc"), wxT("dont-generate-tags-from-comments"), _("Do not try to parse tags from cue sheet comments"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("c8"), wxT("cue-sheet-utf8"), _("Save cue sheet using UTF-8 encoding (default: no - default encoding is used)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nc8"), wxT("no-cue-sheet-utf8"), _("Save cue sheet using default encoding"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("t1i1"), wxT("track-01-index-01"), _("For first track assume index 01 as beginning of track (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("t1i0"), wxT("track-01-index-00"), _("For first track assume index 00 as beginning of track"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("a"), wxT("abort-on-error"), _("Abort when conversion errors occurs (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("na"), wxT("dont-abort-on-error"), _("Do not abort when conversion errors occurs"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("r"), wxT("round-down-to-full-frames"), _("Round down track end time to full frames (default: no)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nr"), wxT("dont-round-down-to-full-frames"), _("Do not round down track end time to full frames"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("hi"), wxT("hidden-indexes"), _("Convert indexes to hidden (sub)chapters"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nhi"), wxT("no-hidden-indexes"), _("Convert indexes to normal (non-hidden) (sub)chapters (default)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("dce"), wxT("default-cue-sheet-file-extension"), wxString::Format( _("Default cue sheet file extension (default: %s)"), CUE_SHEET_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("dme"), wxT("default-matroska-chapters-file-extension"), wxString::Format( _("Default Matroska chapters XML file extension (default: %s)"), MATROSKA_CHAPTERS_EXT) , wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("dte"), wxT("default-matroska-tags-file-extension"), wxString::Format( _("Default Matroska tags XML file extension (default: %s)"), MATROSKA_TAGS_EXT) , wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("cq"), wxT("correct-quotation-marks"), _("Correct \"simple 'quotation' marks\" to \u201Cenglish \u2018quotation\u2019 marks\u201D inside strings (default: on)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("ncq"), wxT("dont-correct-quotation-marks"), _("Dont correct \"simple 'quotation' marks\" to \u201Cenglish \u2018quotation\u2019 marks\u201D inside strings"), wxCMD_LINE_PARAM_OPTIONAL );
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

	if ( cmdLine.Found( wxT("c8") ) ) m_bCueSheetFileUtf8Encoding = true;
	if ( cmdLine.Found( wxT("nc8") ) ) m_bCueSheetFileUtf8Encoding = false;

	if ( cmdLine.Found( wxT("eu") ) ) m_bGenerateEditionUID = true;
	if ( cmdLine.Found( wxT("neu") ) ) m_bGenerateEditionUID = false;

	if ( cmdLine.Found( wxT("tc") ) ) m_bGenerateTagsFromComments = true;
	if ( cmdLine.Found( wxT("ntc") ) ) m_bGenerateTagsFromComments = false;

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

	if ( cmdLine.Found( wxT("dte"), &s ) )
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

void wxConfiguration::FillArray( wxArrayString& as ) const
{
	as.Add( wxString::Format( wxT("Save cue sheet: %s"), BoolToStr(m_bSaveCueSheet) ) );
	as.Add( wxString::Format( wxT("Generate tags file: %s"), BoolToStr(m_bGenerateTags) ) );
	as.Add( wxString::Format( wxT("Generate edition UID: %s"), BoolToStr(m_bGenerateEditionUID) ) );
	as.Add( wxString::Format( wxT("Generate tags from comments: %s"), BoolToStr(m_bGenerateTagsFromComments) ) );
	as.Add( wxString::Format( wxT("Cue sheet file encoding: %s"), (m_bCueSheetFileUtf8Encoding? wxT("UTF-8") : wxT("Default") ) ) );
	as.Add( wxString::Format( wxT("Calculate end time of chapters: %s"), BoolToStr(m_bChapterTimeEnd) ) );
	as.Add( wxString::Format( wxT("Read embedded cue sheet: %s"), BoolToStr(m_bEmbedded) ) );
	as.Add( wxString::Format( wxT("Use data files to calculate end time of chapters: %s"), BoolToStr(m_bUseDataFiles) ) );

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
	as.Add( wxString::Format( wxT("Default Matroska tags XML file extension: %s"), m_sMatroskaTagsXmlExt ) );
	as.Add( wxString::Format( wxT("Correct \"simple 'quotation' marks\" inside strings: %s"), BoolToStr(m_bCorrectQuotationMarks) ) );
}

void wxConfiguration::Dump() const
{
	if ( wxLog::IsLevelEnabled( wxLOG_Info, wxLOG_COMPONENT ) && wxLog::GetVerbose() )
	{
		wxString sSeparator( wxT('='), 50 );
		wxArrayString as;
		as.Add( sSeparator );
		as.Add( _("Configuration:") );
		FillArray( as );
		as.Add( wxString::Format( wxT("Output path: \u201C%s\u201D"), m_outputFile.GetFullPath() ) );
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

void wxConfiguration::BuildXmlComments( const wxInputFile& inputFile, const wxString& sOutputFile, wxXmlNode* pNode ) const
{
	wxString sInit;
	sInit.Printf( wxT("This file was created by %s tool"), wxGetApp().GetAppDisplayName() );
	wxXmlNode* pComment = new wxXmlNode( (wxXmlNode*)NULL, wxXML_COMMENT_NODE, wxEmptyString, sInit );
	pNode->AddChild( pComment );

	wxArrayString as;
	wxDateTime dtNow( wxDateTime::Now() );

	as.Add( wxString::Format( wxT("Application version: %s"), wxGetApp().APP_VERSION ) );
	as.Add( wxString::Format( wxT("Application vendor: %s"), wxGetApp().GetVendorDisplayName() ) );
	as.Add( wxString::Format( wxT("Creation time: %s %s"), dtNow.FormatISODate(), dtNow.FormatISOTime() ) );
	as.Add( wxString::Format( wxT("CUE file: \u201C%s\u201D"), inputFile.ToString(false) ) );
	as.Add( wxString::Format( wxT("Output file: \u201C%s\u201D"), GetFileName(sOutputFile) ) );

	FillArray( as );

	size_t strings = as.GetCount();
	for( size_t i=0; i<strings; i++ )
	{
		wxXmlNode* pComment = new wxXmlNode( (wxXmlNode*)NULL, wxXML_COMMENT_NODE, wxEmptyString, as[i] );
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

void wxConfiguration::GetOutputFile( const wxInputFile& _inputFile,
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

bool wxConfiguration::GenerateEditionUID() const
{
	return m_bGenerateEditionUID;
}

bool wxConfiguration::GenerateTagsFromComments() const
{
	return m_bGenerateTagsFromComments;
}

bool wxConfiguration::IsCueSheetFileUtf8Encoding() const
{
	return m_bCueSheetFileUtf8Encoding;
}

const wxMBConv& wxConfiguration::GetCueSheetFileEncoding()
{
	if ( m_bCueSheetFileUtf8Encoding )
	{
		return wxConvUTF8;
	}
	else
	{
		return wxConvLocal;
	}
}

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayInputFile );