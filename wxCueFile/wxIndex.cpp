/*
   wxIndex.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxIndex.h>

wxIMPLEMENT_DYNAMIC_CLASS( wxIndex, wxObject ) wxIndex::wxIndex( void ):
	m_number( 0 ), m_offset( 0, 0 ), m_bCdFrames( false )
{}

wxIndex::wxIndex( unsigned int number, wxULongLong offset ):
	m_number( number ), m_offset( offset ), m_bCdFrames( false )
{}

wxIndex::wxIndex( const wxIndex& idx )
{
	copy( idx );
}

wxIndex& wxIndex::operator =( const wxIndex& idx )
{
	copy( idx );
	return *this;
}

void wxIndex::copy( const wxIndex& idx )
{
	m_number	= idx.m_number;
	m_offset	= idx.m_offset;
	m_bCdFrames = idx.m_bCdFrames;
}

size_t wxIndex::GetNumber() const
{
	return m_number;
}

bool wxIndex::HasCdFrames() const
{
	return m_bCdFrames;
}

const wxULongLong& wxIndex::GetOffset() const
{
	return m_offset;
}

bool wxIndex::IsValid( bool bPrePost ) const
{
	return
		( bPrePost?( m_number == 0u ) : true ) &&
		( m_number < 100u );
}

wxIndex& wxIndex::SetNumber( unsigned int number )
{
	m_number = number;
	return *this;
}

wxIndex& wxIndex::SetOffset( wxULongLong offset )
{
	m_offset = offset;
	return *this;
}

wxIndex& wxIndex::Assign( size_t number, wxULongLong offset )
{
	m_number	= number;
	m_offset	= offset;
	m_bCdFrames = false;
	return *this;
}

wxIndex& wxIndex::Assign( size_t number, unsigned long minutes, unsigned long seconds, unsigned long frames )
{
	m_number = number;
	wxULongLong cdFrames( 0, minutes );
	cdFrames   *= wxULL( 4500 );
	cdFrames   += wxULL( 75 ) * seconds;
	cdFrames   += frames;
	m_offset	= cdFrames;
	m_bCdFrames = true;
	return *this;
}

wxString wxIndex::GetTimeStr( unsigned int hours, unsigned int minutes, double seconds )
{
	wxString s;

	s.Printf( wxT( "%02d:%02d:%012.9f" ), hours, minutes, seconds );
	FixDecimalPoint( s );
	return s;
}

void wxIndex::FixDecimalPoint( wxString& s )
{
#if wxUSE_INTL
	wxString sep = wxLocale::GetInfo( wxLOCALE_DECIMAL_POINT,
		wxLOCALE_CAT_NUMBER );
#else // !wxUSE_INTL
	// As above, this is the most common alternative value. Notice that here it
	// doesn't matter if we guess wrongly and the current separator is already
	// ".": we'll just waste a call to Replace() in this case.
	wxString sep( "," );
#endif // wxUSE_INTL/!wxUSE_INTL

	s.Replace( sep, "." );
}

wxIndex& wxIndex::operator -=( wxULongLong frames )
{
	wxASSERT( !HasCdFrames() );
	m_offset -= frames;
	return *this;
}

wxIndex& wxIndex::operator +=( wxULongLong frames )
{
	wxASSERT( !HasCdFrames() );
	m_offset += frames;
	return *this;
}

int wxIndex::CompareFn( wxIndex** i1, wxIndex** i2 )
{
	if ( ( *i1 )->GetNumber() < ( *i2 )->GetNumber() )
	{
		return -1;
	}
	else if ( ( *i1 )->GetNumber() > ( *i2 )->GetNumber() )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayIndex );