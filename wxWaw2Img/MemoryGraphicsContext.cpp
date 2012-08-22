/*
 *      MemoryGraphicsContext.cpp
 */
#include "StdWx.h"
#include "MemoryGraphicsContext.h"

MemoryGraphicsContext::MemoryGraphicsContext( const wxSize& imgSize, int nImgColourDepth, bool bCreateMemContext ):
	m_imgSize( imgSize )
{
	m_bmp = wxBitmap( imgSize, nImgColourDepth );

	if ( bCreateMemContext )
	{
		m_pMemDc.reset( new wxMemoryDC( m_bmp ) );
	}
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
		wxImage img( m_bmp.ConvertToImage() );
		m_pMemDc->SelectObject( m_bmp );

		return img;
	}
	else
	{
		return wxNullImage;
	}
}

const wxSize& MemoryGraphicsContext::GetSize() const
{
	return m_imgSize;
}

