/*
 * wxUnquoter.cpp
 */

#include <wxCueFile/wxUnquoter.h>

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxUnquoter, wxStringProcessor );

// ===============================================================================

const char wxUnquoter::RE_SINGLE_QUOTES[] = "^\\p{Z}*'(.*)'\\p{Z}*$";
const char wxUnquoter::RE_DOUBLE_QUOTES[] = "^\\p{Z}*\"(.*)\"\\p{Z}*$";

// ===============================================================================

wxUnquoter::wxUnquoter( void ) :
	m_reSingleQuotes( RE_SINGLE_QUOTES ),
	m_reDoubleQuotes( RE_DOUBLE_QUOTES )
{
	wxASSERT( m_reSingleQuotes.IsValid() );
	wxASSERT( m_reDoubleQuotes.IsValid() );
}

wxStringProcessor* const wxUnquoter::Clone() const
{
	return new wxUnquoter();
}

bool wxUnquoter::Process( const wxString& qs, wxString& res ) const
{
	wxString s;

	if (m_reSingleQuotes.Matches(qs))
	{
		res = m_reSingleQuotes.GetMatch(qs, 1);
		return true;
	}

	if (m_reDoubleQuotes.Matches(qs))
	{
		res = m_reDoubleQuotes.GetMatch(qs, 1);
		return true;
	}

	return false;
}

const wxRegEx& wxUnquoter::GetReSingleQuotes() const
{
	return m_reSingleQuotes;
}

const wxRegEx& wxUnquoter::GetReDoubleQuotes() const
{
	return m_reDoubleQuotes;
}

bool wxUnquoter::IsQuoted( const wxString& s ) const
{
	return m_reSingleQuotes.Matches( s ) || m_reDoubleQuotes.Matches( s );
}

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxQuoteCorrector, wxStringProcessor);

// ===============================================================================

// DC1=11H, DC2=12H
const char wxQuoteCorrector::OPENING_QOUTATION_MARK_REPLACEMENT = '\x11';
const char wxQuoteCorrector::CLOSING_QOUTATION_MARK_REPLACEMENT = '\x12';
const char wxQuoteCorrector::GENERIC_REPLACEMENT[] = "\\1\x11\\2\x12";

// ===============================================================================

const char wxQuoteCorrector::RE_SINGLE_QUOTES[] = "([\\p{Z}\\p{P}]|^)[']((?:[^']|\\B['])*)(?!\\B)['](?=[\\p{Z}\\p{P}]|$)";
const char wxQuoteCorrector::RE_DOUBLE_QUOTES[] = "([\\p{Z}\\p{P}]|^)[\"]((?:[^\"]|\\B[\"])*)(?!\\B)[\"](?=[\\p{Z}\\p{P}]|$)";
const char wxQuoteCorrector::RE_PSEUDO_DOUUBLE_QUOTES[] = "([\\p{Z}\\p{P}]|^)[']{2}(([^']|\\B['])+)(?!\\B)[']{2}(?=[\\p{Z}\\p{P}]|$)";

// ===============================================================================

const wxQuoteCorrector::QUOTATION_MARKS wxQuoteCorrector::ASCII_QUOTES[] =
{
	{ wxS("\""), wxS("\"") },
	{ wxS("'"), wxS("'") }
};

// ===============================================================================

const wxQuoteCorrector::QUOTATION_MARKS wxQuoteCorrector::ENGLISH_QUOTES[] =
{
	{ wxS("\u201C"), wxS("\u201D") },
	{ wxS("\u2018"), wxS("\u2019") }
};

// ===============================================================================

const wxQuoteCorrector::QUOTATION_MARKS wxQuoteCorrector::POLISH_QUOTES[] =
{
	{ wxS("\u201E"), wxS("\u201D") },
	{ wxS("\u201A"), wxS("\u2019") }
};

const wxChar wxQuoteCorrector::POLISH_DOUBLE_QUOTES[] = wxS("\\1\u201E\\2\u201D");
const wxChar wxQuoteCorrector::POLISH_SINGLE_QUOTES[] = wxS("\\1\u201A\\2\u2019");

// ===============================================================================

const wxQuoteCorrector::QUOTATION_MARKS wxQuoteCorrector::GERMAN_QUOTES[] =
{
	{ wxS("\u201E"), wxS("\u201C") },
	{ wxS("\u201A"), wxS("\u2018") }
};

// ===============================================================================

const wxQuoteCorrector::QUOTATION_MARKS wxQuoteCorrector::FRENCH_QUOTES[] =
{
	{ wxS("\u00AB\u2005"), wxS("\u2005\u00BB") },
	{ wxS("\u2039\u2005"), wxS("\u2005\u203A") }
};

wxQuoteCorrector::wxQuoteCorrector(void) :
	m_sGenericReplacement(GENERIC_REPLACEMENT),
	m_reSingleQuotes(RE_SINGLE_QUOTES),
	m_reDoubleQuotes(RE_DOUBLE_QUOTES),
	m_rePseudoDoubleQuotes(RE_PSEUDO_DOUUBLE_QUOTES),
	m_replacement_method(&wxQuoteCorrector::get_standard_replacement)
{
	wxASSERT(m_reSingleQuotes.IsValid());
	wxASSERT(m_reDoubleQuotes.IsValid());
	wxASSERT(m_rePseudoDoubleQuotes.IsValid());
}

wxQuoteCorrector::wxQuoteCorrector(const wxQuoteCorrector& corrector) :
	m_sGenericReplacement(GENERIC_REPLACEMENT),
	m_reSingleQuotes(RE_SINGLE_QUOTES),
	m_reDoubleQuotes(RE_DOUBLE_QUOTES),
	m_rePseudoDoubleQuotes(RE_PSEUDO_DOUUBLE_QUOTES),
	m_replacement_method(corrector.m_replacement_method)
{
	wxASSERT(m_reSingleQuotes.IsValid());
	wxASSERT(m_reDoubleQuotes.IsValid());
	wxASSERT(m_rePseudoDoubleQuotes.IsValid());
}

wxStringProcessor* const wxQuoteCorrector::Clone() const
{
	return new wxQuoteCorrector(*this);
}

/*
 *      http://en.wikipedia.org/wiki/Quotation_mark,_non-English_usage
 */
bool wxQuoteCorrector::correct_polish_qm(const wxString& sLang)
{
	return sLang.CmpNoCase("pol") == 0;
}

bool wxQuoteCorrector::correct_english_qm(const wxString& sLang)
{
	return sLang.CmpNoCase("eng") == 0;
}

bool wxQuoteCorrector::correct_german_qm(const wxString& sLang)
{
	return
		(sLang.CmpNoCase("ger") == 0) ||
		(sLang.CmpNoCase("gem") == 0) ||
		(sLang.CmpNoCase("cze") == 0) ||
		(sLang.CmpNoCase("geo") == 0) ||
		(sLang.CmpNoCase("est") == 0) ||
		(sLang.CmpNoCase("ice") == 0) ||
		(sLang.CmpNoCase("bul") == 0) ||
		(sLang.CmpNoCase("srp") == 0) ||
		(sLang.CmpNoCase("rus") == 0)
		;
}

bool wxQuoteCorrector::correct_french_qm(const wxString& sLang)
{
	return sLang.CmpNoCase("fre") == 0;
}

void wxQuoteCorrector::SetLang(const wxString& sLang)
{
	if (correct_polish_qm(sLang)) m_replacement_method = &wxQuoteCorrector::get_polish_replacement;
	else if (correct_english_qm(sLang)) m_replacement_method = &wxQuoteCorrector::get_english_replacement;
	else if (correct_german_qm(sLang)) m_replacement_method = &wxQuoteCorrector::get_german_replacement;
	else if (correct_french_qm(sLang)) m_replacement_method = &wxQuoteCorrector::get_french_replacement;
	else m_replacement_method = &wxQuoteCorrector::get_standard_replacement;
}

bool wxQuoteCorrector::Process(const wxString& s, wxString& res) const
{
	int nRes = 0;
	wxString w(s);

	nRes += m_rePseudoDoubleQuotes.ReplaceAll(&w, m_sGenericReplacement);
	nRes += m_reSingleQuotes.ReplaceAll(&w, m_sGenericReplacement);
	nRes += m_reDoubleQuotes.ReplaceAll(&w, m_sGenericReplacement);

	if (nRes > 0)
	{
		wxString wo;
		if (InternalCorrectQuotes(w, wo))
		{
			res = wo;
		}
		else
		{
			res = w;
		}
		return true;
	}

	return false;
}

wxQuoteCorrector::wxScopedCharBuffer wxQuoteCorrector::get_standard_replacement(int nLevel, bool bOpening) const
{
	return get_replacement(ASCII_QUOTES, nLevel, bOpening);
}

wxQuoteCorrector::wxScopedCharBuffer wxQuoteCorrector::get_english_replacement(int nLevel, bool bOpening) const
{
	return get_replacement(ENGLISH_QUOTES, nLevel, bOpening);
}

wxQuoteCorrector::wxScopedCharBuffer wxQuoteCorrector::get_polish_replacement(int nLevel, bool bOpening) const
{
	return get_replacement(POLISH_QUOTES, nLevel, bOpening);
}

wxQuoteCorrector::wxScopedCharBuffer wxQuoteCorrector::get_german_replacement(int nLevel, bool bOpening) const
{
	return get_replacement(GERMAN_QUOTES, nLevel, bOpening);
}

wxQuoteCorrector::wxScopedCharBuffer wxQuoteCorrector::get_french_replacement(int nLevel, bool bOpening) const
{
	return get_replacement(FRENCH_QUOTES, nLevel, bOpening);
}

bool wxQuoteCorrector::InternalCorrectQuotes(const wxString& s, wxString& res) const
{
	int      nLevel = 0;
	wxString w;
	bool corrected = false;

	for (wxString::const_iterator i = s.begin(); i != s.end(); ++i)
	{
		if (*i == OPENING_QOUTATION_MARK_REPLACEMENT)
		{
			w << (this->*m_replacement_method)(nLevel, true);
			corrected = true;
			nLevel += 1;
		}
		else if (*i == CLOSING_QOUTATION_MARK_REPLACEMENT)
		{
			nLevel -= 1;
			corrected = true;
			w << (this->*m_replacement_method)(nLevel, false);
		}
		else
		{
			w << *i;
		}
	}

	if (corrected)
	{
		res = w;
		return true;
	}

	return false;
}

const wxRegEx& wxQuoteCorrector::GetReSingleQuotes() const
{
	return m_reSingleQuotes;
}

const wxRegEx& wxQuoteCorrector::GetReDoubleQuotes() const
{
	return m_reDoubleQuotes;
}

bool wxQuoteCorrector::IsQuoted(const wxString& s) const
{
	return m_reSingleQuotes.Matches(s) || m_reDoubleQuotes.Matches(s);
}
