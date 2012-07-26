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
		const wxColour& clrBg
		)
{
	wxLogInfo( _( "Creating bitmap" ) );
	m_bmp.reset( new wxBitmap( imageSize.GetWidth(), imageSize.GetHeight(), nImageColourDepth ) );

	wxLogInfo( _( "Creating memory context" ) );
	m_mc.reset( new wxMemoryDC() );

	m_mc->SelectObject( *m_bmp );

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
	m_gc->SetBrush( clrBg );
	m_gc->DrawRectangle( 0, 0, imageSize.GetWidth(), imageSize.GetHeight() );
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

