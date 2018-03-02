/*
 *      WaveDrawerRasrer1.h
 */
#ifndef _WAVE_DRAWER_RASTER1_H_
#define _WAVE_DRAWER_RASTER1_H_

class Raster1WaveDrawer:
	public ColumnPainterWaveDrawer
{
	public:

		Raster1WaveDrawer( wxUint64,
						   wxGraphicsContext*,
						   const wxRect2DInt&,
						   const DrawerSettings&,
						   const ChaptersArrayScopedPtr& );

	protected:

		virtual void GetThreeColours( wxFloat32, wxColour&, wxColour&, wxColour& );
		virtual void GetTwoColours( wxFloat32, bool, wxColour&, wxColour& );
};

#endif

