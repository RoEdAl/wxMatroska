/*
 *      WaveDrawerPolyPdf.h
 */
#ifndef _WAVE_DRAWER_POLY_PDF_H_
#define _WAVE_DRAWER_POLY_PDF_H_

class PdfPolyWaveDrawer:
	public PdfWaveDrawer
{
	public:

		PdfPolyWaveDrawer( wxUint64,
						wxPdfDocument*,
						const wxRect2DInt&,
						const DrawerSettings&,
						const ChaptersArrayScopedPtr& );

	protected:

		virtual void ProcessInitializer();

		virtual void NextColumn( wxFloat32, wxFloat32 );
		virtual void ProcessFinalizer();

	protected:

		Point2DDoubleArray m_points;

	protected:

		wxPdfShape build_path() const;
};

#endif

