/*
 *      WaveDrawerRasrer1.h
 */
#ifndef _WAVE_DRAWER_RASTER1_H_
#define _WAVE_DRAWER_RASTER1_H_

class Raster1WaveDrawer:
	public RasterWaveDrawer
{
	public:

		Raster1WaveDrawer( wxUint64,
						   wxGraphicsContext*,
						   const wxRect2DInt&,
						   const DrawerSettings&,
						   bool, const wxTimeSpanArray& );

	protected:

		virtual void ProcessInitializer();

		virtual void NextColumn( wxFloat32, wxFloat32 );
		virtual void ProcessFinalizer();

	protected:

		wxGraphicsBitmap m_bmUp;
		wxGraphicsBitmap m_bmDown;

	protected:

		wxImage create_solid_bitmap( wxMemoryDC&, const ColourSettings&, bool ) const;
		wxImage create_log_gradient_bitmap( wxMemoryDC&, const ColourSettings&, bool ) const;
		wxImage create_gradient_bitmap( wxMemoryDC&, const ColourSettings&, bool ) const;
};

#endif

