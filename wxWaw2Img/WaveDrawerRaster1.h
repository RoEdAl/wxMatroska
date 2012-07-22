/*
	WaveDrawerRasrer1.h
*/
#ifndef _WAVE_DRAWER_RASTER1_H_
#define _WAVE_DRAWER_RASTER1_H_

class Raster1WaveDrawer :public RasterWaveDrawer
{
	public:

	Raster1WaveDrawer(	
		wxUint64,
		wxGraphicsContext*,
		bool, bool, wxFloat32,
		wxRect2DInt, const wxColour&, const wxColour& );

	protected:

	virtual void ProcessInitializer();
	virtual void NextColumn( wxFloat32, wxFloat32 );

	protected:

	wxGraphicsBitmap m_bm;
};

#endif

