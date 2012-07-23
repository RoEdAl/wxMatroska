/*
        WaveDrawerGraphicsContext.h
 */

#ifndef _WAVE_DRIVER_GRAPHICS_CONTEXT_H_
#define _WAVE_DRIVER_GRAPHICS_CONTEXT_H_

class GraphicsContextWaveDrawer:
	public SampleChunker
{
public:

	GraphicsContextWaveDrawer( wxUint64,
							   wxGraphicsContext* gc,
							   bool, wxFloat32,
							   wxRect2DInt );

protected:

	virtual void ProcessInitializer();
	virtual void ProcessFinalizer();

protected:

	wxGraphicsContext* m_gc;
	wxRect2DInt		   m_rc;
	wxFloat32		   m_height2;
	wxFloat32		   m_yoffset;
	wxInt32			   m_nImgHeight;
};

#endif

