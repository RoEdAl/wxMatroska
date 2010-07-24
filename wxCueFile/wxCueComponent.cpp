/*
	wxCueComponent.cpp
*/

#include "StdWx.h"
#include "wxCueComponent.h"

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

bool wxCueComponent::GetCdTextInfoFormat( const wxString& sKeyword, ENTRY_FORMAT& fmt )
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

bool wxCueComponent::GetEntryType( const wxString& sKeyword, ENTRY_TYPE& et )
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
	m_cdTextInfo = component.m_cdTextInfo;
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

void wxCueComponent::ParseComment( const wxString& sComment )
{
	m_comments.Add( sComment );
}

void wxCueComponent::ParseGarbage( const wxString& sLine )
{
	m_garbage.Add( sLine );
}

const wxCueComponent::wxHashString& wxCueComponent::GetCdTextInfo() const
{
	return m_cdTextInfo;
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
			m_cdTextInfo[ CdTextFields[i].keyword ] = sBody;
			return true;
		}
	}

	return false;
}

void wxCueComponent::Clear()
{
	m_comments.Clear();
	m_garbage.Clear();
	m_cdTextInfo.clear();
}

void wxCueComponent::DumpString( wxTextOutputStream& stream, const wxChar* szEntry, const wxString& text ) const
{
	if ( !text.IsEmpty() )
	{
		wxString sLine;
		sLine.Printf( wxT("%s %s\n"), szEntry, text.GetData() );
		if ( m_bTrack )
		{
			sLine = sLine.Prepend( wxT("\t") );
		}
		stream.WriteString( sLine );
	}
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

void wxCueComponent::ToStream(wxTextOutputStream& stream, int nDumpFlags ) const
{
	// dump comments
	if ( (nDumpFlags & DUMP_COMMENTS) != 0 )
	{
		for( wxArrayString::const_iterator i = m_comments.begin(); i != m_comments.end(); i++ )
		{
			DumpString( stream, wxT("REM"), *i );
		}
	}

	// dump CT-TEXT info
	for( wxHashString::const_iterator i=m_cdTextInfo.begin(); i != m_cdTextInfo.end(); i++ )
	{
		DumpString( stream, i->first, FormatCdTextData( i->first, i->second ) );
	}

	// dump garbage
	if ( (nDumpFlags & DUMP_GARBAGE) != 0 )
	{
		for( wxArrayString::const_iterator i = m_garbage.begin(); i != m_garbage.end(); i++ )
		{
			stream.WriteString( *i );
		}
	}
}

wxString wxCueComponent::ToString(int nDumpFlags) const
{
	wxStringOutputStream sout;
	wxTextOutputStream ts( sout );
	ToStream( ts, nDumpFlags );
	return sout.GetString();
}

void wxCueComponent::GetReplacements( wxCueComponent::wxHashString& replacements ) const
{
	for( size_t i=0; i<CdTextFieldsSize; i++ )
	{
		if ( CdTextFields[i].replacement == wxT('\000') ) continue;

		wxString sValue;
		wxHashString::const_iterator it = m_cdTextInfo.find( CdTextFields[i].keyword );
		if ( it != m_cdTextInfo.end() )
		{
			sValue = it->second;
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