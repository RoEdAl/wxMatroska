/*
 * wxReduntantSpacesRemover.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxReduntantSpacesRemover.h>

// ===============================================================================

const char wxReduntantSpacesRemover::REG_EX[] = "[[:space:]]{2,}";

// ===============================================================================

wxReduntantSpacesRemover::wxReduntantSpacesRemover():
	m_reReduntantSpaces( REG_EX, wxRE_ADVANCED ),
    m_cReplacement( 0x20 ), m_cLongReplacement( 0x2003 )
{
	wxASSERT( m_reReduntantSpaces.IsValid() );
}

const wxRegEx& wxReduntantSpacesRemover::GetRegEx() const
{
	return m_reReduntantSpaces;
}

int wxReduntantSpacesRemover::RemoveEx( const wxString& sIn, wxString& sOut ) const
{
	sOut = sIn;
    return m_reReduntantSpaces.ReplaceAll( &sOut, m_cReplacement );
}

int wxReduntantSpacesRemover::Remove( wxString& s ) const
{
    return m_reReduntantSpaces.ReplaceAll( &s, m_cReplacement );
}

