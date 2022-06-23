/*
 * wxEllipsizer.cpp
 */

#include "wxEllipsizer.h"

// ===============================================================================

const char wxEllipsizer::REG_EX[] = "(\\p{Xps}*\\.\\.\\.)\\p{Xps}*$";
const wxUChar wxEllipsizer::ELLIPSIS = wxS('\u2026');

// ===============================================================================

wxEllipsizer::wxEllipsizer():
    m_reEllipsis(REG_EX)
{
    wxASSERT(m_reEllipsis.IsValid());
}

wxStringProcessor* const wxEllipsizer::Clone() const
{
    return new wxEllipsizer();
}

bool wxEllipsizer::Process(const wxString& in, wxString& out) const
{
    out = in;
    wxString res(in);
    int      repl = m_reEllipsis.ReplaceAll(&res, ELLIPSIS);

    if (repl > 0)
    {
        out = res;
        return true;
    }

    return false;
}

