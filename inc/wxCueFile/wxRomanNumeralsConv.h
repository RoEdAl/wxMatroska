/*
 * wxRomanNumeralsConv.h
 */

#ifndef _WX_ROMAN_NUMERALS_H_
#define _WX_ROMAN_NUMERALS_H_

class wxRomanNumeralsConv:
	public wxObject
{
    public:

    static const char REGEX_UPPER[];
    static const char REGEX_LOWER[];

    public:

    struct roman_1_12
    {
        const char* const text;
        wxUChar ureplacement;
    };

    struct roman_group
    {
        char chars[4];
        wxUChar ureplacement[4];
    };

    struct roman_numeral_conv
    {
        roman_1_12 c12[12];
        roman_group cmd;
        roman_group xcl;
        roman_group ixv;
    };

    static const roman_numeral_conv CONV_UPPER;
    static const roman_numeral_conv CONV_LOWER;

	public:

        wxRomanNumeralsConv( void );

        wxString ConvertUpper( const wxString& ) const;
        wxString ConvertLower( const wxString& ) const;

	protected:

		wxRegEx m_reUpper;
        wxRegEx m_reLower;

    protected:

    template<size_t SIZE>
    static bool convert_roman_12( wxString& roman, const roman_1_12( &r2u )[SIZE] )
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
    static size_t replace_n( wxString& roman, const char( &ascii )[SIZE], const wxUChar( &unicode )[SIZE] )
    {
        size_t repl = 0;
        for (size_t i = 0; i < (SIZE - 1); ++i) repl += roman.Replace( ascii[i], unicode[i] );

        return repl;
    }

    static size_t replace_part( wxString& roman, const roman_group& part )
    {
        return replace_n( roman, part.chars, part.ureplacement );
    }

    static wxString convert( const wxString&, const wxString&, const wxString&, const roman_numeral_conv& );
    static wxString convert_upper( const wxArrayString& );
    static wxString convert_lower( const wxArrayString& );
};
#endif

