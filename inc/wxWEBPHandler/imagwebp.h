/////////////////////////////////////////////////////////////////////////////
// Name:        wx/imagwebp.hpp
// Purpose:     wxImage WebP handler
// Author:      Hermann Höhne
// Copyright:   (c) Hermann Höhne
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGWEBP_H_
#define _WX_IMAGWEBP_H_

#include "wx/image.h"

//-----------------------------------------------------------------------------
// wxWEBPHandler
//-----------------------------------------------------------------------------

#if wxUSE_WEBP

#define wxBITMAP_TYPE_WEBP ((wxBitmapType)(wxBITMAP_TYPE_MACCURSOR_RESOURCE+1))

class wxWEBPHandler : public wxImageHandler
{
public:
    inline wxWEBPHandler()
    {
        m_name = wxT("WebP file");
        m_extension = wxT("webp");
        m_type = wxBITMAP_TYPE_WEBP;
        m_mime = wxT("image/webp");
    }

#if wxUSE_STREAMS
    virtual bool LoadFile(wxImage*, wxInputStream&, bool = true, int = -1) wxOVERRIDE;
    virtual bool SaveFile(wxImage*, wxOutputStream&, bool = true) wxOVERRIDE;
protected:
    virtual bool DoCanRead(wxInputStream&) wxOVERRIDE;
#endif // wxUSE_STREAMS

private:
    wxDECLARE_DYNAMIC_CLASS(wxWEBPHandler);
    wxDECLARE_NO_COPY_CLASS(wxWEBPHandler);
};

#endif // wxUSE_WEBP

#endif // _WX_IMAGWEBP_H_
