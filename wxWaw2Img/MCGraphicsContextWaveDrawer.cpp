/*
 *      MCGrpahicsContextWaveDrawer.cpp
 */
#include "StdWx.h"
#include "LogarithmicScale.h"
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
	wxASSERT( m_bmp );
	return m_bmp->ConvertToImage();
}

#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE

wxEnhMetaFile* McGraphicalContextWaveDrawer::GetMetafile() const
{
	if ( m_emf )
	{
		return m_emf.get();
	}
	else
	{
		return NULL;
	}
}

#endif
#endif

wxGraphicsContext* McGraphicalContextWaveDrawer::Initialize(
		const wxSize& imageSize,
		int nImageColourDepth,
		const wxColour& clrBg,
		const wxRegion& rgn
		)
{
#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE

	if ( nImageColourDepth > 0 )
	{
		if ( !create_context_on_bitmap( imageSize, nImageColourDepth ) )
		{
			return NULL;
		}
	}
	else
	{
		create_context_on_emf( imageSize );
	}

#endif
#else

	if ( !create_context_on_bitmap( imageSize, nImageColourDepth ) )
	{
		return NULL;
	}

#endif

	//wxCompositionMode mode = m_gc->GetCompositionMode();

	wxLogInfo( _( "Initializing graphics context" ) );
	m_gc->SetAntialiasMode( wxANTIALIAS_DEFAULT );
	m_gc->SetInterpolationQuality( wxINTERPOLATION_BEST );
	m_gc->SetCompositionMode( wxCOMPOSITION_SOURCE );
	m_gc->SetPen( wxNullPen );
	m_gc->SetBrush( clrBg );
	m_gc->DrawRectangle( 0, 0, imageSize.GetWidth(), imageSize.GetHeight() );
	m_gc->SetBrush( wxNullBrush );
	m_gc->SetCompositionMode( wxCOMPOSITION_DEST );
	m_gc->Clip( rgn );

	return m_gc.get();
}

void McGraphicalContextWaveDrawer::ProcessFinalizer()
{
	__super::ProcessFinalizer();

	m_gc->ResetClip();
	m_gc.release();

	if ( m_mc )
	{
		m_mc->SelectObject( wxNullBitmap );
		m_mc.release();
	}

#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE
	
	if ( m_emfDc )
	{
		m_emf.reset( m_emfDc->Close() );
		m_emfDc.release();
	}

#endif
#endif
}

bool McGraphicalContextWaveDrawer::create_context_on_bitmap( const wxSize& imageSize, int nImageColourDepth )
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
		return false;
	}

	wxLogInfo( _( "Creating graphics context" ) );
	m_gc.reset( wxGraphicsContext::Create( *m_mc ) );

	return true;
}

#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE

void McGraphicalContextWaveDrawer::create_context_on_emf( const wxSize& imageSize )
{
	wxLogInfo( _( "Creating enhanced metafile" ) );
	m_emfDc.reset( new wxEnhMetaFileDC( wxEmptyString, imageSize.GetWidth(), imageSize.GetHeight() ) );

	wxLogInfo( _( "Creating graphics context" ) );
	m_gc.reset( wxGraphicsContext::Create( *m_emfDc ) );
}

#endif
#endif
