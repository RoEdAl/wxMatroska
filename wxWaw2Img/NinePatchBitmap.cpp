/*
 *      NinePatchBitmap.cpp
 */
#include "StdWx.h"
#include "NinePatchBitmap.h"
#include "MemoryGraphicsContext.h"

NinePatchBitmap::NinePatchBitmap( void )
{}

NinePatchBitmap::~NinePatchBitmap( void )
{}

bool NinePatchBitmap::IsOk() const
{
	return m_img.IsOk() && !m_stretchedArea.IsEmpty();
}

bool NinePatchBitmap::Init( const wxColour& clrBorder, const wxColour& crlInside, int nBorderWidth )
{
	wxSize s( 8, 8 );

	s.IncBy( 2 * nBorderWidth );

	MemoryGraphicsContext mgc( s, 32 );

	{
		wxScopedPtr< wxGraphicsContext > pGc( mgc.CreateGraphicsContext() );
		pGc->SetAntialiasMode( wxANTIALIAS_NONE );
		pGc->SetInterpolationQuality( wxINTERPOLATION_NONE );
		pGc->SetCompositionMode( wxCOMPOSITION_SOURCE );
		pGc->SetPen( wxNullPen );

		if ( nBorderWidth > 0 )
		{
			wxPen pen( clrBorder, 2 * nBorderWidth );
			pGc->SetPen( pen );
		}

		wxGraphicsPath p = pGc->CreatePath();
		p.AddRectangle( 0, 0, s.GetWidth(), s.GetHeight() );

		wxBrush brush( crlInside );
		pGc->SetBrush( brush );
		pGc->DrawPath( p );
	}

	m_stretchedArea.m_x		 = nBorderWidth;
	m_stretchedArea.m_y		 = nBorderWidth;
	m_stretchedArea.m_width	 = 8;
	m_stretchedArea.m_height = 8;

	wxImage img( mgc.GetImage() );

	// img.SaveFile( "C:/Users/Normal/Documents/Visual Studio 2010/Projects/wxMatroska/test.png" );
	m_img = img;

	return true;
}

class PixelIterator
{
	public:

		enum PixelColor
		{
			PixelOtherColour,
			PixelBlack,
			PixelWhiteOrTransparent
		};

		PixelIterator( const wxImage& img, wxUint32 nRow, wxUint32 nColumn ):
			m_img( img ), m_nRow( nRow ), m_nColumn( nColumn )
		{
			wxASSERT( img.IsOk() );
		}

		PixelIterator( const PixelIterator& pi ):
			m_img( pi.m_img ), m_nRow( pi.m_nRow ), m_nColumn( pi.m_nColumn )
		{}

		bool operator ==( const PixelIterator& pi ) const
		{
			return m_nRow == pi.m_nRow && m_nColumn == pi.m_nColumn;
		}

		bool operator !=( const PixelIterator& pi ) const
		{
			return m_nRow != pi.m_nRow || m_nColumn != pi.m_nColumn;
		}

		PixelColor GetPixelColor() const
		{
			unsigned char r = m_img.GetRed( m_nColumn, m_nRow );
			unsigned char g = m_img.GetGreen( m_nColumn, m_nRow );
			unsigned char b = m_img.GetBlue( m_nColumn, m_nRow );
			unsigned char a = m_img.HasAlpha() ? m_img.GetAlpha( m_nColumn, m_nRow ) : 255;

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

	protected:

		const wxImage& m_img;
		wxUint32	   m_nRow;
		wxUint32	   m_nColumn;
};

class HorizontalPixelIterator:
	public PixelIterator
{
	public:

		static HorizontalPixelIterator GetStartPixelIterator( const wxImage& img, wxUint32 nRow )
		{
			return HorizontalPixelIterator( img, nRow );
		}

		static HorizontalPixelIterator GetEndPixelIterator( const wxImage& img, wxUint32 nRow )
		{
			return HorizontalPixelIterator( img, nRow, img.GetWidth() );
		}

		HorizontalPixelIterator( const wxImage& img, wxUint32 nRow ):
			PixelIterator( img, nRow, 0u )
		{}

		HorizontalPixelIterator( const HorizontalPixelIterator& hpi ):
			PixelIterator( hpi )
		{}

		HorizontalPixelIterator& operator ++()
		{
			m_nColumn += 1u;
			return *this;
		}

		wxUint32 GetPosition() const
		{
			return m_nColumn;
		}

	protected:

		HorizontalPixelIterator( const wxImage& img, wxUint32 nRow, wxUint32 nColumn ):
			PixelIterator( img, nRow, nColumn )
		{}
};

class VerticalPixelIterator:
	public PixelIterator
{
	public:

		static VerticalPixelIterator GetStartPixelIterator( const wxImage& img, wxUint32 nColumn )
		{
			return VerticalPixelIterator( img, nColumn );
		}

		static VerticalPixelIterator GetEndPixelIterator( const wxImage& img, wxUint32 nColumn )
		{
			return VerticalPixelIterator( img, img.GetHeight(), nColumn );
		}

		VerticalPixelIterator( const wxImage& img, wxUint32 nColumn ):
			PixelIterator( img, 0u, nColumn )
		{}

		VerticalPixelIterator( const VerticalPixelIterator& vpi ):
			PixelIterator( vpi )
		{}

		VerticalPixelIterator& operator ++()
		{
			m_nRow += 1u;
			return *this;
		}

		wxUint32 GetPosition() const
		{
			return m_nRow;
		}

	protected:

		VerticalPixelIterator( const wxImage& img, wxUint32 nRow, wxUint32 nColumn ):
			PixelIterator( img, nRow, nColumn )
		{}
};

template< class I >
bool get_black_position_and_range( const I& startIt, const I& stopIt, wxUint32& nPos, wxUint32& nLen )
{
	int		 phase = 0;
	wxUint32 nStart, nStop;

	for ( I i( startIt ); i != stopIt && phase >= 0; ++i )
	{
		PixelIterator::PixelColor pc = i.GetPixelColor();

		if ( pc == PixelIterator::PixelOtherColour )
		{
			phase = -1;
			continue;
		}

		switch ( phase )
		{
			case 0:
			{
				if ( pc == PixelIterator::PixelBlack )
				{
					if ( i != startIt )
					{
						nStart = i.GetPosition();
						phase  = 1;
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
				if ( pc == PixelIterator::PixelWhiteOrTransparent )
				{
					nStop = i.GetPosition();
					phase = 2;
				}
				break;
			}
		}
	}

	if ( phase != 2 )
	{
		return false;
	}

	nPos = nStart;
	nLen = nStop - nStart;

	return true;
}

bool NinePatchBitmap::analyze_image( const wxImage& img, wxRect2DInt& stretchedArea )
{
	wxASSERT( img.IsOk() );

	wxUint32 lineXStart, lineXStop;
	wxUint32 lineYStart, lineYStop;

	if (
		get_black_position_and_range( HorizontalPixelIterator::GetStartPixelIterator( img, 0u ), HorizontalPixelIterator::GetEndPixelIterator( img, 0u ), lineXStart, lineXStop ) &&
		get_black_position_and_range( VerticalPixelIterator::GetStartPixelIterator( img, 0u ), VerticalPixelIterator::GetEndPixelIterator( img, 0u ), lineYStart, lineYStop ) )
	{
		stretchedArea.m_x	  = lineXStart;
		stretchedArea.m_width = lineXStop - lineXStart;

		stretchedArea.m_y	   = lineYStart;
		stretchedArea.m_height = lineYStop - lineYStart;

		stretchedArea.m_x -= 1;
		stretchedArea.m_y -= 1;

		return true;
	}

	return false;
}

bool NinePatchBitmap::Init( const wxString& sImg, bool initAlpha )
{
	wxImage img( sImg );

	if ( initAlpha && img.IsOk() && !img.HasAlpha() )
	{
		img.InitAlpha();
	}
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

	m_img = img.GetSubImage( wxRect( 1, 1, img.GetWidth() - 2, img.GetHeight() - 2 ) );
	return true;
}

wxSize NinePatchBitmap::GetMinimumImageSize() const
{
	wxASSERT( m_img.IsOk() );

	wxSize s( m_img.GetSize() );
	s.DecBy( m_stretchedArea.GetSize() );

	return s;
}

wxImage NinePatchBitmap::GetStretchedEx( wxRect2DInt& rcStretchedArea, wxImageResizeQuality resizeQuality ) const
{
	rcStretchedArea.m_x -= m_stretchedArea.m_x;
	rcStretchedArea.m_y -= m_stretchedArea.m_y;

	wxSize minSize( GetMinimumImageSize() );
	rcStretchedArea.m_width	 += minSize.GetWidth();
	rcStretchedArea.m_height += minSize.GetHeight();

	return GetStretched( rcStretchedArea.GetSize(), resizeQuality );
}

wxImage NinePatchBitmap::GetStretchedEx( wxSize rcStretchedAreaSize, wxImageResizeQuality resizeQuality ) const
{
	rcStretchedAreaSize.IncBy( GetMinimumImageSize() );
	return GetStretched( rcStretchedAreaSize, resizeQuality );
}

void NinePatchBitmap::draw_bmp( wxImage& img, const wxRect2DInt& rcSrc, const wxRect2DInt& rcDst, wxImageResizeQuality resizeQuality ) const
{
	if ( !( rcSrc.IsEmpty() || rcDst.IsEmpty() ) )
	{
		wxImage simg( m_img.GetSubImage( wxRect( rcSrc.m_x, rcSrc.m_y, rcSrc.m_width, rcSrc.m_height ) ) );
		simg.Rescale( rcDst.m_width, rcDst.m_height, resizeQuality );
		img.Paste( simg, rcDst.m_x, rcDst.m_y );
	}
}

wxImage NinePatchBitmap::GetStretched( const wxSize& rcSize, wxImageResizeQuality resizeQuality ) const
{
	if ( rcSize.GetWidth() == 0 || rcSize.GetHeight() == 0 )
	{
		return wxNullImage;
	}

	wxSize sizeMin( GetMinimumImageSize() );
	wxSize imgSize( m_img.GetSize() );

	if ( rcSize.GetWidth() < sizeMin.GetWidth() || rcSize.GetHeight() < sizeMin.GetHeight() )
	{
		return wxNullImage;
	}

	wxImage si( rcSize, true );

	if ( m_img.HasAlpha() )
	{
		si.InitAlpha();
	}

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

	draw_bmp( si, rcSrc, rcDst, resizeQuality );

	// upper - middle, stretched horizontally

	rcSrc.m_x	 += rcSrc.m_width;
	rcSrc.m_width = m_stretchedArea.m_width;

	rcDst.m_x	 += rcDst.m_width;
	rcDst.m_width = rcSize.GetWidth() - ( imgSize.GetWidth() - m_stretchedArea.m_width );

	draw_bmp( si, rcSrc, rcDst, resizeQuality );

	// upper - right corner
	rcSrc.m_x	 += rcSrc.m_width;
	rcSrc.m_width = imgSize.GetWidth() - m_stretchedArea.GetRight();

	rcDst.m_x	 += rcDst.m_width;
	rcDst.m_width = rcSrc.m_width;

	draw_bmp( si, rcSrc, rcDst, resizeQuality );

	// middle - left, stretchd vertically

	rcSrc.m_x	   = 0;
	rcSrc.m_y	   = m_stretchedArea.m_y;
	rcSrc.m_width  = m_stretchedArea.m_x;
	rcSrc.m_height = m_stretchedArea.m_height;

	rcDst.m_x	   = 0;
	rcDst.m_y	  += rcDst.m_height;
	rcDst.m_width  = rcSrc.m_width;
	rcDst.m_height = rcSize.GetHeight() - ( imgSize.GetHeight() - m_stretchedArea.m_height );

	draw_bmp( si, rcSrc, rcDst, resizeQuality );

	// middle - middle, stretched horizontally and vertically
	rcSrc = m_stretchedArea;

	rcDst.m_x	 += rcDst.m_width;
	rcDst.m_width = rcSize.GetWidth() - ( imgSize.GetWidth() - m_stretchedArea.m_width );

	draw_bmp( si, rcSrc, rcDst, resizeQuality );

	// middle - right, stretched vertically
	rcSrc.m_x	 += rcSrc.m_width;
	rcSrc.m_width = imgSize.GetWidth() - m_stretchedArea.GetRight();

	rcDst.m_x	 += rcDst.m_width;
	rcDst.m_width = rcSrc.m_width;

	draw_bmp( si, rcSrc, rcDst, resizeQuality );

	// bottom - left corner
	rcSrc.m_x	   = 0;
	rcSrc.m_y	   = m_stretchedArea.GetBottom();
	rcSrc.m_width  = m_stretchedArea.m_x;
	rcSrc.m_height = imgSize.GetHeight() - m_stretchedArea.GetBottom();

	rcDst.m_x	   = 0;
	rcDst.m_y	  += rcDst.m_height;
	rcDst.m_width  = rcSrc.m_width;
	rcDst.m_height = rcSrc.m_height;

	draw_bmp( si, rcSrc, rcDst, resizeQuality );

	// bottom - middle, stretched horizontally

	rcSrc.m_x	 += rcSrc.m_width;
	rcSrc.m_width = m_stretchedArea.m_width;

	rcDst.m_x	 += rcDst.m_width;
	rcDst.m_width = rcSize.GetWidth() - ( imgSize.GetWidth() - m_stretchedArea.m_width );

	draw_bmp( si, rcSrc, rcDst, resizeQuality );

	// bottom - right corner
	rcSrc.m_x	 += rcSrc.m_width;
	rcSrc.m_width = imgSize.GetWidth() - m_stretchedArea.GetRight();

	rcDst.m_x	 += rcDst.m_width;
	rcDst.m_width = rcSrc.m_width;

	draw_bmp( si, rcSrc, rcDst, resizeQuality );

	return si;
}

