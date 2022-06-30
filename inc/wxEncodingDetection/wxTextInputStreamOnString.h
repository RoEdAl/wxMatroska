/*
 * wxTextInputStreamOnString.h
 */

#ifndef _WX_TEXT_INPUT_STREAM_ON_STRING_H_
#define _WX_TEXT_INPUT_STREAM_ON_STRING_H_

#ifndef _NO_CONV_H_
#include "wxNoConv.h"
#endif

class wxTextInputStreamOnString
{
    wxDECLARE_NO_COPY_CLASS(wxTextInputStreamOnString);

    public:

    typedef wxCharTypeBuffer< wxChar > CharBufferType;

    wxTextInputStreamOnString( const wxString& );
    wxTextInputStreamOnString( const CharBufferType& );

    const wxMemoryInputStream& GetMemoryStream() const;
    wxTextInputStream& GetStream();
    const CharBufferType& GetBuffer() const;
    wxTextInputStream& operator *() const;
    bool Eof() const;

    protected:

    wxNoConv m_conv;
    CharBufferType m_charBuffer;
    wxMemoryInputStream m_inputStream;
    wxTextInputStream m_textInputStream;
};

#endif

