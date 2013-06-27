/*
 *      ChannelMixer.cpp
 */
#include "StdWx.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "MultiChannelWaveDrawer.h"
#include "MCChainWaveDrawer.h"
#include "ChannelMixer.h"

ChannelMixer::ChannelMixer( wxUint16 nChannels, MultiChannelWaveDrawer* pMcWaveDrawer, bool bPowerMix ):
	McChainWaveDrawer( nChannels, pMcWaveDrawer )
{
	wxASSERT( pMcWaveDrawer->GetNumberOfChannels() == 1 );

	if ( bPowerMix )
	{
		m_fFactor = 1.0f / sqrtf( nChannels );
	}
	else
	{
		m_fFactor = 1.0f / nChannels;
	}
}

void ChannelMixer::ProcessFrame( const wxFloat32* frame )
{
	wxFloat32 fMix = 0.0;

	for ( wxUint16 i = 0; i < m_nChannels; i++ )
	{
		fMix += frame[ i ] * m_fFactor;
	}

	if ( fMix <= -1.0f )
	{
		fMix = -1.0f;
	}
	else if ( fMix >= 1.0f )
	{
		fMix = 1.0f;
	}

	__super::ProcessFrame( &fMix );
}


