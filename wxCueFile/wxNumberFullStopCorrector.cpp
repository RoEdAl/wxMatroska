/*
 * wxReduntantSpacesRemover.cpp
 */

#include "wxNumberFullStopCorrector.h"

wxIMPLEMENT_DYNAMIC_CLASS( wxNumberFullStopCorrector, wxStringProcessor )

// ===============================================================================

const char wxNumberFullStopCorrector::REG_EX[] = "\\b(?=\\d)(\\d{1,2}\\.\\p{Z}+)";

// ===============================================================================

wxNumberFullStopCorrector::wxNumberFullStopCorrector() :
	m_re( REG_EX )
{
	wxASSERT( m_re.IsValid() );
}

wxStringProcessor* const wxNumberFullStopCorrector::Clone() const
{
	return new wxNumberFullStopCorrector();
}

namespace
{
	bool get_num( const wxString& num, unsigned int& n )
	{
		const int pos = num.First( '.' );

		if ( !num.Left( pos ).ToUInt( &n ) ) return false;

		if ( n < 1 || n > 20 ) return false;

		return true;
	}
}

bool wxNumberFullStopCorrector::Process( const wxString& sIn, wxString& sOut ) const
{
	sOut = wxEmptyString;

	wxString        w( sIn );
	const wxUniChar thinSpace( 0x2009 );
	bool            replaced = false;

	while ( m_re.Matches( w ) )
	{
		size_t idx, len;

		if ( !m_re.GetMatch( &idx, &len, 1 ) )
		{
			replaced = false;
			break;
		}

		sOut += w.Mid( 0, idx );

		const wxString num = w.Mid( idx, len );
		unsigned int   n;

		if ( get_num( num, n ) )
		{
			const wxUniChar c( 0x2487 + n );
			sOut    += c;
			sOut    += thinSpace;
			replaced = true;
		}
		else
		{
			sOut += num;
		}

		w.Remove( 0, idx + len );
	}

	if ( replaced )
	{
		sOut += w;
		return true;
	}

	return false;
}

