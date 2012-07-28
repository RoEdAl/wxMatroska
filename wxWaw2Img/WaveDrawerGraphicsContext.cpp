/*
 *      WaveDrawerGraphicsContext.cpp
 */

#include "StdWx.h"
#include "FloatArray.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "DrawerSettings.h"
#include "WaveDrawerGraphicsContext.h"

GraphicsContextWaveDrawer::GraphicsContextWaveDrawer( wxUint64 nNumberOfSamples, wxGraphicsContext* gc,
													  bool bCalcLogarithmic, wxFloat32 fLogBase,
													  const wxRect2DInt& rc,
													  const DrawerSettings& drawerSettings,
													  bool bUseCuePoints, const wxTimeSpanArray& cuePoints ):
	SampleChunker( nNumberOfSamples, rc.m_width, bCalcLogarithmic, fLogBase ),
	m_gc( gc ),
	m_rc( rc ),
	m_drawerSettings( drawerSettings ),
	m_bUseCuePoints( bUseCuePoints ), m_cuePoints( cuePoints )
{}

void GraphicsContextWaveDrawer::ProcessInitializer()
{
	m_heightUp	 = m_rc.m_height * ( 1.0f - m_drawerSettings.GetBaselinePosition() );
	m_heightDown = m_rc.m_height * m_drawerSettings.GetBaselinePosition();
	m_yoffset	 = m_rc.m_y + m_heightUp;

	m_gc->SetCompositionMode( wxCOMPOSITION_SOURCE );
	if ( m_drawerSettings.OneBackgroundColour() )
	{
		m_gc->SetBrush( m_drawerSettings.GetTopColourSettings().GetBackgroundColour() );
		m_gc->DrawRectangle( m_rc.m_x, m_rc.m_y, m_rc.m_width, m_rc.m_height );
	}
	else
	{
		wxRect2DDouble rc( m_rc.m_x, m_rc.m_y, m_rc.m_width, m_heightUp );
		m_gc->SetBrush( m_drawerSettings.GetTopColourSettings().GetBackgroundColour() );
		m_gc->DrawRectangle( rc.m_x, rc.m_y, rc.m_width, rc.m_height );

		rc.m_y	   += m_rc.m_height;
		rc.m_height = m_heightDown;
		m_gc->SetBrush( m_drawerSettings.GetBottomColourSettings().GetBackgroundColour() );
		m_gc->DrawRectangle( rc.m_x, rc.m_y, rc.m_width, rc.m_height );
	}
	m_gc->SetCompositionMode( wxCOMPOSITION_OVER );

	// Secondary background colour

	if ( m_bUseCuePoints )
	{
		bool bDrawCueStrippes = m_drawerSettings.GetDrawCueStrippes();

		if ( m_drawerSettings.OneBackgroundColour2() )
		{
			wxGraphicsPath path	= m_gc->CreatePath();

			if ( bDrawCueStrippes )
			{
				create_cue_segments_path( m_rc, m_cuePoints, path );
				m_gc->SetBrush( m_drawerSettings.GetTopColourSettings().GetBackgroundColour2() );
				m_gc->FillPath( path );
			}
			else
			{
				create_cue_lines_path( m_rc, m_cuePoints, path );
				m_gc->SetPen( m_drawerSettings.GetTopColourSettings().GetBackgroundColour2() );
				m_gc->StrokePath( path );
			}
		}
		else
		{
			wxGraphicsPath pathTop	  = m_gc->CreatePath();
			wxGraphicsPath pathBottom = m_gc->CreatePath();

			if ( bDrawCueStrippes )
			{
				create_cue_segments_paths( m_rc, m_drawerSettings.GetBaselinePosition(), m_cuePoints, pathTop, pathBottom );

				m_gc->SetBrush( m_drawerSettings.GetTopColourSettings().GetBackgroundColour2() );
				m_gc->FillPath( pathTop );

				m_gc->SetBrush( m_drawerSettings.GetBottomColourSettings().GetBackgroundColour2() );
				m_gc->FillPath( pathBottom );
			}
			else
			{
				create_cue_lines_paths( m_rc, m_drawerSettings.GetBaselinePosition(), m_cuePoints, pathTop, pathBottom );

				m_gc->SetPen( m_drawerSettings.GetTopColourSettings().GetBackgroundColour2() );
				m_gc->StrokePath( pathTop );

				m_gc->SetPen( m_drawerSettings.GetBottomColourSettings().GetBackgroundColour2() );
				m_gc->StrokePath( pathBottom );
			}
		}
	}

	m_gc->SetBrush( wxNullBrush );
}

void GraphicsContextWaveDrawer::ProcessFinalizer()
{}

void GraphicsContextWaveDrawer::create_cue_segments_paths(
		const wxRect2DInt& rect,
		wxFloat32 fBaseline,
		const wxTimeSpanArray& cuePoints,
		wxGraphicsPath& pathTop, wxGraphicsPath& pathBottom )
{
	wxASSERT( cuePoints.GetCount() > 1 );

	wxFloat64 endPos = cuePoints.Last().GetMilliseconds().ToDouble();

	wxDouble fHeightTop	   = rect.m_height * ( 1.0f - fBaseline );
	wxDouble fHeightBottom = rect.m_height * fBaseline;

	for ( size_t i = 0, nCount1 = cuePoints.GetCount() - 1; i < nCount1; i += 2 )
	{
		wxFloat64 tsFrom = cuePoints[ i ].GetMilliseconds().ToDouble();
		wxFloat64 tsTo	 = cuePoints[ i + 1 ].GetMilliseconds().ToDouble();

		wxRect2DDouble rc( tsFrom * rect.m_width / endPos, 0, ( tsTo - tsFrom ) * rect.m_width / endPos, fHeightTop );
		rc.m_x += rect.m_x;
		rc.m_y += rect.m_y;

		pathTop.AddRectangle( rc.m_x, rc.m_y, rc.m_width, rc.m_height );

		rc.m_y	   += fHeightTop;
		rc.m_height = fHeightBottom;

		pathBottom.AddRectangle( rc.m_x, rc.m_y, rc.m_width, rc.m_height );
	}
}

void GraphicsContextWaveDrawer::create_cue_lines_paths(
		const wxRect2DInt& rect,
		wxFloat32 fBaseline,
		const wxTimeSpanArray& cuePoints,
		wxGraphicsPath& pathTop, wxGraphicsPath& pathBottom )
{
	wxASSERT( cuePoints.GetCount() > 1 );

	wxFloat64 endPos = cuePoints.Last().GetMilliseconds().ToDouble();

	wxDouble fHeightTop	   = rect.m_height * ( 1.0f - fBaseline );
	wxDouble fHeightBottom = rect.m_height * fBaseline;

	for ( size_t i = 0, nCount1 = cuePoints.GetCount() - 1; i < nCount1; i += 1 )
	{
		wxFloat64 ts = cuePoints[ i ].GetMilliseconds().ToDouble();

		wxPoint2DDouble pt( ts * rect.m_width / endPos, 0.0 );
		pt.m_x += rect.m_x;
		pt.m_y += rect.m_y;

		pathTop.MoveToPoint( pt );
		pt.m_y += fHeightTop;
		pathTop.AddLineToPoint( pt );
		pathBottom.MoveToPoint( pt );
		pt.m_y += fHeightBottom;
		pathBottom.AddLineToPoint( pt );
	}
}

void GraphicsContextWaveDrawer::create_cue_segments_path(
		const wxRect2DInt& rect,
		const wxTimeSpanArray& cuePoints,
		wxGraphicsPath& path )
{
	wxASSERT( cuePoints.GetCount() > 1 );

	wxFloat64 endPos = cuePoints.Last().GetMilliseconds().ToDouble();

	for ( size_t i = 0, nCount1 = cuePoints.GetCount() - 1; i < nCount1; i += 2 )
	{
		wxFloat64 tsFrom = cuePoints[ i ].GetMilliseconds().ToDouble();
		wxFloat64 tsTo	 = cuePoints[ i + 1 ].GetMilliseconds().ToDouble();

		wxRect2DDouble rc( tsFrom * rect.m_width / endPos, 0, ( tsTo - tsFrom ) * rect.m_width / endPos, rect.m_height );
		rc.m_x += rect.m_x;
		rc.m_y += rect.m_y;

		path.AddRectangle( rc.m_x, rc.m_y, rc.m_width, rc.m_height );
	}
}

void GraphicsContextWaveDrawer::create_cue_lines_path(
		const wxRect2DInt& rect,
		const wxTimeSpanArray& cuePoints,
		wxGraphicsPath& path )
{
	wxASSERT( cuePoints.GetCount() > 1 );

	wxFloat64 endPos = cuePoints.Last().GetMilliseconds().ToDouble();

	for ( size_t i = 0, nCount1 = cuePoints.GetCount() - 1; i < nCount1; i += 1 )
	{
		wxFloat64 ts = cuePoints[ i ].GetMilliseconds().ToDouble();

		wxPoint2DDouble pt( ts * rect.m_width / endPos, 0 );
		pt.m_x += rect.m_x;
		pt.m_y += rect.m_y;
		path.MoveToPoint( pt );

		pt.m_y += rect.m_height;
		path.AddLineToPoint( pt );
	}
}

