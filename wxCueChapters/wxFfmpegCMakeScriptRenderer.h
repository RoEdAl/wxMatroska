/*
 * wxFfmpegCMakeScriptRenderer.h
 */

#ifndef _WX_FFMPEG_CMAKE_SCRIPT_RENDERER_H_
#define _WX_FFMPEG_CMAKE_SCRIPT_RENDERER_H_

#ifndef _WX_CMAKE_SCRIPT_RENDERER_H_
#include "wxCMakeScriptRenderer.h"
#endif

class wxFfmpegCMakeScriptRenderer:
    public wxCMakeScriptRenderer
{
    public:

    wxFfmpegCMakeScriptRenderer(const wxConfiguration&);

    void RenderDisc(const wxCueSheet&, const wxFileName&, const wxInputFile&, const wxFileName&);
    void RenderDiscDraft(const wxCueSheet&, const wxFileName&, const wxString&, bool, bool) const;

    bool Save(const wxFileName&);
    bool SaveDraft(const wxFileName&, const wxString&, wxFileName&, wxFileName&);

    protected:

    void render_ffmpeg_codec(const wxArrayDataFile&) const;

};

#endif

