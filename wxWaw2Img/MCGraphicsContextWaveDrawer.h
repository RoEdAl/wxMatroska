/*
 *      MCChainWaveDrawer.h
 */

#ifndef _MC_GC_WAVE_DRAWER_H_
#define _MC_GC_WAVE_DRAWER_H_

class McGraphicalContextWaveDrawer:
	public ArrayWaveDrawer
{
	protected:

		wxScopedPtr< wxBitmap >			 m_bmp;
		wxScopedPtr< wxMemoryDC >		 m_mc;
		wxScopedPtr< wxGraphicsContext > m_gc;

	public:

		McGraphicalContextWaveDrawer( wxUint16 );
		wxGraphicsContext* Initialize( const wxSize&, int, const wxColour & );
		wxImage GetBitmap() const;

	protected:

		virtual void ProcessFinalizer();
};

#endif

