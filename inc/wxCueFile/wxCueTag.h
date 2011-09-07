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
		TAG_UNKNOWN		   = 0,
		TAG_CD_TEXT		   = 1,
		TAG_CUE_COMMENT	   = 2,
		TAG_MEDIA_METADATA = 4,
		TAG_AUTO_GENERATED = 8
	} TAG_SOURCE;

	typedef wxUint32 TagSources;

	static struct Name
	{
		static const wxChar CUESHEET[];
		static const wxChar TOTALTRACKS[];
		static const wxChar ARRANGER[];
		static const wxChar COMPOSER[];
		static const wxChar ISRC[];
		static const wxChar TITLE[];
		static const wxChar ALBUM[];
		static const wxChar PERFORMER[];
		static const wxChar ARTIST[];
		static const wxChar ALBUM_ARTIST[];
		static const wxChar CATALOG[];
		static const wxChar CDTEXTFILE[];
		static const wxChar DISC_ID[];
		static const wxChar GENRE[];
		static const wxChar MESSAGE[];
		static const wxChar SONGWRITER[];
		static const wxChar UPC_EAN[];
		static const wxChar SIZE_INFO[];
		static const wxChar TOC_INFO[];
		static const wxChar TOC_INFO2[];
		static const wxChar DISCNUMBER[];
		static const wxChar TOTALDISCS[];
	};

public:

	wxCueTag();
	wxCueTag( const wxCueTag& );
	wxCueTag( TAG_SOURCE, const wxString&, const wxString& );

	TAG_SOURCE GetSource() const;

	bool TestSource( TagSources ) const;
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
	static wxString SourcesToString( TagSources );
	static bool		TestTagSources( TagSources, TagSources );

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

