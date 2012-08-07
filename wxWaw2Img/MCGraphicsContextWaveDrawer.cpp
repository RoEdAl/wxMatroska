/*
 *      MCGrpahicsContextWaveDrawer.cpp
 */
#include "StdWx.h"
#include "FloatArray.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "MultiChannelWaveDrawer.h"
#include "ArrayWaveDrawer.h"
#include "MCGraphicsContextWaveDrawer.h"

McGraphicalContextWaveDrawer::McGraphicalContextWaveDrawer( wxUint16 nChannels ):
	ArrayWaveDrawer( nChannels )
{}

const wxRect2DIntArray& McGraphicalContextWaveDrawer::GetRects() const
{
	return m_rects;
}

wxImage McGraphicalContextWaveDrawer::GetBitmap() const
{
	return m_img;
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

static wxRegion GetDrawersRegion( const wxRect2DIntArray& drawerRects )
{
	wxRegion rgn;
	for( wxRect2DIntArray::const_iterator i = drawerRects.begin(), end = drawerRects.end(); i != end; ++i )
	{
		const wxRect2DInt& rc = *i;
		rgn.Union( rc.m_x, rc.m_y, rc.m_width, rc.m_height );
	}

	return rgn;
}

wxGraphicsContext* McGraphicalContextWaveDrawer::Initialize(
		const wxSize& imageSize,
		int nImageColourDepth,
		const wxColour& clrBg,
		const wxRect2DIntArray& rects
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

	// wxCompositionMode mode = m_gc->GetCompositionMode();

	m_rects.Empty();
	for( wxRect2DIntArray::const_iterator i = rects.begin(), end = rects.end(); i != end; ++i )
	{
		m_rects.Add( *i );
	}

	wxLogInfo( _( "Initializing graphics context" ) );
	m_gc->SetAntialiasMode( wxANTIALIAS_DEFAULT );
	m_gc->SetInterpolationQuality( wxINTERPOLATION_BEST );
	m_gc->SetCompositionMode( wxCOMPOSITION_SOURCE );
	m_gc->SetPen( wxNullPen );
	m_gc->SetBrush( clrBg );
	m_gc->DrawRectangle( 0, 0, imageSize.GetWidth(), imageSize.GetHeight() );
	m_gc->SetBrush( wxNullBrush );
	m_gc->SetCompositionMode( wxCOMPOSITION_DEST );
	m_gc->Clip( GetDrawersRegion( rects ) );

	return m_gc.get();
}

void McGraphicalContextWaveDrawer::ProcessFinalizer()
{
	__super::ProcessFinalizer();

	m_gc->ResetClip();
	m_gc.reset();

	if ( m_memDc )
	{
		m_memDc->SelectObject( wxNullBitmap );
		m_memDc.reset();
		m_img = m_bmp.ConvertToImage();
		m_bmp = wxNullBitmap;
	}

#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE
	if ( m_emfDc )
	{
		m_emf.reset( m_emfDc->Close() );
		m_emfDc.reset();
	}
#endif
#endif
}

bool McGraphicalContextWaveDrawer::create_context_on_bitmap( const wxSize& imageSize, int nImageColourDepth )
{
	wxLogInfo( _( "Creating bitmap" ) );
	m_bmp = wxBitmap( imageSize, nImageColourDepth );

	wxLogInfo( _( "Creating memory context" ) );
	m_memDc.reset( new wxMemoryDC( m_bmp ) );

	wxLogInfo( _( "Creating graphics context" ) );
	m_gc.reset( wxGraphicsContext::Create( *m_memDc ) );

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

