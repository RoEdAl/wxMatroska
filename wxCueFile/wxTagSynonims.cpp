/*
   wxTagSynonims.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxTagSynonims.h>
#include <wxCueFile/wxCueComponent.h>

IMPLEMENT_DYNAMIC_CLASS( wxTagSynonims, wxObject )

wxTagSynonims::wxTagSynonims( void )
{}

wxTagSynonims::wxTagSynonims( const wxString& sName, const wxArrayString& asSynonims )
	: m_sName( sName ), m_asSynonims( asSynonims )
{
	wxASSERT( !sName.IsEmpty() );
}

wxTagSynonims::wxTagSynonims( const wxTagSynonims& synonims )
{
	copy( synonims );
}

wxTagSynonims& wxTagSynonims::operator =( const wxTagSynonims& synonims )
{
	copy( synonims );
	return *this;
}

void wxTagSynonims::copy( const wxTagSynonims& synonims )
{
	m_sName		 = synonims.m_sName;
	m_asSynonims = synonims.m_asSynonims;
}

wxTagSynonims& wxTagSynonims::SetName( const wxString& sName )
{
	wxASSERT( !sName.IsEmpty() );
	m_sName = sName;
	return *this;
}

wxTagSynonims& wxTagSynonims::SetSynonims( const wxArrayString& synonims )
{
	m_asSynonims = synonims;
	return *this;
}

const wxString& wxTagSynonims::GetName() const
{
	return m_sName;
}

const wxArrayString& wxTagSynonims::GetSynonims() const
{
	return m_asSynonims;
}

bool wxTagSynonims::GetName( const wxString& sSynonim, wxString& sName ) const
{
	if ( m_sName.CmpNoCase( sSynonim ) == 0 )
	{
		sName = m_sName;
		return true;
	}

	size_t nSynonims = m_asSynonims.GetCount();
	for ( size_t i = 0 ; i < nSynonims ; i++ )
	{
		if ( m_asSynonims[ i ].CmpNoCase( sSynonim ) == 0 )
		{
			sName = m_asSynonims[ i ];
			return true;
		}
	}

	sName = sSynonim;
	return false;
}

bool wxTagSynonims::GetName( const wxCueTag& synonim, wxCueTag& cueTag ) const
{
	if ( m_sName.CmpNoCase( synonim.GetName() ) == 0 )
	{
		cueTag.SetSource( synonim.GetSource() );
		cueTag.SetName( m_sName );
		cueTag.SetValue( synonim.GetValue() );
		return true;
	}

	size_t nSynonims = m_asSynonims.GetCount();
	for ( size_t i = 0 ; i < nSynonims ; i++ )
	{
		if ( m_asSynonims[ i ].CmpNoCase( synonim.GetName() ) == 0 )
		{
			cueTag.SetSource( synonim.GetSource() );
			cueTag.SetName( m_sName );
			cueTag.SetValue( synonim.GetValue() );
			return true;
		}
	}

	cueTag = synonim;
	return false;
}

// =================================================================================

wxTagSynonimsCollection::wxTagSynonimsCollection( void )
{}

wxTagSynonimsCollection::wxTagSynonimsCollection( const wxTagSynonimsCollection& src )
	: _wxArrayTagSynonims( src )
{}

wxTagSynonimsCollection& wxTagSynonimsCollection::operator =( const wxTagSynonimsCollection& src )
{
	_wxArrayTagSynonims::operator =( src );
	return *this;
}

bool wxTagSynonimsCollection::GetName( const wxString& sSynonim, wxString& sName ) const
{
	size_t nSynonims = GetCount();
	for ( size_t i = 0 ; i < nSynonims ; i++ )
	{
		if ( Item( i ).GetName( sSynonim, sName ) )
		{
			return true;
		}
	}

	sName = sSynonim;
	return false;
}

bool wxTagSynonimsCollection::GetName( const wxCueTag& synonim, wxCueTag& cueTag ) const
{
	size_t nSynonims = GetCount();
	for ( size_t i = 0 ; i < nSynonims ; i++ )
	{
		if ( Item( i ).GetName( synonim, cueTag ) )
		{
			return true;
		}
	}

	cueTag = synonim;
	return false;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( _wxArrayTagSynonims );