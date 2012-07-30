/*
 *      MCChainWaveDrawer.h
 */

#ifndef _MC_GC_WAVE_DRAWER_H_
#define _MC_GC_WAVE_DRAWER_H_

class McGraphicalContextWaveDrawer:
	public ArrayWaveDrawer
{
	protected:

		wxScopedPtr< wxGraphicsContext > m_gc;
		wxScopedPtr< wxMemoryDC >		 m_mc;
		wxScopedPtr< wxBitmap >			 m_bmp;

#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE
		wxScopedPtr< wxEnhMetaFileDC > m_emfDc;
		wxScopedPtr< wxEnhMetaFile >   m_emf;
#endif
#endif

	public:

		McGraphicalContextWaveDrawer( wxUint16 );
		wxGraphicsContext* Initialize( const wxSize&, int, const wxColour&, const wxRegion& );

		wxImage GetBitmap() const;

#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE
		wxEnhMetaFile* GetMetafile() const;

#endif
#endif

	protected:

		virtual void ProcessFinalizer();

	protected:

		bool create_context_on_bitmap( const wxSize&, int );

#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE
		void create_context_on_emf( const wxSize& );

#endif
#endif
};

#endif

