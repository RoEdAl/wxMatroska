/*
 * wxSmallLetterParenthesizedCorrector.cpp
 */

#include "wxSmallLetterParenthesizedCorrector.h"

// ===============================================================================

const char wxSmallLetterParenthesizedCorrector::REG_EX[] = "\\([a-z]\\)";

// ===============================================================================

wxSmallLetterParenthesizedCorrector::wxSmallLetterParenthesizedCorrector():
    m_re(REG_EX)
{
    wxASSERT(m_re.IsValid());
}

wxStringProcessor* const wxSmallLetterParenthesizedCorrector::Clone() const
{
    return new wxSmallLetterParenthesizedCorrector();
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

