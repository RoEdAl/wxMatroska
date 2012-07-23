/*
        WaveDrawerRaster1.cpp
 */
#include "StdWx.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerRaster.h"
#include "WaveDrawerRaster1.h"

Raster1WaveDrawer::Raster1WaveDrawer( wxUint64 nNumberOfSamples,
									  wxGraphicsContext* gc,
									  bool bLogarithmicScale, bool bLogarithmicColorGradient, wxFloat32 fLogBase,
									  wxRect2DInt rc, const wxColour& clrFrom, const wxColour& clrTo ):
	RasterWaveDrawer( nNumberOfSamples, gc, bLogarithmicScale, bLogarithmicColorGradient, fLogBase, rc, clrFrom, clrTo )
{}

void Raster1WaveDrawer::ProcessInitializer()
{
	__super::ProcessInitializer();

	wxImage img;
	if ( m_bLogarithmicColorGradient )
	{
		img = create_log_gradient_bitmap( m_clrFrom, m_clrTo, m_nImgHeight, GetLogarithmicScale() );
	}
	else
	{
		img = create_gradient_bitmap( m_clrFrom, m_clrTo, m_nImgHeight );
	}

	wxASSERT( ( 2 * m_nImgHeight ) == img.GetHeight() );

	m_bm = m_gc->CreateBitmapFromImage( img );
}

void Raster1WaveDrawer::NextColumn( wxFloat32 fValue, wxFloat32 fLogValue )
{
	wxPoint2DDouble point_central( m_nCurrentColumn + m_rc.m_x, m_yoffset );
	wxRect2DDouble	rc( 0, 0, 1, abs( ( m_bLogarithmicScale ? fLogValue : fValue ) * m_nImgHeight ) );

	m_gc->DrawBitmap( m_bm, point_central.m_x, point_central.m_y - rc.m_height, rc.m_width, 2 * rc.m_height );
}

