/*
 * wxRomanNumeralsConv.h
 */

#ifndef _WX_ROMAN_NUMERALS_H_
#define _WX_ROMAN_NUMERALS_H_

#include <wxEncodingDetection/wxStringEx.h>

struct roman_utils
{
    struct roman_1_12
    {
        const char* const text;
        wxUChar ureplacement;
    };

    struct roman_group
    {
        char chars[4];
        wxUChar ureplacements[4];
    };

    struct roman_numeral_conv
    {
        roman_1_12 c12[12];
        roman_group cmd;
        roman_group xcl;
        roman_group ixv;
    };

    template<size_t SIZE>
    static inline bool convert_roman_12( wxString& roman, const roman_1_12( &r2u )[SIZE] )
    {
        for (size_t i = 0; i < SIZE; ++i)
        {
            if (roman.Cmp( r2u[i].text ) == 0)
            {
                roman = r2u[i].ureplacement;
                return true;
            }
        }

        return false;
    }

    template<size_t SIZE>
    static inline size_t replace_n( wxString& roman, const char( &ascii )[SIZE], const wxUChar( &unicode )[SIZE] )
    {
        size_t repl = 0;
        for (size_t i = 0; i < (SIZE - 1); ++i) repl += roman.Replace( ascii[i], unicode[i] );

        return repl;
    }

    static inline size_t replace_part( wxString& roman, const roman_group& part )
    {
        return replace_n( roman, part.chars, part.ureplacements );
    }

    static inline wxString convert( const wxString& cmdPart, const wxString& xclPart, const wxString& ixvPart, const roman_numeral_conv& cinfo )
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
};

template<bool UPPER>
struct roman_numeral_traits
{};

template<>
struct roman_numeral_traits<true>
{
    static const char REGEX[];
    static const roman_utils::roman_numeral_conv CINFO;
};

template<>
struct roman_numeral_traits<false>
{
    static const char REGEX[];
    static const roman_utils::roman_numeral_conv CINFO;
};

template<bool UPPER>
class wxRomanNumeralsConv
{
    public:

        typedef roman_numeral_traits<UPPER> numeral_traits;

    public:

        wxRomanNumeralsConv()
            :m_re( numeral_traits::REGEX, wxRE_ADVANCED )
        {
            wxASSERT( m_re.IsValid( ) );
        }

        wxString Convert( const wxString& text ) const
        {
            wxString result( text );

            if (wxStringEx::Replace( m_re, result, converter, 0 ) > 0)
            {
                return result;
            }
            else
            {
                return text;
            }
        }

	protected:

		wxRegEx m_re;

    protected:

        static inline wxString converter( const wxArrayString& matches )
        {
            wxASSERT( matches.GetCount( ) == 4 );
            return roman_utils::convert( matches[1], matches[2], matches[3], numeral_traits::CINFO );
        }
};
#endif

