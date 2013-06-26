/*
 *      MCPdfWaveDrawer.h
 */

#ifndef _MC_PDF_WAVE_DRAWER_H_
#define _MC_PDF_WAVE_DRAWER_H_

class McPdfWaveDrawer:
	public ArrayWaveDrawer
{
	protected:

		wxScopedPtr< wxPdfDocument > m_pPdf;

	public:

		McPdfWaveDrawer( wxUint16 );
		wxPdfDocument* Initialize( const wxSize &, const wxColour &, const wxRect2DIntArray &, const wxString&  );
		bool Save( const wxFileName& );

	protected:

		virtual void ProcessFinalizer();
};

#endif

