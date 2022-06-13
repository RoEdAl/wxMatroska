/*
 *      wxNoConv.h
 */

#ifndef _NO_CONV_H_
#define _NO_CONV_H_

 // From Unicode to Unicode, just copying
class wxNoConv:
    public wxMBConv
{
    public:

    wxNoConv()
    {}

    public:

    virtual size_t ToWChar(
        wchar_t* dst , size_t dstLen ,
        const char* src , size_t srcLen = wxNO_LEN ) const
    {
        size_t nLen = srcLen;

        if ( srcLen == wxNO_LEN )
        {
            const wchar_t* wsrc = (const wchar_t*)src;
            nLen = wxStrlen( wsrc ) * sizeof( wchar_t );
        }

        if ( dst != NULL && ( dstLen * sizeof( wchar_t ) ) < nLen ) nLen = dstLen * sizeof( wchar_t );

        if ( dst != NULL && nLen >= sizeof( wchar_t ) ) memcpy( dst , src , nLen );

        return ( nLen >= sizeof( wchar_t ) ) ? nLen / sizeof( wchar_t ) : wxCONV_FAILED;
    }

    virtual size_t FromWChar(
        char* dst , size_t dstLen ,
        const wchar_t* src , size_t srcLen = wxNO_LEN ) const
    {
        size_t nLen = srcLen * sizeof( wchar_t );

        if ( srcLen == wxNO_LEN ) nLen = wxStrlen( src ) * sizeof( wchar_t );

        if ( dst != NULL && dstLen < nLen ) nLen = srcLen;

        if ( dst != NULL && nLen > 0u ) memcpy( dst , src , nLen );

        return ( nLen == 0 ) ? wxCONV_FAILED : nLen;
    }

    virtual size_t GetMBNulLen() const
    {
        return sizeof( wchar_t );
    }

    virtual wxMBConv* Clone() const
    {
        return new wxNoConv();
    }
};

#endif  // _NO_CONV_H_

