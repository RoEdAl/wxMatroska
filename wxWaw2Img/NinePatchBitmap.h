/*
	NinePatchBitmap.h
*/
#ifndef _NINE_PATCH_BITMAP_H_
#define _NINE_PATCH_BITMAP_H_

class NinePatchBitmap
{
	public:

	NinePatchBitmap(void);
	~NinePatchBitmap(void);

	bool Init( const wxString& );
	bool Init( const wxImage& );

	wxImage GetStretched( const wxSize& ) const;

	wxSize GetMinimumImageSize() const;

	protected:

	enum PixelColor
	{
		PixelOtherColour,
		PixelBlack,
		PixelWhiteOrTransparent
	};

	bool IsOk() const;

	static PixelColor get_pixel_color( const wxImage&, const wxPoint& );
	static bool analyze_image(const wxImage&, wxRect2DInt& );
	void draw_bmp( wxImage&, const wxRect2DInt&, const wxRect2DInt& ) const;

	protected:

	wxImage m_img;
	wxImage m_imgTruncated;
	wxRect2DInt m_stretchedArea;
};

#endif
