/*
 * wxUnquoter.cpp
 */

#include <wxCueFile/wxUnquoter.h>
 
namespace
{
    constexpr char RE_SINGLE_QUOTES[] = "^\\p{Xps}*'(.*)'\\p{Xps}*$";
    constexpr char RE_DOUBLE_QUOTES[] = "^\\p{Xps}*\"(.*)\"\\p{Xps}*$";
}

wxUnquoter::wxUnquoter(void):
    m_reSingleQuotes(RE_SINGLE_QUOTES),
    m_reDoubleQuotes(RE_DOUBLE_QUOTES)
{
    wxASSERT(m_reSingleQuotes.IsValid());
    wxASSERT(m_reDoubleQuotes.IsValid());
}

wxStringProcessor* const wxUnquoter::Clone() const
{
    return new wxUnquoter();
}

bool wxUnquoter::Process(const wxString& qs, wxString& res) const
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

bool wxUnquoter::IsQuoted(const wxString& s) const
{
    return m_reSingleQuotes.Matches(s) || m_reDoubleQuotes.Matches(s);
}

namespace
{

    // DC1=11H, DC2=12H
    constexpr char OPENING_QOUTATION_MARK_REPLACEMENT = '\x11';
    constexpr char CLOSING_QOUTATION_MARK_REPLACEMENT = '\x12';
    constexpr char GENERIC_REPLACEMENT[] = "\\1\x11\\2\x12";

    // ===============================================================================

    constexpr char REQ_SINGLE_QUOTES[] = "([\\p{Xps}\\p{Xps}]|^)[']((?:[^']|\\B['])*)(?!\\B)['](?=[\\p{Xps}\\p{P}]|$)";
    constexpr char REQ_DOUBLE_QUOTES[] = "([\\p{Xps}\\p{P}]|^)[\"]((?:[^\"]|\\B[\"])*)(?!\\B)[\"](?=[\\p{Xps}\\p{P}]|$)";
    constexpr char REQ_PSEUDO_DOUUBLE_QUOTES[] = "([\\p{Xps}\\p{P}]|^)[']{2}(([^']|\\B['])+)(?!\\B)[']{2}(?=[\\p{Xps}\\p{P}]|$)";

    // ===============================================================================

    constexpr wxQuoteCorrector::QUOTATION_MARKS ASCII_QUOTES[] =
    {
        { wxS("\""), wxS("\"") },
        { wxS("'"), wxS("'") }
    };

    // ===============================================================================

    constexpr wxQuoteCorrector::QUOTATION_MARKS ENGLISH_QUOTES[] =
    {
        { wxS("\u201C"), wxS("\u201D") },
        { wxS("\u2018"), wxS("\u2019") }
    };

    // ===============================================================================

    constexpr wxQuoteCorrector::QUOTATION_MARKS POLISH_QUOTES[] =
    {
        { wxS("\u201E"), wxS("\u201D") },
        { wxS("\u201A"), wxS("\u2019") }
    };

    constexpr wxChar POLISH_DOUBLE_QUOTES[] = wxS("\\1\u201E\\2\u201D");
    constexpr wxChar POLISH_SINGLE_QUOTES[] = wxS("\\1\u201A\\2\u2019");

    // ===============================================================================

    constexpr wxQuoteCorrector::QUOTATION_MARKS GERMAN_QUOTES[] =
    {
        { wxS("\u201E"), wxS("\u201C") },
        { wxS("\u201A"), wxS("\u2018") }
    };

    // ===============================================================================

    constexpr wxQuoteCorrector::QUOTATION_MARKS FRENCH_QUOTES[] =
    {
        { wxS("\u00AB\u2005"), wxS("\u2005\u00BB") },
        { wxS("\u2039\u2005"), wxS("\u2005\u203A") }
    };
}

wxQuoteCorrector::wxQuoteCorrector(void):
    m_genericReplacement(GENERIC_REPLACEMENT),
    m_reSingleQuotes(REQ_SINGLE_QUOTES),
    m_reDoubleQuotes(REQ_DOUBLE_QUOTES),
    m_rePseudoDoubleQuotes(REQ_PSEUDO_DOUUBLE_QUOTES),
    m_replacement_method(&wxQuoteCorrector::get_standard_replacement)
{
    wxASSERT(m_reSingleQuotes.IsValid());
    wxASSERT(m_reDoubleQuotes.IsValid());
    wxASSERT(m_rePseudoDoubleQuotes.IsValid());
}

wxQuoteCorrector::wxQuoteCorrector(const wxQuoteCorrector& corrector):
    m_genericReplacement(GENERIC_REPLACEMENT),
    m_reSingleQuotes(REQ_SINGLE_QUOTES),
    m_reDoubleQuotes(REQ_DOUBLE_QUOTES),
    m_rePseudoDoubleQuotes(REQ_PSEUDO_DOUUBLE_QUOTES),
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
    int      nRes = 0;
    wxString w(s);

    nRes += m_rePseudoDoubleQuotes.ReplaceAll(&w, m_genericReplacement);
    nRes += m_reSingleQuotes.ReplaceAll(&w, m_genericReplacement);
    nRes += m_reDoubleQuotes.ReplaceAll(&w, m_genericReplacement);

    if (nRes > 0)
    {
        wxString wo;

        if (InternalCorrectQuotes(w, wo)) res = wo;
        else res = w;
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
    bool     corrected = false;

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

bool wxQuoteCorrector::IsQuoted(const wxString& s) const
{
    return m_reSingleQuotes.Matches(s) || m_reDoubleQuotes.Matches(s);
}

