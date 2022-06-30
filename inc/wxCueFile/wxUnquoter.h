/*
 * wxUnquoter.h
 */

#ifndef _WX_UNQUOTER_H_
#define _WX_UNQUOTER_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

class wxUnquoter: public wxStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxUnquoter);

    protected:

    wxRegEx m_reSingleQuotes;
    wxRegEx m_reDoubleQuotes;

    public:

    wxUnquoter(void);

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;

    bool IsQuoted(const wxString&) const;
};

class wxQuoteCorrector: public wxStringProcessor
{
    wxDECLARE_NO_ASSIGN_CLASS(wxQuoteCorrector);

    protected:

    wxRegEx m_reSingleQuotes;
    wxRegEx m_reDoubleQuotes;
    wxRegEx m_rePseudoDoubleQuotes;

    wxString m_genericReplacement;

    public:

    struct QUOTATION_MARKS
    {
        const wxChar* opening;
        const wxChar* closing;
    };

    static bool correct_polish_qm(const wxString&);
    static bool correct_english_qm(const wxString&);
    static bool correct_german_qm(const wxString&);
    static bool correct_french_qm(const wxString&);

    protected:

    typedef wxScopedCharTypeBuffer< wxChar > wxScopedCharBuffer;
    typedef wxScopedCharBuffer(wxQuoteCorrector::* GET_REPLACEMENT_METHOD)(int, bool) const;

    wxScopedCharBuffer get_standard_replacement(int, bool) const;
    wxScopedCharBuffer get_english_replacement(int, bool) const;
    wxScopedCharBuffer get_polish_replacement(int, bool) const;
    wxScopedCharBuffer get_german_replacement(int, bool) const;
    wxScopedCharBuffer get_french_replacement(int, bool) const;

    template< size_t SIZE >
    static wxScopedCharBuffer get_replacement(const QUOTATION_MARKS(&quotation_marks_array)[SIZE], int nLevel, bool bOpening)
    {
        if (nLevel < 0) return wxScopedCharBuffer();

        if (bOpening) return wxScopedCharBuffer::CreateNonOwned(quotation_marks_array[nLevel % SIZE].opening);
        else return wxScopedCharBuffer::CreateNonOwned(quotation_marks_array[nLevel % SIZE].closing);
    }

    wxQuoteCorrector(const wxQuoteCorrector&);
    bool InternalCorrectQuotes(const wxString&, wxString&) const;

    protected:

    GET_REPLACEMENT_METHOD m_replacement_method;

    public:

    wxQuoteCorrector();

    void SetLang(const wxString&);

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;

    bool IsQuoted(const wxString&) const;
};

#endif

