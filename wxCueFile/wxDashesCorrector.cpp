/*
 * wxDashesCorrector.cpp
 */

#include "wxDashesCorrector.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxDashesCorrector, wxStringProcessor)

// ===============================================================================

const char wxDashesCorrector::REG_EX_EN[] = "\\p{Xps}+\\p{Pd}\\p{Xps}+";
const char wxDashesCorrector::REG_EX_EM[] = "\\p{Xps}+\\p{Pd}{2}\\p{Xps}+";
const char wxDashesCorrector::REG_EX_EM2[] = "\\p{Xps}+\\p{Pd}{3,}\\p{Xps}+";

// ===============================================================================

wxString wxDashesCorrector::GetReplacement(uvalue_type nUniSpace, uvalue_type nUniDash)
{
    wxString sResult;

    wxUniChar uSpace(nUniSpace);
    wxUniChar uDash(nUniDash);

    sResult << uSpace << uDash << uSpace;

    return sResult;
}

// en dash - U+2013, em dash - U+2014, thin space - U+2009
wxDashesCorrector::wxDashesCorrector(bool smallEmDash):
    wxDashesCorrector(0x2009, 0x2013, smallEmDash ? 0xFE58 : 0x2014, 0x2E3A)
{
}

wxDashesCorrector& wxDashesCorrector::SmallEmDash(bool smallEmDash)
{
    return Init(0x2009, 0x2013, smallEmDash ? 0xFE58 : 0x2014);
}

wxStringProcessor* const wxDashesCorrector::Clone() const
{
    wxDashesCorrector* const res = new wxDashesCorrector(m_space, m_enDash, m_emDash, m_em2Dash);

    return res;
}

wxDashesCorrector::wxDashesCorrector(uvalue_type nUniSpace, uvalue_type nUniEnDash, uvalue_type nUniEmDash, uvalue_type nUniEm2Dash):
    m_space(nUniSpace), m_enDash(nUniEnDash), m_emDash(nUniEmDash), m_em2Dash(nUniEm2Dash),
    m_reEn(REG_EX_EN), m_reEm(REG_EX_EM), m_reEm2(REG_EX_EM2),
    m_sEnReplacement(GetReplacement(nUniSpace, nUniEnDash)),
    m_sEmReplacement(GetReplacement(nUniSpace, nUniEmDash)),
    m_sEm2Replacement(GetReplacement(nUniSpace, nUniEm2Dash))
{
    wxASSERT(m_reEn.IsValid());
    wxASSERT(m_reEm.IsValid());
    wxASSERT(m_reEm2.IsValid());
}

wxDashesCorrector& wxDashesCorrector::Init(uvalue_type nUniSpace, uvalue_type nUniEnDash, uvalue_type nUniEmDash, uvalue_type nUniEm2Dash)
{
    m_space = nUniSpace;
    m_enDash = nUniEnDash;
    m_emDash = nUniEmDash;
    m_em2Dash = nUniEm2Dash;

    m_sEnReplacement = GetReplacement(nUniSpace, nUniEnDash);
    m_sEmReplacement = GetReplacement(nUniSpace, nUniEmDash);
    m_sEm2Replacement = GetReplacement(nUniSpace, nUniEm2Dash);

    return *this;
}

const wxRegEx& wxDashesCorrector::GetEnRegEx() const
{
    return m_reEn;
}

const wxRegEx& wxDashesCorrector::GetEmRegEx() const
{
    return m_reEm;
}

const wxRegEx& wxDashesCorrector::GetEm2RegEx() const
{
    return m_reEm2;
}

bool wxDashesCorrector::Process(const wxString& sIn, wxString& sOut) const
{
    int      repl = 0;
    wxString w(sIn);

    repl += m_reEn.ReplaceAll(&w, m_sEnReplacement);
    repl += m_reEm.ReplaceAll(&w, m_sEmReplacement);
    repl += m_reEm2.ReplaceAll(&w, m_sEm2Replacement);

    if (repl > 0)
    {
        sOut = w;
        return true;
    }
    else
    {
        return false;
    }
}

