/*
 * wxDashesCorrector.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxDashesCorrector.h>

// ===============================================================================

const char wxDashesCorrector::REG_EX_EN[] = "[[:space:]]{1,}[[.hyphen-minus.][.-.]\\u2012\\u2013\\u2014\\u2015][[:space:]]{1,}";
const char wxDashesCorrector::REG_EX_EM[] = "[[:space:]]{1,}[[.hyphen-minus.][.-.]\\u2012\\u2013\\u2014\\u2015]{2,}[[:space:]]{1,}";

// ===============================================================================

wxString wxDashesCorrector::GetReplacement( uvalue_type nUniSpace, uvalue_type nUniDash )
{
    wxString sResult;

    wxUniChar uSpace( nUniSpace );
    wxUniChar uDash( nUniDash );

    sResult << uSpace << uDash << uSpace;

    return sResult;
}

// en dash - U+2013, en dash - U+2014, thin space - U+2009
wxDashesCorrector::wxDashesCorrector( ) :
    m_reEn( REG_EX_EN, wxRE_ADVANCED ), m_reEm( REG_EX_EM, wxRE_ADVANCED ),
    m_sEnReplacement( GetReplacement( 0x2009, 0x2013 ) ), m_sEmReplacement( GetReplacement( 0x2009, 0x2014 ) )
{
    wxASSERT( m_reEn.IsValid( ) );
    wxASSERT( m_reEm.IsValid( ) );
}

wxDashesCorrector::wxDashesCorrector( uvalue_type nUniSpace, uvalue_type nUniEnDash, uvalue_type nUniEmDash ) :
    m_reEn( REG_EX_EN, wxRE_ADVANCED ), m_reEm( REG_EX_EM, wxRE_ADVANCED ),
    m_sEnReplacement( GetReplacement( nUniSpace, nUniEnDash ) ), m_sEmReplacement( GetReplacement( nUniSpace, nUniEmDash ) )
{
    wxASSERT( m_reEn.IsValid( ) );
    wxASSERT( m_reEm.IsValid( ) );
}

const wxRegEx& wxDashesCorrector::GetEnRegEx( ) const
{
	return m_reEn;
}

const wxRegEx& wxDashesCorrector::GetEmRegEx( ) const
{
    return m_reEm;
}

void wxDashesCorrector::ReplaceEx( const wxString& sIn, wxString& sOut ) const
{
	sOut = sIn;
    Replace( sOut );
}

void wxDashesCorrector::Replace( wxString& s ) const
{
	m_reEn.ReplaceAll( &s, m_sEnReplacement );
    m_reEm.ReplaceAll( &s, m_sEmReplacement );
}


