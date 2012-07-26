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
#include "WaveDrawerPoly.h"

PolyWaveDrawer::PolyWaveDrawer( wxUint64 nNumberOfSamples,
								wxGraphicsContext* gc,
								const wxRect2DInt& rc,
								const DrawerSettings& drawerSettings,
								bool bUseCuePoints, const wxTimeSpanArray& cuePoints ):
	RasterWaveDrawer( nNumberOfSamples, gc, rc, drawerSettings, bUseCuePoints, cuePoints )
{}

void PolyWaveDrawer::ProcessInitializer()
{
	__super::ProcessInitializer();
	m_points.Clear();
}

void PolyWaveDrawer::NextColumn( wxFloat32 fValue, wxFloat32 fLogValue )
{
	m_points.Add( wxPoint2DDouble( m_nCurrentColumn, m_drawerSettings.UseLogarithmicScale() ? fLogValue : fValue ) );
}

void PolyWaveDrawer::ProcessFinalizer()
{
	if ( m_drawerSettings.DrawWithGradient() )
	{
		wxPoint2DDouble first_point;
		wxGraphicsPath	path = m_gc->CreatePath();

		for ( size_t i = 0, nCount = m_points.GetCount(); i < nCount; i++ )
		{
			wxPoint2DDouble pt( m_points[ i ] );

			pt.m_x += m_rc.m_x;
			pt.m_y	= m_yoffset - ( abs( pt.m_y ) * m_heightUp );

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

		for ( size_t i = m_points.GetCount(); i > 0; i-- )
		{
			wxPoint2DDouble pt( m_points[ i - 1 ] );

			pt.m_x += m_rc.m_x;
			pt.m_y	= m_yoffset + ( abs( pt.m_y ) * m_heightDown );

			path.AddLineToPoint( pt );
		}

		path.AddLineToPoint( first_point );

		wxGraphicsGradientStops stops( m_drawerSettings.GetTopColourSettings().GetEdgeColour(), m_drawerSettings.GetBottomColourSettings().GetEdgeColour() );

		if ( m_drawerSettings.UseLogarithmicColorGradient() )
		{
			create_log_stops(
					stops,
					m_drawerSettings.GetTopColourSettings().GetEdgeColour(),
					m_drawerSettings.GetTopColourSettings().GetMiddleColour(),
					m_drawerSettings.GetBottomColourSettings().GetEdgeColour(),
					m_rc.m_height,
					m_drawerSettings.GetBaselinePosition(),
					GetLogarithmicScale() );
		}
		else
		{
			stops.Add( m_drawerSettings.GetTopColourSettings().GetMiddleColour(), 1.0f - m_drawerSettings.GetBaselinePosition() );
		}

		wxGraphicsBrush brush = m_gc->CreateLinearGradientBrush( m_rc.m_x, m_rc.m_y, m_rc.m_x, m_rc.m_y + m_rc.m_height - 1, stops );

		m_gc->SetBrush( brush );
		m_gc->FillPath( path );
		m_gc->SetBrush( wxNullBrush );
	}
	else
	{
		wxPoint2DDouble first_point( m_rc.m_x, m_yoffset );

		wxGraphicsPath	pathUp = m_gc->CreatePath();
		pathUp.MoveToPoint( first_point );

		for ( size_t i = 0, nCount = m_points.GetCount(); i < nCount; i++ )
		{
			wxPoint2DDouble pt( m_points[ i ] );

			pt.m_x += m_rc.m_x;
			pt.m_y	= m_yoffset - ( abs( pt.m_y ) * m_heightUp );

			pathUp.AddLineToPoint( pt );
		}

		pathUp.AddLineToPoint( m_rc.m_x + m_rc.m_width, m_yoffset );
		pathUp.AddLineToPoint( first_point );

		wxGraphicsPath	pathDown = m_gc->CreatePath();
		pathDown.MoveToPoint( first_point );

		for ( size_t i = 0, nCount = m_points.GetCount(); i < nCount; i++ )
		{
			wxPoint2DDouble pt( m_points[ i ] );

			pt.m_x += m_rc.m_x;
			pt.m_y	= m_yoffset + ( abs( pt.m_y ) * m_heightDown );

			pathDown.AddLineToPoint( pt );
		}

		pathDown.AddLineToPoint( m_rc.m_x + m_rc.m_width, m_yoffset );
		pathDown.AddLineToPoint( first_point );

		wxGraphicsBrush brush = m_gc->CreateBrush( wxBrush( m_drawerSettings.GetTopColourSettings().GetMiddleColour() ) );
		m_gc->SetBrush( brush );
		m_gc->FillPath( pathUp );

		brush = m_gc->CreateBrush( wxBrush( m_drawerSettings.GetBottomColourSettings().GetMiddleColour() ) );
		m_gc->SetBrush( brush );
		m_gc->FillPath( pathDown );

		m_gc->SetBrush( wxNullBrush );
	}
	__super::ProcessFinalizer();
}

