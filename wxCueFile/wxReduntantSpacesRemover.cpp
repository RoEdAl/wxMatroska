/*
 * wxReduntantSpacesRemover.cpp
 */

#include "wxReduntantSpacesRemover.h"

namespace
{
    constexpr char NORMAL_REG_EX[] = "\\p{Xps}{2}"; // two spaces -> simple space
    constexpr char EN_REG_EX[] = "\\p{Xps}{3}";	// three spaces -> en space
    constexpr char EM_REG_EX[] = "\\p{Xps}{4,}";// four or more spaces -> em space

};

wxReduntantSpacesRemover::wxReduntantSpacesRemover():
    m_re(NORMAL_REG_EX),
    m_reEn(EN_REG_EX),
    m_reEm(EM_REG_EX),
    m_replacementNormal(wxUniChar(0x0020)),
    m_replacementEn(wxUniChar(0x2002)),
    m_replacementEm(wxUniChar(0x2001))
{
    wxASSERT(m_re.IsValid());
    wxASSERT(m_reEn.IsValid());
    wxASSERT(m_reEm.IsValid());
}

wxStringProcessor* const wxReduntantSpacesRemover::Clone() const
{
    return new wxReduntantSpacesRemover();
}

bool wxReduntantSpacesRemover::Process(const wxString& in, wxString& out) const
{
    wxString w(in);
    int      res = 0;

    res += m_re.ReplaceAll(&w, m_replacementNormal);
    res += m_reEn.ReplaceAll(&w, m_replacementEn);
    res += m_reEm.ReplaceAll(&w, m_replacementEm);

    if (res > 0)
    {
        out = w;
        return true;
    }
    else
    {
        return false;
    }
}

