/*
 * wxConfiguration.h
 */

#ifndef _WX_CONFIGURATION_H_
#define _WX_CONFIGURATION_H_

#ifndef _WX_INPUT_FILE_H_
#include "wxInputFile.h"
#endif

#ifndef _WX_CUE_COMPONENT_H
#include <wxCueComponent.h>
#endif

#ifndef _WX_CUE_SHEET_READER_H_
#include <wxCueFile/wxCueSheetReader.h>
#endif

WX_DECLARE_OBJARRAY( wxInputFile, wxArrayInputFile );

class wxConfiguration:
	public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxConfiguration );

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

		typedef enum
		{
			CUESHEET_ATTACH_NONE,
			CUESHEET_ATTACH_SOURCE,
			CUESHEET_ATTACH_DECODED,
			CUESHEET_ATTACH_RENDERED
		} CUESHEET_ATTACH_MODE;

		static wxString GetCueSheetAttachModeStr( CUESHEET_ATTACH_MODE );
		static bool GetCueSheetAttachModeFromStr( const wxString&, CUESHEET_ATTACH_MODE&, bool& );

	protected:

		typedef struct _CuesheetAttachModeName
		{
			CUESHEET_ATTACH_MODE eMode;
			const wxChar* pszName;
		} CuesheetAttachModeName;

		static const CuesheetAttachModeName AttachModeNames[];
		static const size_t					AttachModeNamesSize;

	protected:

		bool						m_bChapterTimeEnd;				// default=true
		bool						m_bUnknownChapterTimeEndToNextChapter;	// default=false
		unsigned long				m_nChapterOffset;				// in frames
		bool						m_bUseDataFiles;// default=true
		bool						m_bEmbedded;
		bool						m_bCorrectQuotationMarks;
		bool						m_bSaveCueSheet;
		bool						m_bGenerateTags;
		bool						m_bGenerateMkvmergeOpts;
		bool						m_bRunMkvmerge;
		bool						m_bGenerateEditionUID;
		bool						m_bGenerateTagsFromComments;
		FILE_ENCODING				m_eCueSheetFileEncoding;
		bool						m_bTrackOneIndexOne;// or zero
		bool						m_bAbortOnError;
		bool						m_bHiddenIndexes;
		bool						m_bMerge;
		wxCueSheetReader::ReadFlags m_nReadFlags;
		wxCueTag::TagSources		m_nTagSources;
		bool						m_bUseMLang;
		bool						m_bUseFullPaths;
		bool						m_bEllipsizeTags;
		CUESHEET_ATTACH_MODE		m_eCsAttachMode;
		bool						m_bRemoveExtraSpaces;

		wxString m_sAlternateExtensions;
		wxString m_sLang;
		wxString m_sTrackNameFormat;
		wxString m_sMatroskaNameFormat;

		wxArrayInputFile m_inputFile;
		wxFileName		 m_outputFile;
		wxFileName		 m_mkvmergeDir;

		wxString m_sCueSheetExt;
		wxString m_sMatroskaChaptersXmlExt;
		wxString m_sMatroskaTagsXmlExt;
		wxString m_sMatroskaOptsExt;

		wxSortedArrayString m_asLang;

	protected:

		static bool ReadLanguagesStrings( wxSortedArrayString& );
		static bool check_ext( const wxString& );
		static wxString BoolToStr( bool );
		static wxString BoolToIdx( bool );
		static wxString GetReadFlagsDesc( wxCueSheetReader::ReadFlags );
		static void AddFlag( wxArrayString&, wxCueSheetReader::ReadFlags, wxCueSheetReader::ReadFlags, const wxString& );

		bool CheckLang( const wxString& ) const;
		void FillArray( wxArrayString& as ) const;
		static bool ReadNegatableSwitchValue( const wxCmdLineParser&, const wxString&, bool& );
		static bool ReadNegatableSwitchValueAndNegate( const wxCmdLineParser&, const wxString&, bool& );
		bool ReadReadFlags( const wxCmdLineParser&, const wxString&, wxCueSheetReader::ReadFlags );
		bool ReadTagSources( const wxCmdLineParser&, const wxString&, wxCueTag::TagSources );

	public:

		bool GetChapterTimeEnd() const;
		bool GetUnknownChapterTimeEndToNextChapter() const;
		unsigned long GetChapterOffset() const;
		bool GetUseDataFiles() const;
		const wxString& GetAlternateExtensions() const;
		bool HasAlternateExtensions() const;
		const wxString&			GetLang() const;
		const wxString&			GetTrackNameFormat() const;
		const wxString&			GetMatroskaNameFormat() const;
		const wxArrayInputFile& GetInputFiles() const;
		bool IsEmbedded() const;
		bool CorrectQuotationMarks() const;
		bool SaveCueSheet() const;
		bool TrackOneIndexOne() const;
		bool AbortOnError() const;
		bool HiddenIndexes() const;
		const wxString& CueSheetExt() const;
		const wxString& MatroskaChaptersXmlExt() const;
		const wxString& MatroskaTagsXmlExt() const;
		const wxString& MatroskaOptsExt() const;
		bool GenerateTags() const;
		bool GenerateMkvmergeOpts() const;
		bool RunMkvmerge() const;
		const wxFileName& GetMkvmergeDir() const;
		bool GenerateEditionUID() const;
		bool GenerateTagsFromComments() const;
		FILE_ENCODING GetCueSheetFileEncoding() const;

		wxSharedPtr< wxTextOutputStream > GetOutputTextStream( wxOutputStream& )
		const;
		bool GetMerge() const;
		wxCueSheetReader::ReadFlags GetReadFlags() const;
		wxCueTag::TagSources GetTagSources() const;
		bool UseMLang() const;
		bool UseFullPaths() const;
		bool EllipsizeTags() const;
		bool AttachEacLog() const;
		CUESHEET_ATTACH_MODE GetCueSheetAttachMode() const;
		bool AttachCover() const;
		bool RemoveExtraSpaces() const;
		wxString GetOutputFile( const wxInputFile& ) const;
		void GetOutputFile( const wxInputFile&, wxFileName&, wxFileName& ) const;
		void GetOutputMatroskaFile( const wxInputFile&, wxFileName&, wxFileName& ) const;
		bool GetOutputCueSheetFile( const wxInputFile&, const wxString&, wxFileName& ) const;

	public:

		static const wxChar CUE_SHEET_EXT[];
		static const wxChar MATROSKA_CHAPTERS_EXT[];
		static const wxChar MATROSKA_TAGS_EXT[];
		static const wxChar MATROSKA_OPTS_EXT[];
		static const wxChar MATROSKA_AUDIO_EXT[];
		static const wxChar CUESHEET_EXT[];
		static const wxChar MATROSKA_NAME_FORMAT[];
		static const wxChar TRACK_NAME_FORMAT[];
		static const size_t MAX_EXT_LEN;
		static const wxChar LANG_FILE_URL[];
		static const wxChar LANG_FILE_NAME[];

	public:

		wxConfiguration( void );

		static void AddCmdLineParams( wxCmdLineParser& );
		bool Read( const wxCmdLineParser& );

		void Dump() const;
		void BuildXmlComments( const wxFileName&, wxXmlNode* ) const;
};

#endif	// _WX_CONFIGURATION_H

