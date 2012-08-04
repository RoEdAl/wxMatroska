/*
 *      NinePatchBitmap.h
 */
#ifndef _NINE_PATCH_BITMAP_H_
#define _NINE_PATCH_BITMAP_H_

class NinePatchBitmap
{
	public:

		NinePatchBitmap( void );
		~NinePatchBitmap( void );

		bool Init( const wxString&, bool = true );
		bool Init( const wxImage& );

		wxImage GetStretched( const wxSize& ) const;
		wxImage GetStretchedEx( wxSize = wxSize() ) const;
		wxImage GetStretchedEx( wxRect2DInt& ) const;

		bool IsOk() const;
		wxSize GetMinimumImageSize() const;

	protected:

		static bool analyze_image( const wxImage&, wxRect2DInt& );
		void draw_bmp( wxImage&, const wxRect2DInt&, const wxRect2DInt& ) const;

	protected:

		wxImage		m_img;
		wxRect2DInt m_stretchedArea;
};

#endif

