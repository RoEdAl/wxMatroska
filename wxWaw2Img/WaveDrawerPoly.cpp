/*
	WaveDrawerRaster1.cpp
*/
#include "StdWx.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerRaster.h"
#include "WaveDrawerPoly.h"

PolyWaveDrawer::PolyWaveDrawer(
	wxUint64 nNumberOfSamples,
	wxGraphicsContext* gc,
	bool bLogarithmicScale, bool bLogarithmicColorGradient, wxFloat32 fLogBase,
	wxRect2DInt rc, const wxColour& clrFrom, const wxColour& clrTo )
	:RasterWaveDrawer( nNumberOfSamples, gc, bLogarithmicScale, fLogBase, rc, clrFrom, clrTo ),
	m_bLogarithmicScale(bLogarithmicScale), m_bLogarithmicColorGradient(bLogarithmicColorGradient)
{}

void PolyWaveDrawer::ProcessInitializer()
{
	__super::ProcessInitializer();
	m_points.Clear();
}

void PolyWaveDrawer::NextColumn( wxFloat32 fValue, wxFloat32 fLogValue )
{
	m_points.Add( wxPoint2DDouble( m_nCurrentColumn, m_bLogarithmicScale? fLogValue : fValue ) );
}

void PolyWaveDrawer::ProcessFinalizer()
{
	wxPoint2DDouble first_point;
	wxGraphicsPath path = m_gc->CreatePath();
	for( size_t i=0, nCount = m_points.GetCount(); i < nCount; i++ )
	{
		wxPoint2DDouble pt( m_points[i] );

		pt.m_x += m_rc.m_x;
		pt.m_y = m_yoffset + ( abs( pt.m_y ) * m_nImgHeight );

		if ( i == 0 )
		{
			path.MoveToPoint( pt );
			first_point = pt;
		}
		else
		{
			path.AddLineToPoint( pt );
		}
	}

	for( size_t i = m_points.GetCount(); i > 0; i-- )
	{
		wxPoint2DDouble pt( m_points[i-1] );

		pt.m_x += m_rc.m_x;
		pt.m_y = m_yoffset - ( abs( pt.m_y ) * m_nImgHeight );

		path.AddLineToPoint( pt );
	}

	path.AddLineToPoint( first_point );

	wxGraphicsGradientStops stops( m_clrTo, m_clrTo );
	if ( m_bLogarithmicColorGradient )
	{
		create_log_stops( stops, m_clrFrom, m_clrTo, m_nImgHeight, m_fLogBase );
	}
	else
	{
		stops.Add( m_clrFrom, 0.5f );
	}

	wxGraphicsBrush brush = m_gc->CreateLinearGradientBrush( 0,0, 0,2*m_nImgHeight, stops );

	m_gc->SetBrush( brush );
	m_gc->FillPath( path );
	m_gc->SetBrush( wxNullBrush );

	__super::ProcessFinalizer();
}

void PolyWaveDrawer::create_log_stops( wxGraphicsGradientStops& stops, const wxColour& clrFrom, const wxColour& clrTo, wxUint32 nHeight, wxFloat32 fLogBase )
{
	wxASSERT( nHeight > 0 );

	wxFloat32 fLogLogBase = log( fLogBase );

	for( wxUint32 i=nHeight; i > 0; i-- )
	{
		wxFloat32 p = 1.0f * i / nHeight;
		wxFloat32 pl = log( p * (fLogBase - 1.0f) + 1.0f ) / fLogLogBase;

		stops.Add( linear_interpolation( clrFrom, clrTo, pl ), (1.0f - p) / 2.0f );
	}

	for( wxUint32 i=0; i < nHeight; i++ )
	{
		wxFloat32 p = 1.0f * i / nHeight;
		wxFloat32 pl = log( p * (fLogBase - 1.0f) + 1.0f ) / fLogLogBase;

		stops.Add( linear_interpolation( clrFrom, clrTo, pl ), 0.5f + (p / 2.0f) );
	}
}
