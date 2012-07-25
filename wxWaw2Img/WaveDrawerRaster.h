/*
 *      WaveDrawerRasrer1.h
 */
#ifndef _WAVE_DRAWER_RASTER_H_
#define _WAVE_DRAWER_RASTER_H_

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

		static void create_log_stops( wxGraphicsGradientStops &, const wxColour &, const wxColour &, const wxColour&, wxUint32, wxFloat32, const LogarithmicScale & );

		static wxImage create_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, const wxColour&, wxUint32, wxFloat32 );
		static wxImage create_gradient_bitmap( const wxColour &, const wxColour &, const wxColour&, wxUint32, wxFloat32 );

		static wxImage create_log_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, const wxColour&, wxUint32, wxFloat32, const LogarithmicScale & );
		static wxImage create_log_gradient_bitmap( const wxColour &, const wxColour &, const wxColour&, wxUint32, wxFloat32, const LogarithmicScale & );

	private:

		static wxImage draw_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, const wxColour&, wxUint32, wxFloat32 );
		static wxImage draw_log_gradient_bitmap( wxMemoryDC &, const wxColour &, const wxColour &, const wxColour&, wxUint32, wxFloat32, const LogarithmicScale & );
};

#endif

