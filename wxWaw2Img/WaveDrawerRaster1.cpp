/*
 *      WaveDrawerRaster1.cpp
 */
#include "StdWx.h"
#include "FloatArray.h"
#include "ColourInterpolation.h"
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

	wxImage imgUp, imgDown;

	if ( m_drawerSettings.DrawWithGradient() )
	{
		if ( m_drawerSettings.UseLogarithmicColorGradient() )
		{
			if ( m_drawerSettings.OneMiddleColour() )
			{
				imgUp = RasterWaveDrawer::create_log_gradient_bitmap(
						m_drawerSettings.GetTopColourSettings().GetEdgeColour(),
						m_drawerSettings.GetTopColourSettings().GetMiddleColour(),
						m_drawerSettings.GetBottomColourSettings().GetEdgeColour(),
						m_rc.m_height,
						m_drawerSettings.GetBaselinePosition(),
						GetLogarithmicScale()
						);
			}
			else
			{
				wxMemoryDC mdc;

				imgUp	= create_log_gradient_bitmap( mdc, m_drawerSettings.GetTopColourSettings(), true );
				imgDown = create_log_gradient_bitmap( mdc, m_drawerSettings.GetBottomColourSettings(), false );
			}
		}
		else
		{
			if ( m_drawerSettings.OneMiddleColour() )
			{
				imgUp = RasterWaveDrawer::create_gradient_bitmap(
						m_drawerSettings.GetTopColourSettings().GetEdgeColour(),
						m_drawerSettings.GetTopColourSettings().GetMiddleColour(),
						m_drawerSettings.GetBottomColourSettings().GetEdgeColour(),
						m_rc.m_height,
						m_drawerSettings.GetBaselinePosition() );
			}
			else
			{
				wxMemoryDC mdc;

				imgUp	= create_gradient_bitmap( mdc, m_drawerSettings.GetTopColourSettings(), true );
				imgDown = create_gradient_bitmap( mdc, m_drawerSettings.GetBottomColourSettings(), false );
			}
		}
	}
	else
	{
		wxMemoryDC mdc;

		imgUp	= create_solid_bitmap( mdc, m_drawerSettings.GetTopColourSettings(), true );
		imgDown = create_solid_bitmap( mdc, m_drawerSettings.GetBottomColourSettings(), false );
	}

	m_bmUp = m_gc->CreateBitmapFromImage( imgUp );

	if ( !m_drawerSettings.OneMiddleColour() )
	{
		m_bmDown = m_gc->CreateBitmapFromImage( imgDown );
	}
}

void Raster1WaveDrawer::NextColumn( wxFloat32 fValue, wxFloat32 fLogValue )
{
	wxPoint2DDouble point_central( m_nCurrentColumn + m_rc.m_x, m_yoffset );

	wxFloat32 v = abs( m_drawerSettings.UseLogarithmicScale() ? fLogValue : fValue );

	if ( m_drawerSettings.OneMiddleColour() )
	{
		m_gc->DrawBitmap( m_bmUp,
				point_central.m_x,
				point_central.m_y - ( v * m_heightUp ),
				1.0,
				v * m_rc.m_height );
	}
	else
	{
		m_gc->DrawBitmap( m_bmUp,
				point_central.m_x,
				point_central.m_y,
				1.0,
				-( v * m_heightUp ) );

		m_gc->DrawBitmap( m_bmDown,
				point_central.m_x,
				point_central.m_y,
				1.0,
				( v * m_heightDown ) );
	}
}

void Raster1WaveDrawer::ProcessFinalizer()
{
	m_bmUp	 = wxNullGraphicsBitmap;
	m_bmDown = wxNullGraphicsBitmap;

	__super::ProcessFinalizer();
}

wxImage Raster1WaveDrawer::create_solid_bitmap( wxMemoryDC& mdc, const ColourSettings& colourSettings, bool bUp ) const
{
	return __super::create_solid_bitmap( mdc, colourSettings.GetMiddleColour() );
}

wxImage Raster1WaveDrawer::create_gradient_bitmap( wxMemoryDC& mdc, const ColourSettings& colourSettings, bool bUp ) const
{
	if ( bUp )
	{
		return RasterWaveDrawer::create_gradient_bitmap( mdc, colourSettings.GetEdgeColour(), colourSettings.GetMiddleColour(), ceil( m_heightUp ) );
	}
	else
	{
		return RasterWaveDrawer::create_gradient_bitmap( mdc, colourSettings.GetMiddleColour(), colourSettings.GetEdgeColour(), ceil( m_heightDown ) );
	}
}

wxImage Raster1WaveDrawer::create_log_gradient_bitmap( wxMemoryDC& mdc, const ColourSettings& colourSettings, bool bUp ) const
{
	if ( bUp )
	{
		return RasterWaveDrawer::create_log_gradient_bitmap( mdc, colourSettings.GetEdgeColour(), colourSettings.GetMiddleColour(), ceil( m_heightUp ), GetLogarithmicScale().GetInverted() );
	}
	else
	{
		return RasterWaveDrawer::create_log_gradient_bitmap( mdc, colourSettings.GetMiddleColour(), colourSettings.GetEdgeColour(), ceil( m_heightDown ), GetLogarithmicScale() );
	}
}

