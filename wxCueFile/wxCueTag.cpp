/*
   wxCueTag.cpp
 */
#include "StdWx.h"
#include <wxCueFile/wxCueTag.h>
#include <wxCueFile/wxUnquoter.h>
#include <wxCueFile/wxTrailingSpacesRemover.h>
#include <wxCueFile/wxReduntantSpacesRemover.h>
#include <wxCueFile/wxEllipsizer.h>
#include <wxCueFile/wxTextOutputStreamOnString.h>
#include <wxCueFile/wxTextInputStreamOnString.h>

// ===============================================================================

const wxChar* const wxCueTag::Name::CUESHEET	 = wxT( "CUESHEET" );
const wxChar* const wxCueTag::Name::TOTALTRACKS	 = wxT( "TOTALTRACKS" );
const wxChar* const wxCueTag::Name::ARRANGER	 = wxT( "ARRANGER" );
const wxChar* const wxCueTag::Name::COMPOSER	 = wxT( "COMPOSER" );
const wxChar* const wxCueTag::Name::ISRC		 = wxT( "ISRC" );
const wxChar* const wxCueTag::Name::TITLE		 = wxT( "TITLE" );
const wxChar* const wxCueTag::Name::ALBUM		 = wxT( "ALBUM" );
const wxChar* const wxCueTag::Name::PERFORMER	 = wxT( "PERFORMER" );
const wxChar* const wxCueTag::Name::ARTIST		 = wxT( "ARTIST" );
const wxChar* const wxCueTag::Name::ALBUM_ARTIST = wxT( "ALBUM ARTIST" );
const wxChar* const wxCueTag::Name::CATALOG		 = wxT( "CATALOG" );
const wxChar* const wxCueTag::Name::CDTEXTFILE	 = wxT( "CDTEXTFILE" );
const wxChar* const wxCueTag::Name::DISC_ID		 = wxT( "DISC_ID" );
const wxChar* const wxCueTag::Name::GENRE		 = wxT( "GENRE" );
const wxChar* const wxCueTag::Name::MESSAGE		 = wxT( "MESSAGE" );
const wxChar* const wxCueTag::Name::SONGWRITER	 = wxT( "SONGWRITER" );
const wxChar* const wxCueTag::Name::UPC_EAN		 = wxT( "UPC_EAN" );
const wxChar* const wxCueTag::Name::SIZE_INFO	 = wxT( "SIZE_INFO" );
const wxChar* const wxCueTag::Name::TOC_INFO	 = wxT( "TOC_INFO" );
const wxChar* const wxCueTag::Name::TOC_INFO2	 = wxT( "TOC_INFO2" );
const wxChar* const wxCueTag::Name::DISCNUMBER	 = wxT( "DISCNUMBER" );
const wxChar* const wxCueTag::Name::TOTALDISCS	 = wxT( "TOTALDISCS" );

// ===============================================================================

const wxCueTag::SOURCE2TEXT wxCueTag::SOURCE2TEXT_MAPPING[] =
{
	{ TAG_UNKNOWN, _( "Unknown" ) },
	{ TAG_CD_TEXT, _( "CD-TEXT" ) },
	{ TAG_CUE_COMMENT, _( "CUE Comment" ) },
	{ TAG_MEDIA_METADATA, _( "Media metadata" ) },
	{ TAG_AUTO_GENERATED, _( "Automatically generated" ) }
};

const size_t wxCueTag::SOURCE2TEXT_MAPPING_SIZE = WXSIZEOF( wxCueTag::SOURCE2TEXT_MAPPING );

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxCueTag, wxObject );

// ===============================================================================

wxString wxCueTag::SourceToString( wxCueTag::TAG_SOURCE eSource )
{
	for ( size_t i = 0; i < SOURCE2TEXT_MAPPING_SIZE; i++ )
	{
		if ( eSource == SOURCE2TEXT_MAPPING[ i ].eSource )
		{
			return SOURCE2TEXT_MAPPING[ i ].pText;
		}
	}

	return wxString::Format( wxT( "TAG_SOURCE <%d>" ), eSource );
}

wxCueTag::wxCueTag():
	m_eSource( TAG_UNKNOWN ), m_bMultiline( false )
{}

wxCueTag::wxCueTag( wxCueTag::TAG_SOURCE eSource, const wxString& sName, const wxString& sValue ):
	m_eSource( eSource ), m_sName( sName.Upper() )
{
	SetValue( sValue );
}

wxCueTag::wxCueTag( const wxCueTag& cueTag )
{
	copy( cueTag );
}

wxCueTag::TAG_SOURCE wxCueTag::GetSource() const
{
	return m_eSource;
}

wxString wxCueTag::GetSourceAsString() const
{
	return SourceToString( m_eSource );
}

const wxString& wxCueTag::GetName() const
{
	return m_sName;
}

const wxString& wxCueTag::GetValue() const
{
	return m_sValue;
}

const wxCueTag& wxCueTag::GetValue( wxArrayString& asLines ) const
{
	if ( m_bMultiline )
	{
		wxTextInputStreamOnString tis( m_sValue );
		while ( !tis.Eof() )
		{
			asLines.Add( ( *tis ).ReadLine() );
		}
	}
	else
	{
		asLines.Add( m_sValue );
	}

	return *this;
}

wxString wxCueTag::GetQuotedValue( bool bEscape ) const
{
	if ( bEscape )
	{
		return Quote( Escape( m_sValue ) );
	}
	else
	{
		return Quote( m_sValue );
	}
}

wxString wxCueTag::GetFlattenValue() const
{
	if ( m_bMultiline )
	{
		wxTextInputStreamOnString  tis( m_sValue );
		wxTextOutputStreamOnString tos;

		while ( !tis.Eof() )
		{
			*tos << ( *tis ).ReadLine() << wxT( '/' );
		}

		( *tos ).Flush();

		const wxString& sOut = tos.GetString();
		return wxString( sOut, sOut.Length() - 1 );
	}
	else
	{
		return m_sValue;
	}
}

wxCueTag& wxCueTag::SetSource( wxCueTag::TAG_SOURCE eSource )
{
	m_eSource = eSource;
	return *this;
}

wxCueTag& wxCueTag::SetName( const wxString& sName )
{
	m_sName = sName.Upper();
	return *this;
}

wxCueTag& wxCueTag::SetValue( const wxString& sValue )
{
	size_t					   nLines = 0;
	wxTextInputStreamOnString  tis( sValue );
	wxTextOutputStreamOnString tos;

	while ( !tis.Eof() )
	{
		*tos << ( *tis ).ReadLine() << endl;
		nLines += 1;
	}

	if ( nLines > 1 )
	{
		m_sValue	 = tos.GetString();
		m_bMultiline = true;
	}
	else
	{
		m_sValue	 = sValue;
		m_bMultiline = false;
	}

	return *this;
}

void wxCueTag::copy( const wxCueTag& cueTag )
{
	m_eSource	 = cueTag.m_eSource;
	m_sName		 = cueTag.m_sName.Upper();
	m_sValue	 = cueTag.m_sValue;
	m_bMultiline = cueTag.m_bMultiline;
}

wxCueTag& wxCueTag::operator =( const wxCueTag& cueTag )
{
	copy( cueTag );
	return *this;
}

bool wxCueTag::operator ==( const wxCueTag& tag ) const
{
	return
		m_sName.CmpNoCase( tag.m_sName ) == 0 &&
		m_sValue.Cmp( tag.m_sValue ) == 0;
}

bool wxCueTag::operator ==( const wxString& sTagName ) const
{
	return m_sName.CmpNoCase( sTagName ) == 0;
}

bool wxCueTag::IsMultiline() const
{
	return m_bMultiline;
}

void wxCueTag::Unquote( const wxUnquoter& unquoter )
{
	m_sValue = unquoter.UnquoteAndCorrect( m_sValue );
}

void wxCueTag::RemoveTrailingSpaces( const wxTrailingSpacesRemover& spacesRemover )
{
	m_sValue = spacesRemover.Remove( m_sValue );
}

int wxCueTag::RemoveExtraSpaces( const wxReduntantSpacesRemover& spacesRemover )
{
	if ( m_bMultiline )
	{
		return 0;
	}
	else
	{
		return spacesRemover.Remove( m_sValue );
	}
}

void wxCueTag::Ellipsize( const wxEllipsizer& ellipsizer )
{
	ellipsizer.EllipsizeEx( m_sValue, m_sValue );
}

wxString wxCueTag::Escape( const wxString& sValue )
{
	wxString s( sValue );

	s.Replace( wxT( '\"' ), wxT( "\\\"" ) );
	s.Replace( wxT( '\'' ), wxT( "\\'" ) );
	s.Replace( wxT( '\\' ), wxT( "\\\\" ) );
	return s;
}

wxString wxCueTag::UnEscape( const wxString& sValue )
{
	wxString s( sValue );

	s.Replace( wxT( "\\'" ), wxT( '\'' ) );
	s.Replace( wxT( "\\\"" ), wxT( '\"' ) );
	s.Replace( wxT( "\\\\" ), wxT( '\\' ) );
	return s;
}

wxString wxCueTag::Quote( const wxString& sValue )
{
	return wxString::Format( wxT( "\"%s\"" ), sValue );
}

size_t wxCueTag::GetTags( const wxArrayCueTag& sourceTags, const wxString& sTagName, wxArrayCueTag& tags )
{
	size_t nCounter = 0;

	for ( size_t i = 0, nCount = sourceTags.Count(); i < nCount; i++ )
	{
		if ( sourceTags[ i ] == sTagName )
		{
			AddTag( tags, sourceTags[ i ] );
			nCounter += 1;
		}
	}

	return nCounter;
}

size_t wxCueTag::MoveTags( wxArrayCueTag& sourceTags, const wxString& sTagName, wxArrayCueTag& tags )
{
	size_t nCounter = 0;

	for ( size_t i = 0, nCount = sourceTags.Count(); i < nCount; i++ )
	{
		if ( sourceTags[ i ] == sTagName )
		{
			AddTag( tags, sourceTags[ i ] );
			sourceTags.RemoveAt( i );

			nCounter += 1;
			nCount	 -= 1;
			i		 -= 1;
		}
	}

	return nCounter;
}

bool wxCueTag::FindTag( const wxArrayCueTag& tags, const wxCueTag& cueTag )
{
	for ( size_t numTags = tags.Count(), i = 0; i < numTags; i++ )
	{
		if ( tags[ i ] == cueTag )
		{
			return true;
		}
	}

	return false;
}

bool wxCueTag::AddTag( wxArrayCueTag& tags, const wxCueTag& cueTag )
{
	if ( !FindTag( tags, cueTag ) )
	{
		tags.Add( cueTag );
		return true;
	}

	return false;
}

bool wxCueTag::AddTagIf( wxArrayCueTag& tags, const wxCueTag& tagToAdd, const wxCueTag& tagToCheck )
{
	if ( FindTag( tags, tagToCheck ) )
	{
		return AddTag( tags, tagToAdd );
	}
	else
	{
		return false;
	}
}

bool wxCueTag::AddTagIfAndRemove( wxArrayCueTag& tags, const wxCueTag& tagToAdd, const wxCueTag& tagToCheck )
{
	if ( FindTag( tags, tagToCheck ) )
	{
		RemoveTag( tags, tagToCheck );
		return AddTag( tags, tagToAdd );
	}
	else
	{
		return false;
	}
}

size_t wxCueTag::RemoveTag( wxArrayCueTag& tags, const wxCueTag& cueTag )
{
	size_t nCounter = 0;

	for ( size_t i = 0, nCount = tags.Count(); i < nCount; i++ )
	{
		if ( tags[ i ] == cueTag )
		{
			tags.RemoveAt( i );

			nCounter += 1;
			nCount	 -= 1;
			i		 -= 1;
		}
	}

	return nCounter;
}

size_t wxCueTag::RemoveTag( wxArrayCueTag& tags, const wxString& sTagName )
{
	size_t nCounter = 0;

	for ( size_t i = 0, nCount = tags.Count(); i < nCount; i++ )
	{
		if ( tags[ i ] == sTagName )
		{
			tags.RemoveAt( i );

			nCounter += 1;
			nCount	 -= 1;
			i		 -= 1;
		}
	}

	return nCounter;
}

void wxCueTag::AddTags( wxArrayCueTag& tags, const wxArrayCueTag& newTags )
{
	for ( size_t nCount = newTags.Count(), i = 0; i < nCount; i++ )
	{
		AddTag( tags, newTags[ i ] );
	}
}

size_t wxCueTag::RemoveTags( wxArrayCueTag& tags, const wxArrayCueTag& tagsToDelete )
{
	size_t nCounter = 0;

	for ( size_t nCount = tagsToDelete.Count(), i = 0; i < nCount; i++ )
	{
		nCounter += RemoveTag( tags, tagsToDelete[ i ] );
	}

	return nCounter;
}

void wxCueTag::CommonTags( wxArrayCueTag& commonTags, const wxArrayCueTag& group1, const wxArrayCueTag& group2 )
{
	commonTags.Clear();
	for ( size_t i = 0, nCount = group1.Count(); i < nCount; i++ )
	{
		if ( FindTag( group2, group1[ i ] ) )
		{
			commonTags.Add( group1[ i ] );
		}
	}
}

bool wxCueTag::FindCommonPart( wxCueTag& commonTag, const wxCueTag& tag1, const wxCueTag& tag2 )
{
	wxASSERT( tag1 == tag2.GetName() );

	wxString sValue1( tag1.GetValue() ), sValue2( tag2.GetValue() );

	size_t nLen = sValue1.Length();
	if ( sValue2.Length() < nLen )
	{
		nLen = sValue2.Length();
	}

	if ( nLen == 0u )
	{
		return false;
	}

	sValue1.Truncate( nLen );
	sValue2.Truncate( nLen );
	for ( size_t i = nLen; i > 0; i--, sValue1.RemoveLast(), sValue2.RemoveLast() )
	{
		if ( sValue1.CmpNoCase( sValue2 ) == 0 )
		{
			commonTag.SetSource( wxCueTag::TAG_AUTO_GENERATED )
			.SetName( tag1.GetName() )
			.SetValue( sValue1 );

			return true;
		}
	}

	return false;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( wxArrayCueTag );

