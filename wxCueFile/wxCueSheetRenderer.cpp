/*
	wxCueSheetRenderer.cpp
*/

#include "StdWx.h"
#include "wxIndex.h"
#include "wxTrack.h"
#include "wxCueSheetRenderer.h"

IMPLEMENT_ABSTRACT_CLASS( wxCueSheetRenderer, wxObject )

wxCueSheetRenderer::wxCueSheetRenderer(void)
{
}

wxCueSheetRenderer::~wxCueSheetRenderer(void)
{
}

bool wxCueSheetRenderer::Render(const wxCueSheet& cueSheet)
{
	return RenderDisc( cueSheet );
}

bool wxCueSheetRenderer::RenderDisc(const wxCueSheet& cueSheet)
{
	if ( !OnPreRenderDisc( cueSheet ) )
	{
		return false;
	}

	if ( !OnRenderDisc( cueSheet ) )
	{
		return false;
	}	

	if ( !OnPostRenderDisc( cueSheet ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderDisc(const wxCueSheet& WXUNUSED(cueSheet) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderDisc(const wxCueSheet& cueSheet)
{
	if ( !RenderTracks( cueSheet.GetTracks() ) )
	{
		return false;
	}	

	return true;
}

bool wxCueSheetRenderer::OnPostRenderDisc(const wxCueSheet& WXUNUSED(cueSheet) )
{
	return true;
}


bool wxCueSheetRenderer::RenderTracks( const wxArrayTrack& tracks )
{
	if ( !OnPreRenderTracks( tracks ) )
	{
		return false;
	}

	if ( !OnRenderTracks( tracks ) )
	{
		return false;
	}

	if ( !OnPostRenderTracks( tracks ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderTracks( const wxArrayTrack& WXUNUSED(tracks) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderTracks( const wxArrayTrack& tracks )
{
	size_t tracksCnt = tracks.Count();
	for( size_t i=0; i<tracksCnt; i++ )
	{
		if ( !RenderTrack( tracks[i] ) )
		{
			return false;
		}
	}

	return true;
}

bool wxCueSheetRenderer::OnPostRenderTracks( const wxArrayTrack& WXUNUSED(tracks) )
{
	return true;
}

bool wxCueSheetRenderer::RenderTrack(const wxTrack& track)
{
	if ( !OnPreRenderTrack(track) )
	{
		return false;
	}

	if ( !OnRenderTrack(track) )
	{
		return false;
	}

	if ( !OnPostRenderTrack(track) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderTrack(const wxTrack& WXUNUSED(track) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderTrack(const wxTrack& track )
{
	if ( !RenderIndexes( track, track.GetIndexes() ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPostRenderTrack(const wxTrack& WXUNUSED(track) )
{
	return true;
}

bool wxCueSheetRenderer::RenderIndexes( const wxTrack& track, const wxArrayIndex& indexes )
{
	if ( !OnPreRenderIndexes( track, indexes ) )
	{
		return false;
	}

	if ( !OnRenderIndexes( track, indexes ) )
	{
		return false;
	}

	if ( !OnPostRenderIndexes( track, indexes ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderIndexes( const wxTrack& WXUNUSED(track), const wxArrayIndex& WXUNUSED(indexes) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderIndexes( const wxTrack& track, const wxArrayIndex& indexes )
{
	if ( track.HasPreGap() )
	{
		if ( !RenderPreGap( track, track.GetPreGap() ) )
		{
			return false;
		}
	}

	for( size_t i=0; i<indexes.Count(); i++ )
	{
		if ( !RenderIndex( track, indexes[i] ) )
		{
			return false;
		}
	}

	if ( track.HasPostGap() )
	{
		if ( !RenderPostGap( track, track.GetPostGap() ) )
		{
			return false;
		}
	}

	return true;
}

bool wxCueSheetRenderer::OnPostRenderIndexes( const wxTrack& WXUNUSED(track), const wxArrayIndex& WXUNUSED(indexes) )
{
	return true;
}

bool wxCueSheetRenderer::RenderIndex( const wxTrack& track, const wxIndex& index )
{
	if ( !OnPreRenderIndex( track, index ) )
	{
		return false;
	}

	if ( !OnRenderIndex( track, index ) )
	{
		return false;
	}

	if ( !OnPostRenderIndex( track, index ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderIndex( const wxTrack& WXUNUSED(track), const wxIndex& WXUNUSED(index) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderIndex( const wxTrack& WXUNUSED(track), const wxIndex& WXUNUSED(index) )
{
	return true;
}

bool wxCueSheetRenderer::OnPostRenderIndex( const wxTrack& WXUNUSED(track), const wxIndex& WXUNUSED(index) )
{
	return true;
}

bool wxCueSheetRenderer::RenderPreGap(const wxTrack& track, const wxIndex& preGap )
{
	if ( !OnPreRenderPreGap( track, preGap ) )
	{
		return false;
	}

	if ( !OnRenderPreGap( track, preGap ) )
	{
		return false;
	}

	if ( !OnPostRenderPreGap( track, preGap ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderPreGap(const wxTrack& WXUNUSED(track), const wxIndex& WXUNUSED(preGap) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderPreGap(const wxTrack& WXUNUSED(track), const wxIndex& WXUNUSED(preGap) )
{
	return true;
}

bool wxCueSheetRenderer::OnPostRenderPreGap(const wxTrack& WXUNUSED(track), const wxIndex& WXUNUSED(preGap) )
{
	return true;
}

bool wxCueSheetRenderer::RenderPostGap(const wxTrack& track, const wxIndex& postGap )
{
	if ( !OnPreRenderPostGap( track, postGap ) )
	{
		return false;
	}

	if ( !OnRenderPostGap( track, postGap ) )
	{
		return false;
	}

	if ( !OnPostRenderPostGap( track, postGap ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderPostGap(const wxTrack& WXUNUSED(track), const wxIndex& WXUNUSED(preGap) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderPostGap(const wxTrack& WXUNUSED(track), const wxIndex& WXUNUSED(preGap) )
{
	return true;
}

bool wxCueSheetRenderer::OnPostRenderPostGap(const wxTrack& WXUNUSED(track), const wxIndex& WXUNUSED(preGap) )
{
	return true;
}
