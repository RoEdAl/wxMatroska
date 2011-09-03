/*
   wxCueSheetReader.h
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

#ifndef _WX_CUE_SHEET_CONTENT_H_
class wxCueSheetContent;
#endif

class wxCueSheetReader:
	public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxCueSheetReader );

public:

	static const wxChar LOG_EXT[];

	static const wxChar* const CoverNames[];
	static const size_t		   CoverNamesSize;

	static const wxChar* const CoverExts[];
	static const size_t		   CoverExtsSize;

	typedef wxUint32 ReadFlags;

	enum
	{
		EC_FLAC_READ_NONE					= 0,
		EC_FLAC_READ_TAG_FIRST_THEN_COMMENT = 1,
		EC_FLAC_READ_COMMENT_FIRST_THEN_TAG = 2,
		EC_FLAC_READ_COMMENT_ONLY			= 3,
		EC_FLAC_READ_TAG_ONLY				= 4,
		EC_FLAC_READ_MASK					= 7,
		EC_MEDIA_READ_TAGS					= 8,
		EC_MEDIA_MASK						= 15,
		EC_SINGLE_MEDIA_FILE				= 16,
		EC_FIND_COVER						= 32,
		EC_FIND_LOG							= 64
	};

	static bool TestReadFlags( ReadFlags, ReadFlags );

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
	wxString				 m_sOneTrackCue;

	// settings
	bool	 m_bErrorsAsWarnings;
	bool	 m_bParseComments;
	bool	 m_bEllipsizeTags;
	bool	 m_bRemoveExtraSpaces;
	wxString m_sLang;
	wxString m_sAlternateExt;

	// work
	wxCueSheetContent m_cueSheetContent;
	wxArrayString	  m_errors;
	wxDataFile		  m_dataFile;
	wxCueSheet		  m_cueSheet;

protected:

	static wxString GetDataModeRegExp();
	static wxString GetKeywordsRegExp();
	static wxString GetCdTextInfoRegExp();
	static wxString GetDataFileRegExp();
	static wxString GetOneTrackCue();

	static bool GetLogFile( const wxFileName&, wxFileName& );

	static bool IsCoverFile( const wxFileName& );
	static bool GetCoverFile( const wxDir&, const wxString&, wxFileName& );
	static bool GetCoverFile( const wxFileName&, wxFileName& );

	void AddError0( const wxString& );
	void AddError( const wxChar* pszFormat, ... );

	void DumpErrors( size_t ) const;

	bool CheckEntryType( wxCueComponent::ENTRY_TYPE ) const;

protected:

	wxString internalReadCueSheet( wxInputStream& stream, wxMBConv& conv );

	bool ParseCue( const wxCueSheetContent &, ReadFlags = 0 );

	bool ParseCueLine( const wxString &, size_t );
	void ProcessMediaInfoCueSheet( wxString& );

	typedef void ( wxCueSheetReader::* PARSE_METHOD )( const wxString&, const wxString& );
	typedef struct _PARSE_STRUCT
	{
		const wxChar* token;
		PARSE_METHOD method;
	} PARSE_STRUCT;

	static const PARSE_STRUCT parseArray[];
	static const size_t		  parseArraySize;

	bool IsTrack() const;
	wxTrack&	   GetLastTrack();
	const wxTrack& GetLastTrack() const;

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

	// FLAC
	bool ReadEmbeddedInFlacCueSheet( const wxString &, ReadFlags );
	bool ReadCueSheetFromVorbisComment( const wxFlacMetaDataReader&, bool );
	bool ReadCueSheetFromCueSheetTag( const wxFlacMetaDataReader&, bool );
	bool AppendFlacTags( const wxFlacMetaDataReader&, bool );

	bool AppendFlacTags( const wxFlacMetaDataReader &, size_t, size_t );

	// WavPack
	bool ReadEmbeddedInWavpackCueSheet( const wxString &, ReadFlags );
	bool ReadWavpackTags( const wxString&, bool );

	bool ReadWavpackTags( const wxString &, size_t, size_t );

	bool BuildFromSingleMediaFile( const wxString& );
	void ReadTagsFromRelatedFiles();

	bool ReadTagsFromMediaFile( const wxDataFile &, size_t, size_t );

public:

	wxCueSheetReader( void );

	// settings
	bool ErrorsAsWarnings() const;
	wxCueSheetReader& SetErrorsAsWarnings( bool = true );
	bool ParseComments() const;
	wxCueSheetReader& SetParseComments( bool = true );
	bool EllipsizeTags() const;
	wxCueSheetReader& SetEllipsizeTags( bool = true );
	bool RemoveExtraSpaces() const;
	wxCueSheetReader& SetRemoveExtraSpaces( bool = true );
	wxCueSheetReader& CorrectQuotationMarks( bool, const wxString& );
	const wxString&	  GetAlternateExt() const;
	wxCueSheetReader& SetAlternateExt( const wxString& );

	// parsing
	bool ReadCueSheet( const wxString &, ReadFlags = 0, bool = true );
	bool ReadCueSheet( const wxString &, wxMBConv &, ReadFlags = 0 );
	bool ReadCueSheet( wxInputStream &, ReadFlags = 0 );
	bool ReadCueSheet( wxInputStream &, wxMBConv &, ReadFlags = 0 );
	bool ReadEmbeddedCueSheet( const wxString &, ReadFlags = 0 );

	// reading
	const wxCueSheet& GetCueSheet() const;
};

#endif

