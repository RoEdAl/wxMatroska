/*
	WaveDrawerRaster2.cpp
*/
#include "StdWx.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerRaster.h"
#include "WaveDrawerRaster2.h"

Raster2WaveDrawer::Raster2WaveDrawer(
	wxUint64 nNumberOfSamples, wxGraphicsContext* gc,
	bool bLogarithmicScale, bool bLogarithmicColorGradient, wxFloat32 fLogBase,
	wxRect2DInt rc,
	const wxColour& clrFrom, const wxColour& clrTo )
	:RasterWaveDrawer(  nNumberOfSamples, gc, bLogarithmicColorGradient || bLogarithmicScale, fLogBase, rc, clrFrom, clrTo ),
	m_bLogarithmicScale(bLogarithmicScale), m_bLogarithmicColorGradient(bLogarithmicColorGradient)
{
}

void Raster2WaveDrawer::ProcessInitializer()
{
	__super::ProcessInitializer();

	m_mdc.reset( new wxMemoryDC() );
}

void Raster2WaveDrawer::NextColumn( wxFloat32 fValue, wxFloat32 fLogValue )
{
	wxColour clrFrom = linear_interpolation( m_clrTo, m_clrFrom, abs( m_bLogarithmicColorGradient? fLogValue : fValue ) );
	wxColour clrTo = m_clrTo;

	wxGraphicsBitmap bmp = m_gc->CreateBitmap( create_gradient_bitmap( *m_mdc, clrFrom, clrTo, m_nImgHeight ) );

	wxPoint2DDouble point_central( m_nCurrentColumn + m_rc.m_x, m_yoffset );
	wxRect2DDouble rc( 0 ,0, 1, abs( (m_bLogarithmicScale? fLogValue : fValue) * m_nImgHeight ) );
	m_gc->DrawBitmap( bmp, point_central.m_x, point_central.m_y - rc.m_height, rc.m_width, 2 * rc.m_height );
}

void Raster2WaveDrawer::ProcessFinalizer()
{
	m_mdc.release();
}

