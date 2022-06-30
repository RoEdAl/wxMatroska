/*
 * wxTextOutputStreamOnString.h
 */

#ifndef _WX_TEXT_OUTPUT_STREAM_ON_STRING_H_
#define _WX_TEXT_OUTPUT_STREAM_ON_STRING_H_

#ifndef _NO_CONV_H_
#include "wxNoConv.h"
#endif

class wxTextOutputStreamOnString
{
    wxDECLARE_NO_COPY_CLASS(wxTextOutputStreamOnString);

    public:

    wxTextOutputStreamOnString();

    const wxMemoryOutputStream& GetMemoryStream() const;
    wxTextOutputStream& GetStream();
    wxTextOutputStream& operator *() const;
    wxTextOutputStream* operator ->() const;

    wxString GetString() const;

    static void SaveTo( wxTextOutputStream& , const wxString& );
    void SaveTo( wxTextOutputStream& ) const;

    protected:

    wxNoConv m_conv;
    wxMemoryOutputStream m_outputStream;
    wxTextOutputStream m_textOutputStream;
};

#endif

