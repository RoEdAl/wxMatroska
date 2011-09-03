/*
   wxCueTag.h
 */

#ifndef _WX_CUE_TAG_H_
#define _WX_CUE_TAG_H_

#ifndef _WX_UNQUOTER_H_
class wxUnquoter;
#endif

#ifndef _WX_TRAILING_SPACES_REMOVER_H_
class wxTrailingSpacesRemover;
#endif

#ifndef _WX_REDUNTANT_SPACES_REMOVER_H_
class wxReduntantSpacesRemover;
#endif

#ifndef _WX_ELLIPSIZER_H_
class wxEllipsizer;
#endif

#ifndef _WX_TAG_SYNONIMS_H_
class wxTagSynonimsCollection;
#endif

class wxCueTag;
WX_DECLARE_OBJARRAY( wxCueTag, wxArrayCueTag );
WX_DECLARE_STRING_HASH_MAP( wxArrayCueTag, wxHashArrayCueTag );

class wxCueTag:
	public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxCueTag );

public:

	typedef enum _TAG_SOURCE
	{
		TAG_UNKNOWN,
		TAG_CD_TEXT,
		TAG_CUE_COMMENT,
		TAG_MEDIA_METADATA,
		TAG_AUTO_GENERATED
	} TAG_SOURCE;

	static struct Name
	{
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
		static const wxChar* const CATALOG;
		static const wxChar* const CDTEXTFILE;
		static const wxChar* const DISC_ID;
		static const wxChar* const GENRE;
		static const wxChar* const MESSAGE;
		static const wxChar* const SONGWRITER;
		static const wxChar* const UPC_EAN;
		static const wxChar* const SIZE_INFO;
		static const wxChar* const TOC_INFO;
		static const wxChar* const TOC_INFO2;
		static const wxChar* const DISCNUMBER;
		static const wxChar* const TOTALDISCS;
	};

public:

	wxCueTag();
	wxCueTag( const wxCueTag& );
	wxCueTag( TAG_SOURCE, const wxString&, const wxString& );

	TAG_SOURCE GetSource() const;
	wxString GetSourceAsString() const;

	const wxString& GetName() const;

	const wxString& GetValue() const;
	const wxCueTag& GetValue( wxArrayString& ) const;
	wxString GetQuotedValue( bool = true ) const;
	bool IsMultiline() const;
	wxString GetFlattenValue() const;
	wxString GetFlattenValue( const wxString& ) const;

	wxCueTag& SetSource( TAG_SOURCE );
	wxCueTag& SetName( const wxString& );
	wxCueTag& SetValue( const wxString& );

	bool operator ==( const wxString& ) const;
	bool operator ==( const wxCueTag& ) const;
	wxCueTag& operator =( const wxCueTag& );

	void Unquote( const wxUnquoter& );
	void RemoveTrailingSpaces( const wxTrailingSpacesRemover& );
	int RemoveExtraSpaces( const wxReduntantSpacesRemover& );
	void Ellipsize( const wxEllipsizer& );

protected:

	TAG_SOURCE m_eSource;
	wxString   m_sName;
	wxString   m_sValue;
	bool	   m_bMultiline;

protected:

	typedef struct _SOURCE2TEXT
	{
		TAG_SOURCE eSource;
		const wxChar* pText;
	} SOURCE2TEXT;

	static const SOURCE2TEXT SOURCE2TEXT_MAPPING[];
	static const size_t		 SOURCE2TEXT_MAPPING_SIZE;

public:

	static wxString SourceToString( TAG_SOURCE );

	static size_t GetTags( const wxArrayCueTag&, const wxString&, wxArrayCueTag& );
	static size_t MoveTags( wxArrayCueTag&, const wxString&, wxArrayCueTag& );
	static size_t MoveTags( wxArrayCueTag&, const wxTagSynonimsCollection&, wxArrayCueTag& );
	static bool FindTag( const wxArrayCueTag&, const wxCueTag& );
	static bool AddTag( wxArrayCueTag&, const wxCueTag& );
	static bool AddTagIf( wxArrayCueTag&, const wxCueTag&, const wxCueTag& );
	static bool AddTagIfAndRemove( wxArrayCueTag&, const wxCueTag&, const wxCueTag& );
	static size_t RemoveTag( wxArrayCueTag&, const wxCueTag& );
	static size_t RemoveTag( wxArrayCueTag&, const wxString& );
	static void AddTags( wxArrayCueTag&, const wxArrayCueTag& );
	static void AddTags( wxArrayCueTag&, const wxHashArrayCueTag& );
	static size_t RemoveTags( wxArrayCueTag&, const wxArrayCueTag& );
	static void CommonTags( wxArrayCueTag&, const wxArrayCueTag&, const wxArrayCueTag& );
	static bool FindCommonPart( wxCueTag&, const wxCueTag&, const wxCueTag& );
	static wxString GetFlattenValues( const wxArrayCueTag&, const wxString& );
	static wxString Quote( const wxString& );
	static wxString Escape( const wxString& );
	static wxString UnEscape( const wxString& );

protected:

	void copy( const wxCueTag& );
};

#endif

