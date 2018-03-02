/*
 *      WaveDrawerRaster2.cpp
 */
#include "StdWx.h"
#include "Arrays.h"
#include "LogarithmicScale.h"
#include "ColourInterpolation.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "DrawerSettings.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerColumnPainter.h"
#include "WaveDrawerRaster2.h"

Raster2WaveDrawer::Raster2WaveDrawer( wxUint64 nNumberOfSamples,
									  wxGraphicsContext* gc,
									  const wxRect2DInt& rc,
									  const DrawerSettings& drawerSettings,
									  const ChaptersArrayScopedPtr& pChapters ) :
	ColumnPainterWaveDrawer( nNumberOfSamples, gc, rc, drawerSettings, pChapters )
{}

void Raster2WaveDrawer::GetThreeColours( wxFloat32 v, wxColour& clrTop, wxColour& clrMiddle, wxColour& clrBottom )
{
	clrTop    = m_drawerSettings.GetTopColourSettings().GetEdgeColour();
	clrMiddle = ColourInterpolation::linear_interpolation( m_drawerSettings.GetTopColourSettings().GetEdgeColour(), m_drawerSettings.GetTopColourSettings().GetMiddleColour(), v );
	clrBottom = m_drawerSettings.GetBottomColourSettings().GetEdgeColour();
}

void Raster2WaveDrawer::GetTwoColours( wxFloat32 v, bool bUp, wxColour& clrFrom, wxColour& clrTo )
{
	if ( bUp )
	{
		clrFrom = m_drawerSettings.GetTopColourSettings().GetEdgeColour();
		clrTo   = ColourInterpolation::linear_interpolation( m_drawerSettings.GetTopColourSettings().GetEdgeColour(), m_drawerSettings.GetTopColourSettings().GetMiddleColour(), v );
	}
	else
	{
		clrFrom = ColourInterpolation::linear_interpolation( m_drawerSettings.GetBottomColourSettings().GetEdgeColour(), m_drawerSettings.GetBottomColourSettings().GetMiddleColour(), v );
		clrTo   = m_drawerSettings.GetBottomColourSettings().GetEdgeColour();
	}
}

