/*
 * wxRegExpStringProcessor.cpp
 */

#include <wxCueFile/wxRegExStringProcessor.h>

wxRegExStringProcessor::wxRegExStringProcessor(const wxString& regEx, const wxString& replacement):
    m_regEx(regEx), m_replacement(replacement)
{
    wxASSERT(m_regEx.IsValid());
}

bool wxRegExStringProcessor::Process(const wxString& in, wxString& out) const
{
    wxString res(in);
    int      repl = m_regEx.ReplaceAll(&res, m_replacement);

    if (repl > 0)
    {
        out = res;
        return true;
    }

    return false;
}

