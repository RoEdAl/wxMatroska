/*
 * wxReduntantSpacesRemover.cpp
 */

#include "wxNumberFullStopCorrector.h"

namespace
{
    constexpr char REG_EX[] = "\\b(?=\\d)(\\d{1,2}\\.\\p{Xps}+)";

    bool get_num(const wxString& num, unsigned int& n)
    {
        const int pos = num.First('.');

        if (!num.Left(pos).ToUInt(&n)) return false;

        if (n < 1 || n > 20) return false;

        return true;
    }
};

wxNumberFullStopCorrector::wxNumberFullStopCorrector():
    m_re(REG_EX)
{
    wxASSERT(m_re.IsValid());
}

wxStringProcessor* const wxNumberFullStopCorrector::Clone() const
{
    return new wxNumberFullStopCorrector();
}

bool wxNumberFullStopCorrector::Process(const wxString& in, wxString& out) const
{
    out = wxEmptyString;

    wxString        w(in);
    const wxUniChar thinSpace(0x2009);
    bool            replaced = false;

    while (m_re.Matches(w))
    {
        size_t idx, len;

        if (!m_re.GetMatch(&idx, &len, 1))
        {
            replaced = false;
            break;
        }

        out += w.Mid(0, idx);

        const wxString num = w.Mid(idx, len);
        unsigned int   n;

        if (get_num(num, n))
        {
            const wxUniChar c(0x2487 + n);
            out += c;
            out += thinSpace;
            replaced = true;
        }
        else
        {
            out += num;
        }

        w.Remove(0, idx + len);
    }

    if (replaced)
    {
        out += w;
        return true;
    }

    return false;
}
