/*
	wxUnquoter.cpp
*/

#include "StdWx.h"
#include "wxUnquoter.h"

IMPLEMENT_DYNAMIC_CLASS( wxUnquoter, wxObject )

const wxChar wxUnquoter::ENGLISH_DOUBLE_QUOTES[] = wxT("\u201C\\1\u201D");
const wxChar wxUnquoter::ENGLISH_SINGLE_QUOTES[] = wxT("\u2018\\1\u2019");

const wxChar wxUnquoter::POLISH_DOUBLE_QUOTES[] = wxT("\u201E\\1\u201D");
const wxChar wxUnquoter::POLISH_SINGLE_QUOTES[] = wxT("\u201A\\1\u2019");

const wxChar wxUnquoter::GERMAN_DOUBLE_QUOTES[] = wxT("\u201E\\1\u201C");
const wxChar wxUnquoter::GERMAN_SINGLE_QUOTES[] = wxT("\u201A\\1\u2018");

const wxChar wxUnquoter::FRENCH_DOUBLE_QUOTES[] = wxT("\u00AB\u2005\\1\u2005\u00BB");
const wxChar wxUnquoter::FRENCH_SINGLE_QUOTES[] = wxT("\u2039\u2005\\1\u2005\u203A");


wxUnquoter::wxUnquoter(void)
	:m_reQuotes( wxT("\\'(([^\\']|\\\')*)\\'(?![[:alnum:]])"), wxRE_ADVANCED ),
	 m_reDoubleQuotes( wxT("\\\"(([^\\\"]|\\\\\")*)\\\""), wxRE_ADVANCED )
{
	wxASSERT( m_reQuotes.IsValid() );
	wxASSERT( m_reDoubleQuotes.IsValid() );
}

wxUnquoter::~wxUnquoter(void)
{
}

/*
	http://en.wikipedia.org/wiki/Quotation_mark,_non-English_usage
*/

static bool correct_english_qm( const wxString& sLang )
{
	return sLang.CmpNoCase( wxT("eng") ) == 0;
}

static bool correct_german_qm( const wxString& sLang )
{
	return 
		(sLang.CmpNoCase( wxT("ger") ) == 0) ||
		(sLang.CmpNoCase( wxT("gem") ) == 0) ||
		(sLang.CmpNoCase( wxT("cze") ) == 0) ||
		(sLang.CmpNoCase( wxT("geo") ) == 0) ||
		(sLang.CmpNoCase( wxT("est") ) == 0) ||
		(sLang.CmpNoCase( wxT("ice") ) == 0) ||
		(sLang.CmpNoCase( wxT("bul") ) == 0) ||
		(sLang.CmpNoCase( wxT("srp") ) == 0) ||
		(sLang.CmpNoCase( wxT("rus") ) == 0)
	;
}

static bool correct_french_qm( const wxString& sLang )
{
	return sLang.CmpNoCase( wxT("fre") ) == 0;
}

void wxUnquoter::SetLang(const wxString& sLang)
{
	if ( sLang.CmpNoCase( wxT("pol") ) == 0 )
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
		wxLogDebug( wxT("Converting quotation marks in language %s is not supported."), sLang );
		m_sSingleQuotes = m_sDoubleQuotes = wxEmptyString;
	}
}

wxString wxUnquoter::Unquote( const wxString& qs ) const
{
	wxString s;
	if ( m_reQuotes.Matches( qs ) )
	{
		s = m_reQuotes.GetMatch( qs, 1 );
		s.Replace( wxT("\\'"), wxT("'") );
	}
	else if ( m_reDoubleQuotes.Matches( qs ) )
	{
		s = m_reDoubleQuotes.GetMatch( qs, 1 );
		s.Replace( wxT("\\\""), wxT("\"") );
	}
	else
	{
		//::wxLogDebug( wxT("Character data not quoted") );
		s = qs;
	}

	return s;
}

wxString wxUnquoter::UnquoteAndCorrect( const wxString& qs ) const
{
	wxString s( Unquote(qs) );
	if ( !m_sSingleQuotes.IsEmpty() && !m_sDoubleQuotes.IsEmpty() )
	{
		m_reQuotes.ReplaceAll( &s, m_sSingleQuotes );
		m_reDoubleQuotes.ReplaceAll( &s, m_sDoubleQuotes );
	}

	return s;
}

const wxRegEx& wxUnquoter::GetReSingleQuotes() const
{
	return m_reQuotes;
}

const wxRegEx& wxUnquoter::GetReDoubleQuotes() const
{
	return m_reDoubleQuotes;
}