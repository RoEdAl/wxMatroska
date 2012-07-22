/*
	WaveDrawerRasrer1.h
*/
#ifndef _WAVE_DRAWER_RASTER_H_
#define _WAVE_DRAWER_RASTER_H_

class RasterWaveDrawer :public GraphicsContextWaveDrawer
{
	public:

	RasterWaveDrawer(	
		wxUint64,
		wxGraphicsContext*,
		bool, bool, wxFloat32,
		wxRect2DInt, const wxColour&, const wxColour& );

	protected:

	static bool has_alpha( const wxColour& );
	static unsigned int linear_interpolation( unsigned int, unsigned int, wxFloat32 );
	static wxColour linear_interpolation( const wxColour&, const wxColour&, wxFloat32 );

	static void create_log_stops( wxGraphicsGradientStops&, const wxColour&, const wxColour&, wxUint32, const LogarithmicScale& );

	static wxImage create_gradient_bitmap( wxMemoryDC&, const wxColour&, const wxColour&, wxUint32 );
	static wxImage create_gradient_bitmap( const wxColour&, const wxColour&, wxUint32 );

	static wxImage create_log_gradient_bitmap( wxMemoryDC&, const wxColour&, const wxColour&, wxUint32, const LogarithmicScale& );
	static wxImage create_log_gradient_bitmap( const wxColour&, const wxColour&, wxUint32, const LogarithmicScale& );

	private:

	static wxImage draw_gradient_bitmap( wxMemoryDC&, const wxColour&, const wxColour&, wxUint32 );
	static wxImage draw_log_gradient_bitmap( wxMemoryDC&, const wxColour&, const wxColour&, wxUint32, const LogarithmicScale& );

	protected:

	wxColour m_clrFrom;
	wxColour m_clrTo;

	bool m_bLogarithmicScale;
	bool m_bLogarithmicColorGradient;
};

#endif

