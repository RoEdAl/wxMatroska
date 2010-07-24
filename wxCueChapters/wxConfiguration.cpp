/*
	wxConfiguration.cpp
*/

#include "StdWx.h"
#include "wxConfiguration.h"

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
	 m_bSaveCueSheet(false)
{
}

wxConfiguration::~wxConfiguration(void)
{
}

void wxConfiguration::AddCmdLineParams( wxCmdLineParser& cmdLine )
{
	cmdLine.AddOption( wxT("o"), wxT("output"), _("Output Matroska chapter file or cue sheet file (see -c option)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("ce"), wxT("chapter-time-end"), _("Calculate end time of chapters if possible (default: on)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nce"), wxT("no-chapter-time-end"), _("Do not calculate end time of chapters"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("uc"), wxT("unknown-chapter-end-to-next-track"), _("If track's end time is unknown set it to next track position using frame offset (default: off)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("nuc"), wxT("no-unknown-chapter-end-to-next-track"), _("Do not set end time of track if unknown"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("cf"), wxT("chapter-offset"), _("Chapter offset in frames to use with -uc option (default: 150)"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
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
	cmdLine.AddSwitch( wxT("pqm"), wxT("polish-quotation-marks"), _("Convert quotation marks to polish quotation marks inside strings (default: on)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("npqm"), wxT("no-polish-quotation-marks"), _("Don't convert quotation marks to polish quotation marks inside strings"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddParam( _("<Input CUE file>"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY );
}

bool wxConfiguration::Read( const wxCmdLineParser& cmdLine )
{
	bool bRes = true;

	if ( cmdLine.Found( wxT("ce") ) ) m_bChapterTimeEnd = true;
	if ( cmdLine.Found( wxT("nce") ) ) m_bChapterTimeEnd = false;

	if ( cmdLine.Found( wxT("uc") ) ) m_bUnknownChapterTimeEndToNextChapter = true;
	if ( cmdLine.Found( wxT("nuc") ) ) m_bUnknownChapterTimeEndToNextChapter = false;

	wxString s;
	if ( cmdLine.Found( wxT("cf"), &s ) )
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
		wxFileName fn( cmdLine.GetParam() );
		if ( fn.FileExists() && !fn.IsDir() )
		{
			m_sInputFile = fn.GetFullPath();
			fn.SetExt( wxT("xml") );
			m_sOutputFile = fn.GetFullPath();
		}
		else
		{
			wxLogWarning( _("Invalid input file") );
			bRes = false;
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
			wxLogWarning( _("Invalid laguage - %s"), s.GetData() );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("o"), &s ) )
	{
		m_sOutputFile = s;
	}

	if ( cmdLine.Found( wxT("f"), &s ) )
	{
		m_sSingleDataFile = s;
	}

	return bRes;
}

static wxString BoolToStr( bool b )
{
	return b? wxT("yes") : wxT("no");
}

wxXmlNode* wxConfiguration::BuildXmlComments( wxXmlNode*& pLast ) const
{
	wxString sInit( wxT("This chapter file was created by cue2mkc tool") );
	wxXmlNode* pComment = new wxXmlNode( (wxXmlNode*)NULL, wxXML_COMMENT_NODE, wxEmptyString, sInit );

	wxArrayString as;
	wxDateTime dtNow( wxDateTime::Now() );

	as.Add( wxString::Format( wxT("Creation time: %s %s"), dtNow.FormatISODate(), dtNow.FormatISOTime() ) );
	as.Add( wxString::Format( wxT("CUE file: %s"), m_sInputFile ) );
	as.Add( wxString::Format( wxT("Output file: %s"), m_sOutputFile ) );
	as.Add( wxString::Format( wxT("Save cue sheet: %s"), BoolToStr(m_bSaveCueSheet) ) );
	as.Add( wxString::Format( wxT("Calculate end time of chapters: %s"), BoolToStr(m_bChapterTimeEnd) ) );
	as.Add( wxString::Format( wxT("Read embedded cue sheet: %s"), BoolToStr(m_bEmbedded) ) );
	as.Add( wxString::Format( wxT("Convert quotation marks to polish quotation marks: %s"), BoolToStr(m_bPolishQuotationMarks) ) );
	as.Add( wxString::Format( wxT("Use data files to calculate end time of chapters: %s"), BoolToStr(m_bUseDataFiles) ) );
	as.Add( wxString::Format( wxT("Single data file: %s"), m_sSingleDataFile ) );
	as.Add( wxString::Format( wxT("Alternate extensions: %s"), m_sAlternateExtensions ) );
	as.Add( wxString::Format( wxT("Unknown chapter end time to next chapter: %s"), BoolToStr(m_bUnknownChapterTimeEndToNextChapter) ) );
	as.Add( wxString::Format( wxT("Chapter offset (frames): %d"), m_nChapterOffset ) );
	as.Add( wxString::Format( wxT("Track name format: %s"), m_sTrackNameFormat ) );
	as.Add( wxString::Format( wxT("Chapter string language: %s"), m_sLang ) );

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

const wxString& wxConfiguration::GetSingleDataFile() const
{
	return m_sSingleDataFile;
}

bool wxConfiguration::HasSingleDataFile() const
{
	return !m_sSingleDataFile.IsEmpty();
}

const wxString& wxConfiguration::GetTrackNameFormat() const
{
	return m_sTrackNameFormat;
}

const wxString& wxConfiguration::GetLang() const
{
	return m_sLang;
}

const wxString& wxConfiguration::GetInputFile() const
{
	return m_sInputFile;
}

const wxString& wxConfiguration::GetOutputFile() const
{
	return m_sOutputFile;
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
