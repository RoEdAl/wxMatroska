/*
 *      MCDcSvgWaveDrawer.cpp
 */
#include "StdWx.h"
#include "Arrays.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "MultiChannelWaveDrawer.h"
#include "ArrayWaveDrawer.h"
#include "MCDcWaveDrawer.h"
#include "MCDcSvgWaveDrawer.h"

McDcSvgWaveDrawer::McDcSvgWaveDrawer( wxUint16 nChannels ):
	McDcWaveDrawer( nChannels )
{}

wxDC* McDcSvgWaveDrawer::Initialize(
		const wxSize& pageSize,
		const wxColour& clrBg,
		const wxRect2DIntArray& rects,
		const wxString& sTitle
		)
{
	m_tmpFileName.AssignTempFileName( wxS( "wav2img" ) );
	wxSVGFileDC* pSvgDc = new wxSVGFileDC( m_tmpFileName.GetFullPath(), pageSize.GetWidth(), pageSize.GetHeight(), 72 );
	__super::Initialize( pSvgDc );
	return m_pDc.get();
}

void McDcSvgWaveDrawer::ProcessFinalizer()
{
	__super::ProcessFinalizer();

	m_pDc.reset( NULL );
}

bool McDcSvgWaveDrawer::Save( const wxFileName& fn )
{
	if ( wxRenameFile( m_tmpFileName.GetFullPath(), fn.GetFullPath(), true ) )
	{
		return true;
	}
	else
	{
		wxRemoveFile( m_tmpFileName.GetFullPath() );
		return false;
	}
}

