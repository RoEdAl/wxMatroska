/*
 * wxDashesCorrector.h
 */

#ifndef _WX_DASHES_CORRECTOR_H_
#define _WX_DASHES_CORRECTOR_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

class wxDashesCorrector: public wxStringProcessor
{
    public:

    typedef wxUniChar::value_type uvalue_type;

    static const char REG_EX_EN[];
    static const char REG_EX_EM[];
    static const char REG_EX_EM2[];

    static wxString GetReplacement(uvalue_type, uvalue_type);

    wxDashesCorrector(bool = false);

    wxDashesCorrector& SmallEmDash(bool = true);

    wxStringProcessor* const Clone() const;
    virtual bool Process(const wxString&, wxString&) const;

    const wxRegEx& GetEnRegEx() const;
    const wxRegEx& GetEmRegEx() const;
    const wxRegEx& GetEm2RegEx() const;

    protected:

    wxDashesCorrector(uvalue_type, uvalue_type, uvalue_type, uvalue_type = 0x2E3A);
    wxDashesCorrector& Init(uvalue_type, uvalue_type, uvalue_type, uvalue_type = 0x2E3A);

    protected:

    wxUniChar m_space;
    wxUniChar m_enDash;
    wxUniChar m_emDash;
    wxUniChar m_em2Dash;

    wxRegEx m_reEn;
    wxRegEx m_reEm;
    wxRegEx m_reEm2;

    wxString m_enReplacement;
    wxString m_emReplacement;
    wxString m_em2Replacement;
};

#endif

