/*
 * wxAsciiToUnicode.cpp
 */

#include <wxCueFile/wxAsciiToUnicode.h>

const wxAsciiToUnicode::ASCII2UNI wxAsciiToUnicode::REPL_TABLE[] = {
    { "(C)", "(?<!\\()\\(C\\)(?!\\))", 0xA9 },
    { "(P)", "(?<!\\()\\(P\\)(?!\\))", 0x2117 },
    { "(R)", "(?<!\\()\\(R\\)(?!\\))", 0xAE },
    { "(I)", "(?<!\\()\\(I\\)(?!\\))", 0x1F6C8 },
    { "(+)", "(?<!\\()\\(\\+\\)(?!\\))", 0x2295 },
    { "(-)", "(?<!\\()\\(\\-\\)(?!\\))", 0x229D },
    { "(/)", "(?<!\\()\\(/\\)(?!\\))", 0x2298 },
    { "(=)", "(?<!\\()\\(\\=\\)(?!\\))", 0x229C },
    { "(.)", "(?<!\\()\\(\\.\\)(?!\\))", 0x2299 },
    { "(*)", "(?<!\\()\\(\\*\\)(?!\\))", 0x229B },
    { "No.", "(?<!\\p{Xan})No\\.(?=\\p{Xps}*\\p{N})", 0x2116 },
    { "ae", "ae", 0xE6 },
    { "AE", "AE", 0xC6 },
    { "oe", "oe", 0x153 },
    { "OE", "OE", 0x152 },
    { "ij", "ij", 0x133 },
    { "IJ", "IJ", 0x132 },
    { "ff", "f{2}", 0xFB00 },
    { "fi", "fi", 0xFB01 },
    { "ffi", "f{2}i", 0xFB03 },
    { "fl", "fl", 0xFB02 },
    { "ffl", "f{2}l", 0xFB04 },
    { "||", "\\|{2}", 0x2016 },
    { "!!", "\\!{2}", 0x203C },
    { "<=", "(?<!\\<)\\<\\=(?!\\=)", 0x2264 },
    { ">=", "(?<!\\>)\\>\\=(?!\\=)", 0x2265 },
    { "><", "(?<!\\>)\\>\\<(?!\\<)", 0x2AA5 },
    { ".>", "(?<!\\.)\\.\\>(?!\\>)", 0x22D7 },
    { "(>", "(?<!\\))\\(\\>(?!\\>)", 0x2AA7 },
    { ">>>>", "\\>{4,}", 0x22D9 },
    { ">>", "\\>{2,3}", 0x226B },
    { "(>=", "(?<!\\()\\(\\>\\=(?!\\=)", 0x2AA9 },
    { "<)", "(?<!\\<)\\<\\)(?!\\))", 0x2AA6 },
    { "<.", "(?<!\\<)\\<\\.(?!\\.)", 0x22D6 },
    { "<<<<", "\\<{4,}", 0x22D8 },
    { "<<", "\\<{2,3}", 0x226A },
    { "<=)", "(?<!\\<)\\<\\=\\)(?!\\))", 0x2AA8},
    { "+-", "(?<!\\+)\\+\\-(?!\\-)", 0xB1 },
    { ".+", "(?<!\\.)\\.\\+(?!\\+)", 0x2214 },
    { "!?", "(?<!\\!)\\!\\?(?!\\?)", 0x203D },
    { "!=", "(?<!\\!)\\!\\=(?!\\=)", 0x2260 },
    { "?=", "(?<!\\?)\\?\\=(?!\\=)", 0x225F },
    { ":=", "(?<!\\:)\\:\\=(?!\\=)", 0x2251 },
    { "==", "\\={2}", 0x2261 },
    { "~~", "~{2}", 0x2248 },
    { "~=", "(?<!~)~\\=(?!\\=)", 0x2245 },
    { "!~=", "(?<!\\!)\\!~\\=(?!\\=)", 0x2246 },
    { "~-", "(?<!~)~\\-(?!\\-)", 0x2243 },
    { "!~-", "(?<!\\!)\\!~\\-(?!\\-)", 0x2244 },
    { ":-)", "\\:\\-\\)", 0x263A },
    { "(-:", "\\(\\-\\:", 0x263B },
    { "1/2", "(?<!\\p{N})1/2(?!\\p{N})", 0xBD },
    { "0/3", "(?<!\\p{N})0/3(?!\\p{N})", 0x2189 },
    { "1/3", "(?<!\\p{N})1/3(?!\\p{N})", 0x2153 },
    { "2/3", "(?<!\\p{N})2/3(?!\\p{N})", 0x2154 },
    { "1/4", "(?<!\\p{N})1/4(?!\\p{N})", 0xBC },
    { "3/4", "(?<!\\p{N})3/4(?!\\p{N})", 0xBE },
    { "1/5", "(?<!\\p{N})1/5(?!\\p{N})", 0x2155 },
    { "2/5", "(?<!\\p{N})2/5(?!\\p{N})", 0x2156 },
    { "3/5", "(?<!\\p{N})3/5(?!\\p{N})", 0x2157 },
    { "4/5", "(?<!\\p{N})4/5(?!\\p{N})", 0x2158 },
    { "1/6", "(?<!\\p{N})1/6(?!\\p{N})", 0x2159 },
    { "5/6", "(?<!\\p{N})5/6(?!\\p{N})", 0x215A },
    { "1/7", "(?<!\\p{N})1/7(?!\\p{N})", 0x2150 },
    { "1/8", "(?<!\\p{N})1/8(?!\\p{N})", 0x215B },
    { "3/8", "(?<!\\p{N})3/8(?!\\p{N})", 0x215C },
    { "5/8", "(?<!\\p{N})5/8(?!\\p{N})", 0x215D },
    { "7/8", "(?<!\\p{N})7/8(?!\\p{N})", 0x215E },
    { "1/9", "(?<!\\p{N})1/9(?!\\p{N})", 0x2151 },
    { "/#/", "(?<!\\/)/#/(?!\\/)", 0x266F },
    { "{*}", "(?<!\\{)\\{\\*\\}(?!\\})", 0x2022 },
    { "{**}", "(?<!\\{)\\{\\*{2}\\}(?!\\})", 0x25CF },
    { "{.}", "(?<!\\{)\\{\\.\\}(?!\\})", 0x25E6 },
    { "{..}", "(?<!\\{)\\{\\.{2}\\}(?!\\})", 0x25CB },
    { "[*]", "(?<!\\[)\\[\\*\\](?!\\])", 0x25AA },
    { "[**]", "(?<!\\[)\\[\\*{2}\\](?!\\])", 0x25A0 },
    { "[.]", "(?<!\\[)\\[\\.\\](?!\\])", 0x25AB },
    { "[..]", "(?<!\\[)\\[\\.{2}\\](?!\\])", 0x25A1 },
    { "<*>", "(?<!\\<)\\<\\*\\>(?!\\>)", 0x2666 },
    { "<.>", "(?<!\\<)\\<\\.\\>(?!\\>)", 0x25CA }
};

namespace {

    template<size_t N>
    wxString get_regex(const wxAsciiToUnicode::ASCII2UNI(&elems)[N])
    {
        wxString res;
        for (size_t i = 0; i < N; ++i)
        {
            res << elems[i].regex << '|';
        }
        return res.RemoveLast().Prepend('(').Append(')');
    }

    template<size_t N>
    wxUniChar get_uchr(const wxAsciiToUnicode::ASCII2UNI(&elems)[N], const wxString& match)
    {
        for (size_t i = 0; i < N; ++i)
        {
            if (match == elems[i].str)
            {
                return wxUniChar(elems[i].uniChar);
            }
        }

        wxASSERT_MSG(false, wxString::Format("Ascii2Unicode: fail to find Unicode replacement for %s", match));
        return wxUniChar();
    }

    template<size_t N>
    void show_ascii_to_unicode_table(wxMessageOutput& out, const wxAsciiToUnicode::ASCII2UNI(&elems)[N])
    {
        for (size_t i = 0; i < N; ++i)
        {
            const wxString str(elems[i].str);
            const wxUniChar uc(elems[i].uniChar);
            const wxString us(uc);

            if (elems[i].uniChar <= 0xFFFF)
            {
                out.Printf(_("\t%-10s%s\tU+%04X"), str, us, elems[i].uniChar);
            }
            else
            {
                out.Printf(_("\t%-10s%s\tU+%X"), str, us, elems[i].uniChar);
            }
        }
    }

    wxString get_reg_ex_str()
    {
        return get_regex(wxAsciiToUnicode::REPL_TABLE);
    }

    wxUniChar get_uchr(const wxString& match)
    {
        return get_uchr(wxAsciiToUnicode::REPL_TABLE, match);
    }
}

void wxAsciiToUnicode::ShowReplTable(wxMessageOutput& out)
{
    show_ascii_to_unicode_table(out, REPL_TABLE);
}

wxAsciiToUnicode::wxAsciiToUnicode():
    m_re(get_reg_ex_str())
{
    wxASSERT(m_re.IsValid());
}

wxStringProcessor* const wxAsciiToUnicode::Clone() const
{
    return new wxAsciiToUnicode();
}

bool wxAsciiToUnicode::Process(const wxString& text, wxString& out) const
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

        res.Append(w.Mid(0, idx)).
            Append(get_uchr(w.Mid(idx, len)));
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

