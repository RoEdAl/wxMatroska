/*
 * wxSmallLetterParenthesizedCorrector.cpp
 */

#include <wxCueFile/wxSmallLetterParenthesizedCorrector.h>

namespace
{
    constexpr char REG_EX[] = "\\([a-z]\\)";

    wxString get_parenthesized_char(char c)
    {
        wxString res = c;
        return res.Prepend('(').Append(')');
    }

    void show_character(wxMessageOutput& out, char c, int uniChar)
    {
        const wxUniChar uc(uniChar);
        const wxString us = uc;
        const wxString str = get_parenthesized_char(c);
        if (uniChar <= 0xFFFF)
        {
            out.Printf(_("\t%-10s%s\tU+%04X"), str, us, uniChar);
        }
        else
        {
            out.Printf(_("\t%-10s%s\tU+%X"), str, us, uniChar);
        }
    }
}

wxSmallLetterParenthesizedCorrector::wxSmallLetterParenthesizedCorrector():
    m_re(REG_EX)
{
    wxASSERT(m_re.IsValid());
}

wxStringProcessor* const wxSmallLetterParenthesizedCorrector::Clone() const
{
    return new wxSmallLetterParenthesizedCorrector();
}

void wxSmallLetterParenthesizedCorrector::ShowCharacters(wxMessageOutput& out)
{
    for (char i = 'a'; i <= 'z'; ++i)
    {
        show_character(out, i, 0x249C + (i - 'a'));
    }
}

bool wxSmallLetterParenthesizedCorrector::Process(const wxString& in, wxString& out) const
{
    out = wxEmptyString;

    wxString        w(in);
    const wxUniChar smallA('a');
    bool            replaced = false;

    while (m_re.Matches(w))
    {
        size_t idx, len;

        if (!m_re.GetMatch(&idx, &len))
        {
            replaced = false;
            break;
        }

        out += w.Mid(0, idx);

        const wxUniChar letter = w[idx + 1];
        const int       sh = (int)letter - (int)smallA;
        const wxUniChar parenthesized(0x249C + sh);
        out += parenthesized;
        replaced = true;

        w.Remove(0, idx + len);
    }

    if (replaced)
    {
        out += w;
        return true;
    }

    return false;
}

