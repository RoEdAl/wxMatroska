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
    public:

    class Configurator
    {
        friend class wxStringCorrector;

        public:

        Configurator();

        Configurator& RemoveExtraSpaces(bool = true);
        Configurator& Ellipsize(bool = true);
        Configurator& RomanNumeralsUpper(bool = true);
        Configurator& RomanNumeralsLower(bool = true);
        Configurator& Dashes(bool = true);
        Configurator& SmallEmDash(bool = true);
        Configurator& NumberFullStop(bool = true);
        Configurator& SmallLetterParenthesized(bool = true);
        Configurator& AsciiToUnicode(bool = true);

        bool RemoveExtraSpaces() const;
        bool Ellipsize() const;
        bool RomanNumeralsUpper() const;
        bool RomanNumeralsLower() const;
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
        bool m_romanNumeralsUpper;
        bool m_romanNumeralsLower;
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

