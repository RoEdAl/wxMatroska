/*
	wxConfiguration.h
*/

#ifndef _WX_CONFIGURATION_H_
#define _WX_CONFIGURATION_H_

#ifndef _WX_INPUT_FILE_H_
#include "wxInputFile.h"
#endif

WX_DECLARE_OBJARRAY( wxInputFile, wxArrayInputFile );

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
	wxString m_sLang;
	wxString m_sTrackNameFormat;

	wxArrayInputFile m_inputFile;
	wxFileName m_outputFile;

	wxString m_sCueSheetExt;
	wxString m_sMatroskaChaptersXmlExt;

	wxSortedArrayString m_asLang;

protected:

	static bool ReadLanguagesStrings( wxSortedArrayString& );
	bool CheckLang( const wxString& ) const;
	void FillArray( wxArrayString& as ) const;

public:

	bool GetChapterTimeEnd() const;
	bool GetUnknownChapterTimeEndToNextChapter() const;
	unsigned long GetChapterOffset() const;
	bool GetUseDataFiles() const;
	const wxString& GetAlternateExtensions() const;
	bool HasAlternateExtensions() const;
	const wxString& GetLang() const;
	const wxString& GetTrackNameFormat() const;
	const wxArrayInputFile& GetInputFiles() const;
	bool IsEmbedded() const;
	wxString GetOutputFile( const wxInputFile& ) const;
	bool UsePolishQuotationMarks() const;
	bool SaveCueSheet() const;
	bool TrackOneIndexOne() const;
	bool AbortOnError() const;
	bool RoundDownToFullFrames() const;
	bool HiddenIndexes() const;
	const wxString& CueSheetExt() const;
	const wxString& MatroskaChaptersXmlExt() const;

protected:

	static const wxChar CUE_SHEET_EXT[];
	static const wxChar MATROSKA_CHAPTERS_EXT[];

public:

	wxConfiguration(void);
	~wxConfiguration(void);

	static void  AddCmdLineParams( wxCmdLineParser& );
	bool Read( const wxCmdLineParser& );

	void Dump() const;
	wxXmlNode* BuildXmlComments( const wxInputFile&, const wxString&, wxXmlNode*& ) const;
};

#endif // _WX_CONFIGURATION_H
