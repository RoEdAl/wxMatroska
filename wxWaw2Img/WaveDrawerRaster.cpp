/*
 *      WaveDrawerRaster.cpp
 */
#include "StdWx.h"
#include "FloatArray.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "DrawerSettings.h"
#include "ColourInterpolation.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerRaster.h"

RasterWaveDrawer::RasterWaveDrawer( wxUint64 nNumberOfSamples,
									wxGraphicsContext* gc,
									const wxRect2DInt& rc,
									const DrawerSettings& drawerSettings,
									bool bUseCuePoints, const wxTimeSpanArray& cuePoints ):
	GraphicsContextWaveDrawer(
		nNumberOfSamples,
		gc,
		drawerSettings.UseLogarithmicScale() || drawerSettings.UseLogarithmicColorGradient(),
		drawerSettings.GetLogarithmBase(),
		rc,
		drawerSettings,
		bUseCuePoints, cuePoints )
{}

void RasterWaveDrawer::create_log_stops(
		wxGraphicsGradientStops& stops,
		const wxColour& clrFrom, const wxColour& clrMiddle, const wxColour& clrTo,
		wxUint32 nHeight, wxFloat32 fBaselinePosition,
		const LogarithmicScale& logarithmicScale )
{
	wxASSERT( nHeight > 0 );

	wxFloat32 fBaselinePosition1 = 1.0f - fBaselinePosition;

	for ( wxUint32 i = 1; i < nHeight; i++ )
	{
		wxFloat32 p = 1.0f * i / nHeight;

		if ( p < fBaselinePosition1 )
		{
			wxFloat32 pp = ( fBaselinePosition1 - p ) / fBaselinePosition1;
			stops.Add( ColourInterpolation::linear_interpolation( clrMiddle, clrFrom, logarithmicScale( pp ) ), p );
		}
		else if ( fBaselinePosition != 0.0f )
		{
			wxFloat32 pp = ( p - fBaselinePosition1 ) / fBaselinePosition;
			stops.Add( ColourInterpolation::linear_interpolation( clrMiddle, clrTo, logarithmicScale( pp ) ), p );
		}
	}
}

void RasterWaveDrawer::create_log_stops(
		wxGraphicsGradientStops& stops,
		const wxColour& clrFrom, const wxColour& clrTo,
		wxUint32 nHeight,
		const LogarithmicScale& logarithmicScale )
{
	wxASSERT( nHeight > 0 );

	for ( wxUint32 i = 1; i < nHeight; i++ )
	{
		wxFloat32 p = 1.0f * i / nHeight;
		stops.Add( ColourInterpolation::linear_interpolation( clrFrom, clrTo, logarithmicScale( p ) ), p );
	}
}

wxImage RasterWaveDrawer::draw_gradient_bitmap(
		wxMemoryDC& mdc,
		const wxColour& clrFrom, const wxColour& clrTo,
		wxUint32 nHeight )
{
	wxASSERT( nHeight > 0 );

	class drawer
	{
		public:

		drawer( const wxColour& clrFrom, const wxColour& clrTo, wxUint32 nHeight ):
				m_clrFrom( clrFrom ), m_clrTo( clrTo ), m_nHeight( nHeight )
			{}

		void operator()( wxGraphicsContext& gc )
		{
			wxGraphicsBrush brush = gc.CreateLinearGradientBrush( 0, 0, 0, m_nHeight, m_clrFrom, m_clrTo );
			gc.SetPen( wxNullPen );
			gc.SetBrush( brush );
			gc.DrawRectangle( 0, 0, 1, m_nHeight );
		}
			
		protected:

		const wxColour& m_clrFrom;
		const wxColour& m_clrTo;
		wxUint32 m_nHeight;
	};

	return draw_on_bitmap(
		mdc,
		wxSize( 1, nHeight ),
		( ColourInterpolation::has_alpha( clrFrom ) || ColourInterpolation::has_alpha( clrTo ) ) ? 32 : 24,
		drawer( clrFrom, clrTo, nHeight ) );
}

wxImage RasterWaveDrawer::draw_gradient_bitmap(
		wxMemoryDC& mdc,
		const wxColour& clrFrom, const wxColour& clrMiddle, const wxColour& clrTo,
		wxUint32 nHeight, wxFloat32 fBaselinePosition
		)
{
	wxASSERT( nHeight > 0 );

	class drawer
	{
		public:

		drawer( const wxColour& clrFrom, const wxColour& clrMiddle, const wxColour& clrTo, wxUint32 nHeight, wxFloat32 fBaseline ):
				m_clrFrom( clrFrom ), m_clrMiddle( clrMiddle), m_clrTo( clrTo ), m_nHeight( nHeight ), m_fBaseline( fBaseline )
			{}

		void operator()( wxGraphicsContext& gc )
		{
			wxGraphicsGradientStops gstops( m_clrFrom, m_clrTo );
			gstops.Add( m_clrMiddle, 1.0f - m_fBaseline );

			wxGraphicsBrush brush = gc.CreateLinearGradientBrush( 0, 0, 0, m_nHeight, gstops );

			gc.SetPen( wxNullPen );
			gc.SetBrush( brush );
			gc.DrawRectangle( 0, 0, 1, m_nHeight );
		}
			
		protected:

		const wxColour& m_clrFrom;
		const wxColour& m_clrMiddle;
		const wxColour& m_clrTo;
		wxUint32 m_nHeight;
		wxFloat32 m_fBaseline;
	};

	return draw_on_bitmap(
		mdc,
		wxSize( 1, nHeight ),
		( ColourInterpolation::has_alpha( clrFrom ) || ColourInterpolation::has_alpha( clrMiddle ) || ColourInterpolation::has_alpha( clrTo ) ) ? 32 : 24,
		drawer( clrFrom, clrMiddle, clrTo, nHeight, fBaselinePosition ) );
}

wxImage RasterWaveDrawer::draw_gradient_bitmap(
		wxMemoryDC& mdc,
		const wxColour& clrTop, const wxColour& clrTopMiddle,
		const wxColour& clrBottomMiddle, const wxColour& clrBottom,
		wxUint32 nHeight, wxFloat32 fBaselinePosition )
{
	wxASSERT( nHeight > 0 );

	class drawer
	{
		public:

		drawer( const wxColour& clrTop, const wxColour& clrTopMiddle,
				const wxColour& clrBottomMiddle, const wxColour& clrBottom,
				wxUint32 nHeight, wxFloat32 fBaseline ):
				m_clrTop( clrTop ), m_clrTopMiddle( clrTopMiddle),
				m_clrBottomMiddle( clrBottomMiddle ), m_clrBottom( clrBottom ),
				m_nHeight( nHeight ), m_fBaseline( fBaseline )
			{}

		void operator()( wxGraphicsContext& gc )
		{
			wxGraphicsGradientStops gstops( m_clrTop, m_clrBottom );
			gstops.Add( m_clrTopMiddle, 1.0f - m_fBaseline );
			gstops.Add( m_clrBottomMiddle, 1.0f - m_fBaseline );

			wxGraphicsBrush brush = gc.CreateLinearGradientBrush( 0, 0, 0, m_nHeight, gstops );

			gc.SetPen( wxNullPen );
			gc.SetBrush( brush );
			gc.DrawRectangle( 0, 0, 1, m_nHeight );
		}
			
		protected:

		const wxColour& m_clrTop;
		const wxColour& m_clrTopMiddle;
		const wxColour& m_clrBottomMiddle;
		const wxColour& m_clrBottom;
		wxUint32 m_nHeight;
		wxFloat32 m_fBaseline;
	};

	return draw_on_bitmap(
		mdc,
		wxSize( 1, nHeight ),
		( ColourInterpolation::has_alpha( clrTop ) || ColourInterpolation::has_alpha( clrTopMiddle ) ||	ColourInterpolation::has_alpha( clrBottomMiddle ) || ColourInterpolation::has_alpha( clrBottom ) ) ? 32 : 24,
		drawer( clrTop, clrTopMiddle, clrBottomMiddle, clrBottom, nHeight, fBaselinePosition ) );
}

wxImage RasterWaveDrawer::create_gradient_bitmap(
		wxMemoryDC& mdc,
		const wxColour& clrFrom, const wxColour& clrTo,
		wxUint32 nHeight )
{
	return draw_gradient_bitmap( mdc, clrFrom, clrTo, nHeight );
}

wxImage RasterWaveDrawer::create_gradient_bitmap(
		const wxColour& clrFrom, const wxColour& clrTo,
		wxUint32 nHeight )
{
	wxMemoryDC mdc;

	return draw_gradient_bitmap( mdc, clrFrom, clrTo, nHeight );
}

wxImage RasterWaveDrawer::create_gradient_bitmap(
		wxMemoryDC& mdc,
		const wxColour& clrFrom, const wxColour& clrMiddle, const wxColour& clrTo,
		wxUint32 nHeight, wxFloat32 fBaselinePosition )
{
	return draw_gradient_bitmap( mdc, clrFrom, clrMiddle, clrTo, nHeight, fBaselinePosition );
}

wxImage RasterWaveDrawer::create_gradient_bitmap(
		const wxColour& clrFrom, const wxColour& clrMiddle, const wxColour& clrTo,
		wxUint32 nHeight, wxFloat32 fBaselinePosition )
{
	wxMemoryDC mdc;

	return draw_gradient_bitmap( mdc, clrFrom, clrMiddle, clrTo, nHeight, fBaselinePosition );
}

wxImage RasterWaveDrawer::create_gradient_bitmap(
		wxMemoryDC& mdc,
		const wxColour& clrTop, const wxColour& clrTopMiddle,
		const wxColour& clrBottomMiddle, const wxColour& clrBottom,
		wxUint32 nHeight, wxFloat32 fBaselinePosition )
{
	return draw_gradient_bitmap( mdc, clrTop, clrTopMiddle, clrBottomMiddle, clrBottom, nHeight, fBaselinePosition );
}

wxImage RasterWaveDrawer::create_gradient_bitmap(
		const wxColour& clrTop, const wxColour& clrTopMiddle,
		const wxColour& clrBottomMiddle, const wxColour& clrBottom,
		wxUint32 nHeight, wxFloat32 fBaselinePosition )
{
	wxMemoryDC mdc;

	return draw_gradient_bitmap( mdc, clrTop, clrTopMiddle, clrBottomMiddle, clrBottom, nHeight, fBaselinePosition );
}

wxImage RasterWaveDrawer::draw_log_gradient_bitmap(
		wxMemoryDC& mdc,
		const wxColour& clrFrom, const wxColour& clrTo,
		wxUint32 nHeight,
		const LogarithmicScale& logarithmicScale )
{
	wxASSERT( nHeight > 0 );
	wxBitmap bmp( 1, nHeight, ( ColourInterpolation::has_alpha( clrFrom ) || ColourInterpolation::has_alpha( clrTo ) ) ? 32 : 24 );

	{
		mdc.SelectObject( bmp );

		wxScopedPtr< wxGraphicsContext > gc( wxGraphicsContext::Create( mdc ) );

		wxGraphicsGradientStops gstops( clrTo, clrTo );
		create_log_stops( gstops, clrFrom, clrTo, nHeight, logarithmicScale );

		wxGraphicsBrush brush = gc->CreateLinearGradientBrush( 0, 0, 0, nHeight - 1, gstops );

		gc->SetPen( wxNullPen );
		gc->SetBrush( brush );
		gc->DrawRectangle( 0, 0, 1, nHeight );

		mdc.SelectObject( wxNullBitmap );
	}

	// bmp.SaveFile( "C:/Documents and Settings/Normal/My Documents/Visual Studio 2010/Projects/wxMatroska/wxWaw2Img/test1.png", wxBITMAP_TYPE_PNG );
	return bmp.ConvertToImage();
}

wxImage RasterWaveDrawer::draw_log_gradient_bitmap(
		wxMemoryDC& mdc,
		const wxColour& clrFrom, const wxColour& clrMiddle, const wxColour& clrTo,
		wxUint32 nHeight, wxFloat32 fBaselinePosition,
		const LogarithmicScale& logarithmicScale )
{
	wxASSERT( nHeight > 0 );
	wxBitmap bmp( 1, nHeight, ( ColourInterpolation::has_alpha( clrFrom ) || ColourInterpolation::has_alpha( clrMiddle ) || ColourInterpolation::has_alpha( clrTo ) ) ? 32 : 24 );

	{
		mdc.SelectObject( bmp );

		wxScopedPtr< wxGraphicsContext > gc( wxGraphicsContext::Create( mdc ) );

		wxGraphicsGradientStops gstops( clrTo, clrTo );
		create_log_stops( gstops, clrFrom, clrMiddle, clrTo, nHeight, fBaselinePosition, logarithmicScale );

		wxGraphicsBrush brush = gc->CreateLinearGradientBrush( 0, 0, 0, nHeight - 1, gstops );

		gc->SetPen( wxNullPen );
		gc->SetBrush( brush );
		gc->DrawRectangle( 0, 0, 1, nHeight );

		mdc.SelectObject( wxNullBitmap );
	}

	// bmp.SaveFile( "C:/Documents and Settings/Normal/My Documents/Visual Studio 2010/Projects/wxMatroska/wxWaw2Img/test1.png", wxBITMAP_TYPE_PNG );
	return bmp.ConvertToImage();
}

wxImage RasterWaveDrawer::create_log_gradient_bitmap(
		wxMemoryDC& mdc,
		const wxColour& clrFrom, const wxColour& clrTo,
		wxUint32 nHeight,
		const LogarithmicScale& logarithmicScale )
{
	return draw_log_gradient_bitmap( mdc, clrFrom, clrTo, nHeight, logarithmicScale );
}

wxImage RasterWaveDrawer::create_log_gradient_bitmap(
		const wxColour& clrFrom, const wxColour& clrTo,
		wxUint32 nHeight,
		const LogarithmicScale& logarithmicScale )
{
	wxMemoryDC mdc;

	return draw_log_gradient_bitmap( mdc, clrFrom, clrTo, nHeight, logarithmicScale );
}

wxImage RasterWaveDrawer::create_log_gradient_bitmap(
		wxMemoryDC& mdc,
		const wxColour& clrFrom, const wxColour& clrMiddle, const wxColour& clrTo,
		wxUint32 nHeight, wxFloat32 fBaselinePosition,
		const LogarithmicScale& logarithmicScale )
{
	return draw_log_gradient_bitmap( mdc, clrFrom, clrMiddle, clrTo, nHeight, fBaselinePosition, logarithmicScale );
}

wxImage RasterWaveDrawer::create_log_gradient_bitmap(
		const wxColour& clrFrom, const wxColour& clrMiddle, const wxColour& clrTo,
		wxUint32 nHeight, wxFloat32 fBaselinePosition,
		const LogarithmicScale& logarithmicScale )
{
	wxMemoryDC mdc;

	return draw_log_gradient_bitmap( mdc, clrFrom, clrMiddle, clrTo, nHeight, fBaselinePosition, logarithmicScale );
}

wxImage RasterWaveDrawer::create_solid_bitmap( wxMemoryDC& mdc, const wxColour& clr ) const
{
	class drawer
	{
		public:

		drawer( const wxColour& clr, wxUint32 nHeight ):
				m_clr( clr ), m_nHeight( nHeight )
			{}

		void operator()( wxGraphicsContext& gc )
		{
			gc.SetPen( wxNullPen );
			gc.SetBrush( m_clr );
			gc.DrawRectangle( 0, 0, 1, m_nHeight );
		}
			
		protected:

		const wxColour& m_clr;
		wxUint32 m_nHeight;
	};

	return draw_on_bitmap(
		mdc,
		wxSize( 1, m_rc.m_height ),
		ColourInterpolation::has_alpha( clr ) ? 32 : 24,
		drawer( clr, m_rc.m_height ) );
}

