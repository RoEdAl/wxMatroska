/*
 *      WaveDrawerSimple.h
 */

#ifndef _WAVE_DRAWER_SIMPLE_H_
#define _WAVE_DRAWER_SIMPLE_H_

class SimpleWaveDrawer:
	public GraphicsContextWaveDrawer
{
	public:

		SimpleWaveDrawer( wxUint64,
						  wxGraphicsContext* gc,
						  const wxRect2DInt&,
						  const DrawerSettings&,
						  bool, const wxTimeSpanArray& );

	protected:

		virtual void ProcessInitializer();

		virtual void NextColumn( wxFloat32, wxFloat32 );
		virtual void ProcessFinalizer();

	protected:

		bool		   m_bOneMiddleColour;
		wxGraphicsPath m_pathUp;
		wxGraphicsPath m_pathDown;
};

#endif

