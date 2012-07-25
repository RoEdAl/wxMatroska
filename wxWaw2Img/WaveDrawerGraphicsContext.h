/*
 *      WaveDrawerGraphicsContext.h
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
								   const wxRect2DInt&,
								   const DrawerSettings&,
								   bool, const wxTimeSpanArray& );

	protected:

		virtual void ProcessInitializer();
		virtual void ProcessFinalizer();

	private:

		static wxGraphicsPath create_cue_region( wxGraphicsContext&, const wxRect2DInt&, const wxTimeSpanArray& );

	protected:

		wxGraphicsContext*	   m_gc;
		wxRect2DInt			   m_rc;
		wxFloat32			   m_height2;
		wxFloat32			   m_yoffset;
		wxInt32				   m_nImgHeight;

		const DrawerSettings& m_drawerSettings;
		bool				   m_bUseCuePoints;
		const wxTimeSpanArray& m_cuePoints;
};

#endif

