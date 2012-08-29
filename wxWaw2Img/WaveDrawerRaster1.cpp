/*
 *      WaveDrawerRaster1.cpp
 */
#include "StdWx.h"
#include "Arrays.h"
#include "ColourInterpolation.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "DrawerSettings.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerColumnPainter.h"
#include "WaveDrawerRaster1.h"

Raster1WaveDrawer::Raster1WaveDrawer( wxUint64 nNumberOfSamples,
									  wxGraphicsContext* gc,
									  const wxRect2DInt& rc,
									  const DrawerSettings& drawerSettings,
									  bool bUseCuePoints, const wxTimeSpanArray& cuePoints ):
	ColumnPainterWaveDrawer( nNumberOfSamples, gc, rc, drawerSettings, bUseCuePoints, cuePoints )
{}

void Raster1WaveDrawer::GetThreeColours( wxFloat32, wxColour& clrTop, wxColour& clrMiddle, wxColour& clrBottom )
{
	clrTop	  = m_drawerSettings.GetTopColourSettings().GetEdgeColour();
	clrMiddle = m_drawerSettings.GetTopColourSettings().GetMiddleColour();
	clrBottom = m_drawerSettings.GetBottomColourSettings().GetEdgeColour();
}

void Raster1WaveDrawer::GetTwoColours( wxFloat32, bool bUp, wxColour& clrFrom, wxColour& clrTo )
{
	if ( bUp )
	{
		clrFrom = m_drawerSettings.GetTopColourSettings().GetEdgeColour();
		clrTo	= m_drawerSettings.GetTopColourSettings().GetMiddleColour();
	}
	else
	{
		clrFrom = m_drawerSettings.GetBottomColourSettings().GetMiddleColour();
		clrTo	= m_drawerSettings.GetBottomColourSettings().GetEdgeColour();
	}
}

