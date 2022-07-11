/*
 * wxReduntantSpacesRemover.cpp
 */

#include "wxNumberFullStopCorrector.h"

namespace
{
    constexpr char REG_EX[] = "\\b(?=\\d)(\\d{1,2}\\.\\p{Xps}+)";

    std::optional<WXUINT> get_num(const wxString& num)
    {
        const int pos = num.First('.');
        wxASSERT(pos > 0);

        WXUINT n;
        if (!num.Left(pos).ToUInt(&n)) return std::nullopt;
        if (n > 20) return std::nullopt;
        return n;
    }

    std::optional<wxUniChar> get_uchar(const wxString& num)
    {
        const std::optional<WXUINT> n = get_num(num);
        if (!n.has_value()) return std::nullopt;
        if (n.value() == 0u)
        {
            return 0x1F100; // DIGIT ZERO FULL STOP
        }
        else
        {
            return 0x2487 + n.value(); // DIGIT [ONE to TWENTY] FULL STOP
        }
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
        const std::optional<wxUniChar> c = get_uchar(num);
        if (c.has_value())
        {
            out += c.value();
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
