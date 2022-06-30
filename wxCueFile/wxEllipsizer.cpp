/*
 * wxEllipsizer.cpp
 */

#include "wxEllipsizer.h"

namespace
{
    constexpr char REG_EX[] = "(\\p{Xps}*\\.\\.\\.)\\p{Xps}*$";
    constexpr wxUChar ELLIPSIS = wxS('\u2026');
}


wxEllipsizer::wxEllipsizer():
    wxRegExStringProcessor(REG_EX, ELLIPSIS)
{
}

wxStringProcessor* const wxEllipsizer::Clone() const
{
    return new wxEllipsizer();
}

