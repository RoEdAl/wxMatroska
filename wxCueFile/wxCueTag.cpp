/*
 * wxCueTag.cpp
 */
#include "StdWx.h"
#include <wxCueFile/wxCueTag.h>
#include <wxCueFile/wxUnquoter.h>
#include <wxCueFile/wxTagSynonims.h>
#include <wxCueFile/wxTrailingSpacesRemover.h>
#include <wxCueFile/wxReduntantSpacesRemover.h>
#include <wxCueFile/wxEllipsizer.h>
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#include <wxEncodingDetection/wxTextInputStreamOnString.h>

// ===============================================================================

const wxChar wxCueTag::Name::CUESHEET[]		= wxS( "CUESHEET" );
const wxChar wxCueTag::Name::TOTALTRACKS[]	= wxS( "TOTALTRACKS" );
const wxChar wxCueTag::Name::ARRANGER[]		= wxS( "ARRANGER" );
const wxChar wxCueTag::Name::COMPOSER[]		= wxS( "COMPOSER" );
const wxChar wxCueTag::Name::ISRC[]			= wxS( "ISRC" );
const wxChar wxCueTag::Name::TITLE[]		= wxS( "TITLE" );
const wxChar wxCueTag::Name::ALBUM[]		= wxS( "ALBUM" );
const wxChar wxCueTag::Name::PERFORMER[]	= wxS( "PERFORMER" );
const wxChar wxCueTag::Name::ARTIST[]		= wxS( "ARTIST" );
const wxChar wxCueTag::Name::ALBUM_ARTIST[] = wxS( "ALBUM ARTIST" );
const wxChar wxCueTag::Name::CATALOG[]		= wxS( "CATALOG" );
const wxChar wxCueTag::Name::CDTEXTFILE[]	= wxS( "CDTEXTFILE" );
const wxChar wxCueTag::Name::DISC_ID[]		= wxS( "DISC_ID" );
const wxChar wxCueTag::Name::GENRE[]		= wxS( "GENRE" );
const wxChar wxCueTag::Name::MESSAGE[]		= wxS( "MESSAGE" );
const wxChar wxCueTag::Name::SONGWRITER[]	= wxS( "SONGWRITER" );
const wxChar wxCueTag::Name::UPC_EAN[]		= wxS( "UPC_EAN" );
const wxChar wxCueTag::Name::SIZE_INFO[]	= wxS( "SIZE_INFO" );
const wxChar wxCueTag::Name::TOC_INFO[]		= wxS( "TOC_INFO" );
const wxChar wxCueTag::Name::TOC_INFO2[]	= wxS( "TOC_INFO2" );
const wxChar wxCueTag::Name::DISCNUMBER[]	= wxS( "DISCNUMBER" );
const wxChar wxCueTag::Name::TOTALDISCS[]	= wxS( "TOTALDISCS" );

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

wxString wxCueTag::SourcesToString( wxCueTag::TagSources nTagSources )
{
	wxString s;

	for ( size_t i = 0; i < SOURCE2TEXT_MAPPING_SIZE; i++ )
	{
		if ( ( nTagSources & SOURCE2TEXT_MAPPING[ i ].eSource ) != 0u )
		{
			s << SOURCE2TEXT_MAPPING[ i ].pText << wxT( ',' );
		}
	}

	if ( !s.IsEmpty() )
	{
		s.RemoveLast();
	}

	return s;
}

bool wxCueTag::TestTagSources( wxCueTag::TagSources sources, wxCueTag::TagSources mask )
{
	return ( sources & mask ) == mask;
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

bool wxCueTag::TestSource( wxCueTag::TagSources nTagSources ) const
{
	return TestTagSources( nTagSources, m_eSource );
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
	return GetFlattenValue( wxT( '/' ) );
}

wxString wxCueTag::GetFlattenValue( const wxString& sSeparator ) const
{
	if ( m_bMultiline )
	{
		wxTextInputStreamOnString  tis( m_sValue );
		wxTextOutputStreamOnString tos;

		while ( !tis.Eof() )
		{
			*tos << ( *tis ).ReadLine() << sSeparator;
		}

		( *tos ).Flush();

		const wxString& sOut = tos.GetString();
		return wxString( sOut, sOut.Length() - sSeparator.Length() );
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
	unquoter.CorrectQuotes( m_sValue );
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

	for ( size_t i = 0, nCount = sourceTags.GetCount(); i < nCount; i++ )
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

	for ( size_t i = 0, nCount = sourceTags.GetCount(); i < nCount; i++ )
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

size_t wxCueTag::MoveTags( wxArrayCueTag& sourceTags, const wxTagSynonimsCollection& tagSynonims, wxArrayCueTag& tags )
{
	size_t nCounter = 0;

	wxCueTag tag;

	for ( size_t i = 0, nCount = sourceTags.GetCount(); i < nCount; i++ )
	{
		if ( tagSynonims.GetName( sourceTags[ i ], tag ) )
		{
			AddTag( tags, tag );
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
	for ( size_t i = 0, nCount = tags.GetCount(); i < nCount; i++ )
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

	for ( size_t i = 0, nCount = tags.GetCount(); i < nCount; i++ )
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

	for ( size_t i = 0, nCount = tags.GetCount(); i < nCount; i++ )
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
	for ( size_t i = 0, nCount = newTags.GetCount(); i < nCount; i++ )
	{
		AddTag( tags, newTags[ i ] );
	}
}

void wxCueTag::AddTags( wxArrayCueTag& tags, const wxHashArrayCueTag& tagsHash )
{
	for ( wxHashArrayCueTag::const_iterator i = tagsHash.begin(); i != tagsHash.end(); i++ )
	{
		AddTags( tags, i->second );
	}
}

size_t wxCueTag::RemoveTags( wxArrayCueTag& tags, const wxArrayCueTag& tagsToDelete )
{
	size_t nCounter = 0;

	for ( size_t i = 0, nCount = tagsToDelete.GetCount(); i < nCount; i++ )
	{
		nCounter += RemoveTag( tags, tagsToDelete[ i ] );
	}

	return nCounter;
}

void wxCueTag::CommonTags( wxArrayCueTag& commonTags, const wxArrayCueTag& group1, const wxArrayCueTag& group2 )
{
	commonTags.Clear();
	for ( size_t i = 0, nCount = group1.GetCount(); i < nCount; i++ )
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

wxString wxCueTag::GetFlattenValues( const wxArrayCueTag& tags, const wxString& sSeparator )
{
	wxString sResult;

	for ( size_t i = 0, nCount = tags.GetCount(); i < nCount; i++ )
	{
		sResult.Append( tags[ i ].GetFlattenValue( sSeparator ) );
		sResult.Append( sSeparator );
	}

	if ( tags.IsEmpty() )
	{
		return sResult;
	}
	else
	{
		return sResult.Truncate( sResult.Length() - sSeparator.Length() );
	}
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( wxArrayCueTag );

