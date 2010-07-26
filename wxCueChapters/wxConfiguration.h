/*
	wxConfiguration.h
*/

#ifndef _WX_CONFIGURATION_H_
#define _WX_CONFIGURATION_H_

class wxConfiguration :public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxConfiguration)

protected:

	bool m_bChapterTimeEnd; // default=true
	bool m_bUnknownChapterTimeEndToNextChapter; // default=false
	unsigned long m_nChapterOffset; // in frames
	bool m_bUseDataFiles; // default=true
	bool m_bEmbedded;
	bool m_bPolishQuotationMarks;
	bool m_bSaveCueSheet;
	bool m_bTrackOneIndexOne; // or zero
	bool m_bAbortOnError;
	bool m_bRoundDownToFullFrames;
	bool m_bHiddenIndexes;

	wxString m_sAlternateExtensions;
	wxFileName m_singleDataFile;
	wxString m_sLang;
	wxString m_sTrackNameFormat;

	wxArrayString m_inputFile;
	wxFileName m_outputFile;

public:

	bool GetChapterTimeEnd() const;
	bool GetUnknownChapterTimeEndToNextChapter() const;
	unsigned long GetChapterOffset() const;
	bool GetUseDataFiles() const;
	const wxString& GetAlternateExtensions() const;
	bool HasAlternateExtensions() const;
	const wxFileName& GetSingleDataFile() const;
	bool HasSingleDataFile() const;
	const wxString& GetLang() const;
	const wxString& GetTrackNameFormat() const;
	const wxArrayString& GetInputFiles() const;
	bool IsEmbedded() const;
	wxString GetOutputFile( const wxString& ) const;
	bool UsePolishQuotationMarks() const;
	bool SaveCueSheet() const;
	bool TrackOneIndexOne() const;
	bool AbortOnError() const;
	bool RoundDownToFullFrames() const;
	bool HiddenIndexes() const;

public:

	static const wxChar CUE_SHEET_EXT[];
	static const wxChar MATROSKA_CHAPTERS_EXT[];

public:

	wxConfiguration(void);
	~wxConfiguration(void);

	static void  AddCmdLineParams( wxCmdLineParser& );
	bool Read( const wxCmdLineParser& );

	wxXmlNode* BuildXmlComments( const wxString&, const wxString&, wxXmlNode*& ) const;
};

#endif // _WX_CONFIGURATION_H
