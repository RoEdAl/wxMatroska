/*
 * wxCueSheetReader.h
 */

#ifndef _WX_CUE_SHEET_READER_H_
#define _WX_CUE_SHEET_READER_H_

#ifndef _WX_CUE_SHEET_H_
#include "wxCueSheet.h"
#endif

#ifndef _WX_INDEX_H_
class wxIndex;
#endif

#ifndef _WX_DATA_FILE_H_
#include "wxDataFile.h"
#endif

#ifndef _WX_FLAC_META_DATA_READER_H_
class wxFlacMetaDataReader;
#endif

#ifndef _WX_UNQUOTER_H_
#include "wxUnquoter.h"
#endif

#ifndef _WX_TRAILING_SPACES_REMOVER_H_
#include "wxTrailingSpacesRemover.h"
#endif

#ifndef _WX_REDUNTANT_SPACES_REMOVER_H_
#include "wxReduntantSpacesRemover.h"
#endif

#ifndef _WX_ELLIPSIZER_H_
#include "wxEllipsizer.h"
#endif

#ifndef _WX_ROMAN_NUMERALS_H_
#include "wxRomanNumeralsConv.h"
#endif

#ifndef _WX_CUE_SHEET_CONTENT_H_
class wxCueSheetContent;
#endif

#ifdef __WXDEBUG__

class TagLibDebugListener:
	public TagLib::DebugListener
{
	public:

		virtual void printMessage( const TagLib::String& );
};
#endif

class wxCueSheetReader:
	public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxCueSheetReader );

	public:

		static const char LOG_EXT[];
		static const char LOG_MASK[];

		typedef wxUint32 ReadFlags;

		enum
		{
			EC_PARSE_COMMENTS	   = 1,
			EC_ELLIPSIZE_TAGS	   = 2,
			EC_REMOVE_EXTRA_SPACES = 4,
			EC_MEDIA_READ_TAGS	   = 8,
			EC_SINGLE_MEDIA_FILE   = 16,
			EC_FIND_COVER		   = 32,
			EC_FIND_LOG			   = 64,
            EC_CONVERT_UPPER_ROMAN_NUMERALS = 128,
            EC_CONVERT_LOWER_ROMAN_NUMERALS = 256
		};

		bool TestReadFlags( ReadFlags );

	protected:

		// regex
		wxRegEx					 m_reKeywords;
		wxRegEx					 m_reCdTextInfo;
		wxRegEx					 m_reEmpty;
		wxRegEx					 m_reIndex;
		wxRegEx					 m_reMsf;
		wxUnquoter				 m_unquoter;
		wxUnquoter				 m_genericUnquoter;
		wxRegEx					 m_reQuotesEx;
		wxRegEx					 m_reFlags;
		wxRegEx					 m_reDataMode;
		wxRegEx					 m_reDataFile;
		wxRegEx					 m_reCatalog;
		wxRegEx					 m_reIsrc;
		wxRegEx					 m_reTrackComment;
		wxRegEx					 m_reCommentMeta;
		wxTrailingSpacesRemover	 m_trailingSpacesRemover;
		wxReduntantSpacesRemover m_reduntantSpacesRemover;
		wxEllipsizer			 m_ellipsizer;
        wxRomanNumeralsConv      m_romanNumveralsConv;
		wxString				 m_sOneTrackCue;

		// settings
		bool	  m_bErrorsAsWarnings;
		wxString  m_sLang;
		wxString  m_sAlternateExt;
		ReadFlags m_nReadFlags;

		// work
		wxCueSheetContent m_cueSheetContent;
		wxArrayString	  m_errors;
		wxCueSheet		  m_cueSheet;

		// TagLib debug listener
#ifdef __WXDEBUG__
		TagLibDebugListener m_debugListener;
#endif

	protected:

		static wxString GetOneTrackCue();

		static bool GetLogFile( const wxFileName&, bool, wxFileName& );

		void AddError0( const wxString& );
		void AddError( const wxString&, ... );

		void DumpErrors( size_t ) const;

		bool CheckEntryType( wxCueComponent::ENTRY_TYPE ) const;

	protected:

		typedef void ( wxCueSheetReader::* PARSE_METHOD )( const wxString&, const wxString& );
		struct PARSE_STRUCT
		{
			const char* token;
			PARSE_METHOD method;
		};

		static const PARSE_STRUCT parseArray[];

	protected:

		wxString internalReadCueSheet( wxInputStream& stream, wxMBConv& conv );

		bool ParseCue( const wxCueSheetContent& );

		bool ParseCueLine( const wxString &, size_t );

		template< size_t SIZE >
		bool ParseLine( const wxString&, const wxString&, const PARSE_STRUCT(&)[ SIZE ] );

		void ParseLine( size_t, const wxString &, const wxString & );
		void ParseCdTextInfo( size_t, const wxString &, const wxString & );

		void ParseGarbage( const wxString& );
		void ParseComment( const wxString&, const wxString& );
		void ParseComment( wxCueComponent&, const wxString& );
		bool ParseMsf( const wxString&, wxIndex&, bool = false );
		wxString Unquote( const wxString& );

		void ParsePreGap( const wxString&, const wxString& );
		void ParsePostGap( const wxString&, const wxString& );
		void ParseIndex( const wxString&, const wxString& );
		void ParseFile( const wxString&, const wxString& );
		void ParseFlags( const wxString&, const wxString& );
		void ParseTrack( const wxString&, const wxString& );
		void ParseCatalog( const wxString&, const wxString& );
		void ParseCdTextFile( const wxString&, const wxString& );

		bool AddCdTextInfo( const wxString&, const wxString& );
		void AppendTags( const wxArrayCueTag&, bool );

		void AppendTags( const wxArrayCueTag &, size_t, size_t );

	protected:

		bool BuildFromSingleMediaFile( const wxDataFile& );
		bool ReadTagsFromRelatedFiles();

		bool ReadTagsFromMediaFile( const wxDataFile &, size_t, size_t );

		bool FindLog( const wxCueSheetContent& );

		void FindCoversInRelatedFiles();
		bool FindCover( const wxCueSheetContent& );
		void ExtractCoversFromDataFile( const wxDataFile& );

	public:

		wxCueSheetReader( void );

#ifdef __WXDEBUG__
		virtual ~wxCueSheetReader( void );
#endif

		static wxString GetTagLibVersion();

		// settings
		bool ErrorsAsWarnings() const;
		wxCueSheetReader& SetErrorsAsWarnings( bool = true );
		wxCueSheetReader& CorrectQuotationMarks( bool, const wxString& );
		const wxString& GetAlternateExt() const;
		wxCueSheetReader& SetAlternateExt( const wxString& );
		ReadFlags GetReadFlags() const;

		wxCueSheetReader& SetReadFlags( ReadFlags );

		// parsing
		bool ReadCueSheet( const wxString&, bool = true );
		bool ReadCueSheet( const wxString&, wxMBConv& );
		bool ReadCueSheet( wxInputStream& );
		bool ReadCueSheet( wxInputStream&, wxMBConv& );
		bool ReadCueSheetEx( const wxString&, bool );

		// reading
		const wxCueSheet& GetCueSheet() const;
};
#endif

