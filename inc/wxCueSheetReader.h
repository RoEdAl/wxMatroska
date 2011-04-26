/*
	wxCueSheetReader.h
*/

#ifndef _WX_CUE_SHEET_READER_H_
#define _WX_CUE_SHEET_READER_H_

#include "wxCueSheet.h"

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

class wxCueSheetReader :public wxObject
{
	wxDECLARE_DYNAMIC_CLASS(wxCueSheetReader);

public:

	enum
	{
		EC_FLAC_READ_NONE = 0,
		EC_FLAC_READ_TAG_FIRST_THEN_COMMENT = 1,
		EC_FLAC_READ_COMMENT_FIRST_THEN_TAG = 2,
		EC_FLAC_READ_COMMENT_ONLY = 3,
		EC_FLAC_READ_TAG_ONLY = 4,
		EC_FLAC_READ_MASK = 7,
		EC_MEDIA_READ_TAGS = 8,
		EC_MEDIA_MASK = 15,
		EC_SINGLE_MEDIA_FILE = 16
	};

protected:

	typedef enum _MEDIA_TYPE
	{
		MEDIA_TYPE_UNKNOWN,
		MEDIA_TYPE_FLAC,
		MEDIA_TYPE_WAVPACK
	} MEDIA_TYPE;

	wxArrayString m_errors;

	wxRegEx m_reKeywords;
	wxRegEx m_reCdTextInfo;
	wxRegEx m_reEmpty;

	wxRegEx m_reIndex;
	wxRegEx m_reMsf;
	wxUnquoter m_unquoter;
	wxRegEx m_reQuotesEx;
	wxRegEx m_reFlags;
	wxRegEx m_reDataMode;
	wxRegEx m_reDataFile;
	wxRegEx m_reCatalog;
	wxRegEx m_reIsrc;

	wxDataFile m_dataFile;
	wxArrayString m_cueLines;
	wxCueSheet m_cueSheet;
	bool m_bErrorsAsWarnings;
	bool m_bParseComments;
	wxString m_sLang;
	wxFileName m_cueFileName;

protected:

	static wxString GetDataModeRegExp();
	static wxString GetKeywordsRegExp();
	static wxString GetCdTextInfoRegExp();
	static wxString GetDataFileRegExp();

	void AddError0( const wxString& );
	void AddError( const wxChar *pszFormat, ...);
	void DumpErrors( size_t ) const;

	bool CheckEntryType( wxCueComponent::ENTRY_TYPE ) const;

protected:

	bool internalReadCueSheet( wxInputStream& stream, wxMBConv& conv );
	bool ParseCue(void);
	void ProcessMediaInfoCueSheet( wxString& );

	typedef void (wxCueSheetReader::* PARSE_METHOD)( const wxString&, const wxString& );
	typedef struct _PARSE_STRUCT
	{
		const wxChar* token;
		PARSE_METHOD method;
	} PARSE_STRUCT;

	static PARSE_STRUCT parseArray[];
	static size_t parseArraySize;

	bool IsTrack() const;
	wxTrack& GetLastTrack();
	const wxTrack& GetLastTrack() const;

	void ParseLine( size_t, const wxString&, const wxString& );
	void ParseCdTextInfo( size_t, const wxString&, const wxString& );

	void ParseGarbage( const wxString& );
	void ParseComment( const wxString&, const wxString& );
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
	void AppendComments( const wxArrayCueTag&, bool );

	bool ReadEmbeddedInFlacCueSheet( const wxString&, int );
	bool ReadCueSheetFromVorbisComment( const wxFlacMetaDataReader&, bool );
	bool ReadCueSheetFromCueSheetTag( const wxFlacMetaDataReader&, bool );
	bool AppendFlacComments( const wxFlacMetaDataReader&, bool );

	bool ReadEmbeddedInWavpackCueSheet( const wxString&, int );
	bool ReadEmbeddedInWavpackTags( const wxString&, bool );

	void BuildFromSingleMediaFile( const wxString& );

public:

	wxCueSheetReader(void);
	virtual ~wxCueSheetReader(void);

	bool ReadCueSheet( const wxString& );
	bool ReadCueSheet( const wxString&, wxMBConv& );
	bool ReadCueSheet( wxInputStream& );
	bool ReadCueSheet( wxInputStream&, wxMBConv& );
	bool ReadEmbeddedCueSheet( const wxString&, int = 0 );

	bool ErrorsAsWarnings() const;
	wxCueSheetReader& SetErrorsAsWarnings( bool = true );
	bool ParseComments() const;
	wxCueSheetReader& SetParseComments( bool = true );
	void CorrectQuotationMarks( bool, const wxString& );

	const wxArrayString& GetCueLines() const;
	const wxCueSheet& GetCueSheet() const;
};

#endif
