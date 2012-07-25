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
	m_bBaseline50( false )
{}

void SimpleWaveDrawer::ProcessInitializer()
{
	__super::ProcessInitializer();

	m_bBaseline50 = ( m_drawerSettings.GetBaselinePosition() == 0.5f ) && !m_drawerSettings.DrawWithGradient();

	if ( m_bBaseline50 )
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

	if ( m_bBaseline50 )
	{
		wxFloat32 p = ( m_drawerSettings.UseLogarithmicScale() ? fLogValue : fValue ) * m_heightUp;

		m_pathUp.MoveToPoint( point_central.m_x, point_central.m_y - p );
		m_pathUp.AddLineToPoint( point_central.m_x, point_central.m_y + p );
	}
	else
	{
		wxFloat32 v = abs( m_drawerSettings.UseLogarithmicScale() ? fLogValue : fValue );

		wxFloat32 p = v * m_heightUp;
		m_pathUp.MoveToPoint( point_central.m_x, point_central.m_y );
		m_pathUp.AddLineToPoint( point_central.m_x, point_central.m_y - p );

		p = v * m_heightDown;
		m_pathDown.MoveToPoint( point_central.m_x, point_central.m_y );
		m_pathDown.AddLineToPoint( point_central.m_x, point_central.m_y + p );
	}
}

void SimpleWaveDrawer::ProcessFinalizer()
{
	__super::ProcessFinalizer();

	wxGraphicsPen pen = m_gc->CreatePen( wxPen( m_drawerSettings.GetColourFrom() ) );
	m_gc->SetPen( pen );
	m_gc->DrawPath( m_pathUp );

	if ( !m_bBaseline50 )
	{
		pen = m_gc->CreatePen( wxPen( m_drawerSettings.GetColourTo() ) );
		m_gc->SetPen( pen );
		m_gc->DrawPath( m_pathDown );
	}
}

