/*
 *      WaveDrawerPdf.h
 */

#ifndef _WAVE_DRIVER_PDF_H_
#define _WAVE_DRIVER_PDF_H_

class PdfWaveDrawer:
	public SampleChunker
{
	public:

		PdfWaveDrawer( wxUint64,
					   wxPdfDocument*,
					   bool, wxFloat32,
					   const wxRect2DInt&,
					   const DrawerSettings&,
					   const ChaptersArrayScopedPtr& );

	protected:

		virtual void ProcessInitializer();
		virtual void ProcessFinalizer();

	private:

		static void create_chapter_segments_paths(
			const wxRect2DInt&,
			wxFloat32,
			const ChaptersArray&,
			wxPdfShape&, wxPdfShape& );

		static void create_chapter_lines_paths(
			const wxRect2DInt&, wxFloat32,
			const ChaptersArray&,
			wxPdfShape&, wxPdfShape& );

		static void create_chapter_segments_path( const wxRect2DInt&,
												  const ChaptersArray&,
												  wxPdfShape& );

		static void create_chapter_lines_path( const wxRect2DInt&,
											   const ChaptersArray&,
											   wxPdfShape& );

	protected:

		wxPdfDocument* m_pPdf;
		wxRect2DInt m_rc;
		wxFloat32 m_heightUp;
		wxFloat32 m_heightDown;
		wxFloat32 m_yoffset;
		const DrawerSettings& m_drawerSettings;
		const ChaptersArrayScopedPtr& m_pChapters;
};

#endif

