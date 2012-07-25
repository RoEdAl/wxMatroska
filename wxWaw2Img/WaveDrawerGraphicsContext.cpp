/*
 *      WaveDrawerGraphicsContext.cpp
 */

#include "StdWx.h"
#include "FloatArray.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "WaveDrawerGraphicsContext.h"

GraphicsContextWaveDrawer::GraphicsContextWaveDrawer( wxUint64 nNumberOfSamples, wxGraphicsContext* gc,
													  bool bCalcLogarithmic, wxFloat32 fLogBase,
													  wxRect2DInt rc,
													  bool bUseCuePoints, const wxTimeSpanArray& cuePoints,
													  const wxColour& clrBgSecond ):
	SampleChunker( nNumberOfSamples, rc.m_width, bCalcLogarithmic, fLogBase ),
	m_gc( gc ),
	m_rc( rc ),
	m_bUseCuePoints( bUseCuePoints ), m_cuePoints( cuePoints ),
	m_clrBgSecond( clrBgSecond )
{}

void GraphicsContextWaveDrawer::ProcessInitializer()
{
	m_height2	 = m_rc.m_height / 2.0f;
	m_yoffset	 = m_rc.m_y + m_height2;
	m_nImgHeight = ceil( m_height2 );

	if ( m_bUseCuePoints )
	{
		wxGraphicsPath path = create_cue_region( *m_gc, m_rc, m_cuePoints );
		m_gc->SetBrush( m_clrBgSecond );
		m_gc->FillPath( path );
		m_gc->SetBrush( wxNullBrush );
	}
}

void GraphicsContextWaveDrawer::ProcessFinalizer()
{}

wxGraphicsPath GraphicsContextWaveDrawer::create_cue_region( wxGraphicsContext& gc, const wxRect2DInt& rect, const wxTimeSpanArray& cuePoints )
{
	wxASSERT( cuePoints.GetCount() > 1 );

	wxFloat64 endPos = cuePoints.Last().GetMilliseconds().ToDouble();

	wxGraphicsPath path = gc.CreatePath();
	for ( size_t i = 0, nCount1 = cuePoints.GetCount() - 1; i < nCount1; i += 2 )
	{
		wxFloat64 tsFrom = cuePoints[ i ].GetMilliseconds().ToDouble();
		wxFloat64 tsTo	 = cuePoints[ i + 1 ].GetMilliseconds().ToDouble();

		wxRect2DDouble rc( tsFrom * rect.m_width / endPos, 0, ( tsTo - tsFrom ) * rect.m_width / endPos, rect.m_height );
		rc.m_x += rect.m_x;
		rc.m_y += rect.m_y;

		path.AddRectangle( rc.m_x, rc.m_y, rc.m_width, rc.m_height );
	}

	return path;
}

