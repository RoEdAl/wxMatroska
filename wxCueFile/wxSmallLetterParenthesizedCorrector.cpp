/*
 * wxSmallLetterParenthesizedCorrector.cpp
 */

#include "wxSmallLetterParenthesizedCorrector.h"

wxIMPLEMENT_DYNAMIC_CLASS( wxSmallLetterParenthesizedCorrector, wxStringProcessor )

// ===============================================================================

const char wxSmallLetterParenthesizedCorrector::REG_EX[] = "\\([a-z]\\)";

// ===============================================================================

wxSmallLetterParenthesizedCorrector::wxSmallLetterParenthesizedCorrector() :
	m_re( REG_EX )
{
	wxASSERT( m_re.IsValid() );
}

wxStringProcessor* const wxSmallLetterParenthesizedCorrector::Clone() const
{
	return new wxSmallLetterParenthesizedCorrector();
}

bool wxSmallLetterParenthesizedCorrector::Process( const wxString& sIn, wxString& sOut ) const
{
	sOut = wxEmptyString;

	wxString        w( sIn );
	const wxUniChar smallA( 'a' );
	bool            replaced = false;

	while ( m_re.Matches( w ) )
	{
		size_t idx, len;

		if ( !m_re.GetMatch( &idx, &len ) )
		{
			replaced = false;
			break;
		}

		sOut += w.Mid( 0, idx );

		const wxUniChar letter = w[ idx + 1 ];
		const int       sh     = (int)letter - (int)smallA;
		const wxUniChar parenthesized( 0x249C + sh );
		sOut    += parenthesized;
		replaced = true;

		w.Remove( 0, idx + len );
	}

	if ( replaced )
	{
		sOut += w;
		return true;
	}

	return false;
}

