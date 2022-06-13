/*
 * wxFfMetadataRenderer.h
 */

#ifndef _WX_FFMETADATA_RENDERER_H_
#define _WX_FFMETADATA_RENDERER_H_

#ifndef _WX_TAG_SYNONIMS_H_
#include <wxCueFile/wxTagSynonims.h>
#endif

#ifndef _WX_PRIMITIVE_RENDERER_H_
#include "wxPrimitiveRenderer.h"
#endif

class wxFfMetadataRenderer:
    public wxPrimitiveRenderer
{
    public:

    wxFfMetadataRenderer(const wxConfiguration&);

    void RenderDisc(const wxCueSheet&);
    bool Save(const wxFileName&);

    private:

    wxString get_matroska_title(const wxCueSheet&, const wxStringProcessor&) const;
    void render_tags(const wxArrayCueTag&, bool) const;
    const wxIndex& get_idx_from_first_track(const wxTrack&) const;
    const wxIndex& get_idx_from_second_track(const wxTrack&) const;

};

#endif

