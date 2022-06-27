/*
 * wxCMakeScriptRenderer.h
 */

#ifndef _WX_CMAKE_SCRIPT_RENDERER_H_
#define _WX_CMAKE_SCRIPT_RENDERER_H_

#ifndef _WX_PRIMITIVE_RENDERER_H_
#include "wxPrimitiveRenderer.h"
#endif

class wxCMakeScriptRenderer:
    public wxPrimitiveRenderer
{
    protected:

    wxCMakeScriptRenderer(const wxConfiguration&);

    public:

    static wxString GetCMakePath(const wxFileName&);

    void RenderHeader() const;
    void RenderMinimumVersion() const;

    void RenderToolFinder(const wxString&, const wxString&) const;
    void RenderFfmpegFinder() const;
    void RenderToolEnvCheck(const wxString&) const;

    bool SaveScript(const wxFileName&);
};

#endif

