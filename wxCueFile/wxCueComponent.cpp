/*
   wxCueComponent.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxTagSynonims.h>
#include <wxCueFile/wxCueTag.h>
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxUnquoter.h>
#include <wxCueFile/wxTextOutputStreamOnString.h>

// ===============================================================================

wxIMPLEMENT_ABSTRACT_CLASS( wxCueComponent, wxObject )

// ===============================================================================

wxCueComponent::CDTEXT_ENTRY wxCueComponent::CdTextFields[] =
{
	{ wxCueTag::Name::ARRANGER, wxCueComponent::ANY, wxCueComponent::CHARACTER, wxT( 'a' ) },
	{ wxCueTag::Name::COMPOSER, wxCueComponent::ANY, wxCueComponent::CHARACTER, wxT( 'c' ) },
	{ wxCueTag::Name::DISC_ID, wxCueComponent::ANY, wxCueComponent::BINARY, wxT( '\000' ) },
	{ wxCueTag::Name::GENRE, wxCueComponent::ANY, wxCueComponent::BINARY, wxT( '\000' ) },
	{ wxCueTag::Name::ISRC, wxCueComponent::TRACK, wxCueComponent::BINARY, wxT( '\000' ) },
	{ wxCueTag::Name::MESSAGE, wxCueComponent::ANY, wxCueComponent::CHARACTER, wxT( '\000' ) },
	{ wxCueTag::Name::PERFORMER, wxCueComponent::ANY, wxCueComponent::CHARACTER, wxT( 'p' ) },
	{ wxCueTag::Name::SONGWRITER, wxCueComponent::ANY, wxCueComponent::CHARACTER, wxT( 's' ) },
	{ wxCueTag::Name::TITLE, wxCueComponent::ANY, wxCueComponent::CHARACTER, wxT( 't' ) },
	{ wxCueTag::Name::UPC_EAN, wxCueComponent::DISC, wxCueComponent::CHARACTER, wxT( '\000' ) },
	{ wxCueTag::Name::SIZE_INFO, wxCueComponent::ANY, wxCueComponent::CHARACTER, wxT( '\000' ) },
	{ wxCueTag::Name::TOC_INFO, wxCueComponent::ANY, wxCueComponent::BINARY, wxT( '\000' ) },
	{ wxCueTag::Name::TOC_INFO2, wxCueComponent::ANY, wxCueComponent::BINARY, wxT( '\000' ) }
};

size_t wxCueComponent::CdTextFieldsSize = WXSIZEOF( wxCueComponent::CdTextFields );

wxCueComponent::KEYWORD_ENTRY wxCueComponent::Keywords[] =
{
	{ wxT( "REM" ), wxCueComponent::ANY },
	{ wxT( "INDEX" ), wxCueComponent::TRACK },
	{ wxT( "PREGAP" ), wxCueComponent::TRACK },
	{ wxT( "POSTGAP" ), wxCueComponent::TRACK },
	{ wxT( "FILE" ), wxCueComponent::ANY },
	{ wxT( "FLAGS" ), wxCueComponent::TRACK },
	{ wxT( "TRACK" ), wxCueComponent::ANY },
	{ wxT( "CATALOG" ), wxCueComponent::DISC },
	{ wxT( "CDTEXTFILE" ), wxCueComponent::DISC }
};

size_t wxCueComponent::KeywordsSize = WXSIZEOF( wxCueComponent::Keywords );

// ===============================================================================

wxString wxCueComponent::GetCdTextInfoRegExp()
{
	wxTextOutputStreamOnString tos;

	for ( size_t i = 0; i < CdTextFieldsSize; i++ )
	{
		*tos << CdTextFields[ i ].keyword << wxT( '|' );
	}

	( *tos ).Flush();
	const wxString& s = tos.GetString();
	wxASSERT( !s.IsEmpty() );
	return wxString::Format( wxT( "(%s)" ), s.Left( s.Length() - 1 ) );
}

wxString wxCueComponent::GetKeywordsRegExp()
{
	wxTextOutputStreamOnString tos;

	for ( size_t i = 0; i < KeywordsSize; i++ )
	{
		*tos << Keywords[ i ].keyword << wxT( '|' );
	}

	( *tos ).Flush();
	const wxString& s = tos.GetString();
	wxASSERT( !s.IsEmpty() );
	return wxString::Format( wxT( "(%s)" ), s.Left( s.Length() - 1 ) );
}

bool wxCueComponent::GetCdTextInfoFormat( const wxString& sKeyword, wxCueComponent::ENTRY_FORMAT& fmt )
{
	for ( size_t i = 0; i < CdTextFieldsSize; i++ )
	{
		if ( sKeyword.CmpNoCase( CdTextFields[ i ].keyword ) == 0 )
		{
			fmt = CdTextFields[ i ].format;
			return true;
		}
	}

	return false;
}

bool wxCueComponent::GetCdTextInfoType( const wxString& sKeyword, wxCueComponent::ENTRY_TYPE& et )
{
	for ( size_t i = 0; i < CdTextFieldsSize; i++ )
	{
		if ( sKeyword.CmpNoCase( CdTextFields[ i ].keyword ) == 0 )
		{
			et = CdTextFields[ i ].type;
			return true;
		}
	}

	return false;
}

bool wxCueComponent::GetEntryType( const wxString& sKeyword, wxCueComponent::ENTRY_TYPE& et )
{
	for ( size_t i = 0; i < KeywordsSize; i++ )
	{
		if ( sKeyword.CmpNoCase( Keywords[ i ].keyword ) == 0 )
		{
			et = Keywords[ i ].type;
			return true;
		}
	}

	return false;
}

wxCueComponent::wxCueComponent( bool bTrack ):
	m_bTrack( bTrack )
{}

wxCueComponent::wxCueComponent( const wxCueComponent& component )
{
	copy( component );
}

wxCueComponent& wxCueComponent::operator =( const wxCueComponent& component )
{
	copy( component );
	return *this;
}

void wxCueComponent::copy( const wxCueComponent& component )
{
	m_bTrack	 = component.m_bTrack;
	m_comments	 = component.m_comments;
	m_garbage	 = component.m_garbage;
	m_cdTextTags = component.m_cdTextTags;
	m_tags		 = component.m_tags;
}

wxCueComponent& wxCueComponent::Append( const wxCueComponent& component )
{
	wxASSERT( m_bTrack == component.m_bTrack );

	WX_APPEND_ARRAY( m_comments, component.m_comments );
	WX_APPEND_ARRAY( m_garbage, component.m_garbage );
	AddCdTextInfoTags( component.m_cdTextTags );
	AddTags( component.m_tags );
	return *this;
}

wxCueComponent& wxCueComponent::operator +=( const wxCueComponent& component )
{
	return Append( component );
}

bool wxCueComponent::HasGarbage() const
{
	return ( m_garbage.Count() > 0 );
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
	wxRegEx	   reCommentMeta( wxT( "\\A([[.quotation-mark.]]{0,1})([[:upper:][.hyphen.][.underscore.][:space:][.low-line.]]+)\\1[[:space:]]+([^[:space:]].+)\\Z" ), wxRE_ADVANCED );
	wxASSERT( reCommentMeta.IsValid() );

	if ( reCommentMeta.Matches( sComment ) )
	{
		AddTag( wxCueTag::TAG_CUE_COMMENT, reCommentMeta.GetMatch( sComment, 2 ), unquoter.Unquote( reCommentMeta.GetMatch( sComment, 3 ) ) );
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

void wxCueComponent::AddTags( const wxArrayCueTag& newTags )
{
	wxCueTag::AddTags( m_tags, newTags );
}

void wxCueComponent::RemoveTags( const wxArrayCueTag& newTags )
{
	wxCueTag::RemoveTags( m_tags, newTags );
}

size_t wxCueComponent::GetTags( const wxString& sTagName, wxArrayCueTag& tags ) const
{
	return wxCueTag::GetTags( m_tags, sTagName, tags );
}

void wxCueComponent::GetTags(
	const wxTagSynonimsCollection& cdTagsSynonims,
	const wxTagSynonimsCollection& tagsSynonims,
	wxArrayCueTag& tags,
	wxArrayCueTag& rest ) const
{
	tags.Clear();
	rest.Clear();

	wxHashCueTag tagsHash;
	wxHashCueTag restHash;

	wxCueTag cueTag;
	for ( size_t nTags = m_cdTextTags.Count(), i = 0; i < nTags; i++ )
	{
		if ( cdTagsSynonims.GetName( m_cdTextTags[ i ], cueTag ) )
		{
			tagsHash[ cueTag.GetName() ].Add( cueTag );
		}
		else
		{
			restHash[ cueTag.GetName() ].Add( cueTag );
		}
	}

	for ( size_t nTags = m_tags.Count(), i = 0; i < nTags; i++ )
	{
		if ( tagsSynonims.GetName( m_tags[ i ], cueTag ) )
		{
			tagsHash[ cueTag.GetName() ].Add( cueTag );
		}
		else
		{
			restHash[ cueTag.GetName() ].Add( cueTag );
		}
	}

	wxRegEx reEmptyValue( wxT( "\\A[[:space:][:punct:]]*\\Z" ), wxRE_ADVANCED | wxRE_ICASE );
	wxASSERT( reEmptyValue.IsValid() );

	remove_duplicates( reEmptyValue, tagsHash );
	remove_duplicates( reEmptyValue, restHash );

	for ( wxHashCueTag::const_iterator i = tagsHash.begin(); i != tagsHash.end(); i++ )
	{
		wxCueTag::AddTags( tags, i->second );
	}

	for ( wxHashCueTag::const_iterator i = restHash.begin(); i != restHash.end(); i++ )
	{
		wxCueTag::AddTags( rest, i->second );
	}
}

void wxCueComponent::remove_duplicates( const wxRegEx& reEmptyValue, wxCueComponent::wxHashCueTag& tagsHash )
{
	for ( wxHashCueTag::iterator i = tagsHash.begin(); i != tagsHash.end(); i++ )
	{
		remove_duplicates( reEmptyValue, i->second );
	}
}

void wxCueComponent::remove_duplicates( const wxRegEx& reEmptyValue, wxArrayCueTag& tags )
{
	for ( size_t nTags = tags.Count(), i = 0; i < nTags; i += 1 )
	{
		wxString sValue( tags[ i ].GetValue() );
		bool	 bRemove = false;
		for ( size_t j = 0; j < nTags; j++ )
		{
			if ( i == j )
			{
				continue;
			}

			size_t n = sValue.Replace( tags[ j ].GetValue(), wxEmptyString, false );
			if ( n > 0 )
			{
				if ( reEmptyValue.Matches( sValue ) )
				{
					bRemove = true;
					break;
				}
			}
		}

		if ( bRemove )
		{
			wxLogInfo( _( "Removing tag \u201C%s\u201D - duplicated value \u201C%s\u201D" ), tags[ i ].GetName(), tags[ i ].GetValue() );
			tags.RemoveAt( i );
			i	  -= 1;
			nTags -= 1;
		}
	}
}

bool wxCueComponent::CheckEntryType( wxCueComponent::ENTRY_TYPE ctype ) const
{
	return ( ctype == ANY ) || ( m_bTrack ? ( ctype == TRACK ) : ( ctype == DISC ) );
}

bool wxCueComponent::AddCdTextInfoTag( const wxString& sKeyword, const wxString& sBody )
{
	for ( size_t i = 0; i < CdTextFieldsSize; i++ )
	{
		if ( ( sKeyword.CmpNoCase( CdTextFields[ i ].keyword ) == 0 ) && CheckEntryType( CdTextFields[ i ].type ) )
		{
			wxCueTag newTag( wxCueTag::TAG_CD_TEXT, sKeyword, sBody );
			return wxCueTag::AddTag( m_cdTextTags, newTag );
		}
	}

	return false;
}

bool wxCueComponent::AddCdTextInfoTag( const wxCueTag& tag )
{
	return AddCdTextInfoTag( tag.GetName(), tag.GetValue() );
}

void wxCueComponent::RemoveCdTextInfoTag( const wxCueTag& tag )
{
	wxCueTag::RemoveTag( m_cdTextTags, tag );
}

void wxCueComponent::AddCdTextInfoTags( const wxArrayCueTag& cueTags )
{
	for ( size_t i = 0, nCount = cueTags.Count(); i < nCount; i++ )
	{
		AddCdTextInfoTag( cueTags[ i ] );
	}
}

void wxCueComponent::RemoveCdTextInfoTags( const wxArrayCueTag& cueTags )
{
	wxCueTag::RemoveTags( m_cdTextTags, cueTags );
}

void wxCueComponent::AddTag( wxCueTag::TAG_SOURCE eSource, const wxString& sKeyword, const wxString& sBody )
{
	wxCueTag newTag( eSource, sKeyword, sBody );

	wxCueTag::AddTag( m_tags, newTag );
}

void wxCueComponent::AddTag( const wxCueTag& tag )
{
	wxCueTag::AddTag( m_tags, tag );
}

void wxCueComponent::RemoveTag( const wxCueTag& tag )
{
	wxCueTag::RemoveTag( m_tags, tag );
}

void wxCueComponent::RemoveTag( const wxString& sTagName )
{
	wxCueTag::RemoveTag( m_tags, sTagName );
}

void wxCueComponent::Clear()
{
	m_comments.Clear();
	m_garbage.Clear();
	m_cdTextTags.Clear();
	m_tags.Clear();
}

wxString wxCueComponent::FormatCdTextData( const wxString& sKeyword, const wxString& sValue )
{
	ENTRY_FORMAT fmt;

	wxCHECK( GetCdTextInfoFormat( sKeyword, fmt ), wxT( "" ) );
	if ( fmt == BINARY )
	{
		return sValue;
	}
	else // characters
	{
		return wxCueTag::Quote( wxCueTag::Escape( sValue ) );
	}
}

static size_t find_keyword( const wxArrayCueTag& tags, const wxString& sKeyword )
{
	for ( size_t numTags = tags.Count(), i = 0; i < numTags; i++ )
	{
		if ( tags[ i ] == sKeyword )
		{
			return i;
		}
	}

	return -1;
}

void wxCueComponent::GetReplacements( wxCueComponent::wxHashString& replacements ) const
{
	for ( size_t i = 0; i < CdTextFieldsSize; i++ )
	{
		if ( CdTextFields[ i ].replacement == wxT( '\000' ) )
		{
			continue;
		}

		wxString sValue;
		size_t	 idx = find_keyword( m_cdTextTags, CdTextFields[ i ].keyword );
		if ( idx != -1 )
		{
			sValue = m_cdTextTags[ idx ].GetValue();
		}

		wxString s( CdTextFields[ i ].replacement );
		s.Prepend( m_bTrack ? wxT( "t" ) : wxT( "d" ) );

		bool bAdd = false;
		switch ( CdTextFields[ i ].type )
		{
			case ANY:
			{
				bAdd = true;
				wxString s( CdTextFields[ i ].replacement );
				s.Prepend( wxT( "a" ) );
				replacements[ s ] = sValue;
				break;
			}

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

