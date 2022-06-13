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

    void RenderDisc(const wxCueSheet&, const wxInputFile&, const wxFileName&);
    bool Save(const wxFileName&);

    protected:

};

#endif

