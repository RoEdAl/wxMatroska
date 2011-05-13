/*
   wxCueSheetRenderer.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxCueSheetRenderer.h>

wxIMPLEMENT_ABSTRACT_CLASS( wxCueSheetRenderer, wxObject )

wxCueSheetRenderer * const wxCueSheetRenderer::Null = ( wxCueSheetRenderer* const )NULL;

wxCueSheetRenderer::wxCueSheetRenderer( void )
{}

bool wxCueSheetRenderer::Render( const wxCueSheet& cueSheet )
{
	return RenderDisc( cueSheet );
}

bool wxCueSheetRenderer::RenderDisc( const wxCueSheet& cueSheet )
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

bool wxCueSheetRenderer::OnPreRenderDisc( const wxCueSheet& WXUNUSED( cueSheet ) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderDisc( const wxCueSheet& cueSheet )
{
	if ( !RenderTracks( cueSheet, cueSheet.GetTracks() ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPostRenderDisc( const wxCueSheet& WXUNUSED( cueSheet ) )
{
	return true;
}

bool wxCueSheetRenderer::RenderTracks( const wxCueSheet& cueSheet, const wxArrayTrack& tracks )
{
	if ( !OnPreRenderTracks( cueSheet, tracks ) )
	{
		return false;
	}

	if ( !OnRenderTracks( cueSheet, tracks ) )
	{
		return false;
	}

	if ( !OnPostRenderTracks( cueSheet, tracks ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderTracks( const wxCueSheet& WXUNUSED( cueSheet ), const wxArrayTrack& WXUNUSED( tracks ) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderTracks( const wxCueSheet& cueSheet, const wxArrayTrack& tracks )
{
	size_t tracksCnt = tracks.Count();
	for ( size_t i = 0 ; i < tracksCnt ; i++ )
	{
		if ( !RenderTrack( cueSheet, tracks[ i ] ) )
		{
			return false;
		}
	}

	return true;
}

bool wxCueSheetRenderer::OnPostRenderTracks( const wxCueSheet& WXUNUSED( cueSheet ), const wxArrayTrack& WXUNUSED( tracks ) )
{
	return true;
}

bool wxCueSheetRenderer::RenderTrack( const wxCueSheet& cueSheet, const wxTrack& track )
{
	if ( !OnPreRenderTrack( cueSheet, track ) )
	{
		return false;
	}

	if ( !OnRenderTrack( cueSheet, track ) )
	{
		return false;
	}

	if ( !OnPostRenderTrack( cueSheet, track ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderTrack( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderTrack( const wxCueSheet& cueSheet, const wxTrack& track )
{
	if ( !RenderIndexes( cueSheet, track, track.GetIndexes() ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPostRenderTrack( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ) )
{
	return true;
}

bool wxCueSheetRenderer::RenderIndexes( const wxCueSheet& cueSheet, const wxTrack& track, const wxArrayIndex& indexes )
{
	if ( !OnPreRenderIndexes( cueSheet, track, indexes ) )
	{
		return false;
	}

	if ( !OnRenderIndexes( cueSheet, track, indexes ) )
	{
		return false;
	}

	if ( !OnPostRenderIndexes( cueSheet, track, indexes ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderIndexes( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ), const wxArrayIndex& WXUNUSED( indexes ) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderIndexes( const wxCueSheet& cueSheet, const wxTrack& track, const wxArrayIndex& indexes )
{
	if ( track.HasPreGap() )
	{
		if ( !RenderPreGap( cueSheet, track, track.GetPreGap() ) )
		{
			return false;
		}
	}

	for ( size_t i = 0 ; i < indexes.Count() ; i++ )
	{
		if ( !RenderIndex( cueSheet, track, indexes[ i ] ) )
		{
			return false;
		}
	}

	if ( track.HasPostGap() )
	{
		if ( !RenderPostGap( cueSheet, track, track.GetPostGap() ) )
		{
			return false;
		}
	}

	return true;
}

bool wxCueSheetRenderer::OnPostRenderIndexes( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ), const wxArrayIndex& WXUNUSED( indexes ) )
{
	return true;
}

bool wxCueSheetRenderer::RenderIndex( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& index )
{
	if ( !OnPreRenderIndex( cueSheet, track, index ) )
	{
		return false;
	}

	if ( !OnRenderIndex( cueSheet, track, index ) )
	{
		return false;
	}

	if ( !OnPostRenderIndex( cueSheet, track, index ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderIndex( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ), const wxIndex& WXUNUSED( index ) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderIndex( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ), const wxIndex& WXUNUSED( index ) )
{
	return true;
}

bool wxCueSheetRenderer::OnPostRenderIndex( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ), const wxIndex& WXUNUSED( index ) )
{
	return true;
}

bool wxCueSheetRenderer::RenderPreGap( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& preGap )
{
	if ( !OnPreRenderPreGap( cueSheet, track, preGap ) )
	{
		return false;
	}

	if ( !OnRenderPreGap( cueSheet, track, preGap ) )
	{
		return false;
	}

	if ( !OnPostRenderPreGap( cueSheet, track, preGap ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderPreGap( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ), const wxIndex& WXUNUSED( preGap ) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderPreGap( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ), const wxIndex& WXUNUSED( preGap ) )
{
	return true;
}

bool wxCueSheetRenderer::OnPostRenderPreGap( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ), const wxIndex& WXUNUSED( preGap ) )
{
	return true;
}

bool wxCueSheetRenderer::RenderPostGap( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& postGap )
{
	if ( !OnPreRenderPostGap( cueSheet, track, postGap ) )
	{
		return false;
	}

	if ( !OnRenderPostGap( cueSheet, track, postGap ) )
	{
		return false;
	}

	if ( !OnPostRenderPostGap( cueSheet, track, postGap ) )
	{
		return false;
	}

	return true;
}

bool wxCueSheetRenderer::OnPreRenderPostGap( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ), const wxIndex& WXUNUSED( preGap ) )
{
	return true;
}

bool wxCueSheetRenderer::OnRenderPostGap( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ), const wxIndex& WXUNUSED( preGap ) )
{
	return true;
}

bool wxCueSheetRenderer::OnPostRenderPostGap( const wxCueSheet& WXUNUSED( cueSheet ), const wxTrack& WXUNUSED( track ), const wxIndex& WXUNUSED( preGap ) )
{
	return true;
}