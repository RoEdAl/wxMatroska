/*
 *      WaveDrawerRaster1.cpp
 */
#include "StdWx.h"
#include "FloatArray.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "DrawerSettings.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerRaster.h"
#include "WaveDrawerRaster1.h"

Raster1WaveDrawer::Raster1WaveDrawer( wxUint64 nNumberOfSamples,
									  wxGraphicsContext* gc,
									  const wxRect2DInt& rc,
									  const DrawerSettings& drawerSettings,
									  bool bUseCuePoints, const wxTimeSpanArray& cuePoints ):
	RasterWaveDrawer( nNumberOfSamples, gc, rc, drawerSettings, bUseCuePoints, cuePoints )
{}

void Raster1WaveDrawer::ProcessInitializer()
{
	__super::ProcessInitializer();

	wxImage img;

	if ( m_drawerSettings.UseLogarithmicColorGradient() )
	{
		img = create_log_gradient_bitmap( m_drawerSettings.GetColourFrom(), m_drawerSettings.GetColourTo(), m_rc.m_height, m_drawerSettings.GetBaselinePosition(), GetLogarithmicScale() );
	}
	else
	{
		img = create_gradient_bitmap( m_drawerSettings.GetColourFrom(), m_drawerSettings.GetColourTo(), m_rc.m_height, m_drawerSettings.GetBaselinePosition() );
	}

	m_bm = m_gc->CreateBitmapFromImage( img );
}

void Raster1WaveDrawer::NextColumn( wxFloat32 fValue, wxFloat32 fLogValue )
{
	wxPoint2DDouble point_central( m_nCurrentColumn + m_rc.m_x, m_yoffset );

	wxFloat32	   v = abs( m_drawerSettings.UseLogarithmicScale() ? fLogValue : fValue );
	wxRect2DDouble rc( 0, 0, 1, v * m_rc.m_height );

	m_gc->DrawBitmap( m_bm,
			point_central.m_x,
			point_central.m_y - ( v * m_heightUp ),
			rc.m_width,
			rc.m_height );
}

