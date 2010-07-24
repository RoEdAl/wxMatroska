/*
	wxCueComponent.h
*/

#ifndef _WX_CUE_COMPONENT_H
#define _WX_CUE_COMPONENT_H

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

	enum
	{
		DUMP_COMMENTS = 1,
		DUMP_GARBAGE = 2,
		DUMP_EMPTY_LINES = 4
	};

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

	bool m_bTrack;
	wxArrayString m_comments;
	wxArrayString m_garbage;

	wxHashString m_cdTextInfo;

protected:

	void copy(const wxCueComponent&);
	static wxString FormatCdTextData( const wxString&, const wxString& );
	void DumpString( wxTextOutputStream&, const wxChar*, const wxString& ) const;

public:

	const wxArrayString& GetComments() const;
	const wxArrayString& GetGarbage() const;
	virtual bool HasGarbage() const;
	const wxHashString& GetCdTextInfo() const;

	void ParseComment( const wxString& );
	void ParseGarbage( const wxString& );
	bool AddCdTextInfo( const wxString&, const wxString& );

	bool CheckEntryType( ENTRY_TYPE ) const;

	void Clear(void);

	virtual void GetReplacements( wxHashString& ) const;

public:

	wxCueComponent(bool = false);
	wxCueComponent(const wxCueComponent&);
	wxCueComponent& operator=(const wxCueComponent&);
	~wxCueComponent(void);

	static wxString GetCdTextInfoRegExp();
	static wxString GetKeywordsRegExp();
	static bool GetCdTextInfoFormat( const wxString&, ENTRY_FORMAT& );
	static bool GetEntryType( const wxString&, ENTRY_TYPE& );

	virtual void ToStream(wxTextOutputStream&, int = DUMP_COMMENTS ) const;
	wxString ToString(int = DUMP_COMMENTS) const;
};

#endif
