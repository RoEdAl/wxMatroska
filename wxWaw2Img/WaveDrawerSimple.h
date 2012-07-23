/*
        WaveDrawerSimple.h
 */

#ifndef _WAVE_DRAWER_SIMPLE_H_
#define _WAVE_DRAWER_SIMPLE_H_

class SimpleWaveDrawer:
	public GraphicsContextWaveDrawer
{
public:

	SimpleWaveDrawer( wxUint64,
					  wxGraphicsContext* gc,
					  bool, wxFloat32,
					  wxRect2DInt,
					  const wxColour&,
					  bool, const wxTimeSpanArray&, const wxColour& );

protected:

	virtual void ProcessInitializer();

	virtual void NextColumn( wxFloat32, wxFloat32 );
	virtual void ProcessFinalizer();

protected:

	wxColour m_clr;
	bool	 m_bLogarithmicScale;

	wxGraphicsPath m_path;
};

#endif

