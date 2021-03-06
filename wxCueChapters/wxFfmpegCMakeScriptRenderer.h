/*
 * wxFfmpegCMakeScriptRenderer.h
 */

#ifndef _WX_FFMPEG_CMAKE_SCRIPT_RENDERER_H_
#define _WX_FFMPEG_CMAKE_SCRIPT_RENDERER_H_

#ifndef _WX_CMAKE_SCRIPT_RENDERER_H_
#include "wxCMakeScriptRenderer.h"
#endif

class wxFfmpegCMakeScriptRenderer: public wxCMakeScriptRenderer
{
    wxDECLARE_NO_COPY_CLASS(wxFfmpegCMakeScriptRenderer);

    public:

    wxFfmpegCMakeScriptRenderer(const wxConfiguration&);

    void RenderDisc(const wxInputFile&, const wxCueSheet&, const wxString&, const wxFileName&, const wxFileName&);
    void RenderPre(const wxCueSheet&, const wxFileName&, const wxString&);

    bool SaveDraft(const wxFileName&, const wxString&, wxFileName&, wxFileName&);

    protected:

    void render_ffmpeg_codec(bool, const wxSamplingInfo&, bool, bool) const;

};

#endif

