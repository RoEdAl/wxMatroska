/*
 *      WaveDrawerRasrer2.h
 */
#ifndef _WAVE_DRAWER_RASTER2_H_
#define _WAVE_DRAWER_RASTER2_H_

class Raster2WaveDrawer:
	public RasterWaveDrawer
{
	public:

		Raster2WaveDrawer( wxUint64,
						   wxGraphicsContext*,
						   const wxRect2DInt&,
						   const DrawerSettings&,
						   bool, const wxTimeSpanArray& );

	protected:

		virtual void ProcessInitializer();

		virtual void NextColumn( wxFloat32, wxFloat32 );
		virtual void ProcessFinalizer();

	protected:

		wxScopedPtr< wxMemoryDC > m_mdc;
};

#endif

