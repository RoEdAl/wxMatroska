/*
 *      WaveDrawerGraphicsContextPoly.h
 */

#ifndef _WAVE_DRIVER_GRAPHICS_CONTEXT_POLY_H_
#define _WAVE_DRIVER_GRAPHICS_CONTEXT_POLY_H_

class GraphicsContextWaveDrawerPoly:
	public WaveDrawer
{
	public:

		GraphicsContextWaveDrawerPoly( wxUint64 nNumberOfSamples, wxGraphicsContext* gc, bool bLogarithmicScale, bool bLogarithmicColorGradient, wxFloat32 fLogBase, wxRect2DInt rc );

	public:

		virtual void StartDrawing();
		virtual void EndOfSamples();

	protected:

		virtual void NextColumn( wxFloat32 fValue, wxFloat32 fLogValue );

	protected:

		wxGraphicsContext* m_gc;
		wxRect2DInt		   m_rc;
		wxInt32			   m_height2;
		wxInt32			   m_yoffset;
		wxGraphicsPath	   m_path;
};
#endif

