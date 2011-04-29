/*
	wxConfiguration.h
*/

#ifndef _WX_CONFIGURATION_H_
#define _WX_CONFIGURATION_H_

#ifndef _WX_INPUT_FILE_H_
#include "wxInputFile.h"
#endif

#ifndef _WX_CUE_COMPONENT_H
#include <wxCueComponent.h>
#endif

WX_DECLARE_OBJARRAY( wxInputFile, wxArrayInputFile );
WX_DEFINE_ARRAY_INT( wxCueTag::TAG_SOURCE, wxArrayTagSource );

class wxConfiguration :public wxObject
{
	wxDECLARE_DYNAMIC_CLASS(wxConfiguration);

public:

	typedef enum
	{
		ENCODING_LOCAL,
		ENCODING_UTF8,
		ENCODING_UTF8_WITH_BOM,
		ENCODING_UTF16,
		ENCODING_UTF16_WITH_BOM
	} FILE_ENCODING;

	static wxString GetFileEncodingStr( FILE_ENCODING );
	static bool GetFileEncodingFromStr( const wxString&, FILE_ENCODING& );

protected:

	bool m_bChapterTimeEnd; // default=true
	bool m_bUnknownChapterTimeEndToNextChapter; // default=false
	unsigned long m_nChapterOffset; // in frames
	bool m_bUseDataFiles; // default=true
	bool m_bEmbedded;
	bool m_bCorrectQuotationMarks;
	bool m_bSaveCueSheet;
	bool m_bGenerateTags;
	bool m_bGenerateEditionUID;
	bool m_bGenerateTagsFromComments;
	FILE_ENCODING m_eCueSheetFileEncoding;
	bool m_bTrackOneIndexOne; // or zero
	bool m_bAbortOnError;
	bool m_bRoundDownToFullFrames;
	bool m_bHiddenIndexes;
	bool m_bMerge;
	unsigned int m_nEmbeddedModeFlags;
	wxArrayTagSource m_aeIgnoredSources;
	bool m_bUseMLang;

	wxString m_sAlternateExtensions;
	wxString m_sLang;
	wxString m_sTrackNameFormat;

	wxArrayInputFile m_inputFile;
	wxFileName m_outputFile;

	wxString m_sCueSheetExt;
	wxString m_sMatroskaChaptersXmlExt;
	wxString m_sMatroskaTagsXmlExt;

	wxSortedArrayString m_asLang;

protected:

	static bool ReadLanguagesStrings( wxSortedArrayString& );
	bool CheckLang( const wxString& ) const;
	void FillArray( wxArrayString& as ) const;
	void AddTagSourceToIgnore( wxCueTag::TAG_SOURCE );
	void RemoveTagSourceToIgnore( wxCueTag::TAG_SOURCE );

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
	bool CorrectQuotationMarks() const;
	bool SaveCueSheet() const;
	bool TrackOneIndexOne() const;
	bool AbortOnError() const;
	bool RoundDownToFullFrames() const;
	bool HiddenIndexes() const;
	const wxString& CueSheetExt() const;
	const wxString& MatroskaChaptersXmlExt() const;
	const wxString& MatroskaTagsXmlExt() const;
	bool GenerateTags() const;
	bool GenerateEditionUID() const;
	bool GenerateTagsFromComments() const;
	FILE_ENCODING GetCueSheetFileEncoding() const;
	wxTextOutputStream* GetOutputTextStream( wxOutputStream& );
	bool GetMerge() const;
	unsigned int GetEmbeddedModeFlags() const;
	bool ShouldIgnoreTag( const wxCueTag& ) const;
	bool UseMLang() const;

	wxString GetOutputFile( const wxInputFile& ) const;
	void GetOutputFile( const wxInputFile&, wxString&, wxString& ) const;

protected:

	static const wxChar CUE_SHEET_EXT[];
	static const wxChar MATROSKA_CHAPTERS_EXT[];
	static const wxChar MATROSKA_TAGS_EXT[];

public:

	wxConfiguration(void);
	~wxConfiguration(void);

	static void  AddCmdLineParams( wxCmdLineParser& );
	bool Read( const wxCmdLineParser& );

	void Dump() const;
	void BuildXmlComments( const wxString&, wxXmlNode* ) const;
};

#endif // _WX_CONFIGURATION_H
