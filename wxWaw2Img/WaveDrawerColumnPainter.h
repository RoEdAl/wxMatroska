/*
 *      WaveDrawerColumnPainter.h
 */
#ifndef _WAVE_DRAWER_COLUMN_PAINTER_H_
#define _WAVE_DRAWER_COLUMN_PAINTER_H_

class ColumnPainterWaveDrawer:
	public GraphicsContextWaveDrawer
{
	public:

		ColumnPainterWaveDrawer( wxUint64,
						   wxGraphicsContext*,
						   const wxRect2DInt&,
						   const DrawerSettings&,
						   bool, const wxTimeSpanArray& );

	protected:

		virtual void NextColumn( wxFloat32, wxFloat32 );
		virtual void ProcessFinalizer();

		virtual void GetThreeColours( wxFloat32, wxColour&, wxColour&, wxColour& ) = 0;
		virtual void GetTwoColours( wxFloat32, bool, wxColour&, wxColour& ) = 0;
};

#endif

