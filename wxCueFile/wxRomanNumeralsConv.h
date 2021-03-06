/*
 * wxRomanNumeralsConv.h
 */

#ifndef _WX_ROMAN_NUMERALS_H_
#define _WX_ROMAN_NUMERALS_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

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

    template< size_t SIZE >
    static inline bool convert_roman_12(wxString& roman, const roman_1_12(&r2u)[SIZE])
    {
        for (size_t i = 0; i < SIZE; ++i)
        {
            if (roman.Cmp(r2u[i].text) == 0)
            {
                roman = r2u[i].ureplacement;
                return true;
            }
        }

        return false;
    }

    template< size_t SIZE >
    static inline size_t replace_n(wxString& roman, const char(&ascii)[SIZE], const wxUChar(&unicode)[SIZE])
    {
        size_t repl = 0;

        for (size_t i = 0; i < (SIZE - 1); ++i)
        {
            repl += roman.Replace(ascii[i], unicode[i]);
        }

        return repl;
    }

    static inline size_t replace_part(wxString& roman, const roman_group& part)
    {
        return replace_n(roman, part.chars, part.ureplacements);
    }

    static inline wxString convert(const wxString& cmdPart, const wxString& xclPart, const wxString& ixvPart, const roman_numeral_conv& cinfo)
    {
        wxString newCmdPart(cmdPart);
        size_t   cmdRepl = replace_part(newCmdPart, cinfo.cmd);

        wxString newXclPart(xclPart);
        size_t   xclRepl = replace_part(newXclPart, cinfo.xcl);

        if (cmdRepl > 0 || (xclRepl > 0 && xclPart.Cmp(cinfo.xcl.chars[0]) != 0))
        {
            wxString newIxvPart(ixvPart);
            size_t   ixvRepl = replace_part(newIxvPart, cinfo.ixv);

            return newCmdPart + newXclPart + newIxvPart;
        }
        else
        {
            wxString newIxvPart(ixvPart);

            if (xclPart.Cmp(cinfo.xcl.chars[0]) == 0) newIxvPart.Prepend(xclPart);

            if (!convert_roman_12(newIxvPart, cinfo.c12)) size_t ixvRepl = replace_part(newIxvPart, cinfo.ixv);

            return newIxvPart;
        }
    }
};

template< bool UPPER >
struct roman_numeral_traits
{
};

template< >
struct roman_numeral_traits< true >
{
    static const char REGEX_WEAK[];
    static const char REGEX_STRONG[];
    static const roman_utils::roman_numeral_conv CINFO;
};

template< >
struct roman_numeral_traits< false >
{
    static const char REGEX_STRONG[];
    static const char REGEX_WEAK[];
    static const roman_utils::roman_numeral_conv CINFO;
};

class wxRomanNumeralsConv: public wxStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxRomanNumeralsConv);

    protected:

    wxRomanNumeralsConv(const wxString&);

    protected:

    wxRegEx m_re;

    protected:

    wxString get_match(const wxString&, size_t) const;
};

template< bool UPPER >
class wxRomanNumeralsConvWeak: public wxRomanNumeralsConv
{
    wxDECLARE_NO_COPY_TEMPLATE_CLASS(wxRomanNumeralsConvWeak, UPPER);

    public:

    typedef wxRomanNumeralsConvWeak< UPPER > ThisClass;
    typedef roman_numeral_traits< UPPER > numeral_traits;

    public:

    wxRomanNumeralsConvWeak()
        :wxRomanNumeralsConv(numeral_traits::REGEX_WEAK)
    {
    }

    virtual wxStringProcessor* const Clone() const wxOVERRIDE
    {
        return new ThisClass();
    }

    virtual bool Process(const wxString& text, wxString& out) const wxOVERRIDE
    {
        wxString w(text);
        wxString res;
        bool     replaced = false;

        while (m_re.Matches(w))
        {
            size_t idx, len;

            if (!m_re.GetMatch(&idx, &len))
            {
                replaced = false;
                break;
            }

            res += w.Mid(0, idx);

            const wxString m1 = get_match(w, 1);
            const wxString m2 = get_match(w, 2);

            res += roman_utils::convert(wxEmptyString, m1, m2, numeral_traits::CINFO);
            replaced = true;
            w.Remove(0, idx + len);
        }

        if (replaced)
        {
            res += w;
            out = res;
            return true;
        }

        return false;
    }
};

template< bool UPPER >
class wxRomanNumeralsConvStrong: public wxRomanNumeralsConv
{
    wxDECLARE_NO_COPY_TEMPLATE_CLASS(wxRomanNumeralsConvStrong, UPPER);

    public:

    typedef wxRomanNumeralsConvStrong< UPPER > ThisClass;
    typedef roman_numeral_traits< UPPER > numeral_traits;

    public:

    wxRomanNumeralsConvStrong()
        :wxRomanNumeralsConv(numeral_traits::REGEX_STRONG)
    {
    }

    virtual wxStringProcessor* const Clone() const wxOVERRIDE
    {
        return new ThisClass();
    }

    virtual bool Process(const wxString& text, wxString& out) const wxOVERRIDE
    {
        wxString w(text);
        wxString res;
        bool     replaced = false;

        while (m_re.Matches(w))
        {
            size_t idx, len;

            if (!m_re.GetMatch(&idx, &len))
            {
                replaced = false;
                break;
            }

            res += w.Mid(0, idx);

            const wxString m1 = get_match(w, 1);
            const wxString m2 = get_match(w, 3);
            const wxString m3 = get_match(w, 4);

            wxASSERT(!(m1.IsEmpty() && m2.IsEmpty() && m3.IsEmpty()));

            res += roman_utils::convert(m1, m2, m3, numeral_traits::CINFO);
            replaced = true;
            w.Remove(0, idx + len);
        }

        if (replaced)
        {
            res += w;
            out = res;
            return true;
        }

        return false;
    }
};

#endif

