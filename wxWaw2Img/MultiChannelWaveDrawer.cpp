/*
 *      MultiChannelWaveDrawer.cpp
 */

#include "StdWx.h"
#include "FloatArray.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "MultiChannelWaveDrawer.h"

MultiChannelWaveDrawer::MultiChannelWaveDrawer( wxUint16 nChannels ):
	m_nChannels( nChannels )
{
	wxASSERT( nChannels > 0 );
}

wxUint16 MultiChannelWaveDrawer::GetNumberOfChannels() const
{
	return m_nChannels;
}

void MultiChannelWaveDrawer::ProcessFames( const wxFloat32* frames, size_t nFrames )
{
	for ( size_t i = 0; i < nFrames; i++, frames += m_nChannels )
	{
		ProcessFrame( frames );
	}
}

