/*
 *      WaveDrawerRaster2.cpp
 */
#include "StdWx.h"
#include "FloatArray.h"
#include "LogarithmicScale.h"
#include "ColourInterpolation.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "DrawerSettings.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerRaster.h"
#include "WaveDrawerRaster2.h"

Raster2WaveDrawer::Raster2WaveDrawer( wxUint64 nNumberOfSamples,
									  wxGraphicsContext* gc,
									  const wxRect2DInt& rc,
									  const DrawerSettings& drawerSettings,
									  bool bUseCuePoints, const wxTimeSpanArray& cuePoints ):
	RasterWaveDrawer( nNumberOfSamples, gc, rc, drawerSettings, bUseCuePoints, cuePoints )
{}

void Raster2WaveDrawer::ProcessInitializer()
{
	__super::ProcessInitializer();

	m_mdc.reset( new wxMemoryDC() );
}

void Raster2WaveDrawer::NextColumn( wxFloat32 fValue, wxFloat32 fLogValue )
{
	wxPoint2DDouble point_central( m_nCurrentColumn + m_rc.m_x, m_yoffset );
	wxFloat32 va = abs( fValue );
	wxFloat32	   v = abs( m_drawerSettings.UseLogarithmicScale() ? fLogValue : fValue );

	wxColour clrTo = ColourInterpolation::linear_interpolation( m_drawerSettings.GetTopColourSettings().GetEdgeColour(), m_drawerSettings.GetTopColourSettings().GetMiddleColour(), va );
	wxColour clrFrom = m_drawerSettings.GetTopColourSettings().GetEdgeColour();

	wxImage img;

	if ( m_drawerSettings.UseLogarithmicColorGradient() )
	{
		wxASSERT( UseLogarithmicScale() );
		img = create_log_gradient_bitmap( *m_mdc, clrTo, clrFrom, clrFrom, m_rc.m_height, 0.0f, GetLogarithmicScale() );
	}
	else
	{
		img = create_gradient_bitmap( *m_mdc, clrTo, clrFrom, clrFrom, m_rc.m_height, m_drawerSettings.GetBaselinePosition() );
	}

	wxRect2DDouble rcTop( 0, 0, 1, v * m_heightUp );
	m_gc->DrawBitmap( m_gc->CreateBitmapFromImage( img ),
			point_central.m_x,
			point_central.m_y,
			rcTop.m_width,
			-rcTop.m_height );

	clrFrom = ColourInterpolation::linear_interpolation( m_drawerSettings.GetBottomColourSettings().GetEdgeColour(), m_drawerSettings.GetBottomColourSettings().GetMiddleColour(), va );
	clrTo = m_drawerSettings.GetBottomColourSettings().GetEdgeColour();

	if ( m_drawerSettings.UseLogarithmicColorGradient() )
	{
		wxASSERT( UseLogarithmicScale() );
		img = create_log_gradient_bitmap( *m_mdc, clrFrom, clrTo, clrTo, m_rc.m_height, 0.0f, GetLogarithmicScale() );
	}
	else
	{
		img = create_gradient_bitmap( *m_mdc, clrFrom, clrTo, clrTo, m_rc.m_height, m_drawerSettings.GetBaselinePosition() );
	}

	wxRect2DDouble rcBottom( 0, 0, 1, v * m_heightDown );
	m_gc->DrawBitmap( m_gc->CreateBitmapFromImage( img ),
			point_central.m_x,
			point_central.m_y,
			rcBottom.m_width,
			rcBottom.m_height );
}

void Raster2WaveDrawer::ProcessFinalizer()
{
	m_mdc.release();
}

