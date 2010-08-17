/*
	wxCueComponent.cpp
*/

#include "StdWx.h"
#include <wxCueComponent.h>
#include <wxUnquoter.h>

IMPLEMENT_CLASS( wxCueTag, wxObject )

wxCueTag::wxCueTag()
{
}

wxCueTag::wxCueTag( const wxString& sName, const wxString& sValue )
	:m_sName(sName.Upper()),m_sValue(sValue)
{
}

wxCueTag::wxCueTag( const wxCueTag& cueTag )
{
	copy( cueTag );
}

const wxString& wxCueTag::GetName() const
{
	return m_sName;
}

const wxString& wxCueTag::GetValue() const
{
	return m_sValue;
}

wxCueTag& wxCueTag::SetName(const wxString& sName )
{
	m_sName = sName.Upper();
	return *this;
}

wxCueTag& wxCueTag::SetValue(const wxString& sValue)
{
	m_sValue = sValue;
	return *this;
}

void wxCueTag::copy(const wxCueTag& cueTag )
{
	m_sName = cueTag.m_sName.Upper();
	m_sValue = cueTag.m_sValue;
}

wxCueTag& wxCueTag::operator =(const wxCueTag& cueTag )
{
	copy( cueTag );
	return *this;
}

// ================================================================================

IMPLEMENT_ABSTRACT_CLASS( wxCueComponent, wxObject )

wxCueComponent::CDTEXT_ENTRY wxCueComponent::CdTextFields[] = {
	{ wxT("ARRANGER"),	wxCueComponent::ANY,	wxCueComponent::CHARACTER,		wxT('a') },
	{ wxT("COMPOSER"),	wxCueComponent::ANY,	wxCueComponent::CHARACTER,		wxT('c') },
	{ wxT("DISC_ID"),	wxCueComponent::ANY,	wxCueComponent::BINARY,			wxT('\000') },
	{ wxT("GENRE"),		wxCueComponent::ANY,	wxCueComponent::BINARY,			wxT('\000') },
	{ wxT("ISRC"),		wxCueComponent::TRACK,	wxCueComponent::BINARY,			wxT('\000') },
	{ wxT("MESSAGE"),	wxCueComponent::ANY,	wxCueComponent::CHARACTER,		wxT('\000') },
	{ wxT("PERFORMER"), wxCueComponent::ANY,	wxCueComponent::CHARACTER,		wxT('p') },
	{ wxT("SONGWRITER"),wxCueComponent::ANY,	wxCueComponent::CHARACTER,		wxT('s') },
	{ wxT("TITLE"),		wxCueComponent::ANY,	wxCueComponent::CHARACTER,		wxT('t') },
	{ wxT("UPC_EAN"),	wxCueComponent::DISC,	wxCueComponent::CHARACTER,		wxT('\000') },	
	{ wxT("SIZE_INFO"), wxCueComponent::ANY,	wxCueComponent::CHARACTER,		wxT('\000') },
	{ wxT("TOC_INFO"),	wxCueComponent::ANY,	wxCueComponent::BINARY,			wxT('\000') },
	{ wxT("TOC_INFO2"), wxCueComponent::ANY,	wxCueComponent::BINARY,			wxT('\000') }
};

size_t wxCueComponent::CdTextFieldsSize = sizeof(wxCueComponent::CdTextFields)/sizeof(wxCueComponent::CDTEXT_ENTRY);

wxCueComponent::KEYWORD_ENTRY wxCueComponent::Keywords[] = {
	{  wxT("REM"), wxCueComponent::ANY },
	{  wxT("INDEX"), wxCueComponent::TRACK },
	{  wxT("PREGAP"), wxCueComponent::TRACK },
	{  wxT("POSTGAP"), wxCueComponent::TRACK },
	{  wxT("FILE"), wxCueComponent::ANY },
	{  wxT("FLAGS"), wxCueComponent::TRACK },
	{  wxT("TRACK"), wxCueComponent::ANY },
	{  wxT("CATALOG"), wxCueComponent::DISC },
	{  wxT("CDTEXTFILE"), wxCueComponent::DISC }
};

size_t wxCueComponent::KeywordsSize = sizeof(wxCueComponent::Keywords)/sizeof(wxCueComponent::KEYWORD_ENTRY);

wxString wxCueComponent::GetCdTextInfoRegExp()
{
	wxString s;
	for( size_t i=0; i<CdTextFieldsSize; i++ )
	{
		s += CdTextFields[i].keyword;
		s += wxT('|');
	}
	s = s.RemoveLast();

	wxString sResult;
	sResult.Printf( wxT("(%s)"), s.GetData() );
	return sResult;
}

wxString wxCueComponent::GetKeywordsRegExp()
{
	wxString s;
	for( size_t i=0; i<KeywordsSize; i++ )
	{
		s += Keywords[i].keyword;
		s += wxT('|');
	}
	s = s.RemoveLast();

	wxString sResult;
	sResult.Printf( wxT("(%s)"), s.GetData() );
	return sResult;
}

bool wxCueComponent::GetCdTextInfoFormat( const wxString& sKeyword, wxCueComponent::ENTRY_FORMAT& fmt )
{
	for( size_t i=0; i<CdTextFieldsSize; i++ )
	{
		if ( sKeyword.CmpNoCase( CdTextFields[i].keyword ) == 0 )
		{
			fmt = CdTextFields[i].format;
			return true;
		}
	}
	return false;
}

bool wxCueComponent::GetCdTextInfoType( const wxString& sKeyword, wxCueComponent::ENTRY_TYPE& et )
{
	for( size_t i=0; i<CdTextFieldsSize; i++ )
	{
		if ( sKeyword.CmpNoCase( CdTextFields[i].keyword ) == 0 )
		{
			et = CdTextFields[i].type;
			return true;
		}
	}
	return false;
}

bool wxCueComponent::GetEntryType( const wxString& sKeyword, wxCueComponent::ENTRY_TYPE& et )
{
	for( size_t i=0; i<KeywordsSize; i++ )
	{
		if ( sKeyword.CmpNoCase( Keywords[i].keyword ) == 0 )
		{
			et = Keywords[i].type;
			return true;
		}
	}
	return false;
}

wxCueComponent::wxCueComponent(bool bTrack)
	:m_bTrack(bTrack)
{
}

wxCueComponent::wxCueComponent(const wxCueComponent& component)
{
	copy( component );
}

wxCueComponent::~wxCueComponent(void)
{
}

wxCueComponent& wxCueComponent::operator =(const wxCueComponent& component)
{
	copy( component );
	return *this;
}

void wxCueComponent::copy(const wxCueComponent& component)
{
	m_bTrack = component.m_bTrack;
	m_comments = component.m_comments;
	m_garbage = component.m_garbage;
	m_cdTextTags = component.m_cdTextTags;
	m_tags = component.m_tags;
}

bool wxCueComponent::HasGarbage() const
{
	return (m_garbage.Count() > 0);
}

const wxArrayString& wxCueComponent::GetComments() const
{
	return m_comments;
}

const wxArrayString& wxCueComponent::GetGarbage() const
{
	return m_garbage;
}

void wxCueComponent::ParseComment( const wxString& sComment, bool bParse )
{
	if ( !bParse )
	{
		m_comments.Add( sComment );
		return;
	}

	wxUnquoter unquoter;
	wxRegEx reCommentMeta( wxT("\\A([[.quotation-mark.]]{0,1})([[:upper:][.hyphen.][.underscore.][:space:][.low-line.]]+)\\1[[:space:]]+([^[:space:]].+)\\Z"), wxRE_ADVANCED );
	wxASSERT( reCommentMeta.IsValid() );

	if ( reCommentMeta.Matches( sComment ) )
	{
		AddCdTextInfoEx(
			reCommentMeta.GetMatch( sComment, 2 ), 
			unquoter.Unquote( reCommentMeta.GetMatch( sComment, 3 ) ) );
	}
	else
	{
		m_comments.Add( sComment );
	}
}

void wxCueComponent::ParseGarbage( const wxString& sLine )
{
	m_garbage.Add( sLine );
}

const wxArrayCueTag& wxCueComponent::GetCdTextTags() const
{
	return m_cdTextTags;
}

const wxArrayCueTag& wxCueComponent::GetTags() const
{
	return m_tags;
}

static bool find_tag( const wxArrayCueTag& tags, const wxCueTag& cueTag )
{
	size_t numTags = tags.Count();
	for( size_t i=0; i<numTags; i++ )
	{
		if ( (cueTag.GetName().CmpNoCase( tags[i].GetName() ) == 0) &&
			 (cueTag.GetValue().Cmp( tags[i].GetValue() ) == 0)
		   )
		{
			return true;
		}
	}
	return false;
}

static void ignore_duplicates( wxArrayCueTag& tags )
{
	wxArrayCueTag tags2;
	size_t numTags = tags.Count();
	for( size_t i=0; i<numTags; i++ )
	{
		if ( !find_tag( tags2, tags[i] ) )
		{
			tags2.Add( tags[i] );
		}
	}

	tags.Clear();
	WX_APPEND_ARRAY( tags, tags2 );
}

void wxCueComponent::GetTags( wxArrayCueTag& tags ) const
{
	tags.Clear();
	WX_APPEND_ARRAY( tags, m_tags );
	ignore_duplicates( tags );
}

bool wxCueComponent::CheckEntryType( wxCueComponent::ENTRY_TYPE ctype ) const
{
	return (ctype==ANY) || (m_bTrack? (ctype==TRACK) : (ctype==DISC));
}

bool wxCueComponent::AddCdTextInfo( const wxString& sKeyword, const wxString& sBody )
{
	for( size_t i=0; i<CdTextFieldsSize; i++ )
	{
		if ( ( sKeyword.CmpNoCase( CdTextFields[i].keyword ) == 0 ) &&
			 CheckEntryType( CdTextFields[i].type )
		)
		{
			wxCueTag newTag( sKeyword, sBody );
			m_cdTextTags.Add( newTag );
			return true;
		}
	}

	return false;
}

void wxCueComponent::AddCdTextInfoEx( const wxString& sKeyword, const wxString& sBody )
{
	bool bAdd = false;
	for( size_t i=0; i<CdTextFieldsSize; i++ )
	{
		if ( ( sKeyword.CmpNoCase( CdTextFields[i].keyword ) == 0 ) &&
			 CheckEntryType( CdTextFields[i].type )
		)
		{
			wxCueTag newTag( CdTextFields[i].keyword, sBody );
			m_cdTextTags.Add( newTag );
			bAdd = true;
			break;
		}
	}

	if ( !bAdd )
	{
		wxCueTag newTag( sKeyword, sBody );
		m_tags.Add( newTag );
	}
}

void wxCueComponent::Clear()
{
	m_comments.Clear();
	m_garbage.Clear();
	m_cdTextTags.Clear();
	m_tags.Clear();
}

wxString wxCueComponent::FormatCdTextData(const wxString& sKeyword, const wxString& sValue )
{
	ENTRY_FORMAT fmt;
	wxCHECK( GetCdTextInfoFormat( sKeyword, fmt ), wxT("") );
	if ( fmt == BINARY )
	{
		return sValue;
	}
	else // characters
	{
		wxString s( sValue );
		s.Replace( wxT("\""), wxT("\\\"") );
		s.Replace( wxT("'"), wxT("\\'") );
		s.Prepend( wxT('\"') );
		s.Append( wxT('\"') );

		return s;
	}
}

static size_t find_keyword( const wxArrayCueTag& tags, const wxString& sKeyword )
{
	size_t numTags = tags.Count();
	for( size_t i=0; i<numTags; i++ )
	{
		if ( sKeyword.CmpNoCase( tags[i].GetName() ) == 0 )
		{
			return i;
		}
	}

	return -1;
}

void wxCueComponent::GetReplacements( wxCueComponent::wxHashString& replacements ) const
{
	for( size_t i=0; i<CdTextFieldsSize; i++ )
	{
		if ( CdTextFields[i].replacement == wxT('\000') ) continue;

		wxString sValue;
		size_t idx = find_keyword( m_cdTextTags, CdTextFields[i].keyword );
		if ( idx != -1 )
		{
			sValue = m_cdTextTags[idx].GetValue();
		}

		wxString s( CdTextFields[i].replacement );
		s.Prepend( m_bTrack? wxT("t") : wxT("d") );

		bool bAdd = false;
		switch( CdTextFields[i].type )
		{
			case ANY:
			{
				bAdd = true;
				wxString s( CdTextFields[i].replacement );
				s.Prepend( wxT("a") );
				replacements[ s ] = sValue;
			}
			break;

			case TRACK:
			if ( m_bTrack )
			{
				bAdd = true;
			}
			break;

			case DISC:
			if ( !m_bTrack )
			{
				bAdd = true;
			}
			break;
		}

		replacements[ s ] = sValue;
	}
}

bool wxCueComponent::IsTrack() const
{
	return m_bTrack;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayCueTag);
