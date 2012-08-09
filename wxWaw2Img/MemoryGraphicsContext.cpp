/*
 *      MemoryGraphicsContext.cpp
 */
#include "StdWx.h"
#include "MemoryGraphicsContext.h"

MemoryGraphicsContext::MemoryGraphicsContext( const wxSize& imgSize, int nImgColourDepth )
{
	m_bmp = wxBitmap( imgSize, nImgColourDepth );
}

wxGraphicsContext* MemoryGraphicsContext::CreateGraphicsContext()
{
	if ( !m_pMemDc )
	{
		m_pMemDc.reset( new wxMemoryDC( m_bmp ) );
	}
	return wxGraphicsContext::Create( *m_pMemDc );
}

wxImage MemoryGraphicsContext::GetImage()
{
	if ( m_pMemDc )
	{
		m_pMemDc->SelectObject( wxNullBitmap );
		m_pMemDc.reset();

		return m_bmp.ConvertToImage();
	}
	else
	{
		return wxNullImage;
	}
}

