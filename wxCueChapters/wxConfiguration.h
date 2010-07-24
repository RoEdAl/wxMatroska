/*
	wxConfiguration.h
*/

#ifndef _WX_CONFIGURATION_H
#define _WX_CONFIGURATION_H

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

	wxString m_sAlternateExtensions;
	wxString m_sSingleDataFile;
	wxString m_sLang;
	wxString m_sTrackNameFormat;

	wxString m_sInputFile;
	wxString m_sOutputFile;

public:

	bool GetChapterTimeEnd() const;
	bool GetUnknownChapterTimeEndToNextChapter() const;
	unsigned long GetChapterOffset() const;
	bool GetUseDataFiles() const;
	const wxString& GetAlternateExtensions() const;
	bool HasAlternateExtensions() const;
	const wxString& GetSingleDataFile() const;
	bool HasSingleDataFile() const;
	const wxString& GetLang() const;
	const wxString& GetTrackNameFormat() const;
	const wxString& GetInputFile() const;
	bool IsEmbedded() const;
	const wxString& GetOutputFile() const;
	bool UsePolishQuotationMarks() const;
	bool SaveCueSheet() const;

public:

	wxConfiguration(void);
	~wxConfiguration(void);

	static void  AddCmdLineParams( wxCmdLineParser& );
	bool Read( const wxCmdLineParser& );

	wxXmlNode* BuildXmlComments( wxXmlNode*& ) const;
};

#endif // _WX_CONFIGURATION_H
