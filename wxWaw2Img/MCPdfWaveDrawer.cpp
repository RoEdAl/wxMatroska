/*
 *      MCPdfWaveDrawer.cpp
 */
#include "StdWx.h"
#include "Arrays.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "MultiChannelWaveDrawer.h"
#include "ArrayWaveDrawer.h"
#include "MCPdfWaveDrawer.h"

#include "Arrays.h"
#include "LogarithmicScale.h"
#include "Interval.h"
#include "DrawerSettings.h"
#include "AnimationSettings.h"
#include "wxConfiguration.h"
#include "wxApp.h"

McPdfWaveDrawer::McPdfWaveDrawer( wxUint16 nChannels ):
	ArrayWaveDrawer( nChannels )
{}

wxPdfDocument* McPdfWaveDrawer::Initialize(
		const wxSize& pageSize,
		const wxColour& clrBg,
		const wxRect2DIntArray& rects,
		const wxString& sTitle
		)
{
	m_pPdf.reset( new wxPdfDocument( wxPORTRAIT, pageSize.GetWidth(), pageSize.GetHeight(), wxS( "pt" ) ) );

	m_pPdf->SetCreator( wxString::Format(  wxS( "%s %s" ), wxGetApp().GetAppDisplayName(), wxMyApp::APP_VERSION ) );
	m_pPdf->SetTitle( sTitle );
	m_pPdf->SetDisplayMode( wxPDF_ZOOM_FULLPAGE );

	m_pPdf->AddPage( wxPORTRAIT );

	m_pPdf->SetFillColour( clrBg );
	m_pPdf->Rect( 0, 0, pageSize.GetWidth(), pageSize.GetHeight(), wxPDF_STYLE_FILL );

	return m_pPdf.get();
}

void McPdfWaveDrawer::ProcessFinalizer()
{
	__super::ProcessFinalizer();
	m_pPdf->Close();
}

bool McPdfWaveDrawer::Save( const wxFileName& fn )
{
	m_pPdf->SaveAsFile( fn.GetFullPath() );
	return true;
}
