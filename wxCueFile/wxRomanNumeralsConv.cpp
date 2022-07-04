/*
 * wxRomanNumeralsConv.cpp
 */

#include "wxRomanNumeralsConv.h"

 // ===============================================================================

const char roman_numeral_traits< true >::REGEX_WEAK[] = "\\b(?=\\w)(X{0,3})(I[XV]|V?I{0,3})\\b(?<=\\w)";
const char roman_numeral_traits< true >::REGEX_STRONG[] = "\\b(?=\\w)(M*(C[MD]|D?C{0,3}))(X[CL]|L?X{0,3})(I[XV]|V?I{0,3})\\b(?<=\\w)";

const roman_utils::roman_numeral_conv roman_numeral_traits< true >::CINFO = {
    {
        { "I", wxS('\u2160') },
        { "II", wxS('\u2161') },
        { "III", wxS('\u2162') },
        { "IV", wxS('\u2163') },
        { "V", wxS('\u2164') },
        { "VI", wxS('\u2165') },
        { "VII", wxS('\u2166') },
        { "VIII", wxS('\u2167') },
        { "IX", wxS('\u2168') },
        { "X", wxS('\u2169') },
        { "XI", wxS('\u216A') },
        { "XII", wxS('\u216B') },
    },
    { "CMD", wxS("\u216D\u216F\u216E") },
    { "XCL", wxS("\u2169\u216D\u216C") },
    { "IXV", wxS("\u2160\u2169\u2164") }
};

// ===============================================================================

const char roman_numeral_traits< false >::REGEX_WEAK[] = "\\b(?=\\w)(x{0,3})(i[xv]|v?I{0,3})\\b(?<=\\w)";
const char roman_numeral_traits< false >::REGEX_STRONG[] = "\\b(?=\\w)(m*(c[md]|d?c{0,3}))(x[cl]|l?x{0,3})(i[xv]|v?i{0,3})\\b(?<=\\w)";

const roman_utils::roman_numeral_conv roman_numeral_traits< false >::CINFO = {
    {
        { "i", wxS('\u2170') },
        { "ii", wxS('\u2171') },
        { "iii", wxS('\u2172') },
        { "iv", wxS('\u2173') },
        { "v", wxS('\u2174') },
        { "vi", wxS('\u2175') },
        { "vii", wxS('\u2176') },
        { "viii", wxS('\u2177') },
        { "ix", wxS('\u2178') },
        { "x", wxS('\u2179') },
        { "xi", wxS('\u217A') },
        { "xii", wxS('\u217B') },
    },
    { "cmd", wxS("\u217D\u217F\u217E") },
    { "xcl", wxS("\u2179\u217D\u217C") },
    { "ixv", wxS("\u2170\u2179\u2174") }
};

wxRomanNumeralsConv::wxRomanNumeralsConv(const wxString& regEx)
    :m_re(regEx)
{
    wxASSERT(m_re.IsValid());
}

wxString wxRomanNumeralsConv::get_match(const wxString& s, size_t matchIdx) const
{
    size_t idx, len;

    if (m_re.GetMatch(&idx, &len, matchIdx))
    {
        return s.Mid(idx, len);
    }
    else
    {
        return wxEmptyString;
    }
}
