/*
	WaveDrawerGraphicsContext.cpp
*/

#include "StdWx.h"
#include "WaveDrawer.h"
#include "WaveDrawerGraphicsContextPoly.h"

GraphicsContextWaveDrawerPoly::GraphicsContextWaveDrawerPoly( wxUint64 nNumberOfSamples, wxGraphicsContext* gc, bool bLogarithmicScale, bool bLogarithmicColorGradient,  wxFloat32 fLogBase, wxRect2DInt rc )
	:WaveDrawer( nNumberOfSamples, rc.m_width, bLogarithmicScale, bLogarithmicColorGradient, fLogBase ), m_gc( gc ), m_rc( rc )
{}

void GraphicsContextWaveDrawerPoly::StartDrawing()
{
	WaveDrawer::StartDrawing();

	m_height2 = m_rc.GetCentre().m_y;
	m_yoffset = m_rc.m_y + m_height2;
	m_path = m_gc->CreatePath();
}

void GraphicsContextWaveDrawerPoly::NextColumn( wxFloat32 fValue, wxFloat32 flogValue )
{
	wxFloat32 p = abs(fValue) * m_height2;

	wxPoint2DDouble point_central( m_nCurrentColumn + m_rc.m_x, m_yoffset );

	if ( fValue >= 0.0f )
	{
		wxPoint2DDouble pt( m_nCurrentColumn + m_rc.m_x, m_yoffset - p );
		m_path.MoveToPoint( pt );
		m_path.AddLineToPoint( point_central );
	}
	else
	{
		wxPoint2DDouble pt( m_nCurrentColumn + m_rc.m_x, m_yoffset + p );
		m_path.MoveToPoint( pt );
		m_path.AddLineToPoint( point_central );
	}
}

void GraphicsContextWaveDrawerPoly::EndOfSamples()
{
	m_gc->StrokePath( m_path );
	// TODO : Destroy path if possible
}
