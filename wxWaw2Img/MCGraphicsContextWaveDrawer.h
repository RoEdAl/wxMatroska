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
		wxBitmap						 m_bmp;
		wxImage							 m_img;
		wxScopedPtr< wxMemoryDC >		 m_memDc;

#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE
		wxScopedPtr< wxEnhMetaFileDC > m_emfDc;
		wxScopedPtr< wxEnhMetaFile >   m_emf;
#endif
#endif

		wxRect2DIntArray m_rects;
		wxUint32		 m_nTrackDuration;

	public:

		McGraphicalContextWaveDrawer( wxUint16 );
		wxGraphicsContext* Initialize( const wxSize &, int, const wxColour &, const wxRect2DIntArray &, wxUint32 );

		wxUint32 GetTrackDuration() const;
		const wxRect2DIntArray& GetRects() const;
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

