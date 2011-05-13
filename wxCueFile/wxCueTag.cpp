/*
   wxCueTag.cpp
 */
#include "StdWx.h"
#include <wxCueFile/wxCueTag.h>
#include <wxCueFile/wxTrailingSpacesRemover.h>
#include <wxCueFile/wxEllipsizer.h>
#include <wxCueFile/wxTextOutputStreamOnString.h>
#include "wxTextInputStreamOnString.h"

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

wxIMPLEMENT_DYNAMIC_CLASS( wxCueTag, wxObject )

wxCueTag::wxCueTag()
	:m_eSource( TAG_UNKNOWN )
{}

wxCueTag::wxCueTag( wxCueTag::TAG_SOURCE eSource, const wxString& sName, const wxString& sValue )
	:m_eSource( eSource ), m_sName( sName.Upper() ), m_sValue( sValue )
{}

wxCueTag::wxCueTag( const wxCueTag& cueTag )
{
	copy( cueTag );
}

wxCueTag::TAG_SOURCE wxCueTag::GetSource() const
{
	return m_eSource;
}

wxCueTag::SOURCE2TEXT wxCueTag::SOURCE2TEXT_MAPPING[] =
{
	{ TAG_UNKNOWN, _( "Unknown" ) },
	{ TAG_CD_TEXT, _( "CD-TEXT" ) },
	{ TAG_CUE_COMMENT, _( "CUE Comment" ) },
	{ TAG_MEDIA_METADATA, _( "Media metadata" ) },
	{ TAG_AUTO_GENERATED, _( "Automatically generated" ) }
};

size_t wxCueTag::SOURCE2TEXT_MAPPING_SIZE = WXSIZEOF( wxCueTag::SOURCE2TEXT_MAPPING );

wxString wxCueTag::SourceToString( wxCueTag::TAG_SOURCE eSource )
{
	for ( size_t i = 0 ; i < SOURCE2TEXT_MAPPING_SIZE ; i++ )
	{
		if ( eSource == SOURCE2TEXT_MAPPING[ i ].eSource )
		{
			return SOURCE2TEXT_MAPPING[ i ].pText;
		}
	}

	return wxEmptyString;
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

wxString wxCueTag::GetFlattenValue() const
{
	wxASSERT( IsMultiline() );

	wxTextInputStreamOnString tis( m_sValue );

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

wxCueTag& wxCueTag::operator=( const wxCueTag& cueTag )
{
	copy( cueTag );
	return *this;
}

bool wxCueTag::IsMultiline() const
{
	wxTextInputStreamOnString tis( m_sValue );
	int nLines = 0;
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

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( wxArrayCueTag );