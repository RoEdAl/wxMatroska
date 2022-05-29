/*
 * wxEllipsizer.cpp
 */

#include "wxEllipsizer.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxEllipsizer, wxStringProcessor)

// ===============================================================================

const char    wxEllipsizer::REG_EX[] = "(\\p{Z}*\\.\\.\\.)\\p{Z}*$";
const wxUChar wxEllipsizer::ELLIPSIS = wxS( '\u2026' );

// ===============================================================================

wxEllipsizer::wxEllipsizer() :
	m_reEllipsis( REG_EX )
{
	wxASSERT( m_reEllipsis.IsValid() );
}

wxStringProcessor* const wxEllipsizer::Clone() const
{
	return new wxEllipsizer();
}

bool wxEllipsizer::Process( const wxString& sIn, wxString& sOut ) const
{
	sOut = sIn;
	wxString res(sIn);
	int nRes = m_reEllipsis.ReplaceAll( &res, ELLIPSIS );

	if (nRes > 0)
	{
		sOut = res;
		return true;
	}

	return false;
}
