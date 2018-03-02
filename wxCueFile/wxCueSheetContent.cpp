/*
 *      wxCueSheetContent.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxCueSheetContent.h>

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxCueSheetContent, wxObject );

// ===============================================================================

wxCueSheetContent::wxCueSheetContent( void )
{}

wxCueSheetContent::wxCueSheetContent( const wxString& sValue ) :
	m_sValue( sValue ), m_bEmbedded( false )
{
	wxASSERT( !sValue.IsEmpty() );
}

wxCueSheetContent::wxCueSheetContent( const wxDataFile& source ) :
	m_sValue( source.GetCueSheet() ), m_source( source ), m_bEmbedded( true )
{
	wxASSERT( source.GetFileName().IsOk() && !source.GetFileName().IsDir() );
}

wxCueSheetContent::wxCueSheetContent( const wxString& sValue, const wxDataFile& source, bool bEmbedded ) :
	m_sValue( sValue ), m_source( source ), m_bEmbedded( bEmbedded )
{
	wxASSERT( !sValue.IsEmpty() );
	wxASSERT( source.GetFileName().IsOk() && !source.GetFileName().IsDir() );
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
	m_source    = csContent.m_source;
	m_sValue    = csContent.m_sValue;
	m_bEmbedded = csContent.m_bEmbedded;
}

bool wxCueSheetContent::HasSource() const
{
	return m_source.GetFileName().IsOk();
}

const wxDataFile& wxCueSheetContent::GetSource() const
{
	wxASSERT( HasSource() );
	return m_source;
}

const wxString& wxCueSheetContent::GetValue() const
{
	return m_sValue;
}

bool wxCueSheetContent::IsEmbedded() const
{
	return m_bEmbedded;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( wxArrayCueSheetContent );

