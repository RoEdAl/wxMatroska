/*
 *      MCGrpahicsContextWaveDrawer.cpp
 */

#include "StdWx.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "MultiChannelWaveDrawer.h"
#include "ArrayWaveDrawer.h"
#include "MCGraphicsContextWaveDrawer.h"

McGraphicalContextWaveDrawer::McGraphicalContextWaveDrawer( wxUint16 nChannels ):
	ArrayWaveDrawer( nChannels )
{}

wxImage McGraphicalContextWaveDrawer::GetBitmap() const
{
	wxASSERT( m_bmp.get() != NULL );
	return m_bmp->ConvertToImage();
}

wxGraphicsContext* McGraphicalContextWaveDrawer::Initialize(
	const wxSize& imageSize,
	int nImageColourDepth,
	const wxColour& clrBgTop, const wxColour& clrBgBottom,
	wxFloat32 fBaseline
)
{
	wxLogInfo( _( "Creating bitmap" ) );
	m_bmp.reset( new wxBitmap( imageSize.GetWidth(), imageSize.GetHeight(), nImageColourDepth ) );

	wxLogInfo( _( "Creating memory context" ) );
	m_mc.reset( new wxMemoryDC() );

	m_mc->SelectObject( *m_bmp );

	wxRect2DDouble rcTop( 0,0, imageSize.GetWidth(), ( 1.0f - fBaseline ) * imageSize.GetHeight() );
	wxRect2DDouble rcBottom( 0, rcTop.m_height, imageSize.GetWidth(), ( 1.0f * imageSize.GetHeight() ) - rcTop.m_height );

	if ( !m_mc->IsOk() )
	{
		m_mc.release();
		wxLogError( _( "Fail to set bitmap into memory context" ) );
		return NULL;
	}

	wxLogInfo( _( "Creating graphics context" ) );
	m_gc.reset( wxGraphicsContext::Create( *m_mc ) );

	wxLogInfo( _( "Initializing graphics context" ) );
	m_gc->SetAntialiasMode( wxANTIALIAS_DEFAULT );
	m_gc->SetInterpolationQuality( wxINTERPOLATION_BEST );
	m_gc->SetPen( wxNullPen );
	m_gc->SetBrush( clrBgTop );
	m_gc->DrawRectangle( rcTop.m_x, rcTop.m_y, rcTop.m_width, rcTop.m_height );
	m_gc->SetBrush( clrBgTop );
	m_gc->DrawRectangle( rcBottom.m_x, rcBottom.m_y, rcBottom.m_width, rcBottom.m_height );
	m_gc->SetBrush( *wxTRANSPARENT_BRUSH );

	return m_gc.get();
}

void McGraphicalContextWaveDrawer::ProcessFinalizer()
{
	__super::ProcessFinalizer();

	m_gc.reset();
	m_mc->SelectObject( wxNullBitmap );
	m_mc.reset();
}

