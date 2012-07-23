/*
        MultiChannelWaveDrawer.cpp
 */

#include "StdWx.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "MultiChannelWaveDrawer.h"
#include "MCChainWaveDrawer.h"

McChainWaveDrawer::McChainWaveDrawer( wxUint16 nChannels, MultiChannelWaveDrawer* pMcWaveDrawer ):
	MultiChannelWaveDrawer( nChannels ), m_pMcWaveDrawer( pMcWaveDrawer )
{
	wxASSERT( pMcWaveDrawer != NULL );
}

McChainWaveDrawer::~McChainWaveDrawer()
{
	delete m_pMcWaveDrawer;
}

MultiChannelWaveDrawer* McChainWaveDrawer::GetWaveDrawer() const
{
	return m_pMcWaveDrawer;
}

void McChainWaveDrawer::ProcessInitializer()
{
	m_pMcWaveDrawer->ProcessInitializer();
}

void McChainWaveDrawer::ProcessFrame( const wxFloat32* frame )
{
	m_pMcWaveDrawer->ProcessFrame( frame );
}

void McChainWaveDrawer::ProcessFinalizer()
{
	m_pMcWaveDrawer->ProcessFinalizer();
}

