/*
 * wxTriailingSpacesRemover.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxTrailingSpacesRemover.h>

// ===============================================================================

const char wxTrailingSpacesRemover::REG_EX[] =
	"\\A[[:space:][.newline.][.carriage-return.]]*([^[:space:][.newline.][.carriage-return.]].*[^[:space:][.newline.][.carriage-return.]])[[:space:][.newline.][.carriage-return.]]*\\Z";

// ===============================================================================

wxTrailingSpacesRemover::wxTrailingSpacesRemover() :
	m_reTrailingSpaces( REG_EX, wxRE_ADVANCED )
{
	wxASSERT( m_reTrailingSpaces.IsValid() );
}

const wxRegEx& wxTrailingSpacesRemover::GetRegEx() const
{
	return m_reTrailingSpaces;
}

bool wxTrailingSpacesRemover::RemoveEx( const wxString& sIn, wxString& sOut ) const
{
	if ( m_reTrailingSpaces.Matches( sIn ) )
	{
		wxASSERT( m_reTrailingSpaces.GetMatchCount() >= 1 );
		sOut = m_reTrailingSpaces.GetMatch( sIn, 1 );
		return true;
	}
	else
	{
		sOut = sIn;
		return false;
	}
}

wxString wxTrailingSpacesRemover::Remove( const wxString& s ) const
{
	wxString sOut;

	RemoveEx( s, sOut );
	return sOut;
}

