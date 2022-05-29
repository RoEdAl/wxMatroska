/*
 * wxTriailingSpacesRemover.cpp
 */

#include "wxTrailingSpacesRemover.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxTrailingSpacesRemover, wxStringProcessor)

// ===============================================================================

const char wxTrailingSpacesRemover::REG_EX[] =
	"^[\\p{Z}\\p{Cc}]*([^\\p{Z}\\p{Cc}].*[^\\p{Z}\\p{Cc}])[\\p{Z}\\p{Cc}]*$";

// ===============================================================================

wxTrailingSpacesRemover::wxTrailingSpacesRemover() :
	m_reTrailingSpaces( REG_EX )
{
	wxASSERT( m_reTrailingSpaces.IsValid() );
}

wxStringProcessor* const wxTrailingSpacesRemover::Clone() const
{
	return new wxTrailingSpacesRemover();
}

const wxRegEx& wxTrailingSpacesRemover::GetRegEx() const
{
	return m_reTrailingSpaces;
}

bool wxTrailingSpacesRemover::Process( const wxString& sIn, wxString& sOut ) const
{
	if ( m_reTrailingSpaces.Matches( sIn ) )
	{
		wxASSERT( m_reTrailingSpaces.GetMatchCount() >= 1 );

		size_t start, len;
		m_reTrailingSpaces.GetMatch(&start, &len, 1);

		if (len < sIn.Length())
		{
			sOut = m_reTrailingSpaces.GetMatch(sIn, 1);
			return true;
		}
	}

	return false;
}
