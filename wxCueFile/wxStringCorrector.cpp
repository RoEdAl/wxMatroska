/*
 * wxStringCorrector.cpp
 */

#include "wxStringCorrector.h"
#include "wxTrailingSpacesRemover.h"
#include "wxReduntantSpacesRemover.h"
#include "wxEllipsizer.h"
#include "wxRomanNumeralsConv.h"
#include "wxDashesCorrector.h"
#include "wxNumberFullStopCorrector.h"
#include "wxSmallLetterParenthesizedCorrector.h"
#include <wxCueFile/wxAsciiToUnicode.h>

wxStringCorrector::Configurator::Configurator()
    : m_removeExtraSpaces(true),
    m_ellipsize(false),
    m_romanNumeralsUpper(false),
    m_romanNumeralsLower(false),
    m_dashes(true),
    m_numberFullStop(false),
    m_smallLetterParenthesized(false),
    m_asciiToUnicode(false)
{
}

wxStringCorrector::Configurator::Configurator(const wxStringCorrector::Configurator& configurator)
    : m_removeExtraSpaces(configurator.m_removeExtraSpaces),
    m_ellipsize(configurator.m_ellipsize),
    m_romanNumeralsUpper(configurator.m_romanNumeralsUpper),
    m_romanNumeralsLower(configurator.m_romanNumeralsLower),
    m_dashes(configurator.m_dashes),
    m_numberFullStop(configurator.m_numberFullStop),
    m_smallLetterParenthesized(configurator.m_smallLetterParenthesized),
    m_asciiToUnicode(configurator.m_asciiToUnicode)
{
}

wxStringCorrector::Configurator& wxStringCorrector::Configurator::RemoveExtraSpaces(bool removeExtraSpaces)
{
    m_removeExtraSpaces = removeExtraSpaces;
    return *this;
}

wxStringCorrector::Configurator& wxStringCorrector::Configurator::Ellipsize(bool ellipsize)
{
    m_ellipsize = ellipsize;
    return *this;
}

wxStringCorrector::Configurator& wxStringCorrector::Configurator::RomanNumeralsUpper(bool romanNumeralsUpper)
{
    m_romanNumeralsUpper = romanNumeralsUpper;
    return *this;
}

wxStringCorrector::Configurator& wxStringCorrector::Configurator::RomanNumeralsLower(bool romanNumeralsLower)
{
    m_romanNumeralsLower = romanNumeralsLower;
    return *this;
}

wxStringCorrector::Configurator& wxStringCorrector::Configurator::Dashes(bool dashes)
{
    m_dashes = dashes;
    return *this;
}

wxStringCorrector::Configurator& wxStringCorrector::Configurator::SmallEmDash(bool smallEmDash)
{
    m_smallEmDash = smallEmDash;
    return *this;
}

wxStringCorrector::Configurator& wxStringCorrector::Configurator::NumberFullStop(bool numberFullStop)
{
    m_numberFullStop = numberFullStop;
    return *this;
}

wxStringCorrector::Configurator& wxStringCorrector::Configurator::SmallLetterParenthesized(bool smallLetterParenthesized)
{
    m_smallLetterParenthesized = smallLetterParenthesized;
    return *this;
}

wxStringCorrector::Configurator& wxStringCorrector::Configurator::AsciiToUnicode(bool asciiToUnicode)
{
    m_asciiToUnicode = asciiToUnicode;
    return *this;
}

bool wxStringCorrector::Configurator::RemoveExtraSpaces() const
{
    return m_removeExtraSpaces;
}

bool wxStringCorrector::Configurator::Ellipsize() const
{
    return m_ellipsize;
}

bool wxStringCorrector::Configurator::RomanNumeralsUpper() const
{
    return m_romanNumeralsUpper;
}

bool wxStringCorrector::Configurator::RomanNumeralsLower() const
{
    return m_romanNumeralsLower;
}

bool wxStringCorrector::Configurator::Dashes() const
{
    return m_dashes;
}

bool wxStringCorrector::Configurator::NumberFullStop() const
{
    return m_numberFullStop;
}

bool wxStringCorrector::Configurator::SmallLetterParenthesized() const
{
    return m_smallLetterParenthesized;
}

bool wxStringCorrector::Configurator::AsciiToUnicode() const
{
    return m_asciiToUnicode;
}

wxStringProcessor* const wxStringCorrector::Configurator::Create() const
{
    wxStringCorrector* res = new wxStringCorrector(*this);

    res->AddStringProcessor(new wxTrailingSpacesRemover());
    if (m_numberFullStop) res->AddStringProcessor(new wxNumberFullStopCorrector());
    if (m_removeExtraSpaces) res->AddStringProcessor(new wxReduntantSpacesRemover());
    if (m_ellipsize) res->AddStringProcessor(new wxEllipsizer());
    if (m_romanNumeralsUpper) res->AddStringProcessor(new wxRomanNumeralsConv< true >());
    if (m_romanNumeralsLower) res->AddStringProcessor(new wxRomanNumeralsConv< false >());
    if (m_dashes) res->AddStringProcessor(new wxDashesCorrector(m_smallEmDash));
    if (m_smallLetterParenthesized) res->AddStringProcessor(new wxSmallLetterParenthesizedCorrector());
    if (m_asciiToUnicode) res->AddStringProcessor(new wxAsciiToUnicode());
    return res;
}

wxStringProcessor* const wxStringCorrector::Clone() const
{
    wxStringCorrector* const res = new wxStringCorrector(m_configurator);

    for (size_t i = 0, cnt = m_processors.GetCount(); i < cnt; ++i)
    {
        res->AddStringProcessor(m_processors[i].Clone());
    }

    return res;
}

wxStringCorrector::wxStringCorrector(const wxStringCorrector::Configurator& configurator)
    : m_configurator(configurator)
{
}

void wxStringCorrector::AddStringProcessor(wxStringProcessor* const processor)
{
    m_processors.Add(processor);
}

bool wxStringCorrector::Process(const wxString& in, wxString& out) const
{
    wxString w(in);
    wxString wo;
    bool     processed = false;

    for (size_t i = 0, cnt = m_processors.GetCount(); i < cnt; ++i)
    {
        if (m_processors[i].Process(w, wo))
        {
            w = wo;
            processed = true;
        }
    }

    if (!processed) return false;
    out = w;
    return true;
}

