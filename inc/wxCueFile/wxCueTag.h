/*
   wxCueTag.h
 */

#ifndef _WX_CUE_TAG_H_
#define _WX_CUE_TAG_H_

#ifndef _WX_TRAILING_SPACES_REMOVER_H_
class wxTrailingSpacesRemover;
#endif

#ifndef _WX_ELLIPSIZER_H_
class wxEllipsizer;
#endif

class wxCueTag;
WX_DECLARE_OBJARRAY( wxCueTag, wxArrayCueTag );

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
	};

public:

	wxCueTag();
	wxCueTag( const wxCueTag& );
	wxCueTag( TAG_SOURCE, const wxString&, const wxString& );

	TAG_SOURCE GetSource() const;
	wxString GetSourceAsString() const;

	const wxString& GetName() const;

	const wxString& GetValue() const;
	wxString GetQuotedValue( bool = true ) const;
	bool IsMultiline() const;
	wxString GetFlattenValue() const;
	void GetValueEx( bool, wxString&, bool& ) const;

	wxCueTag& SetSource( TAG_SOURCE );
	wxCueTag& SetName( const wxString& );
	wxCueTag& SetValue( const wxString& );

	bool operator ==( const wxCueTag& );
	wxCueTag& operator =( const wxCueTag& );

	void RemoveTrailingSpaces( const wxTrailingSpacesRemover& );
	void Ellipsize( const wxEllipsizer& );

protected:

	TAG_SOURCE m_eSource;
	wxString   m_sName;
	wxString   m_sValue;

protected:

	typedef struct _SOURCE2TEXT
	{
		TAG_SOURCE eSource;
		const wxChar* pText;
	} SOURCE2TEXT;

	static SOURCE2TEXT SOURCE2TEXT_MAPPING[];
	static size_t	   SOURCE2TEXT_MAPPING_SIZE;

public:

	static wxString SourceToString( TAG_SOURCE );

	static bool FindTag( const wxArrayCueTag&, const wxCueTag& );
	static bool AddTag( wxArrayCueTag&, const wxCueTag& );
	static void AddTags( wxArrayCueTag&, const wxArrayCueTag& );
	static wxString Quote( const wxString& );
	static wxString Escape( const wxString& );
	static wxString UnEscape( const wxString& );

protected:

	void copy( const wxCueTag& );
};

#endif

