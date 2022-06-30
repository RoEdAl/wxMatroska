/*
 * wxTextStreamUtils.cpp
 */

#include "StdWx.h"
#include <wxEncodingDetection/wxTextStreamUtils.h>

namespace
{
#include "wxMBConvUnaccent.hpp"
}

size_t wxTextStreamUtils::Copy(wxTextInputStream& inStream, wxTextOutputStream& outStream)
{
    wxString line;
    size_t cnt = 0;

    while (!inStream.GetInputStream().Eof())
    {
        line = inStream.ReadLine();
        if (line.IsEmpty())
        {
            outStream << endl;
        }
        else
        {
            outStream << line << endl;
        }
        cnt += 1;
    }
    outStream.Flush();
    return cnt;
}

size_t wxTextStreamUtils::Copy(wxTextInputStream& inStream, wxTextOutputStream& outStream, const wxString& separator)
{
    wxString line;
    size_t cnt = 0;

    while (!inStream.GetInputStream().Eof())
    {
        line = inStream.ReadLine();
        if (line.IsEmpty())
        {
            outStream << separator;
        }
        else
        {
            outStream << line << separator;
        }
        cnt += 1;
    }
    outStream.Flush();
    return cnt;
}

wxMBConv* wxTextStreamUtils::GetUnaccentMBConv()
{
    return new wxMBConvUnaccent();
}