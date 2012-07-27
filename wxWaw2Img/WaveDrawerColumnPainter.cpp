/*
 *      WaveDrawerColumnPainter.cpp
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
#include "WaveDrawerColumnPainter.h"

ColumnPainterWaveDrawer::ColumnPainterWaveDrawer( wxUint64 nNumberOfSamples,
									  wxGraphicsContext* gc,
									  const wxRect2DInt& rc,
									  const DrawerSettings& drawerSettings,
									  bool bUseCuePoints, const wxTimeSpanArray& cuePoints ):
	GraphicsContextWaveDrawer(
		nNumberOfSamples,
		gc,
		drawerSettings.UseLogarithmicScale() || drawerSettings.UseLogarithmicColorGradient(),
		drawerSettings.GetLogarithmBase(),
		rc,
		drawerSettings,
		bUseCuePoints, cuePoints )
{}

void ColumnPainterWaveDrawer::NextColumn( wxFloat32 fValue, wxFloat32 fLogValue )
{
	wxPoint2DDouble point_central( m_nCurrentColumn + m_rc.m_x, m_yoffset );
	wxFloat32		va = abs( fValue );
	wxFloat32		v  = abs( m_drawerSettings.UseLogarithmicScale() ? fLogValue : fValue );

	if ( m_drawerSettings.OneMiddleColour() && m_drawerSettings.OneEdgeColour() )
	{
		wxGraphicsBrush brush;

		wxPoint2DDouble topLeft( point_central.m_x, point_central.m_y - (v * m_heightUp) );
		wxPoint2DDouble bottomRight( topLeft.m_x + 1.0, topLeft.m_y + (v * m_rc.m_height) );

		if ( m_drawerSettings.DrawWithGradient() )
		{
			wxColour clrTop, clrMiddle, clrBottom;
			GetThreeColours( va, clrTop, clrMiddle, clrBottom );

			wxGraphicsGradientStops stops( clrTop, clrBottom );

			if ( m_drawerSettings.UseLogarithmicColorGradient() )
			{
				create_log_stops(
						stops,
						clrTop,
						clrMiddle,
						clrBottom,
						m_rc.m_height,
						m_drawerSettings.GetBaselinePosition(),
						GetLogarithmicScale() );
			}
			else
			{
				stops.Add( clrMiddle, 1.0f - m_drawerSettings.GetBaselinePosition() );
			}

			brush = create_brush( stops, topLeft, bottomRight );
		}
		else
		{
			brush = create_brush( m_drawerSettings.GetTopColourSettings().GetMiddleColour() );
		}

		fill_rect( brush, topLeft, bottomRight );
	}
	else
	{
		wxGraphicsBrush brush;

		// UP
		wxPoint2DDouble bottomLeft = point_central;
		wxPoint2DDouble topRight( bottomLeft.m_x + 1.0, bottomLeft.m_y - ( v * m_heightUp ) );

		wxPoint2DDouble topLeft( bottomLeft.m_x, topRight.m_y );
		wxPoint2DDouble bottomRight( topRight.m_x, bottomLeft.m_y );

		if ( m_drawerSettings.DrawWithGradient() )
		{
			wxColour clrFrom, clrTo;
			GetTwoColours( va, true, clrFrom, clrTo );

			wxGraphicsGradientStops stops( clrFrom, clrTo );
			if ( m_drawerSettings.UseLogarithmicColorGradient() )
			{
				wxASSERT( UseLogarithmicScale() );
				create_log_stops( stops, clrFrom, clrTo, m_rc.m_height, GetLogarithmicScale().GetInverted() );
			}

			brush = create_brush( stops, topLeft, bottomRight );
		}
		else
		{
			brush = create_brush( m_drawerSettings.GetTopColourSettings().GetMiddleColour() );
		}

		fill_rect( brush, topLeft, bottomRight );

		// DOWN
		topLeft = point_central;
		bottomRight = topLeft;
		bottomRight.m_x += 1.0;
		bottomRight.m_y += v * m_heightDown;

		if ( m_drawerSettings.DrawWithGradient() )
		{
			wxColour clrFrom, clrTo;
			GetTwoColours( va, false, clrFrom, clrTo );

			wxGraphicsGradientStops stops( clrFrom, clrTo );
			if ( m_drawerSettings.UseLogarithmicColorGradient() )
			{
				wxASSERT( UseLogarithmicScale() );
				create_log_stops( stops, clrFrom, clrTo, m_rc.m_height, GetLogarithmicScale() );
			}

			brush = create_brush( stops, topLeft, bottomRight );
		}
		else
		{
			brush = create_brush( m_drawerSettings.GetBottomColourSettings().GetMiddleColour() );
		}

		fill_rect( brush, topLeft, bottomRight );
	}
}

void ColumnPainterWaveDrawer::ProcessFinalizer()
{
	m_gc->SetBrush( wxNullBrush );

	__super::ProcessFinalizer();
}

wxGraphicsBrush ColumnPainterWaveDrawer::create_brush( const wxColour& clr ) const
{
	return m_gc->CreateBrush( clr );
}

wxGraphicsBrush ColumnPainterWaveDrawer::create_brush( const wxGraphicsGradientStops& stops, const wxPoint2DDouble& topLeft, const wxPoint2DDouble& bottomRight ) const
{
	return m_gc->CreateLinearGradientBrush( topLeft.m_x - 1.0, topLeft.m_y - 1.0, bottomRight.m_x + 1.0, bottomRight.m_y + 1.0, stops );
}

void ColumnPainterWaveDrawer::fill_rect( const wxGraphicsBrush& brush, const wxPoint2DDouble& topLeft, const wxPoint2DDouble& bottomRight ) const
{
	m_gc->SetBrush( brush );
	m_gc->DrawRectangle( topLeft.m_x, topLeft.m_y, bottomRight.m_x - topLeft.m_x, bottomRight.m_y - topLeft.m_y );
}
