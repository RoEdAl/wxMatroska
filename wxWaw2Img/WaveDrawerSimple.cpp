/*
        WaveDrawerSimple.cpp
 */
#include "StdWx.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerSimple.h"

SimpleWaveDrawer::SimpleWaveDrawer( wxUint64 nNumberOfSamples,
									wxGraphicsContext* gc,
									bool bLogarithmicScale, wxFloat32 fLogBase,
									wxRect2DInt rc, const wxColour& clr ):
	GraphicsContextWaveDrawer( nNumberOfSamples, gc, bLogarithmicScale, fLogBase, rc ),
	m_clr( clr ), m_bLogarithmicScale( bLogarithmicScale )
{}

void SimpleWaveDrawer::ProcessInitializer()
{
	__super::ProcessInitializer();

	m_path = m_gc->CreatePath();
}

void SimpleWaveDrawer::NextColumn( wxFloat32 fValue, wxFloat32 fLogValue )
{
	wxFloat32		p = ( m_bLogarithmicScale ? fLogValue : fValue ) * m_nImgHeight;
	wxPoint2DDouble point_central( m_nCurrentColumn + m_rc.m_x, m_yoffset );

	m_path.MoveToPoint( point_central.m_x, point_central.m_y - p );
	m_path.AddLineToPoint( point_central.m_x, point_central.m_y + p );
}

void SimpleWaveDrawer::ProcessFinalizer()
{
	__super::ProcessFinalizer();

	wxGraphicsPen pen = m_gc->CreatePen( wxPen( m_clr ) );
	m_gc->SetPen( pen );
	m_gc->DrawPath( m_path );
}

