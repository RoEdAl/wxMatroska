/*
 *      NinePatchBitmap.h
 */
#ifndef _NINE_PATCH_BITMAP_H_
#define _NINE_PATCH_BITMAP_H_

class NinePatchBitmap
{
	public:

		NinePatchBitmap( void );
		NinePatchBitmap( const NinePatchBitmap& );

		bool Init( const wxString&, bool = true );
		bool Init( const wxImage& );
		bool Init( const wxColour&, const wxColour&, int = 1 );

		wxImage GetStretched( const wxSize&, wxImageResizeQuality  = wxIMAGE_QUALITY_NORMAL ) const;
		wxImage GetStretchedEx( wxSize                             = wxSize(), wxImageResizeQuality = wxIMAGE_QUALITY_NORMAL ) const;
		wxImage GetStretchedEx( wxRect2DInt&, wxImageResizeQuality = wxIMAGE_QUALITY_NORMAL ) const;

		bool IsOk() const;
		wxSize GetMinimumImageSize() const;

	protected:

		static bool analyze_image( const wxImage&, wxRect2DInt& );

		void draw_bmp( wxImage&, const wxRect2DInt&, const wxRect2DInt&, wxImageResizeQuality ) const;

	protected:

		wxImage m_img;
		wxRect2DInt m_stretchedArea;
};

#endif

