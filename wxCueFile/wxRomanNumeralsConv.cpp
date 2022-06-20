/*
 * wxRomanNumeralsConv.cpp
 */

#include "wxRomanNumeralsConv.h"

 // ===============================================================================

const char roman_numeral_traits< true >::REGEX[] = "[[:<:]](?=[MDCLXVI])M*(C[MD]|D?C{0,3})(X[CL]|L?X{0,3})(I[XV]|V?I{0,3})[[:>:]]";

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

const char roman_numeral_traits< false >::REGEX[] = "\\b(?=\\w)(?=[mdclxvi])m*(c[md]|d?C{0,3})(x[cl]|l?x{0,3})(i[xv]|v?I{0,3})\\b(?<=\\w)";

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

