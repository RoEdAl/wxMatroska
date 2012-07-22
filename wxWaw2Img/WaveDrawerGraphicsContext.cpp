/*
	WaveDrawerGraphicsContext.cpp
*/

#include "StdWx.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "WaveDrawerGraphicsContext.h"

GraphicsContextWaveDrawer::GraphicsContextWaveDrawer( 
	wxUint64 nNumberOfSamples, wxGraphicsContext* gc,
	bool bCalcLogarithmic, wxFloat32 fLogBase,
	wxRect2DInt rc )
	:SampleChunker( nNumberOfSamples, rc.m_width, bCalcLogarithmic, fLogBase ),
	m_gc( gc ),
	m_rc( rc )
{}

void GraphicsContextWaveDrawer::ProcessInitializer()
{
	m_height2 = m_rc.m_height / 2.0f;
	m_yoffset = m_rc.m_y + m_height2;
	m_nImgHeight = ceil ( m_height2 );
}

void GraphicsContextWaveDrawer::ProcessFinalizer()
{
}
