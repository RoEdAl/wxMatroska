/*
   wxUnquoter.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxUnquoter.h>

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxUnquoter, wxObject );

// ===============================================================================

const wxChar wxUnquoter::RE_SINGLE_QUOTES[] = wxT( "[[.apostrophe.]](([^[.apostrophe.]]|\\B[[.apostrophe.]])*)[[.apostrophe.]](?![[:alnum:]])" );
const wxChar wxUnquoter::RE_DOUBLE_QUOTES[] = wxT( "[[.quotation-mark.]](([^[.quotation-mark.]]|\\B[[.quotation-mark.]])*)[[.quotation-mark.]]" );
const wxChar wxUnquoter::RE_PSEUDO_DOUUBLE_QUOTES[] = wxT("\\'\\'(([^\\']|(\\\'){1})+)\\'\\'");

const wxChar wxUnquoter::RE_FULL_SINGLE_QUOTES[] = wxT( "\\A[[:space:]]*\\'(([^\\']|\\\')*)\\'[[:space:]]*\\Z" );
const wxChar wxUnquoter::RE_FULL_DOUBLE_QUOTES[] = wxT( "\\A[[:space:]]*\\\"(([^\\\"]|\\\\\")*)\\\"[[:space:]]*\\Z" );

const wxChar wxUnquoter::ENGLISH_DOUBLE_QUOTES[] = wxT( "\u201C\\1\u201D" );
const wxChar wxUnquoter::ENGLISH_SINGLE_QUOTES[] = wxT( "\u2018\\1\u2019" );

const wxChar wxUnquoter::POLISH_DOUBLE_QUOTES[] = wxT( "\u201E\\1\u201D" );
const wxChar wxUnquoter::POLISH_SINGLE_QUOTES[] = wxT( "\u201A\\1\u2019" );

const wxChar wxUnquoter::GERMAN_DOUBLE_QUOTES[] = wxT( "\u201E\\1\u201C" );
const wxChar wxUnquoter::GERMAN_SINGLE_QUOTES[] = wxT( "\u201A\\1\u2018" );

const wxChar wxUnquoter::FRENCH_DOUBLE_QUOTES[] = wxT( "\u00AB\u2005\\1\u2005\u00BB" );
const wxChar wxUnquoter::FRENCH_SINGLE_QUOTES[] = wxT( "\u2039\u2005\\1\u2005\u203A" );

// ===============================================================================

wxUnquoter::wxUnquoter( void ):
	m_reSingleQuotes( RE_SINGLE_QUOTES, wxRE_ADVANCED ),
	m_reDoubleQuotes( RE_DOUBLE_QUOTES, wxRE_ADVANCED ),
	m_rePseudoDoubleQuotes( RE_PSEUDO_DOUUBLE_QUOTES, wxRE_ADVANCED ),
	m_reFullQuotes( RE_FULL_SINGLE_QUOTES, wxRE_ADVANCED ),
	m_reFullDoubleQuotes( RE_FULL_DOUBLE_QUOTES, wxRE_ADVANCED )
{
	wxASSERT( m_reSingleQuotes.IsValid() );
	wxASSERT( m_reDoubleQuotes.IsValid() );
	wxASSERT( m_rePseudoDoubleQuotes.IsValid() );

	wxASSERT( m_reFullQuotes.IsValid() );
	wxASSERT( m_reFullDoubleQuotes.IsValid() );
}

/*
        http://en.wikipedia.org/wiki/Quotation_mark,_non-English_usage
 */
bool wxUnquoter::correct_polish_qm( const wxString& sLang )
{
	return sLang.CmpNoCase( wxT( "pol" ) ) == 0;
}

bool wxUnquoter::correct_english_qm( const wxString& sLang )
{
	return sLang.CmpNoCase( wxT( "eng" ) ) == 0;
}

bool wxUnquoter::correct_german_qm( const wxString& sLang )
{
	return
		( sLang.CmpNoCase( wxT( "ger" ) ) == 0 ) ||
		( sLang.CmpNoCase( wxT( "gem" ) ) == 0 ) ||
		( sLang.CmpNoCase( wxT( "cze" ) ) == 0 ) ||
		( sLang.CmpNoCase( wxT( "geo" ) ) == 0 ) ||
		( sLang.CmpNoCase( wxT( "est" ) ) == 0 ) ||
		( sLang.CmpNoCase( wxT( "ice" ) ) == 0 ) ||
		( sLang.CmpNoCase( wxT( "bul" ) ) == 0 ) ||
		( sLang.CmpNoCase( wxT( "srp" ) ) == 0 ) ||
		( sLang.CmpNoCase( wxT( "rus" ) ) == 0 )
	;
}

bool wxUnquoter::correct_french_qm( const wxString& sLang )
{
	return sLang.CmpNoCase( wxT( "fre" ) ) == 0;
}

void wxUnquoter::SetLang( const wxString& sLang )
{
	if ( correct_polish_qm( sLang ) )
	{
		m_sSingleQuotes = POLISH_SINGLE_QUOTES;
		m_sDoubleQuotes = POLISH_DOUBLE_QUOTES;
	}
	else if ( correct_english_qm( sLang ) )
	{
		m_sSingleQuotes = ENGLISH_SINGLE_QUOTES;
		m_sDoubleQuotes = ENGLISH_DOUBLE_QUOTES;
	}
	else if ( correct_german_qm( sLang ) )
	{
		m_sSingleQuotes = GERMAN_SINGLE_QUOTES;
		m_sDoubleQuotes = GERMAN_DOUBLE_QUOTES;
	}
	else if ( correct_french_qm( sLang ) )
	{
		m_sSingleQuotes = FRENCH_SINGLE_QUOTES;
		m_sDoubleQuotes = FRENCH_DOUBLE_QUOTES;
	}
	else
	{
		wxLogDebug( wxT( "Converting quotation marks in language %s is not supported." ), sLang );
		m_sSingleQuotes = m_sDoubleQuotes = wxEmptyString;
	}
}

wxString wxUnquoter::Unquote( const wxString& qs ) const
{
	wxString s;

	if ( m_reFullQuotes.Matches( qs ) )
	{
		s = m_reFullQuotes.GetMatch( qs, 1 );
	}
	else if ( m_reFullDoubleQuotes.Matches( qs ) )
	{
		s = m_reFullDoubleQuotes.GetMatch( qs, 1 );
	}
	else
	{
		s = qs;
	}

	return s;
}

wxString wxUnquoter::UnquoteAndCorrect( const wxString& qs ) const
{
	wxString s( Unquote( qs ) );

	if ( !m_sDoubleQuotes.IsEmpty() )
	{
		m_rePseudoDoubleQuotes.ReplaceAll( &s, m_sDoubleQuotes );
	}

	if ( !m_sSingleQuotes.IsEmpty() )
	{
		m_reSingleQuotes.ReplaceAll( &s, m_sSingleQuotes );
	}

	if ( !m_sDoubleQuotes.IsEmpty() )
	{
		m_reDoubleQuotes.ReplaceAll( &s, m_sDoubleQuotes );
	}

	return s;
}

const wxRegEx& wxUnquoter::GetReSingleQuotes() const
{
	return m_reSingleQuotes;
}

const wxRegEx& wxUnquoter::GetReDoubleQuotes() const
{
	return m_reDoubleQuotes;
}

const wxRegEx& wxUnquoter::GetReFullSingleQuotes() const
{
	return m_reFullQuotes;
}

const wxRegEx& wxUnquoter::GetReFullDoubleQuotes() const
{
	return m_reFullDoubleQuotes;
}

bool wxUnquoter::IsQuoted( const wxString& s ) const
{
	return m_reSingleQuotes.Matches( s ) || m_reDoubleQuotes.Matches( s );
}
