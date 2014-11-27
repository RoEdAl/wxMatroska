/*
 *      WaveDrawerGraphicsContext.cpp
 */

#include "StdWx.h"
#include "Arrays.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "DrawerSettings.h"
#include "WaveDrawerPdf.h"

PdfWaveDrawer::PdfWaveDrawer( wxUint64 nNumberOfSamples, wxPdfDocument* pPdf,
							  bool bCalcLogarithmic, wxFloat32 fLogBase,
							  const wxRect2DInt& rc,
							  const DrawerSettings& drawerSettings,
							  const ChaptersArrayScopedPtr& pChapters ):
	SampleChunker( nNumberOfSamples, rc.m_width, bCalcLogarithmic, fLogBase ),
	m_pPdf( pPdf ),
	m_rc( rc ),
	m_drawerSettings( drawerSettings ),
	m_pChapters( pChapters )
{}

namespace
{
	void add_rectangle_to_shape( wxPdfShape& shape, const wxRect2DDouble& rc )
	{
		wxPoint2DDouble p;

		p = rc.GetLeftTop();
		shape.MoveTo( p.m_x, p.m_y );

		p = rc.GetRightTop();
		shape.LineTo( p.m_x, p.m_y );

		p = rc.GetRightBottom();
		shape.LineTo( p.m_x, p.m_y );

		p = rc.GetLeftBottom();
		shape.LineTo( p.m_x, p.m_y );

		shape.ClosePath();
	}

	void set_fill_colour( wxPdfDocument& pdfDoc, const wxColour& clr )
	{
		pdfDoc.SetFillColour( clr );
		float fAlpha = static_cast< float >( clr.Alpha() ) / 255.0f;
		pdfDoc.SetAlpha( fAlpha, fAlpha );
	}

	void set_draw_colour( wxPdfDocument& pdfDoc, const wxColour& clr )
	{
		pdfDoc.SetDrawColour( clr );
		float fAlpha = static_cast< float >( clr.Alpha() ) / 255.0f;
		pdfDoc.SetAlpha( fAlpha, fAlpha );
	}

	template< class R >
	void fill_rect( wxPdfDocument& pdfDoc, const R& rc, int roundCorner )
	{
		// pdfDoc.RoundedRect( rc.m_x, rc.m_y, rc.m_width, rc.m_height, 0.45, roundCorner, wxPDF_STYLE_FILL );
		pdfDoc.Rect( rc.m_x, rc.m_y, rc.m_width, rc.m_height, wxPDF_STYLE_FILL );
	}

	template< class R >
	void fill_rect_colour( wxPdfDocument& pdfDoc, const R& rc, const wxColour& clr, int roundCorner )
	{
		set_fill_colour( pdfDoc, clr );
		fill_rect( pdfDoc, rc, roundCorner );
	}

	void fill_shape_colour( wxPdfDocument& pdfDoc, const wxPdfShape& shape, const wxColour& clr )
	{
		set_fill_colour( pdfDoc, clr );
		pdfDoc.Shape( shape, wxPDF_STYLE_FILL );
	}

	void draw_shape_colour( wxPdfDocument& pdfDoc, const wxPdfShape& shape, const wxColour& clr )
	{
		set_draw_colour( pdfDoc, clr );
		pdfDoc.Shape( shape, wxPDF_STYLE_DRAW );
	}
}

void PdfWaveDrawer::ProcessInitializer()
{
	m_heightUp	 = m_rc.m_height * ( 1.0f - m_drawerSettings.GetBaselinePosition() );
	m_heightDown = m_rc.m_height * m_drawerSettings.GetBaselinePosition();
	m_yoffset	 = m_rc.m_y + m_heightUp;

	if ( m_drawerSettings.OneBackgroundColour() )
	{
		fill_rect_colour( *m_pPdf, m_rc, m_drawerSettings.GetTopColourSettings().GetBackgroundColour(), wxPDF_CORNER_ALL );
	}
	else
	{
		wxRect2DDouble rc( m_rc.m_x, m_rc.m_y, m_rc.m_width, m_heightUp );
		fill_rect_colour( *m_pPdf, rc, m_drawerSettings.GetTopColourSettings().GetBackgroundColour(), wxPDF_CORNER_TOP_LEFT | wxPDF_CORNER_TOP_RIGHT );

		rc.m_y	   += m_rc.m_height;
		rc.m_height = m_heightDown;
		fill_rect_colour( *m_pPdf, rc, m_drawerSettings.GetBottomColourSettings().GetBackgroundColour(), wxPDF_CORNER_BOTTOM_LEFT | wxPDF_CORNER_BOTTOM_RIGHT );
	}

	// Secondary background colour

	if ( m_pChapters )
	{
		bool bDrawCueBlocks = m_drawerSettings.GetDrawChapters();

		if ( m_drawerSettings.OneBackgroundColour2() )
		{
			wxPdfShape path;

			if ( bDrawCueBlocks )
			{
				create_chapter_segments_path( m_rc, *m_pChapters, path );
				fill_shape_colour( *m_pPdf, path, m_drawerSettings.GetTopColourSettings().GetBackgroundColour2() );
			}
			else
			{
				create_chapter_lines_path( m_rc, *m_pChapters, path );
				draw_shape_colour( *m_pPdf, path, m_drawerSettings.GetTopColourSettings().GetBackgroundColour2() );
			}
		}
		else
		{
			wxPdfShape pathTop, pathBottom;

			if ( bDrawCueBlocks )
			{
				create_chapter_segments_paths( m_rc, m_drawerSettings.GetBaselinePosition(), *m_pChapters, pathTop, pathBottom );
				fill_shape_colour( *m_pPdf, pathTop, m_drawerSettings.GetTopColourSettings().GetBackgroundColour2() );
			}
			else
			{
				create_chapter_lines_paths( m_rc, m_drawerSettings.GetBaselinePosition(), *m_pChapters, pathTop, pathBottom );
				draw_shape_colour( *m_pPdf, pathBottom, m_drawerSettings.GetBottomColourSettings().GetBackgroundColour2() );
			}
		}
	}

	m_pPdf->SetAlpha();
}

void PdfWaveDrawer::ProcessFinalizer()
{
	m_pPdf->UnsetClipping();
}

void PdfWaveDrawer::create_chapter_segments_paths(
		const wxRect2DInt& rect,
		wxFloat32 fBaseline,
		const ChaptersArray& chapters,
		wxPdfShape& pathTop, wxPdfShape& pathBottom )
{
	wxASSERT( chapters.GetCount() > 1 );

	wxFloat64 endPos = chapters.Last().GetMilliseconds().ToDouble();

	wxDouble fHeightTop	   = rect.m_height * ( 1.0f - fBaseline );
	wxDouble fHeightBottom = rect.m_height * fBaseline;

	for ( size_t i = 0, nCount1 = chapters.GetCount() - 1; i < nCount1; i += 2 )
	{
		wxFloat64 tsFrom = chapters[ i ].GetMilliseconds().ToDouble();
		wxFloat64 tsTo	 = chapters[ i + 1 ].GetMilliseconds().ToDouble();

		wxRect2DDouble rc( tsFrom * rect.m_width / endPos, 0, ( tsTo - tsFrom ) * rect.m_width / endPos, fHeightTop );
		rc.m_x += rect.m_x;
		rc.m_y += rect.m_y;

		add_rectangle_to_shape( pathTop, rc );

		rc.m_y	   += fHeightTop;
		rc.m_height = fHeightBottom;

		add_rectangle_to_shape( pathBottom, rc );
	}
}

void PdfWaveDrawer::create_chapter_lines_paths(
		const wxRect2DInt& rect,
		wxFloat32 fBaseline,
		const ChaptersArray& chapters,
		wxPdfShape& pathTop, wxPdfShape& pathBottom )
{
	wxASSERT( chapters.GetCount() > 1 );

	wxFloat64 endPos = chapters.Last().GetMilliseconds().ToDouble();

	wxDouble fHeightTop	   = rect.m_height * ( 1.0f - fBaseline );
	wxDouble fHeightBottom = rect.m_height * fBaseline;

	for ( size_t i = 0, nCount1 = chapters.GetCount() - 1; i < nCount1; i += 1 )
	{
		wxFloat64 ts = chapters[ i ].GetMilliseconds().ToDouble();

		wxPoint2DDouble pt( ts * rect.m_width / endPos, 0.0 );
		pt.m_x += rect.m_x;
		pt.m_y += rect.m_y;

		pathTop.MoveTo( pt.m_x, pt.m_y );
		pt.m_y += fHeightTop;
		pathTop.LineTo( pt.m_x, pt.m_y );
		pathBottom.MoveTo( pt.m_x, pt.m_y );
		pt.m_y += fHeightBottom;
		pathBottom.LineTo( pt.m_x, pt.m_y );
	}

	pathTop.ClosePath();
	pathBottom.ClosePath();
}

void PdfWaveDrawer::create_chapter_segments_path(
		const wxRect2DInt& rect,
		const ChaptersArray& chapters,
		wxPdfShape& path )
{
	wxASSERT( chapters.GetCount() > 1 );

	wxFloat64 endPos = chapters.Last().GetMilliseconds().ToDouble();

	for ( size_t i = 0, nCount1 = chapters.GetCount() - 1; i < nCount1; i += 2 )
	{
		wxFloat64 tsFrom = chapters[ i ].GetMilliseconds().ToDouble();
		wxFloat64 tsTo	 = chapters[ i + 1 ].GetMilliseconds().ToDouble();

		wxRect2DDouble rc( tsFrom * rect.m_width / endPos, 0, ( tsTo - tsFrom ) * rect.m_width / endPos, rect.m_height );
		rc.m_x += rect.m_x;
		rc.m_y += rect.m_y;

		add_rectangle_to_shape( path, rc );
	}

	path.ClosePath();
}

void PdfWaveDrawer::create_chapter_lines_path(
		const wxRect2DInt& rect,
		const ChaptersArray& chapters,
		wxPdfShape& path )
{
	wxASSERT( chapters.GetCount() > 1 );

	wxFloat64 endPos = chapters.Last().GetMilliseconds().ToDouble();

	for ( size_t i = 0, nCount1 = chapters.GetCount() - 1; i < nCount1; i += 1 )
	{
		wxFloat64 ts = chapters[ i ].GetMilliseconds().ToDouble();

		wxPoint2DDouble pt( ts * rect.m_width / endPos, 0 );
		pt.m_x += rect.m_x;
		pt.m_y += rect.m_y;
		path.MoveTo( pt.m_x, pt.m_y );

		pt.m_y += rect.m_height;
		path.LineTo( pt.m_x, pt.m_y );
	}

	path.ClosePath();
}

