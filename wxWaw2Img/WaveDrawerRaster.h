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
		bool, wxFloat32,
		wxRect2DInt, const wxColour&, const wxColour& );

	public:

	static bool has_alpha( const wxColour& );
	static unsigned int linear_interpolation( unsigned int, unsigned int, wxFloat32 );
	static wxColour linear_interpolation( const wxColour&, const wxColour&, wxFloat32 );
	static wxImage create_gradient_bitmap( wxMemoryDC&, const wxColour&, const wxColour&, wxUint32 );
	static wxImage create_gradient_bitmap( const wxColour&, const wxColour&, wxUint32 );

	private:

	static wxImage draw_gradient_bitmap( wxMemoryDC&, const wxColour&, const wxColour&, wxUint32 );

	protected:

	wxColour m_clrFrom;
	wxColour m_clrTo;
};

#endif

