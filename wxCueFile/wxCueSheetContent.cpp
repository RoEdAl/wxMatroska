/*
        wxCueSheetContent.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxCueSheetContent.h>

wxIMPLEMENT_DYNAMIC_CLASS( wxCueSheetContent, wxObject ) wxCueSheetContent::wxCueSheetContent( void )
{}

wxCueSheetContent::wxCueSheetContent( const wxString& sValue ):
	m_sValue( sValue ), m_bEmbedded( false )
{
	wxASSERT( !sValue.IsEmpty() );
}

wxCueSheetContent::wxCueSheetContent( const wxString& sValue, const wxFileName& source, bool bEmbedded )
	: m_sValue( sValue ), m_source( source ), m_bEmbedded( bEmbedded )
{
	wxASSERT( !sValue.IsEmpty() );
	wxASSERT( source.IsOk() && !source.IsDir() );
}

wxCueSheetContent::wxCueSheetContent( const wxCueSheetContent& csContent )
{
	copy( csContent );
}

wxCueSheetContent& wxCueSheetContent::operator =( const wxCueSheetContent& csContent )
{
	copy( csContent );
	return *this;
}

void wxCueSheetContent::copy( const wxCueSheetContent& csContent )
{
	m_source	= csContent.m_source;
	m_bEmbedded = csContent.m_bEmbedded;
	m_sValue	= csContent.m_sValue;
}

bool wxCueSheetContent::HasSource() const
{
	return m_source.IsOk();
}

const wxFileName& wxCueSheetContent::GetSource() const
{
	wxASSERT( HasSource() );
	return m_source;
}

bool wxCueSheetContent::IsEmbedded() const
{
	return m_bEmbedded;
}

const wxString& wxCueSheetContent::GetValue() const
{
	return m_sValue;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( wxArrayCueSheetContent );