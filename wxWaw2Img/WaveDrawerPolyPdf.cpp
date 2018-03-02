/*
 *      WaveDrawerPolyPdf.cpp
 */
#include "StdWx.h"
#include "Arrays.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "DrawerSettings.h"
#include "WaveDrawerPdf.h"
#include "WaveDrawerPolyPdf.h"

PdfPolyWaveDrawer::PdfPolyWaveDrawer( wxUint64 nNumberOfSamples,
									  wxPdfDocument* pPdf,
									  const wxRect2DInt& rc,
									  const DrawerSettings& drawerSettings,
									  const ChaptersArrayScopedPtr& pChapters ) :
	PdfWaveDrawer(
		nNumberOfSamples,
		pPdf,
		drawerSettings.UseLogarithmicScale() || drawerSettings.UseLogarithmicColorGradient(),
		drawerSettings.GetLogarithmBase(),
		rc,
		drawerSettings,
		pChapters )
{}

void PdfPolyWaveDrawer::ProcessInitializer()
{
	__super::ProcessInitializer();

	m_points.Clear();
}

void PdfPolyWaveDrawer::NextColumn( wxFloat32 fValue, wxFloat32 fLogValue )
{
	m_points.Add( wxPoint2DDouble( m_nCurrentColumn, m_drawerSettings.UseLogarithmicScale() ? fLogValue : fValue ) );
}

void PdfPolyWaveDrawer::ProcessFinalizer()
{
	m_pPdf->ClippingPath( build_path() );

	if ( m_drawerSettings.DrawWithGradient() )
	{
		if ( m_drawerSettings.OneMiddleColour() )
		{
			wxPdfColour edgeColour( m_drawerSettings.GetTopColourSettings().GetEdgeColour() );
			wxPdfColour middleColour( m_drawerSettings.GetTopColourSettings().GetMiddleColour() );
			int         gradient = m_pPdf->MidAxialGradient( edgeColour, middleColour, 0, 0, 0, 1, m_drawerSettings.GetBaselinePosition() );

			double alpha = m_drawerSettings.GetTopColourSettings().GetMiddleColour().Alpha();
			alpha /= 255;
			m_pPdf->SetAlpha( alpha, alpha, wxPDF_BLENDMODE_OVERLAY );
			m_pPdf->SetFillGradient( m_rc.GetLeft(), m_rc.GetTop(), m_rc.GetSize().GetWidth(), m_rc.GetSize().GetHeight(), gradient );
		}
		else
		{
			wxPdfColour c1( m_drawerSettings.GetTopColourSettings().GetEdgeColour() );
			wxPdfColour c2( m_drawerSettings.GetTopColourSettings().GetMiddleColour() );
			int         gr1 = m_pPdf->LinearGradient( c2, c1, wxPDF_LINEAR_GRADIENT_VERTICAL );

			double alpha = m_drawerSettings.GetTopColourSettings().GetMiddleColour().Alpha();
			alpha /= 255;
			m_pPdf->SetAlpha( alpha, alpha, wxPDF_BLENDMODE_OVERLAY );

			m_pPdf->SetFillGradient( m_rc.m_x, m_rc.m_y, m_rc.m_width, m_heightUp, gr1 );

			c1.SetColour( m_drawerSettings.GetBottomColourSettings().GetMiddleColour() );
			c2.SetColour( m_drawerSettings.GetBottomColourSettings().GetEdgeColour() );

			alpha  = m_drawerSettings.GetBottomColourSettings().GetMiddleColour().Alpha();
			alpha /= 255;
			m_pPdf->SetAlpha( alpha, alpha, wxPDF_BLENDMODE_OVERLAY );

			int gr2 = m_pPdf->LinearGradient( c2, c1, wxPDF_LINEAR_GRADIENT_VERTICAL );
			m_pPdf->SetFillGradient( m_rc.m_x, m_rc.m_y + m_heightUp, m_rc.m_width, m_heightDown, gr2 );
		}
	}
	else
	{
		if ( m_drawerSettings.OneMiddleColour() )
		{
			wxPdfColour c( m_drawerSettings.GetTopColourSettings().GetMiddleColour() );
			m_pPdf->SetFillColour( c );

			double alpha = m_drawerSettings.GetTopColourSettings().GetMiddleColour().Alpha();
			alpha /= 255;
			m_pPdf->SetAlpha( alpha, alpha, wxPDF_BLENDMODE_OVERLAY );

			m_pPdf->Rect( m_rc.GetLeft(), m_rc.GetTop(), m_rc.GetSize().GetWidth(), m_rc.GetSize().GetHeight(), wxPDF_STYLE_FILL );
		}
		else
		{
			wxPdfColour c;

			c.SetColour( m_drawerSettings.GetTopColourSettings().GetMiddleColour() );
			m_pPdf->SetFillColour( c );

			double alpha = m_drawerSettings.GetTopColourSettings().GetMiddleColour().Alpha();
			alpha /= 255;
			m_pPdf->SetAlpha( alpha, alpha, wxPDF_BLENDMODE_OVERLAY );

			m_pPdf->Rect( m_rc.m_x, m_rc.m_y, m_rc.m_width, m_heightUp, wxPDF_STYLE_FILL );

			alpha  = m_drawerSettings.GetBottomColourSettings().GetMiddleColour().Alpha();
			alpha /= 255;
			m_pPdf->SetAlpha( alpha, alpha, wxPDF_BLENDMODE_OVERLAY );

			c.SetColour( m_drawerSettings.GetBottomColourSettings().GetMiddleColour() );
			m_pPdf->SetFillColour( c );
			m_pPdf->Rect( m_rc.m_x, m_rc.m_y + m_heightUp, m_rc.m_width, m_heightDown, wxPDF_STYLE_FILL );
		}
	}

	__super::ProcessFinalizer();
}

wxPdfShape PdfPolyWaveDrawer::build_path() const
{
	wxPoint2DDouble first_point( m_rc.m_x, m_yoffset );

	wxPdfShape path;

	path.MoveTo( first_point.m_x, first_point.m_y );

	for ( size_t i = 0, nCount = m_points.GetCount(); i < nCount; i++ )
	{
		wxPoint2DDouble pt( m_points[ i ] );

		pt.m_x += m_rc.m_x;
		pt.m_y  = m_yoffset - ( abs( pt.m_y ) * m_heightUp );

		path.LineTo( pt.m_x, pt.m_y );
	}

	path.LineTo( m_rc.m_x + m_rc.m_width, m_yoffset );

	for ( size_t i = m_points.GetCount(); i > 0; i-- )
	{
		wxPoint2DDouble pt( m_points[ i - 1 ] );

		pt.m_x += m_rc.m_x;
		pt.m_y  = m_yoffset + ( abs( pt.m_y ) * m_heightDown );

		path.LineTo( pt.m_x, pt.m_y );
	}

	path.LineTo( first_point.m_x, first_point.m_y );
	path.ClosePath();
	return path;
}

