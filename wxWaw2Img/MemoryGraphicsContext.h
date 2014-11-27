/*
 *      MemoryGrpahicsContext.h
 */
#ifndef _MEMORY_GRAPHICS_CONTEXT_H_
#define _MEMORY_GRAPHICS_CONTEXT_H_

class MemoryGraphicsContext
{
	public:

		MemoryGraphicsContext( const wxSize&, int, bool );

		wxGraphicsContext* CreateGraphicsContext();
		wxImage GetImage();
		const wxSize& GetSize() const;

	protected:

		wxSize					  m_imgSize;
		wxBitmap				  m_bmp;
		wxScopedPtr< wxMemoryDC > m_pMemDc;
};
#endif

