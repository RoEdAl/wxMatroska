/*
 * wxStringCorrector.h
 */

#ifndef _WX_STRING_CORRECTOR_H_
#define _WX_STRING_CORRECTOR_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

class wxStringCorrector: public wxStringProcessor
{
    wxDECLARE_NO_COPY_CLASS(wxStringCorrector);

    public:

    class Configurator
    {
        friend class wxStringCorrector;
        wxDECLARE_NO_ASSIGN_CLASS(Configurator);

        public:

        Configurator();

        Configurator& RemoveExtraSpaces(bool = true);
        Configurator& Ellipsize(bool = true);
        Configurator& QuoteCorrector(bool = true);
        Configurator& QuoteCorrectorLang(const wxString&);
        Configurator& RomanNumeralsUpper(bool = true);
        Configurator& RomanNumeralsLower(bool = true);
        Configurator& StrongRomanNumeralsParser(bool = true);
        Configurator& Dashes(bool = true);
        Configurator& SmallEmDash(bool = true);
        Configurator& NumberFullStop(bool = true);
        Configurator& SmallLetterParenthesized(bool = true);
        Configurator& AsciiToUnicode(bool = true);

        bool RemoveExtraSpaces() const;
        bool Ellipsize() const;
        bool QuoteCorrector() const;
        const wxString& QuoteCorrectorLang() const;
        bool RomanNumeralsUpper() const;
        bool RomanNumeralsLower() const;
        bool StrongRomanNumeralsParser() const;
        bool Dashes() const;
        bool NumberFullStop() const;
        bool SmallLetterParenthesized() const;
        bool AsciiToUnicode() const;

        wxStringProcessor* const Create() const;

        protected:

        Configurator(const Configurator&);

        protected:

        bool m_removeExtraSpaces;
        bool m_ellipsize;
        bool m_quoteCorrector;
        wxString m_quoteCorrectorLang;
        bool m_romanNumeralsUpper;
        bool m_romanNumeralsLower;
        bool m_strongRomanNumeralsParser;
        bool m_dashes;
        bool m_smallEmDash;
        bool m_numberFullStop;
        bool m_smallLetterParenthesized;
        bool m_asciiToUnicode;
    };

    virtual wxStringProcessor* const Clone() const wxOVERRIDE;
    virtual bool Process(const wxString&, wxString&) const wxOVERRIDE;

    protected:

    wxStringCorrector(const Configurator&);
    void AddStringProcessor(wxStringProcessor* const);

    protected:

    Configurator m_configurator;
    wxArrayStringProcessor m_processors;
};

#endif

