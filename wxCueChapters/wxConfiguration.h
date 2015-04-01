/*
 * wxConfiguration.h
 */

#ifndef _WX_CONFIGURATION_H_
#define _WX_CONFIGURATION_H_

#ifndef _MY_CONFIGURATION_H_
#include <wxConsoleApp/MyConfiguration.h>
#endif

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
	public MyConfiguration
{
	public:

		enum RENDER_MODE
		{
			RENDER_CUE_SHEET,
			RENDER_MATROSKA_CHAPTERS,
			RENDER_WAV2IMG_CUE_POINTS
		};

		enum INFO_SUBJECT
		{
			INFO_NONE,
			INFO_VERSION,
			INFO_USAGE,
			INFO_FORMATTING_DIRECTIVES,
			INFO_LICENSE
		};

		static wxString GetRenderingModes();

		static wxString ToString( RENDER_MODE );
		static bool FromString( const wxString&, RENDER_MODE& );

		enum FILE_ENCODING
		{
			ENCODING_LOCAL,
			ENCODING_UTF8,
			ENCODING_UTF8_WITH_BOM,
			ENCODING_UTF16_LE,
			ENCODING_UTF16_LE_WITH_BOM,
			ENCODING_UTF16_BE,
			ENCODING_UTF16_BE_WITH_BOM,
		};

		static wxString ToString( FILE_ENCODING );
		static bool FromString( const wxString&, FILE_ENCODING& );

		enum CUESHEET_ATTACH_MODE
		{
			CUESHEET_ATTACH_NONE,
			CUESHEET_ATTACH_SOURCE,
			CUESHEET_ATTACH_DECODED,
			CUESHEET_ATTACH_RENDERED
		};

		static wxString ToString( CUESHEET_ATTACH_MODE );
		static bool FromString( const wxString&, CUESHEET_ATTACH_MODE&, bool& );

	protected:

		struct CuesheetAttachModeName
		{
			CUESHEET_ATTACH_MODE value;
			const char* description;
		};

		static const CuesheetAttachModeName AttachModeNames[];

		struct RenderModeName
		{
			RENDER_MODE value;
			const char* description;
		};

		static const RenderModeName RenderModeNames[];

		struct INFO_SUBJECT_DESC
		{
			INFO_SUBJECT value;
			const char* description;
		};

		static const INFO_SUBJECT_DESC InfoSubjectDesc[];

	protected:

		INFO_SUBJECT m_infoSubject;
		bool		 m_bChapterTimeEnd;																																																																																																																																//
		// default=true
		bool		  m_bUnknownChapterTimeEndToNextChapter;		// default=false
		unsigned long m_nChapterOffset;	//
		// in
		// frames
		bool						m_bUseDataFiles;// default=true
		bool						m_bCorrectQuotationMarks;
		RENDER_MODE					m_eRenderMode;
		bool						m_bGenerateTags;
		bool						m_bGenerateMkvmergeOpts;
		bool						m_bRunMkvmerge;
		bool						m_bGenerateEditionUID;
		FILE_ENCODING				m_eFileEncoding;
		bool						m_bTrackOneIndexOne;// or zero
		bool						m_bAbortOnError;
		bool						m_bHiddenIndexes;
		bool						m_bMerge;
		wxCueSheetReader::ReadFlags m_nReadFlags;
		wxCueTag::TagSources		m_nTagSources;
		bool						m_bUseMLang;
		bool						m_bUseFullPaths;
		CUESHEET_ATTACH_MODE		m_eCsAttachMode;

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

        bool m_bRenderArtistForTrack;
        int m_nJpegImageQuality;

	protected:

		static bool ReadLanguagesStrings( wxSortedArrayString& );
		static bool check_ext( const wxString& );
		static wxString BoolToIdx( bool );
		static wxString GetReadFlagsDesc( wxCueSheetReader::ReadFlags );
		static void AddFlag( wxArrayString&, wxCueSheetReader::ReadFlags, wxCueSheetReader::ReadFlags, const wxString& );

		static bool FromString( const wxString&, INFO_SUBJECT& );

		static wxString ToString( INFO_SUBJECT );
		static wxString GetInfoSubjectTexts();

		wxString ReadFlagTestStr( wxCueSheetReader::ReadFlags ) const;
		wxString TagSourcesTestStr( wxCueTag::TagSources ) const;

		bool CheckLang( const wxString& ) const;
		void FillArray( wxArrayString& as ) const;
		bool ReadReadFlags( const wxCmdLineParser&, const wxString&, wxCueSheetReader::ReadFlags );
		bool ReadTagSources( const wxCmdLineParser&, const wxString&, wxCueTag::TagSources );

	public:

		INFO_SUBJECT GetInfoSubject() const;
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
		bool CorrectQuotationMarks() const;
		RENDER_MODE GetRenderMode() const;
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
		FILE_ENCODING GetFileEncoding() const;
		wxString GetXmlFileEncoding() const;
        bool RenderArtistForTrack() const;
        bool ConvertCoversToJpeg() const;
        int GetJpegImageQuality() const;

		wxSharedPtr< wxMBConv > GetXmlEncoding() const;
		wxSharedPtr< wxTextOutputStream > GetOutputTextStream( wxOutputStream& ) const;
		bool GetMerge() const;
		wxCueSheetReader::ReadFlags GetReadFlags() const;
		wxCueTag::TagSources GetTagSources() const;
		bool UseMLang() const;
		bool UseFullPaths() const;
		bool AttachEacLog() const;
		CUESHEET_ATTACH_MODE GetCueSheetAttachMode() const;
		bool AttachCover() const;
		wxString GetExt() const;
		wxString GetOutputFile( const wxInputFile& ) const;
		void GetOutputFile( const wxInputFile&, wxFileName&, wxFileName& ) const;
		void GetOutputMatroskaFile( const wxInputFile&, wxFileName&, wxFileName& ) const;
		bool GetOutputCueSheetFile( const wxInputFile&, const wxString&, wxFileName& ) const;
		bool GetOutputFile( const wxInputFile&, const wxString&, const wxString&, wxFileName& ) const;

	public:

		static const char	CUE_SHEET_EXT[];
		static const char	MATROSKA_CHAPTERS_EXT[];
		static const char	MATROSKA_TAGS_EXT[];
		static const char	MATROSKA_OPTS_EXT[];
		static const char	MATROSKA_AUDIO_EXT[];
		static const char	CUESHEET_EXT[];
		static const char	MATROSKA_NAME_FORMAT[];
		static const char	TRACK_NAME_FORMAT[];
		static const size_t MAX_EXT_LEN;
		static const char	LANG_FILE_URL[];
		static const char	LANG_FILE_NAME[];
		static const char	LANG_UND[];

	public:

		wxConfiguration( void );

		void AddCmdLineParams( wxCmdLineParser& ) const;
		bool Read( const wxCmdLineParser& );

		void Dump() const;
		void BuildXmlComments( const wxFileName&, wxXmlNode* ) const;
};
#endif	// _WX_CONFIGURATION_H

