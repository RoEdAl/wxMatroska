/*
 *      WaveDrawerSimple.cpp
 */
#include "StdWx.h"
#include "FloatArray.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "DrawerSettings.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerSimple.h"

SimpleWaveDrawer::SimpleWaveDrawer( wxUint64 nNumberOfSamples,
									wxGraphicsContext* gc,
									const wxRect2DInt& rc,
									const DrawerSettings& drawerSettings,
									bool bUseCuePoints, const wxTimeSpanArray& cuePoints ):
	GraphicsContextWaveDrawer(
		nNumberOfSamples,
		gc,
		drawerSettings.UseLogarithmicScale(),
		drawerSettings.GetLogarithmBase(),
		rc,
		drawerSettings, bUseCuePoints, cuePoints ),
	m_bOneMiddleColour( false )
{}

void SimpleWaveDrawer::ProcessInitializer()
{
	__super::ProcessInitializer();

	m_bOneMiddleColour = m_drawerSettings.OneMiddleColour();

	if ( m_bOneMiddleColour )
	{
		m_pathUp = m_gc->CreatePath();
	}
	else
	{
		m_pathUp   = m_gc->CreatePath();
		m_pathDown = m_gc->CreatePath();
	}
}

void SimpleWaveDrawer::NextColumn( wxFloat32 fValue, wxFloat32 fLogValue )
{
	wxPoint2DDouble point_central( m_nCurrentColumn + m_rc.m_x, m_yoffset );

	wxFloat32 v	 = abs( m_drawerSettings.UseLogarithmicScale() ? fLogValue : fValue );
	wxFloat32 pu = v * m_heightUp;
	wxFloat32 pd = v * m_heightDown;

	if ( m_bOneMiddleColour )
	{
		m_pathUp.MoveToPoint( point_central.m_x, point_central.m_y - pu );
		m_pathUp.AddLineToPoint( point_central.m_x, point_central.m_y + pd );
	}
	else
	{
		m_pathUp.MoveToPoint( point_central.m_x, point_central.m_y );
		m_pathUp.AddLineToPoint( point_central.m_x, point_central.m_y - pu );

		m_pathDown.MoveToPoint( point_central.m_x, point_central.m_y );
		m_pathDown.AddLineToPoint( point_central.m_x, point_central.m_y + pd );
	}
}

void SimpleWaveDrawer::ProcessFinalizer()
{
	wxGraphicsPen pen = m_gc->CreatePen( m_drawerSettings.GetTopColourSettings().GetMiddleColour() );

	m_gc->SetPen( pen );
	m_gc->DrawPath( m_pathUp );

	if ( !m_bOneMiddleColour )
	{
		pen = m_gc->CreatePen( m_drawerSettings.GetBottomColourSettings().GetMiddleColour() );
		m_gc->SetPen( pen );
		m_gc->DrawPath( m_pathDown );
	}

	m_gc->SetPen( wxNullPen );

	m_pathUp   = wxNullGraphicsPath;
	m_pathDown = wxNullGraphicsPath;

	__super::ProcessFinalizer();
}

