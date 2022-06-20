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
    wxJson RenderChapters(const wxCueSheet&) const;

    bool Save(const wxFileName&);
    bool SaveChapters(const wxJson&, const wxFileName&);

    private:

    wxString get_matroska_title(const wxCueSheet&, const wxStringProcessor&) const;
    void render_tags(const wxArrayCueTag&, bool) const;
    const wxIndex& get_idx_from_first_track(const wxTrack&) const;
    static const wxIndex& get_idx_from_first_track_no_conf(const wxTrack&);
    static const wxIndex& get_idx_from_second_track(const wxTrack&);

};

#endif

