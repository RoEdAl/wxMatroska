/*
 *  wxStringEx.h
 */

#ifndef _WX_STRING_EX_H_
#define _WX_STRING_EX_H_

#include "wxTextOutputStreamOnString.h"

struct wxStringEx
{
    template< typename C >
    static inline int Replace(const wxRegEx& regEx, wxString& text, const C& replacer, size_t maxMatches)
    {
        wxCHECK_MSG(regEx.IsValid(), wxNOT_FOUND, wxT("must successfully Compile() first"));

        // the input string
    #ifndef WXREGEX_CONVERT_TO_MB
        const wxChar* textstr = text.c_str();
        size_t        textlen = text.length();
    #else
        const wxWX2MBbuf textstr = WXREGEX_CHAR(*text);

        if (!textstr)
        {
            wxLogError(_("Failed to find match for regular expression: %s"),
                    GetErrorMsg(0, true).c_str());
            return 0;
        }
        size_t textlen = strlen(textstr);
        text->clear();
    #endif

        // the replacement text
        wxString textNew;

        // the result
        wxTextOutputStreamOnString result;

        // matches
        wxArrayString matches;

        // the position where we start looking for the match
        size_t matchStart = 0;

        // number of replacement made: we won't make more than maxMatches of them
        // (unless maxMatches is 0 which doesn't limit the number of replacements)
        size_t countRepl = 0;

        // note that "^" shouldn't match after the first call to Matches() so we
        // use wxRE_NOTBOL to prevent it from happening
        while ((!maxMatches || countRepl < maxMatches) &&
                regEx.Matches(
           #ifndef WXREGEX_CONVERT_TO_MB
               textstr + matchStart,
           #else
               textstr.data() + matchStart,
           #endif
               countRepl ? wxRE_NOTBOL : 0,
               textlen - matchStart))
        {
            size_t matchCount = regEx.GetMatchCount();
            matches.SetCount(matchCount);

            size_t start, len;

            for (size_t i = 1; i < matchCount; ++i)
            {
                if (!regEx.GetMatch(&start, &len, i))
                {
                    // we did have match as Matches() returned true above!
                    wxFAIL_MSG(wxT("internal logic error in wxStringEx::Replace"));

                    return wxNOT_FOUND;
                }

                matches[i] = wxString(
#ifndef WXREGEX_CONVERT_TO_MB
                        textstr + matchStart + start,
#else
                        textstr.data() + matchStart + start,
#endif
                        len);
            }

            if (!regEx.GetMatch(&start, &len))
            {
                // we did have match as Matches() returned true above!
                wxFAIL_MSG(wxT("internal logic error in wxStringEx::Replace"));

                return wxNOT_FOUND;
            }

            matches[0] = wxString(
#ifndef WXREGEX_CONVERT_TO_MB
                    textstr + matchStart + start,
#else
                    textstr.data() + matchStart + start,
#endif
                    len);

            textNew = replacer(matches);

            if (start > 0)
            {
            #ifndef WXREGEX_CONVERT_TO_MB
                (*result) << text.Mid(matchStart, start);
            #else
                (*result) << wxString(textstr.data() + matchStart, *wxConvCurrent, start);
            #endif
                matchStart += start;
            }

            (*result) << textNew;
            matchStart += len;

            countRepl++;
        }

    #ifndef WXREGEX_CONVERT_TO_MB
        (*result) << text.Mid(matchStart);
    #else
        (*result) << wxString(textstr.data() + matchStart, *wxConvCurrent);
    #endif
        result->Flush();
        text = result.GetString();

        return countRepl;
    }
};

#endif

