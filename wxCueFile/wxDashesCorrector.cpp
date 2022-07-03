/*
 * wxDashesCorrector.cpp
 */

#include "wxDashesCorrector.h"

namespace
{
    constexpr char REG_EX_NONBREAKING_HYPHEN[] = "(?<=\\p{Xan})\\p{Pd}(?=\\p{Xan})";
    constexpr wxUChar NONBREAKING_HYPHEN = wxS('\u2011');

    constexpr char REG_EX_EN[] = "\\p{Xps}+\\p{Pd}\\p{Xps}+";
    constexpr char REG_EX_EM[] = "\\p{Xps}+\\p{Pd}{2}\\p{Xps}+";
    constexpr char REG_EX_EM2[] = "\\p{Xps}+\\p{Pd}{3,}\\p{Xps}+";
};

wxString wxDashesCorrector::GetReplacement(uvalue_type uniSpace, uvalue_type uniDash)
{
    wxString sResult;

    wxUniChar uSpace(uniSpace);
    wxUniChar uDash(uniDash);

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

wxDashesCorrector::wxDashesCorrector(uvalue_type uniSpace, uvalue_type uniEnDash, uvalue_type uniEmDash, uvalue_type uniEm2Dash):
    m_space(uniSpace), m_enDash(uniEnDash), m_emDash(uniEmDash), m_em2Dash(uniEm2Dash),
    m_reNonBreakingHyphen(REG_EX_NONBREAKING_HYPHEN),
    m_nonBreakingHyphenReplacement(NONBREAKING_HYPHEN),
    m_reEn(REG_EX_EN), m_reEm(REG_EX_EM), m_reEm2(REG_EX_EM2),
    m_enReplacement(GetReplacement(uniSpace, uniEnDash)),
    m_emReplacement(GetReplacement(uniSpace, uniEmDash)),
    m_em2Replacement(GetReplacement(uniSpace, uniEm2Dash))
{
    wxASSERT(m_reNonBreakingHyphen.IsValid());
    wxASSERT(m_reEn.IsValid());
    wxASSERT(m_reEm.IsValid());
    wxASSERT(m_reEm2.IsValid());
}

wxDashesCorrector& wxDashesCorrector::Init(uvalue_type uniSpace, uvalue_type uniEnDash, uvalue_type uniEmDash, uvalue_type uniEm2Dash)
{
    m_space = uniSpace;
    m_enDash = uniEnDash;
    m_emDash = uniEmDash;
    m_em2Dash = uniEm2Dash;

    m_enReplacement = GetReplacement(uniSpace, uniEnDash);
    m_emReplacement = GetReplacement(uniSpace, uniEmDash);
    m_em2Replacement = GetReplacement(uniSpace, uniEm2Dash);

    return *this;
}

bool wxDashesCorrector::Process(const wxString& in, wxString& out) const
{
    int      repl = 0;
    wxString w(in);

    repl += m_reNonBreakingHyphen.ReplaceAll(&w, m_nonBreakingHyphenReplacement);
    repl += m_reEn.ReplaceAll(&w, m_enReplacement);
    repl += m_reEm.ReplaceAll(&w, m_emReplacement);
    repl += m_reEm2.ReplaceAll(&w, m_em2Replacement);

    if (repl > 0)
    {
        out = w;
        return true;
    }
    else
    {
        return false;
    }
}

