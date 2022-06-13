/*
 * wxCueSheetRenderer.h
 */

#ifndef _WX_CUE_SHEET_RENDERER_H_
#define _WX_CUE_SHEET_RENDERER_H_

#ifndef _WX_TRACK_H_
class wxTrack;
class wxArrayTrack;
#endif

#ifndef _WX_INDEX_H_
class wxIndex;
class wxArrayIndex;
#endif

#ifndef _WX_CUE_SHEET_H_
class wxCueSheet;
#endif

class wxCueSheetRenderer
{
    protected:

    virtual bool RenderDisc(const wxCueSheet&);

    virtual bool OnPreRenderDisc(const wxCueSheet&);
    virtual bool OnRenderDisc(const wxCueSheet&);
    virtual bool OnPostRenderDisc(const wxCueSheet&);

    virtual bool RenderTracks(const wxCueSheet&, const wxArrayTrack&);

    virtual bool OnPreRenderTracks(const wxCueSheet&, const wxArrayTrack&);
    virtual bool OnRenderTracks(const wxCueSheet&, const wxArrayTrack&);
    virtual bool OnPostRenderTracks(const wxCueSheet&, const wxArrayTrack&);

    virtual bool RenderTrack(const wxCueSheet&, const wxTrack&);

    virtual bool OnPreRenderTrack(const wxCueSheet&, const wxTrack&);
    virtual bool OnRenderTrack(const wxCueSheet&, const wxTrack&);
    virtual bool OnPostRenderTrack(const wxCueSheet&, const wxTrack&);

    virtual bool RenderIndexes(const wxCueSheet&, const wxTrack&, const wxArrayIndex&);

    virtual bool OnPreRenderIndexes(const wxCueSheet&, const wxTrack&, const wxArrayIndex&);
    virtual bool OnRenderIndexes(const wxCueSheet&, const wxTrack&, const wxArrayIndex&);
    virtual bool OnPostRenderIndexes(const wxCueSheet&, const wxTrack&, const wxArrayIndex&);

    virtual bool RenderIndex(const wxCueSheet&, const wxTrack&, const wxIndex&);

    virtual bool OnPreRenderIndex(const wxCueSheet&, const wxTrack&, const wxIndex&);
    virtual bool OnRenderIndex(const wxCueSheet&, const wxTrack&, const wxIndex&);
    virtual bool OnPostRenderIndex(const wxCueSheet&, const wxTrack&, const wxIndex&);

    virtual bool RenderPreGap(const wxCueSheet&, const wxTrack&, const wxIndex&);

    virtual bool OnPreRenderPreGap(const wxCueSheet&, const wxTrack&, const wxIndex&);
    virtual bool OnRenderPreGap(const wxCueSheet&, const wxTrack&, const wxIndex&);
    virtual bool OnPostRenderPreGap(const wxCueSheet&, const wxTrack&, const wxIndex&);

    virtual bool RenderPostGap(const wxCueSheet&, const wxTrack&, const wxIndex&);

    virtual bool OnPreRenderPostGap(const wxCueSheet&, const wxTrack&, const wxIndex&);
    virtual bool OnRenderPostGap(const wxCueSheet&, const wxTrack&, const wxIndex&);
    virtual bool OnPostRenderPostGap(const wxCueSheet&, const wxTrack&, const wxIndex&);

    public:

    wxCueSheetRenderer(void);

    bool Render(const wxCueSheet&);
};

#endif

