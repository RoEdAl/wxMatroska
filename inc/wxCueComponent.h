/*
	wxCueComponent.h
*/

#ifndef _WX_CUE_COMPONENT_H
#define _WX_CUE_COMPONENT_H

#ifndef _WX_TAG_SYNONIMS_H_
class wxTagSynonimsCollection;
#endif

class wxCueTag :public wxObject
{
	DECLARE_ABSTRACT_CLASS(wxCueTag)

	public:

	wxCueTag();
	wxCueTag( const wxCueTag& );
	wxCueTag( const wxString&, const wxString& );

	const wxString& GetName() const;
	const wxString& GetValue() const;

	wxCueTag& SetName( const wxString& );
	wxCueTag& SetValue( const wxString& );

	bool operator==( const wxCueTag& );
	wxCueTag& operator=( const wxCueTag& );

	protected:

	wxString m_sName;
	wxString m_sValue;

	protected:

	void copy( const wxCueTag& );
};

WX_DECLARE_OBJARRAY(wxCueTag, wxArrayCueTag );

class wxCueComponent :public wxObject
{
	DECLARE_ABSTRACT_CLASS(wxCueComponent)

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
	void AddTag( const wxString&, const wxString& );
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
