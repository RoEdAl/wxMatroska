/*
 * wxTriailingSpacesRemover.cpp
 */

#include "wxTrailingSpacesRemover.h"

namespace
{
    constexpr char REG_EX[] ="^[\\p{Xps}\\p{Cc}]*([^\\p{Xps}\\p{Cc}].*[^\\p{Xps}\\p{Cc}])[\\p{Xps}\\p{Cc}]*$";
};

wxTrailingSpacesRemover::wxTrailingSpacesRemover():
    m_reTrailingSpaces(REG_EX)
{
    wxASSERT(m_reTrailingSpaces.IsValid());
}

wxStringProcessor* const wxTrailingSpacesRemover::Clone() const
{
    return new wxTrailingSpacesRemover();
}

bool wxTrailingSpacesRemover::Process(const wxString& in, wxString& out) const
{
    if (m_reTrailingSpaces.Matches(in))
    {
        size_t start, len;
        m_reTrailingSpaces.GetMatch(&start, &len, 1);

        if (len < in.Length())
        {
            wxASSERT(len > 0);
            out = in.SubString(start, start + len - 1);
            return true;
        }
    }

    return false;
}
