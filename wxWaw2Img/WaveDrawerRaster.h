/*
 *      WaveDrawerRasrer1.h
 */
#ifndef _WAVE_DRAWER_RASTER_H_
#define _WAVE_DRAWER_RASTER_H_

template<typename T>
wxImage draw_on_bitmap(
		wxMemoryDC& mdc,
		const wxSize& bmpSize,
		int nImgDepth,
		T drawer )
{
	wxBitmap bmp( bmpSize.GetWidth(), bmpSize.GetHeight(), nImgDepth );

	{
		mdc.SelectObject( bmp );
		wxScopedPtr< wxGraphicsContext > gc( wxGraphicsContext::Create( mdc ) );
		drawer( *gc );
		mdc.SelectObject( wxNullBitmap );
	}

	// bmp.SaveFile( "C:/Documents and Settings/Normal/My Documents/Visual Studio 2010/Projects/wxMatroska/wxWaw2Img/test1.png", wxBITMAP_TYPE_PNG );
	return bmp.ConvertToImage();
}

class RasterWaveDrawer:
	public GraphicsContextWaveDrawer
{
	public:

		RasterWaveDrawer( wxUint64,
						  wxGraphicsContext*,
						  const wxRect2DInt&,
						  const DrawerSettings&,
						  bool, const wxTimeSpanArray& );

	protected:

		static void create_log_stops( wxGraphicsGradientStops &, const wxColour &, const wxColour &, wxUint32, const LogarithmicScale & );
		static void create_log_stops( wxGraphicsGradientStops &, const wxColour &, const wxColour &, const wxColour &, wxUint32, wxFloat32, const LogarithmicScale & );

		static wxImage create_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, wxUint32 );
		static wxImage create_gradient_bitmap( const wxColour &, const wxColour &, wxUint32 );

		static wxImage create_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, const wxColour &, wxUint32, wxFloat32 );
		static wxImage create_gradient_bitmap( const wxColour &, const wxColour &, const wxColour &, wxUint32, wxFloat32 );

		static wxImage create_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, const wxColour &, const wxColour &, wxUint32, wxFloat32 );
		static wxImage create_gradient_bitmap( const wxColour &, const wxColour &, const wxColour &, const wxColour &, wxUint32, wxFloat32 );

		static wxImage create_log_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, wxUint32, const LogarithmicScale & );
		static wxImage create_log_gradient_bitmap( const wxColour &, const wxColour &, wxUint32, const LogarithmicScale & );

		static wxImage create_log_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, const wxColour &, wxUint32, wxFloat32, const LogarithmicScale & );
		static wxImage create_log_gradient_bitmap( const wxColour &, const wxColour &, const wxColour &, wxUint32, wxFloat32, const LogarithmicScale & );

		wxImage create_solid_bitmap( wxMemoryDC&, const wxColour& ) const;

	private:

		static wxImage draw_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, wxUint32 );
		static wxImage draw_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, const wxColour &, wxUint32, wxFloat32 );
		static wxImage draw_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, const wxColour &, const wxColour &, wxUint32, wxFloat32 );

		static wxImage draw_log_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, wxUint32, const LogarithmicScale & );
		static wxImage draw_log_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, const wxColour &, wxUint32, wxFloat32, const LogarithmicScale & );
};

#endif

