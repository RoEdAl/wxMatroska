/*
	wxCueSheetRenderer.h
*/

#ifndef _WX_CUE_SHEET_RENDERER_H_
#define _WX_CUE_SHEET_RENDERER_H_

#ifndef _WX_TRACK_H_
class wxTrack;
#endif

#ifndef _WX_INDEX_H_
class wxIndex;
class wxArrayIndex;
#endif

#ifndef _WX_DATA_FILE_H_
#include "wxDataFile.h"
#endif

#ifndef _WX_CUE_SHEET_H_
#include "wxCueSheet.h"
#endif

class wxCueSheetRenderer :public wxObject
{
	DECLARE_ABSTRACT_CLASS(wxCueSheetRenderer)

public:

	virtual bool RenderDisc( const wxCueSheet& );

	virtual bool OnPreRenderDisc( const wxCueSheet& );
	virtual bool OnRenderDisc( const wxCueSheet& );
	virtual bool OnPostRenderDisc( const wxCueSheet& );

	virtual bool RenderTracks( const wxArrayTrack& );

	virtual bool OnPreRenderTracks( const wxArrayTrack& );
	virtual bool OnRenderTracks( const wxArrayTrack& );
	virtual bool OnPostRenderTracks( const wxArrayTrack& );

	virtual bool RenderTrack( const wxTrack& );

	virtual bool OnPreRenderTrack( const wxTrack& );
	virtual bool OnRenderTrack( const wxTrack& );
	virtual bool OnPostRenderTrack( const wxTrack& );

	virtual bool RenderIndexes( const wxTrack&, const wxArrayIndex& );

	virtual bool OnPreRenderIndexes( const wxTrack&, const wxArrayIndex& );
	virtual bool OnRenderIndexes( const wxTrack&, const wxArrayIndex& );
	virtual bool OnPostRenderIndexes( const wxTrack&, const wxArrayIndex& );

	virtual bool RenderIndex( const wxTrack&, const wxIndex& );

	virtual bool OnPreRenderIndex( const wxTrack&, const wxIndex& );
	virtual bool OnRenderIndex( const wxTrack&, const wxIndex& );
	virtual bool OnPostRenderIndex( const wxTrack&, const wxIndex& );

	virtual bool RenderPreGap( const wxTrack&, const wxIndex& );

	virtual bool OnPreRenderPreGap( const wxTrack&, const wxIndex& );
	virtual bool OnRenderPreGap( const wxTrack&, const wxIndex& );
	virtual bool OnPostRenderPreGap( const wxTrack&, const wxIndex& );

	virtual bool RenderPostGap( const wxTrack&, const wxIndex& );

	virtual bool OnPreRenderPostGap( const wxTrack&, const wxIndex& );
	virtual bool OnRenderPostGap( const wxTrack&, const wxIndex& );
	virtual bool OnPostRenderPostGap( const wxTrack&, const wxIndex& );

public:

	wxCueSheetRenderer(void);
	~wxCueSheetRenderer(void);

	bool Render( const wxCueSheet& );
};

#endif
