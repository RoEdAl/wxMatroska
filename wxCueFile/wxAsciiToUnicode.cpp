/*
 * wxAsciiToUnicode.cpp
 */

#include <wxCueFile/wxAsciiToUnicode.h>

const wxAsciiToUnicode::ASCII2UNI wxAsciiToUnicode::REPL_TABLE[] = {
    { "(C)", "\\(C\\)", 0xA9 },
    { "(P)", "\\(P\\)", 0x2117 },
    { "(R)", "\\(R\\)", 0xAE },
    { "(I)", "\\(I\\)", 0x1F6C8 },
    { "No.", "No\\.", 0x2116 },
    { "ae", "ae", 0xE6 },
    { "oe", "oe", 0x153 },
    { "||", "\\|{2}", 0x2016 },
    { "!!", "\\!{2}", 0x203C },
    { "<=", "\\<\\=", 0x2264 },
    { ">=", "\\>\\=", 0x2265 },
    { "+-", "\\+\\-", 0xB1 },
    { ".+", "\\.\\+", 0x2214 },
    { "!?", "\\!\\?", 0x203D },
    { "!=", "\\!\\=", 0x2260 },
    { "==", "\\={2}", 0x2261 },
    { "~=", "~\\=", 0x2248 },
    { ":-)", "\\:\\-\\)", 0x263A },
    { "(-:", "\\(\\-\\:", 0x263B },
    { "1/2", "1/2", 0xBD },
    { "1/3", "1/3", 0x2153 },
    { "2/3", "2/3", 0x2154 },
    { "1/4", "1/4", 0xBC },
    { "3/4", "3/4", 0xBE },
    { "1/5", "1/5", 0x2155 },
    { "2/5", "2/5", 0x2156 },
    { "3/5", "3/5", 0x2157 },
    { "4/5", "4/5", 0x2158 },
    { "/#/", "/#/", 0x266F },
    { "(*)", "\\(\\*\\)", 0x2022 },
    { "(**)", "\\(\\*{2}\\)", 0x25CF },
    { "(.)", "\\(\\.\\)", 0x25E6 },
    { "(..)", "\\(\\.{2}\\)", 0x25CB },
    { "[*]", "\\[\\*\\]", 0x25AA },
    { "[**]", "\\[\\*{2}\\]", 0x25A0 },
    { "[.]", "\\[\\.\\]", 0x25AB },
    { "[..]", "\\[\\.{2}\\]", 0x25A1 },
    { "<*>", "\\<\\*\\>", 0x2666 },
    { "<.>", "\\<\\.\\>", 0x25CA }
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

