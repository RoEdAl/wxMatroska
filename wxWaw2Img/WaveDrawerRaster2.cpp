/*
        WaveDrawerRaster2.cpp
 */
#include "StdWx.h"
#include "FloatArray.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerRaster.h"
#include "WaveDrawerRaster2.h"

Raster2WaveDrawer::Raster2WaveDrawer( wxUint64 nNumberOfSamples, wxGraphicsContext* gc,
									  bool bLogarithmicScale, bool bLogarithmicColorGradient, wxFloat32 fLogBase,
									  wxRect2DInt rc,
									  const wxColour& clrFrom, const wxColour& clrTo,
									  bool bUseCuePoints, const wxTimeSpanArray& cuePoints,
									  const wxColour& clrBgSecond ):
	RasterWaveDrawer( nNumberOfSamples, gc, bLogarithmicScale, bLogarithmicColorGradient, fLogBase, rc, clrFrom, clrTo, bUseCuePoints, cuePoints, clrBgSecond )
{}

void Raster2WaveDrawer::ProcessInitializer()
{
	__super::ProcessInitializer();

	m_mdc.reset( new wxMemoryDC() );
}

void Raster2WaveDrawer::NextColumn( wxFloat32 fValue, wxFloat32 fLogValue )
{
	// wxColour clrFrom = linear_interpolation( m_clrTo, m_clrFrom, abs( m_bLogarithmicColorGradient? fLogValue : fValue ) );
	wxColour clrFrom = linear_interpolation( m_clrTo, m_clrFrom, abs( fValue ) );
	wxColour clrTo	 = m_clrTo;

	wxImage img;

	if ( m_bLogarithmicColorGradient )
	{
		wxASSERT( UseLogarithmicScale() );
		img = create_log_gradient_bitmap( *m_mdc, clrFrom, clrTo, m_nImgHeight, GetLogarithmicScale() );
	}
	else
	{
		img = create_gradient_bitmap( *m_mdc, clrFrom, clrTo, m_nImgHeight );
	}

	wxPoint2DDouble point_central( m_nCurrentColumn + m_rc.m_x, m_yoffset );
	wxRect2DDouble	rc( 0, 0, 1, abs( ( m_bLogarithmicScale ? fLogValue : fValue ) * m_nImgHeight ) );
	m_gc->DrawBitmap( m_gc->CreateBitmapFromImage( img ), point_central.m_x, point_central.m_y - rc.m_height, rc.m_width, 2 * rc.m_height );
}

void Raster2WaveDrawer::ProcessFinalizer()
{
	m_mdc.release();
}

