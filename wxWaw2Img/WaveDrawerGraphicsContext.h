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

		static void create_cue_segments_paths(
			const wxRect2DInt &,
			wxFloat32,
			const wxTimeSpanArray &,
			wxGraphicsPath &, wxGraphicsPath & );

		static void create_cue_lines_paths(
			const wxRect2DInt &, wxFloat32,
			const wxTimeSpanArray &,
			wxGraphicsPath &, wxGraphicsPath & );

		static void create_cue_segments_path( const wxRect2DInt&,
											  const wxTimeSpanArray&,
											  wxGraphicsPath& );

		static void create_cue_lines_path( const wxRect2DInt&,
										   const wxTimeSpanArray&,
										   wxGraphicsPath& );

	protected:

		wxGraphicsContext*	   m_gc;
		wxRect2DInt			   m_rc;
		wxFloat32			   m_heightUp;
		wxFloat32			   m_heightDown;
		wxFloat32			   m_yoffset;
		const DrawerSettings&  m_drawerSettings;
		bool				   m_bUseCuePoints;
		const wxTimeSpanArray& m_cuePoints;
};

#endif

