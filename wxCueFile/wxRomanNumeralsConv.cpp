/*
 * wxRomanNumeralsConv.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxRomanNumeralsConv.h>
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>

// ===============================================================================

const char wxRomanNumeralsConv::REGEX_UPPER[] = "\\m(?=[MDCLXVI])M*(C[MD]|D?C{0,3})(X[CL]|L?X{0,3})(I[XV]|V?I{0,3})\\M";
const char wxRomanNumeralsConv::REGEX_LOWER[] = "\\m(?=[mdclxvi])m*(c[md]|d?C{0,3})(x[cl]|l?x{0,3})(i[xv]|v?I{0,3})\\M";

// ===============================================================================

const wxRomanNumeralsConv::roman_numeral_conv wxRomanNumeralsConv::CONV_UPPER = {
    {
        {"I", wxS( '\u2160' )},
        {"II", wxS( '\u2161' )},
        {"III", wxS( '\u2162' )},
        {"IV", wxS( '\u2163' )},
        {"V", wxS( '\u2164' )},
        {"VI", wxS( '\u2165' )},
        {"VII", wxS( '\u2166' )},
        {"VIII", wxS( '\u2167' )},
        {"IX", wxS( '\u2168' )},
        {"X", wxS( '\u2169' )},
        {"XI", wxS( '\u216A' )},
        {"XII", wxS( '\u216B' )},
    },
    {"CMD", wxS( "\u216D\u216F\u216E" )},
    {"XCL", wxS( "\u2169\u216D\u216C" )},
    {"IXV", wxS( "\u2160\u2169\u2164" )}
};

const wxRomanNumeralsConv::roman_numeral_conv wxRomanNumeralsConv::CONV_LOWER = {
    {
        {"i", wxS( '\u2170' )},
        {"ii", wxS( '\u2171' )},
        {"iii", wxS( '\u2172' )},
        {"iv", wxS( '\u2173' )},
        {"v", wxS( '\u2174' )},
        {"vi", wxS( '\u2175' )},
        {"vii", wxS( '\u2176' )},
        {"viii", wxS( '\u2177' )},
        {"ix", wxS( '\u2178' )},
        {"x", wxS( '\u2179' )},
        {"xi", wxS( '\u217A' )},
        {"xii", wxS( '\u217B' )},
    },
    {"cmd", wxS( "\u217D\u217F\u217E" )},
    {"xcl", wxS( "\u2179\u217D\u217C" )},
    {"ixv", wxS( "\u2170\u2179\u2174" )}
};

// ===============================================================================

wxRomanNumeralsConv::wxRomanNumeralsConv( ):
m_reUpper( REGEX_UPPER, wxRE_ADVANCED ), m_reLower( REGEX_LOWER, wxRE_ADVANCED )
{
	wxASSERT( m_reUpper.IsValid() );
    wxASSERT( m_reLower.IsValid( ) );
}

wxString wxRomanNumeralsConv::convert( const wxString& cmdPart, const wxString& xclPart, const wxString& ixvPart, const wxRomanNumeralsConv::roman_numeral_conv& cinfo )
{
    wxString newCmdPart( cmdPart );
    size_t cmdRepl = replace_part( newCmdPart, cinfo.cmd );

    wxString newXclPart( xclPart );
    size_t xclRepl = replace_part( newXclPart, cinfo.xcl );

    if (cmdRepl > 0 || (xclRepl > 0 && xclPart.Cmp( cinfo.xcl.chars[0] ) != 0))
    {
        wxString newIxvPart( ixvPart );
        size_t ixvRepl = replace_part( newIxvPart, cinfo.ixv );

        return newCmdPart + newXclPart + newIxvPart;
    }
    else
    {
        wxString newIxvPart( ixvPart );
        if (xclPart.Cmp( cinfo.xcl.chars[0] ) == 0)
        {
            newIxvPart.Prepend( xclPart );
        }

        if (!convert_roman_12( newIxvPart, cinfo.c12 ))
        {
            size_t ixvRepl = replace_part( newIxvPart, cinfo.ixv );
        }

        return newIxvPart;
    }
}

wxString wxRomanNumeralsConv::convert_upper( const wxArrayString& matches )
{
    wxASSERT( matches.GetCount( ) == 4 );
    return convert( matches[1], matches[2], matches[3], CONV_UPPER );
}

wxString wxRomanNumeralsConv::convert_lower( const wxArrayString& matches )
{
    wxASSERT( matches.GetCount() == 4 );
    return convert( matches[1], matches[2], matches[3], CONV_LOWER );
}

namespace
{
    template<typename C>
    int replace( const wxRegEx& regEx, wxString& text, const C& replacer, size_t maxMatches )
    {
        wxCHECK_MSG( regEx.IsValid( ), wxNOT_FOUND, wxT( "must successfully Compile() first" ) );

        // the input string
#ifndef WXREGEX_CONVERT_TO_MB
        const wxChar *textstr = text.c_str( );
        size_t textlen = text.length( );
#else
        const wxWX2MBbuf textstr = WXREGEX_CHAR( *text );
        if (!textstr)
        {
            wxLogError( _( "Failed to find match for regular expression: %s" ),
                        GetErrorMsg( 0, true ).c_str( ) );
            return 0;
        }
        size_t textlen = strlen( textstr );
        text->clear( );
#endif

        // the replacement text
        wxString textNew;

        // the result
        wxTextOutputStreamOnString result;

        // matches
        wxArrayString matches;

        // the position where we start looking for the match
        size_t matchStart = 0;

        // number of replacement made: we won't make more than maxMatches of them
        // (unless maxMatches is 0 which doesn't limit the number of replacements)
        size_t countRepl = 0;

        // note that "^" shouldn't match after the first call to Matches() so we
        // use wxRE_NOTBOL to prevent it from happening
        while ((!maxMatches || countRepl < maxMatches) &&
                regEx.Matches(
#ifndef WXREGEX_CONVERT_TO_MB
                textstr + matchStart,
#else
                textstr.data( ) + matchStart,
#endif
                countRepl ? wxRE_NOTBOL : 0 ))
                //WXREGEX_IF_NEED_LEN( textlen - matchStart ) ))
        {
            size_t  matchCount = regEx.GetMatchCount( );
            matches.SetCount( matchCount );

            size_t start, len;

            for (size_t i = 1; i < matchCount; ++i)
            {
                if (!regEx.GetMatch( &start, &len, i ))
                {
                    // we did have match as Matches() returned true above!
                    wxFAIL_MSG( wxT( "internal logic error in wxRegEx::Replace" ) );

                    return wxNOT_FOUND;
                }

                matches[i] = wxString(
#ifndef WXREGEX_CONVERT_TO_MB
                    textstr + matchStart + start,
#else
                    textstr.data( ) + matchStart + start,
#endif
                    len );
            }

            if (!regEx.GetMatch( &start, &len ))
            {
                // we did have match as Matches() returned true above!
                wxFAIL_MSG( wxT( "internal logic error in wxRegEx::Replace" ) );

                return wxNOT_FOUND;
            }

            matches[0] = wxString(
#ifndef WXREGEX_CONVERT_TO_MB
                textstr + matchStart + start,
#else
                textstr.data( ) + matchStart + start,
#endif
                len );

            textNew = replacer( matches );

            if (start > 0)
            {
#ifndef WXREGEX_CONVERT_TO_MB
                ( *result ) << text.Mid( matchStart, start );
#else
                (*result) << wxString( textstr.data( ) + matchStart, *wxConvCurrent, start );
#endif
                matchStart += start;
            }

            (*result) << textNew;
            matchStart += len;

            countRepl++;
        }

#ifndef WXREGEX_CONVERT_TO_MB
        ( *result ) << text.Mid( matchStart );
#else
        (*result) << wxString( textstr.data( ) + matchStart, *wxConvCurrent );
#endif
        result->Flush( );
        text = result.GetString( );

        return countRepl;
    }
}

wxString wxRomanNumeralsConv::ConvertUpper( const wxString& text ) const
{
    wxString result( text );

    if (replace( m_reUpper, result, convert_upper, 0 ) > 0)
    {
        return result;
    }
    else
    {
        return text;
    }
}

wxString wxRomanNumeralsConv::ConvertLower( const wxString& text ) const
{
    wxString result( text );

    if (replace( m_reLower, result, convert_lower, 0 ) > 0)
    {
        return result;
    }
    else
    {
        return text;
    }
}
