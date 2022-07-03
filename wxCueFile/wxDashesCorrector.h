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
    wxDECLARE_NO_COPY_CLASS(wxDashesCorrector);

    public:

    typedef wxUniChar::value_type uvalue_type;

    static wxString GetReplacement(uvalue_type, uvalue_type);

    wxDashesCorrector(bool = false);

    wxDashesCorrector& SmallEmDash(bool = true);

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;

    protected:

    wxDashesCorrector(uvalue_type, uvalue_type, uvalue_type, uvalue_type = 0x2E3A);
    wxDashesCorrector& Init(uvalue_type, uvalue_type, uvalue_type, uvalue_type = 0x2E3A);

    protected:

    wxUniChar m_space;
    wxUniChar m_enDash;
    wxUniChar m_emDash;
    wxUniChar m_em2Dash;

    wxRegEx m_reNonBreakingHyphen;
    wxRegEx m_reEn;
    wxRegEx m_reEm;
    wxRegEx m_reEm2;

    wxString m_nonBreakingHyphenReplacement;
    wxString m_enReplacement;
    wxString m_emReplacement;
    wxString m_em2Replacement;
};

#endif

