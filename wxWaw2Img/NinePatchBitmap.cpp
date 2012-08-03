/*
 *      NinePatchBitmap.cpp
 */
#include "StdWx.h"
#include "NinePatchBitmap.h"

NinePatchBitmap::NinePatchBitmap( void )
{}

NinePatchBitmap::~NinePatchBitmap( void )
{}

bool NinePatchBitmap::IsOk() const
{
	return m_img.IsOk() && !m_stretchedArea.IsEmpty();
}

NinePatchBitmap::PixelColor NinePatchBitmap::get_pixel_color( const wxImage& img, const wxPoint& pt )
{
	unsigned char r = img.GetRed( pt.x, pt.y );
	unsigned char g = img.GetGreen( pt.x, pt.y );
	unsigned char b = img.GetBlue( pt.x, pt.y );
	unsigned char a = img.HasAlpha() ? img.GetAlpha( pt.x, pt.y ) : 255;

	if ( r == 0u && g == 0u && b == 0u && a == 255u )
	{
		return PixelBlack;
	}
	else if ( a == 0u )	// transparent
	{
		return PixelWhiteOrTransparent;
	}
	else if ( r == 255u && g == 255u && b == 255u && a == 255u )// white
	{
		return PixelWhiteOrTransparent;
	}
	else
	{
		return PixelOtherColour;
	}
}

bool NinePatchBitmap::analyze_image( const wxImage& img, wxRect2DInt& stretchedArea )
{
	wxASSERT( img.IsOk() );

	wxSize imgSize( img.GetSize() );

	wxPoint pt;
	int		phase	   = 0;
	int		lineXStart = -1, lineXStop = -1;
	int		lineYStart = -1, lineYStop = -1;

	for ( int x = 0, nWidth = imgSize.GetWidth(); x < nWidth && phase >= 0; x++ )
	{
		pt.x = x;
		PixelColor pc = get_pixel_color( img, pt );

		if ( pc == PixelOtherColour )
		{
			phase = -1;
			continue;
		}

		switch ( phase )
		{
			case 0:
			{
				if ( pc == PixelBlack )
				{
					if ( x > 1 )
					{
						lineXStart = x;
						phase	   = 1;
					}
					else
					{
						phase = -1;
					}
				}
				break;
			}

			case 1:
			{
				if ( pc == PixelWhiteOrTransparent )
				{
					lineXStop = x;
					phase	  = 2;
				}
				break;
			}
		}
	}

	if ( phase != 2 )
	{
		return false;
	}

	phase = 0;
	pt.x  = 0;
	for ( int y = 0, nHeight = imgSize.GetHeight(); y < nHeight && phase >= 0; y++ )
	{
		pt.y = y;
		PixelColor pc = get_pixel_color( img, pt );

		if ( pc == PixelOtherColour )
		{
			phase = -1;
			continue;
		}

		switch ( phase )
		{
			case 0:
			{
				if ( pc == PixelBlack )
				{
					if ( y > 1 )
					{
						lineYStart = y;
						phase	   = 1;
					}
					else
					{
						phase = -1;
					}
				}
				break;
			}

			case 1:
			{
				if ( pc == PixelWhiteOrTransparent )
				{
					lineYStop = y;
					phase	  = 2;
				}
				break;
			}
		}
	}

	if ( phase != 2 )
	{
		return false;
	}

	stretchedArea.m_x	  = lineXStart;
	stretchedArea.m_width = lineXStop - lineXStart;

	stretchedArea.m_y	   = lineYStart;
	stretchedArea.m_height = lineYStop - lineYStart;

	stretchedArea.m_x -= 1;
	stretchedArea.m_y -= 1;

	return true;
}

bool NinePatchBitmap::Init( const wxString& sImg )
{
	wxImage img( sImg );

	img.InitAlpha();
	return Init( img );
}

bool NinePatchBitmap::Init( const wxImage& img )
{
	if ( !img.IsOk() )
	{
		return false;
	}

	if ( !analyze_image( img, m_stretchedArea ) )
	{
		return false;
	}

	m_img		   = img;
	m_imgTruncated = m_img.GetSubImage( wxRect( 1, 1, m_img.GetWidth() - 2, m_img.GetHeight() - 2 ) );
	return true;
}

wxSize NinePatchBitmap::GetMinimumImageSize() const
{
	wxASSERT( m_img.IsOk() );

	wxSize s( m_img.GetSize() );
	s.DecBy( m_stretchedArea.GetSize() );

	return s;
}

void NinePatchBitmap::draw_bmp( wxImage& img, const wxRect2DInt& rcSrc, const wxRect2DInt& rcDst ) const
{
	wxImage simg( m_imgTruncated.GetSubImage( wxRect( rcSrc.m_x, rcSrc.m_y, rcSrc.m_width, rcSrc.m_height ) ) );

	simg.Rescale( rcDst.m_width, rcDst.m_height, wxIMAGE_QUALITY_HIGH );
	img.Paste( simg, rcDst.m_x, rcDst.m_y );
}

wxImage NinePatchBitmap::GetStretched( const wxSize& rcSize ) const
{
	wxSize sizeMin( GetMinimumImageSize() );
	wxSize imgSize( m_imgTruncated.GetSize() );

	if ( rcSize.GetWidth() < sizeMin.GetWidth() || rcSize.GetHeight() < sizeMin.GetHeight() )
	{
		return false;
	}

	wxImage si( rcSize, true );
	si.InitAlpha();

	wxRect2DInt rcSrc;
	wxRect2DInt rcDst;

	// upper-left corner
	rcSrc.m_x	   = 0;
	rcSrc.m_y	   = 0;
	rcSrc.m_width  = m_stretchedArea.m_x;
	rcSrc.m_height = m_stretchedArea.m_y;

	rcDst.m_x	   = 0;
	rcDst.m_y	   = 0;
	rcDst.m_width  = rcSrc.m_width;
	rcDst.m_height = rcSrc.m_height;

	draw_bmp( si, rcSrc, rcDst );

	// upper - middle, stretched horizontally

	rcSrc.m_x	 += rcSrc.m_width;
	rcSrc.m_width = m_stretchedArea.m_width;

	rcDst.m_x	 += rcDst.m_width;
	rcDst.m_width = rcSize.GetWidth() - ( imgSize.GetWidth() - m_stretchedArea.m_width );

	draw_bmp( si, rcSrc, rcDst );

	// upper - right corner

	rcSrc.m_x	 += rcSrc.m_width;
	rcSrc.m_width = imgSize.GetWidth() - m_stretchedArea.GetRight();

	rcDst.m_x	 += rcDst.m_width;
	rcDst.m_width = rcSrc.m_width;

	draw_bmp( si, rcSrc, rcDst );

	// middle - left, stretchd vertically

	rcSrc.m_x	   = 0;
	rcSrc.m_y	   = m_stretchedArea.m_y;
	rcSrc.m_width  = m_stretchedArea.m_x;
	rcSrc.m_height = m_stretchedArea.m_height;

	rcDst.m_x	   = 0;
	rcDst.m_y	  += rcDst.m_height;
	rcDst.m_width  = rcSrc.m_width;
	rcDst.m_height = rcSize.GetHeight() - ( imgSize.GetHeight() - m_stretchedArea.m_height );

	draw_bmp( si, rcSrc, rcDst );

	// middle - middle, stretched horizontally and vertically
	rcSrc = m_stretchedArea;

	rcDst.m_x	 += rcDst.m_width;
	rcDst.m_width = rcSize.GetWidth() - ( imgSize.GetWidth() - m_stretchedArea.m_width );

	draw_bmp( si, rcSrc, rcDst );

	// middle - right, stretched vertically
	rcSrc.m_x	 += rcSrc.m_width;
	rcSrc.m_width = imgSize.GetWidth() - m_stretchedArea.GetRight();

	rcDst.m_x	 += rcDst.m_width;
	rcDst.m_width = rcSrc.m_width;

	draw_bmp( si, rcSrc, rcDst );

	// bottom - left corner
	rcSrc.m_x	   = 0;
	rcSrc.m_y	   = m_stretchedArea.GetBottom();
	rcSrc.m_width  = m_stretchedArea.m_x;
	rcSrc.m_height = imgSize.GetHeight() - m_stretchedArea.GetBottom();

	rcDst.m_x	   = 0;
	rcDst.m_y	  += rcDst.m_height;
	rcDst.m_width  = rcSrc.m_width;
	rcDst.m_height = rcSrc.m_height;

	draw_bmp( si, rcSrc, rcDst );

	// bottom - middle, stretched horizontally

	rcSrc.m_x	 += rcSrc.m_width;
	rcSrc.m_width = m_stretchedArea.m_width;

	rcDst.m_x	 += rcDst.m_width;
	rcDst.m_width = rcSize.GetWidth() - ( imgSize.GetWidth() - m_stretchedArea.m_width );

	draw_bmp( si, rcSrc, rcDst );

	// bottom - right corner
	rcSrc.m_x	 += rcSrc.m_width;
	rcSrc.m_width = imgSize.GetWidth() - m_stretchedArea.GetRight();

	rcDst.m_x	 += rcDst.m_width;
	rcDst.m_width = rcSrc.m_width;

	draw_bmp( si, rcSrc, rcDst );

	return si;
}

