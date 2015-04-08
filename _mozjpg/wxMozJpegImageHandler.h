/////////////////////////////////////////////////////////////////////////////
// Name:        wx/imagjpeg.h
// Purpose:     wxImage JPEG handler
// Author:      Vaclav Slavik
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAG_MOZJPEG_H_
#define _WX_IMAG_MOZJPEG_H_

#include "wx/defs.h"

//-----------------------------------------------------------------------------
// wxJPEGHandler
//-----------------------------------------------------------------------------


#include "wx/image.h"
#include "wx/versioninfo.h"

class wxMozJpegHandler: public wxImageHandler
{
    public:
    inline wxMozJpegHandler( )
    {
        m_name = wxT( "JPEG file" );
        m_extension = wxT( "jpg" );
        m_altExtensions.Add( wxT( "jpeg" ) );
        m_altExtensions.Add( wxT( "jpe" ) );
        m_type = wxBITMAP_TYPE_JPEG;
        m_mime = wxT( "image/jpeg" );
    }

    static wxVersionInfo GetLibraryVersionInfo( );

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose = true, int index = -1 );
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose = true );
    protected:
    virtual bool DoCanRead( wxInputStream& stream );
#endif

    private:
    DECLARE_DYNAMIC_CLASS( wxMozJpegHandler )
};

#endif // _WX_IMAG_MOZJPEG_H_

