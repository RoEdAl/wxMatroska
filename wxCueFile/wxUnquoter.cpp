/*
 * wxUnquoter.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxUnquoter.h>

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxUnquoter, wxObject );

// ===============================================================================

// DC1=11H, DC2=12H
const char wxUnquoter::OPENING_QOUTATION_MARK_REPLACEMENT = '\x11';
const char wxUnquoter::CLOSING_QOUTATION_MARK_REPLACEMENT = '\x12';
const char wxUnquoter::GENERIC_REPLACEMENT[]				= "\\1\x11\\2\x12";

// ===============================================================================

const char wxUnquoter::RE_SINGLE_QUOTES[]			= "([[:space:][:punct:]]|^)[[.apostrophe.]]((?:[^[.apostrophe.]]|\\B[[.apostrophe.]])*)(?!\\B)[[.apostrophe.]](?=[[:space:][:punct:]]|$)" ;
const char wxUnquoter::RE_SINGLE_QUOTES_EX[]		= "([[:space:][:punct:]]|^)[[.apostrophe.]]((?:[^[.apostrophe.]\\u201E\\\u201D]|\\B[[.apostrophe.]])*)(?!\\B)[[.apostrophe.]](?=[[:space:][:punct:]]|$)";
const char wxUnquoter::RE_DOUBLE_QUOTES[]			= "([[:space:][:punct:]]|^)[[.quotation-mark.]]((?:[^[.quotation-mark.]]|\\B[[.quotation-mark.]])*)(?!\\B)[[.quotation-mark.]](?=[[:space:][:punct:]]|$)";
const char wxUnquoter::RE_PSEUDO_DOUUBLE_QUOTES[]   = "([[:space:][:punct:]]|^)[[.apostrophe.]]{2}(([^[.apostrophe.]]|\\B[[.apostrophe.]])+)(?!\\B)[[.apostrophe.]]{2}(?=[[:space:][:punct:]]|$)";

const char wxUnquoter::RE_FULL_SINGLE_QUOTES[] = "\\A[[:space:]]*\\'(([^\\']|\\\')*)\\'[[:space:]]*\\Z";
const char wxUnquoter::RE_FULL_DOUBLE_QUOTES[] = "\\A[[:space:]]*\\\"(([^\\\"]|\\\\\")*)\\\"[[:space:]]*\\Z";

// ===============================================================================

const wxUnquoter::QUOTATION_MARKS wxUnquoter::ASCII_QUOTES[] =
{
	{ wxS("\""), wxS("\"") },
	{ wxS("'"), wxS("'") }
};

const size_t wxUnquoter::ASCII_QUOTES_SIZE = WXSIZEOF( wxUnquoter::ASCII_QUOTES );

// ===============================================================================

const wxUnquoter::QUOTATION_MARKS wxUnquoter::ENGLISH_QUOTES[] =
{
	{ wxS( "\u201C" ), wxS( "\u201D" ) },
	{ wxS( "\u2018" ), wxS( "\u2019" ) }
};

const size_t wxUnquoter::ENGLISH_QUOTES_SIZE = WXSIZEOF( wxUnquoter::ENGLISH_QUOTES );

// ===============================================================================

const wxUnquoter::QUOTATION_MARKS wxUnquoter::POLISH_QUOTES[] =
{
	{ wxS( "\u201E" ), wxS( "\u201D" ) },
	{ wxS( "\u201A" ), wxS( "\u2019" ) }
};

const size_t wxUnquoter::POLISH_QUOTES_SIZE = WXSIZEOF( wxUnquoter::POLISH_QUOTES );

const wxChar wxUnquoter::POLISH_DOUBLE_QUOTES[] = wxS( "\\1\u201E\\2\u201D" );
const wxChar wxUnquoter::POLISH_SINGLE_QUOTES[] = wxS( "\\1\u201A\\2\u2019" );

// ===============================================================================

const wxUnquoter::QUOTATION_MARKS wxUnquoter::GERMAN_QUOTES[] =
{
	{ wxS( "\u201E" ), wxS( "\u201C" ) },
	{ wxS( "\u201A" ), wxS( "\u2018" ) }
};

const size_t wxUnquoter::GERMAN_QUOTES_SIZE = WXSIZEOF( wxUnquoter::GERMAN_QUOTES );

// ===============================================================================

const wxUnquoter::QUOTATION_MARKS wxUnquoter::FRENCH_QUOTES[] =
{
	{ wxS( "\u00AB\u2005" ), wxS( "\u2005\u00BB" ) },
	{ wxS( "\u2039\u2005" ), wxS( "\u2005\u203A" ) }
};

const size_t wxUnquoter::FRENCH_QUOTES_SIZE = WXSIZEOF( wxUnquoter::FRENCH_QUOTES );

// ===============================================================================

wxUnquoter::wxUnquoter( void ):
	m_sGenericReplacement( GENERIC_REPLACEMENT ),
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

	m_replacement_method = &wxUnquoter::get_standard_replacement;
}

/*
 *      http://en.wikipedia.org/wiki/Quotation_mark,_non-English_usage
 */
bool wxUnquoter::correct_polish_qm( const wxString& sLang )
{
	return sLang.CmpNoCase( "pol" ) == 0;
}

bool wxUnquoter::correct_english_qm( const wxString& sLang )
{
	return sLang.CmpNoCase( "eng" ) == 0;
}

bool wxUnquoter::correct_german_qm( const wxString& sLang )
{
	return
		( sLang.CmpNoCase( "ger" ) == 0 ) ||
		( sLang.CmpNoCase( "gem" ) == 0 ) ||
		( sLang.CmpNoCase( "cze" ) == 0 ) ||
		( sLang.CmpNoCase( "geo" ) == 0 ) ||
		( sLang.CmpNoCase( "est" ) == 0 ) ||
		( sLang.CmpNoCase( "ice" ) == 0 ) ||
		( sLang.CmpNoCase( "bul" ) == 0 ) ||
		( sLang.CmpNoCase( "srp" ) == 0 ) ||
		( sLang.CmpNoCase( "rus" ) == 0 )
	;
}

bool wxUnquoter::correct_french_qm( const wxString& sLang )
{
	return sLang.CmpNoCase( "fre" ) == 0;
}

void wxUnquoter::SetLang( const wxString& sLang )
{
	if ( correct_polish_qm( sLang ) )
	{
		m_replacement_method = &wxUnquoter::get_polish_replacement;
	}
	else if ( correct_english_qm( sLang ) )
	{
		m_replacement_method = &wxUnquoter::get_english_replacement;
	}
	else if ( correct_german_qm( sLang ) )
	{
		m_replacement_method = &wxUnquoter::get_german_replacement;
	}
	else if ( correct_french_qm( sLang ) )
	{
		m_replacement_method = &wxUnquoter::get_french_replacement;
	}
	else
	{
		m_replacement_method = &wxUnquoter::get_standard_replacement;
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

int wxUnquoter::CorrectQuotes( wxString& s ) const
{
	int nRes = 0;

	nRes += m_rePseudoDoubleQuotes.ReplaceAll( &s, m_sGenericReplacement );
	nRes += m_reSingleQuotes.ReplaceAll( &s, m_sGenericReplacement );
	nRes += m_reDoubleQuotes.ReplaceAll( &s, m_sGenericReplacement );

	if ( nRes > 0 )
	{
		InternalCorrectQuotes( s );
	}

	return nRes;
}

wxString wxUnquoter::UnquoteAndCorrect( const wxString& qs ) const
{
	wxString s( Unquote( qs ) );

	CorrectQuotes( s );
	return s;
}

wxUnquoter::wxScopedCharBuffer wxUnquoter::get_replacement( const wxUnquoter::QUOTATION_MARKS* quotation_marks_array, size_t nSize, int nLevel, bool bOpening )
{
	if ( nLevel < 0 )
	{
		return wxScopedCharBuffer();
	}

	if ( bOpening )
	{
		return wxScopedCharBuffer::CreateNonOwned( quotation_marks_array[ nLevel % nSize ].opening );
	}
	else
	{
		return wxScopedCharBuffer::CreateNonOwned( quotation_marks_array[ nLevel % nSize ].closing );
	}
}

wxUnquoter::wxScopedCharBuffer wxUnquoter::get_standard_replacement( int nLevel, bool bOpening ) const
{
	return get_replacement( ASCII_QUOTES, ASCII_QUOTES_SIZE, nLevel, bOpening );
}

wxUnquoter::wxScopedCharBuffer wxUnquoter::get_english_replacement( int nLevel, bool bOpening ) const
{
	return get_replacement( ENGLISH_QUOTES, ENGLISH_QUOTES_SIZE, nLevel, bOpening );
}

wxUnquoter::wxScopedCharBuffer wxUnquoter::get_polish_replacement( int nLevel, bool bOpening ) const
{
	return get_replacement( POLISH_QUOTES, POLISH_QUOTES_SIZE, nLevel, bOpening );
}

wxUnquoter::wxScopedCharBuffer wxUnquoter::get_german_replacement( int nLevel, bool bOpening ) const
{
	return get_replacement( GERMAN_QUOTES, GERMAN_QUOTES_SIZE, nLevel, bOpening );
}

wxUnquoter::wxScopedCharBuffer wxUnquoter::get_french_replacement( int nLevel, bool bOpening ) const
{
	return get_replacement( FRENCH_QUOTES, FRENCH_QUOTES_SIZE, nLevel, bOpening );
}

void wxUnquoter::InternalCorrectQuotes( wxString& s ) const
{
	int		 nLevel = 0;
	wxString res;

	for ( wxString::const_iterator i = s.begin(); i != s.end(); ++i )
	{
		if ( *i == OPENING_QOUTATION_MARK_REPLACEMENT )
		{
			res << ( this->*m_replacement_method )( nLevel, true );
			nLevel += 1;
		}
		else if ( *i == CLOSING_QOUTATION_MARK_REPLACEMENT )
		{
			nLevel -= 1;
			res << ( this->*m_replacement_method )( nLevel, false );
		}
		else
		{
			res << *i;
		}
	}

	wxASSERT( nLevel == 0 );
	s = res;
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

