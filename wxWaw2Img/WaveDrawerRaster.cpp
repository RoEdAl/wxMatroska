/*
	WaveDrawerRaster1.cpp
*/
#include "StdWx.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerRaster.h"

RasterWaveDrawer::RasterWaveDrawer(
	wxUint64 nNumberOfSamples,
	wxGraphicsContext* gc,
	bool bLogarithmicScale, bool bLogarithmicColorGradient, wxFloat32 fLogBase,
	wxRect2DInt rc, const wxColour& clrFrom, const wxColour& clrTo )
	:GraphicsContextWaveDrawer( nNumberOfSamples, gc, bLogarithmicScale || bLogarithmicColorGradient, fLogBase, rc ),
	m_clrFrom( clrFrom ), m_clrTo( clrTo ),
	m_bLogarithmicScale(bLogarithmicScale), m_bLogarithmicColorGradient( bLogarithmicColorGradient )
{}

bool RasterWaveDrawer::has_alpha( const wxColour& clr )
{
	return clr.Alpha() != wxALPHA_OPAQUE;
}

unsigned int RasterWaveDrawer::linear_interpolation( unsigned int v1, unsigned int v2, wxFloat32 f )
{
	int v = ceil( f * ((int)v2-(int)v1) + v1 );
	if ( v < 0 )
	{
		v = 0;
	}
	else if ( v > 255 )
	{
		v = 255;
	}

	return v;
}

wxColour RasterWaveDrawer::linear_interpolation( const wxColour& c1, const wxColour& c2, wxFloat32 f )
{
	unsigned int r = linear_interpolation( c1.Red(), c2.Red(), f );
	unsigned int g = linear_interpolation( c1.Green(), c2.Green(), f );
	unsigned int b = linear_interpolation( c1.Blue(), c2.Blue(), f );
	unsigned int a = linear_interpolation( c1.Alpha(), c2.Alpha(), f );

	return wxColour( r, g, b ,a );
}

void RasterWaveDrawer::create_log_stops( wxGraphicsGradientStops& stops, const wxColour& clrFrom, const wxColour& clrTo, wxUint32 nHeight, wxFloat32 fLogBase )
{
	wxASSERT( nHeight > 0 );

	wxFloat32 fLogLogBase = log( fLogBase );

	for( wxUint32 i=nHeight; i > 0; i-- )
	{
		wxFloat32 p = 1.0f * i / nHeight;
		wxFloat32 pl = log( p * (fLogBase - 1.0f) + 1.0f ) / fLogLogBase;

		stops.Add( linear_interpolation( clrFrom, clrTo, pl ), (1.0f - p) / 2.0f );
	}

	for( wxUint32 i=0; i < nHeight; i++ )
	{
		wxFloat32 p = 1.0f * i / nHeight;
		wxFloat32 pl = log( p * (fLogBase - 1.0f) + 1.0f ) / fLogLogBase;

		stops.Add( linear_interpolation( clrFrom, clrTo, pl ), 0.5f + (p / 2.0f) );
	}
}

wxImage RasterWaveDrawer::draw_gradient_bitmap( wxMemoryDC& mdc, const wxColour& clrFrom, const wxColour& clrTo, wxUint32 nHeight )
{
	wxASSERT( nHeight > 0 );
	wxBitmap bmp( 1, 2 * nHeight, ( has_alpha(clrFrom) || has_alpha(clrTo) ) ? 32 : 24 );

	{
		mdc.SelectObject( bmp );

		wxScopedPtr<wxGraphicsContext> gc( wxGraphicsContext::Create( mdc ) );

		wxGraphicsGradientStops gstops( clrTo, clrTo );
		gstops.Add( clrFrom, 0.5f );
		
		wxGraphicsBrush brush = gc->CreateLinearGradientBrush( 0,0, 0,2*nHeight-1, gstops );

		gc->SetPen( wxNullPen );
		gc->SetBrush( brush );
		gc->DrawRectangle( 0,0,1,2*nHeight);

		mdc.SelectObject( wxNullBitmap );
	}

	//bmp.SaveFile( "C:/Documents and Settings/Normal/My Documents/Visual Studio 2010/Projects/wxMatroska/wxWaw2Img/test1.png", wxBITMAP_TYPE_PNG );
	return bmp.ConvertToImage();
}

wxImage RasterWaveDrawer::create_gradient_bitmap( wxMemoryDC& mdc, const wxColour& clrFrom, const wxColour& clrTo, wxUint32 nHeight )
{
	return draw_gradient_bitmap( mdc, clrFrom, clrTo, nHeight );
}

wxImage RasterWaveDrawer::create_gradient_bitmap( const wxColour& clrFrom, const wxColour& clrTo, wxUint32 nHeight )
{
	wxMemoryDC mdc;
	return draw_gradient_bitmap( mdc, clrFrom, clrTo, nHeight );
}

wxImage RasterWaveDrawer::draw_log_gradient_bitmap( wxMemoryDC& mdc, const wxColour& clrFrom, const wxColour& clrTo, wxUint32 nHeight, wxFloat32 fLogBase )
{
	wxASSERT( nHeight > 0 );
	wxBitmap bmp( 1, 2 * nHeight, ( has_alpha(clrFrom) || has_alpha(clrTo) ) ? 32 : 24 );

	{
		mdc.SelectObject( bmp );

		wxScopedPtr<wxGraphicsContext> gc( wxGraphicsContext::Create( mdc ) );

		wxGraphicsGradientStops gstops( clrTo, clrTo );
		create_log_stops( gstops, clrFrom, clrTo, nHeight, fLogBase );
		
		wxGraphicsBrush brush = gc->CreateLinearGradientBrush( 0,0, 0,2*nHeight-1, gstops );

		gc->SetPen( wxNullPen );
		gc->SetBrush( brush );
		gc->DrawRectangle( 0,0,1,2*nHeight);

		mdc.SelectObject( wxNullBitmap );
	}

	//bmp.SaveFile( "C:/Documents and Settings/Normal/My Documents/Visual Studio 2010/Projects/wxMatroska/wxWaw2Img/test1.png", wxBITMAP_TYPE_PNG );
	return bmp.ConvertToImage();
}

wxImage RasterWaveDrawer::create_log_gradient_bitmap( wxMemoryDC& mdc, const wxColour& clrFrom, const wxColour& clrTo, wxUint32 nHeight, wxFloat32 fLogBase )
{
	return draw_log_gradient_bitmap( mdc, clrFrom, clrTo, nHeight, fLogBase );
}

wxImage RasterWaveDrawer::create_log_gradient_bitmap( const wxColour& clrFrom, const wxColour& clrTo, wxUint32 nHeight, wxFloat32 fLogBase )
{
	wxMemoryDC mdc;
	return draw_log_gradient_bitmap( mdc, clrFrom, clrTo, nHeight, fLogBase );
}
