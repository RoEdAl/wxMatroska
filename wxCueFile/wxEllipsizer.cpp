/*
 * wxEllipsizer.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxEllipsizer.h>

// ===============================================================================

const wxChar wxEllipsizer::REG_EX[] = wxT( "([[:space:]]*\\.\\.\\.)[[:space:]]*\\Z" );
const wxChar wxEllipsizer::ELLIPSIS = wxT( '\u2026' );

// ===============================================================================

wxEllipsizer::wxEllipsizer():
	m_reEllipsis( REG_EX, wxRE_ADVANCED )
{
	wxASSERT( m_reEllipsis.IsValid() );
}

bool wxEllipsizer::EllipsizeEx( const wxString& sIn, wxString& sOut ) const
{
	sOut = sIn;
	int nRes = m_reEllipsis.ReplaceAll( &sOut, ELLIPSIS );
	return ( nRes > 0 );
}

wxString wxEllipsizer::Ellipsize( const wxString& sIn ) const
{
	wxString sOut;

	if ( EllipsizeEx( sIn, sOut ) )
	{
		return sOut;
	}
	else
	{
		return sIn;
	}
}

