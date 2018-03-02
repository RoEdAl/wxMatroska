/*
 * wxReduntantSpacesRemover.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxReduntantSpacesRemover.h>

// ===============================================================================

const char wxReduntantSpacesRemover::REG_EX[] = "[[:space:]]{2,}";

// ===============================================================================

wxReduntantSpacesRemover::wxReduntantSpacesRemover() :
	m_re( REG_EX, wxRE_ADVANCED ),
	m_cReplacement( 0x20 )
{
	wxASSERT( m_re.IsValid() );
}

const wxRegEx& wxReduntantSpacesRemover::GetRegEx() const
{
	return m_re;
}

int wxReduntantSpacesRemover::RemoveEx( const wxString& sIn, wxString& sOut ) const
{
	sOut = sIn;
	return m_re.ReplaceAll( &sOut, m_cReplacement );
}

int wxReduntantSpacesRemover::Remove( wxString& s ) const
{
	return m_re.ReplaceAll( &s, m_cReplacement );
}

