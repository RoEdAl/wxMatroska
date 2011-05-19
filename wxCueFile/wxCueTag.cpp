/*
   wxCueTag.cpp
 */
#include "StdWx.h"
#include <wxCueFile/wxCueTag.h>
#include <wxCueFile/wxTrailingSpacesRemover.h>
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

// ===============================================================================

wxCueTag::SOURCE2TEXT wxCueTag::SOURCE2TEXT_MAPPING[] =
{
	{ TAG_UNKNOWN, _( "Unknown" ) },
	{ TAG_CD_TEXT, _( "CD-TEXT" ) },
	{ TAG_CUE_COMMENT, _( "CUE Comment" ) },
	{ TAG_MEDIA_METADATA, _( "Media metadata" ) },
	{ TAG_AUTO_GENERATED, _( "Automatically generated" ) }
};

size_t wxCueTag::SOURCE2TEXT_MAPPING_SIZE = WXSIZEOF( wxCueTag::SOURCE2TEXT_MAPPING );

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxCueTag, wxObject )

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
	m_eSource( TAG_UNKNOWN )
{}

wxCueTag::wxCueTag( wxCueTag::TAG_SOURCE eSource, const wxString& sName, const wxString& sValue ):
	m_eSource( eSource ), m_sName( sName.Upper() ), m_sValue( sValue )
{}

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

void wxCueTag::GetValueEx( bool bFlatten, wxString& sValue, bool& bMultiline ) const
{
	size_t					   nLines = 0;
	wxTextInputStreamOnString  tis( m_sValue );
	wxTextOutputStreamOnString tos;

	while ( !tis.Eof() )
	{
		if ( bFlatten )
		{
			*tos << ( *tis ).ReadLine() << wxT( '/' );
		}
		else
		{
			*tos << ( *tis ).ReadLine() << endl;
		}

		nLines += 1;
	}

	if ( nLines <= 1 )
	{
		sValue	   = m_sValue;
		bMultiline = false;
	}
	else
	{
		( *tos ).Flush();
		const wxString& sOut = tos.GetString();
		if ( bFlatten )
		{
			sValue = wxString( sOut, sOut.Length() - 1 );
		}
		else
		{
			sValue = sOut;
		}

		bMultiline = true;
	}
}

wxString wxCueTag::GetFlattenValue() const
{
	wxASSERT( IsMultiline() );

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
	m_sValue = sValue;
	return *this;
}

void wxCueTag::copy( const wxCueTag& cueTag )
{
	m_eSource = cueTag.m_eSource;
	m_sName	  = cueTag.m_sName.Upper();
	m_sValue  = cueTag.m_sValue;
}

wxCueTag& wxCueTag::operator =( const wxCueTag& cueTag )
{
	copy( cueTag );
	return *this;
}

bool wxCueTag::IsMultiline() const
{
	wxTextInputStreamOnString tis( m_sValue );
	int						  nLines = 0;

	while ( !( tis.Eof() || ( nLines > 1 ) ) )
	{
		tis.GetStream().ReadLine();
		nLines += 1;
	}

	return ( nLines > 1 );
}

void wxCueTag::RemoveTrailingSpaces( const wxTrailingSpacesRemover& spacesRemover )
{
	m_sValue = spacesRemover.Remove( m_sValue );
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

bool wxCueTag::FindTag( const wxArrayCueTag& tags, const wxCueTag& cueTag )
{
	for ( size_t numTags = tags.Count(), i = 0; i < numTags; i++ )
	{
		if ( cueTag.GetName().CmpNoCase( tags[ i ].GetName() ) == 0 &&
			 cueTag.GetValue().Cmp( tags[ i ].GetValue() ) == 0 )
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

void wxCueTag::AddTags( wxArrayCueTag& tags, const wxArrayCueTag& newTags )
{
	for ( size_t newTagsCount = newTags.Count(), i = 0; i < newTagsCount; i++ )
	{
		AddTag( tags, newTags[ i ] );
	}
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( wxArrayCueTag );

