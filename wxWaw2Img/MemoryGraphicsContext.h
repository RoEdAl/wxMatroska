/*
	MemoryGrpahicsContext.h
*/
#ifndef _MEMORY_GRAPHICS_CONTEXT_H_
#define _MEMORY_GRAPHICS_CONTEXT_H_

class MemoryGraphicsContext
{
	public:

	MemoryGraphicsContext( const wxSize&, int );

	wxGraphicsContext* CreateGraphicsContext();
	wxImage GetImage();

	protected:

	wxBitmap m_bmp;
	wxScopedPtr<wxMemoryDC> m_pMemDc;

};

#endif