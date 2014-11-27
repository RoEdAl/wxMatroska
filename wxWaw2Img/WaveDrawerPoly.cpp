/*
 *      WaveDrawerPoly.cpp
 */
#include "StdWx.h"
#include "Arrays.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "DrawerSettings.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerPoly.h"

PolyWaveDrawer::PolyWaveDrawer( wxUint64 nNumberOfSamples,
								wxGraphicsContext* gc,
								const wxRect2DInt& rc,
								const DrawerSettings& drawerSettings,
								const ChaptersArrayScopedPtr& pChapters ):
	GraphicsContextWaveDrawer(
		nNumberOfSamples,
		gc,
		drawerSettings.UseLogarithmicScale() || drawerSettings.UseLogarithmicColorGradient(),
		drawerSettings.GetLogarithmBase(),
		rc,
		drawerSettings,
		pChapters )
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
		if ( m_drawerSettings.OneMiddleColour() )
		{
			wxGraphicsPath			path = build_path();
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

			wxGraphicsBrush brush = m_gc->CreateLinearGradientBrush(
					m_rc.m_x,
					m_rc.m_y,
					m_rc.m_x,
					m_rc.m_y + m_rc.m_height,
					stops );

			m_gc->SetBrush( brush );
			m_gc->FillPath( path );
		}
		else
		{
			wxGraphicsPath pathUp, pathDown;
			build_paths( pathUp, pathDown );

			wxGraphicsBrush brush;

			if ( m_drawerSettings.UseLogarithmicColorGradient() )
			{
				brush = ceate_logarithmic_brush(
						m_rc.m_x,
						m_rc.m_y,
						m_rc.m_x,
						m_rc.m_y + m_heightUp,
						m_drawerSettings.GetTopColourSettings().GetEdgeColour(),
						m_drawerSettings.GetTopColourSettings().GetMiddleColour(),
						true
						);
			}
			else
			{
				brush = m_gc->CreateLinearGradientBrush(
						m_rc.m_x,
						m_rc.m_y,
						m_rc.m_x,
						m_rc.m_y + m_heightUp,
						m_drawerSettings.GetTopColourSettings().GetEdgeColour(),
						m_drawerSettings.GetTopColourSettings().GetMiddleColour()
						);
			}

			m_gc->SetBrush( brush );
			m_gc->FillPath( pathUp );

			if ( m_drawerSettings.UseLogarithmicColorGradient() )
			{
				brush = ceate_logarithmic_brush(
						m_rc.m_x,
						m_rc.m_y + m_heightUp,
						m_rc.m_x,
						m_rc.m_y + m_rc.m_height,
						m_drawerSettings.GetBottomColourSettings().GetMiddleColour(),
						m_drawerSettings.GetBottomColourSettings().GetEdgeColour(),
						false
						);
			}
			else
			{
				brush = m_gc->CreateLinearGradientBrush(
						m_rc.m_x,
						m_rc.m_y + m_heightUp,
						m_rc.m_x,
						m_rc.m_y + m_rc.m_height,
						m_drawerSettings.GetBottomColourSettings().GetMiddleColour(),
						m_drawerSettings.GetBottomColourSettings().GetEdgeColour()
						);
			}

			m_gc->SetBrush( brush );
			m_gc->FillPath( pathDown );
		}
	}
	else
	{
		if ( m_drawerSettings.OneMiddleColour() )
		{
			m_gc->SetBrush( m_drawerSettings.GetTopColourSettings().GetMiddleColour() );
			m_gc->FillPath( build_path() );
		}
		else
		{
			wxGraphicsPath pathUp, pathDown;
			build_paths( pathUp, pathDown );

			wxGraphicsBrush brush = m_gc->CreateBrush( m_drawerSettings.GetTopColourSettings().GetMiddleColour() );
			m_gc->SetBrush( brush );
			m_gc->FillPath( pathUp );

			brush = m_gc->CreateBrush( m_drawerSettings.GetBottomColourSettings().GetMiddleColour() );
			m_gc->SetBrush( brush );
			m_gc->FillPath( pathDown );
		}
	}

	m_gc->SetBrush( wxNullBrush );
	__super::ProcessFinalizer();
}

wxGraphicsPath PolyWaveDrawer::build_path() const
{
	wxASSERT( m_drawerSettings.OneMiddleColour() );

	wxPoint2DDouble first_point( m_rc.m_x, m_yoffset );

	wxGraphicsPath path = m_gc->CreatePath();
	path.MoveToPoint( first_point );

	for ( size_t i = 0, nCount = m_points.GetCount(); i < nCount; i++ )
	{
		wxPoint2DDouble pt( m_points[ i ] );

		pt.m_x += m_rc.m_x;
		pt.m_y	= m_yoffset - ( abs( pt.m_y ) * m_heightUp );

		path.AddLineToPoint( pt );
	}

	path.AddLineToPoint( m_rc.m_x + m_rc.m_width, m_yoffset );

	for ( size_t i = m_points.GetCount(); i > 0; i-- )
	{
		wxPoint2DDouble pt( m_points[ i - 1 ] );

		pt.m_x += m_rc.m_x;
		pt.m_y	= m_yoffset + ( abs( pt.m_y ) * m_heightDown );

		path.AddLineToPoint( pt );
	}

	path.AddLineToPoint( first_point );
	return path;
}

void PolyWaveDrawer::build_paths( wxGraphicsPath& pathUp, wxGraphicsPath& pathDown ) const
{
	wxPoint2DDouble first_point( m_rc.m_x, m_yoffset );

	pathUp = m_gc->CreatePath();
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

	pathDown = m_gc->CreatePath();
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
}

wxGraphicsBrush PolyWaveDrawer::ceate_logarithmic_brush(
		wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2,
		const wxColour& clrFrom, const wxColour& clrTo,
		bool bInverted ) const
{
	wxGraphicsGradientStops stops( clrFrom, clrTo );

	if ( bInverted )
	{
		create_log_stops( stops, clrFrom, clrTo, m_rc.m_height, !GetLogarithmicScale() );
	}
	else
	{
		create_log_stops( stops, clrFrom, clrTo, m_rc.m_height, GetLogarithmicScale() );
	}
	return m_gc->CreateLinearGradientBrush( x1, y1, x2, y2, stops );
}

