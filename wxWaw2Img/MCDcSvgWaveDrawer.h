/*
 *      MCDcSvgWaveDrawer.h
 */

#ifndef _MC_DC_SVG_WAVE_DRAWER_H_
#define _MC_DC_SVG_WAVE_DRAWER_H_

class McDcSvgWaveDrawer:
	public McDcWaveDrawer
{
	public:

		McDcSvgWaveDrawer( wxUint16 );
		wxDC* Initialize( const wxSize&, const wxColour&, const wxRect2DIntArray&, const wxString& );
		bool Save( const wxFileName& );

	protected:

		virtual void ProcessFinalizer();

	private:

		wxFileName m_tmpFileName;
};
#endif

