/*
	wxCueComponent.h
*/

#ifndef _WX_CUE_COMPONENT_H
#define _WX_CUE_COMPONENT_H

#ifndef _WX_TAG_SYNONIMS_H_
class wxTagSynonimsCollection;
#endif

#ifndef _WX_TRAILING_SPACES_REMOVER_H_
class wxTrailingSpacesRemover;
#endif

#ifndef _WX_ELLIPSIZER_H_
class wxEllipsizer;
#endif

class wxCueTag :public wxObject
{
	wxDECLARE_DYNAMIC_CLASS(wxCueTag);

	public:

	typedef enum _TAG_SOURCE
	{
		TAG_UNKNOWN,
		TAG_CD_TEXT,
		TAG_CUE_COMMENT,
		TAG_MEDIA_METADATA,
		TAG_AUTO_GENERATED
	} TAG_SOURCE;

	static class Name
	{
		public:

		static const wxChar* const CUESHEET;
		static const wxChar* const TOTALTRACKS;
		static const wxChar* const ARRANGER;
		static const wxChar* const COMPOSER;
		static const wxChar* const ISRC;
		static const wxChar* const TITLE;
		static const wxChar* const ALBUM;
		static const wxChar* const PERFORMER;
		static const wxChar* const ARTIST;
		static const wxChar* const ALBUM_ARTIST;
	};

	public:

	wxCueTag();
	wxCueTag( const wxCueTag& );
	wxCueTag( TAG_SOURCE, const wxString&, const wxString& );

	TAG_SOURCE GetSource() const;
	wxString GetSourceAsString() const;
	const wxString& GetName() const;
	const wxString& GetValue() const;

	bool IsMultiline() const;
	wxString GetFlattenValue() const;

	wxCueTag& SetSource( TAG_SOURCE );
	wxCueTag& SetName( const wxString& );
	wxCueTag& SetValue( const wxString& );

	bool operator==( const wxCueTag& );
	wxCueTag& operator=( const wxCueTag& );

	void RemoveTrailingSpaces( const wxTrailingSpacesRemover& );
	void Ellipsize( const wxEllipsizer& );

	protected:

	TAG_SOURCE m_eSource;
	wxString m_sName;
	wxString m_sValue;

	protected:

	typedef struct _SOURCE2TEXT
	{
		TAG_SOURCE eSource;
		const wxChar* pText;
	} SOURCE2TEXT;

	static SOURCE2TEXT SOURCE2TEXT_MAPPING[];
	static size_t SOURCE2TEXT_MAPPING_SIZE;

	public:

	static wxString SourceToString( TAG_SOURCE );

	protected:

	void copy( const wxCueTag& );
};

WX_DECLARE_OBJARRAY(wxCueTag, wxArrayCueTag );

class wxCueComponent :public wxObject
{
	wxDECLARE_ABSTRACT_CLASS(wxCueComponent);

public:

	typedef enum _ENTRY_FORMAT
	{
		CHARACTER, BINARY
	} ENTRY_FORMAT;

	typedef enum _ENTRY_TYPE
	{
		ANY, TRACK, DISC
	} ENTRY_TYPE;

	WX_DECLARE_STRING_HASH_MAP( wxString, wxHashString );
	WX_DECLARE_STRING_HASH_MAP( wxArrayCueTag, wxHashCueTag );

protected:

	typedef struct _CDTEXT_ENTRY
	{
		const wxChar* keyword;
		ENTRY_TYPE type;
		ENTRY_FORMAT format;
		wxChar replacement;
	} CDTEXT_ENTRY;

	static CDTEXT_ENTRY CdTextFields[];
	static size_t CdTextFieldsSize;

	typedef struct _KEYWORD_ENTRY
	{
		const wxChar* keyword;
		ENTRY_TYPE type;
	} KEYWORD_ENTRY;

	static KEYWORD_ENTRY Keywords[];
	static size_t KeywordsSize;

protected:

	wxRegEx m_reCommentMeta;

	bool m_bTrack;
	wxArrayString m_comments;
	wxArrayString m_garbage;

	wxArrayCueTag m_cdTextTags;
	wxArrayCueTag m_tags;

protected:

	void copy(const wxCueComponent&);
	static void remove_duplicates( const wxRegEx&, wxHashCueTag& );
	static void remove_duplicates( const wxRegEx&, wxArrayCueTag& );

public:

	const wxArrayString& GetComments() const;
	const wxArrayString& GetGarbage() const;
	virtual bool HasGarbage() const;
	const wxArrayCueTag& GetCdTextTags() const;
	const wxArrayCueTag& GetTags() const;
	void GetTags( 
		const wxTagSynonimsCollection&,
		const wxTagSynonimsCollection&,
		wxArrayCueTag&,
		wxArrayCueTag& ) const;

	void ParseComment( const wxString&, bool = true );
	void ParseGarbage( const wxString& );
	bool AddCdTextInfo( const wxString&, const wxString& );
	void AddTag( wxCueTag::TAG_SOURCE, const wxString&, const wxString& );
	void AddTag( const wxCueTag& );

	bool CheckEntryType( ENTRY_TYPE ) const;

	void Clear(void);
	bool IsTrack() const;

	virtual void GetReplacements( wxHashString& ) const;

public:

	wxCueComponent(bool = false);
	wxCueComponent(const wxCueComponent&);
	wxCueComponent& operator=(const wxCueComponent&);
	~wxCueComponent(void);

	static wxString GetCdTextInfoRegExp();
	static wxString GetKeywordsRegExp();
	static bool GetCdTextInfoFormat( const wxString&, ENTRY_FORMAT& );
	static bool GetCdTextInfoType( const wxString&, ENTRY_TYPE& );
	static bool GetEntryType( const wxString&, ENTRY_TYPE& );

	static wxString FormatCdTextData( const wxString&, const wxString& );

};

#endif
