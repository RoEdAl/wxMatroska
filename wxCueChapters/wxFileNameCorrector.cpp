/*
 * wxFileNameCorrector.cpp
 */

#include "wxFileNameCorrector.h"

// ===============================================================================

const char wxFileNameCorrector::REG_EX[] = "[\\p{Cc}\\p{Cf}]+";

// ===============================================================================

namespace
{
    struct char_replacement
    {
        char f;
        wxUChar r;
    };

    const char_replacement replacements[] = {
        {'/',  wxS('\u2215')},  // DIVISION SLASH
        {'\\', wxS('\uFE68')},  // SMALL REVERSE SOLIDUS
        {'?',  wxS('\uFE56')},  // SMALL QUESTION MARK
        {'*',  wxS('\uFE61')},  // SMALL ASTERISK
        {':',  wxS('\uFE55')},  // SMALL COLON
        {';',  wxS('\uFE54')},  // SMALL SEMICOLON
        {'|',  wxS('\uFF5C')},  // FULLWIDTH VERTICAL LINE
        {'^',  wxS('\uFF3E')},  // FULLWIDTH CIRCUMFLEX ACCENT
        {'<',  wxS('\uFE64')},  // SMALL LESS-THAN SIGN
        {'>',  wxS('\uFE65')},  // SMALL GREATER-THAN SIGN
        {'.',  wxS('\uFE52')}   // SMALL FULL STOP
    };

    template<size_t SIZE>
    bool replace_characters(const wxString& in, wxString& out, const char_replacement(&repl)[SIZE])
    {
        wxString res(in);
        size_t cnt = 0;
        for (size_t i = 0; i < SIZE; ++i)
        {
            cnt += res.Replace(repl[i].f, repl[i].r);
        }

        if (cnt > 0)
        {
            out = res;
            return true;
        }

        return false;
    }
}

wxFileNameCorrector::wxFileNameCorrector():
    m_reNonPrintable(REG_EX)
{
    wxASSERT(m_reNonPrintable.IsValid());
}

wxStringProcessor* const wxFileNameCorrector::Clone() const
{
    return new wxFileNameCorrector();
}

bool wxFileNameCorrector::Process(const wxString& in, wxString& out) const
{
    wxString res1(in);
    int      repl = m_reNonPrintable.ReplaceAll(&res1, wxEmptyString);
    if (repl > 0)
    {
        wxString res2;
        if (replace_characters(res1, res2, replacements))
        {
            out = res2;
        }
        else
        {
            out = res1;
        }
        return true;
    }
    else
    {
        wxString res2;
        if (replace_characters(in, res2, replacements))
        {
            out = res2;
            return true;
        }
        else
        {
            return false;
        }
    }
}

