/*
 *      ArrayWaveDrawer.cpp
 */

#include "StdWx.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "MultiChannelWaveDrawer.h"
#include "ArrayWaveDrawer.h"

ArrayWaveDrawer::ArrayWaveDrawer( wxUint16 nChannels ) :
	MultiChannelWaveDrawer( nChannels )
{}

ArrayWaveDrawer::~ArrayWaveDrawer()
{
	for ( size_t i = 0, nSize = m_wave_drawer.GetCount(); i < nSize; i++ )
	{
		delete m_wave_drawer[ i ];
	}
}

void ArrayWaveDrawer::AddDrawer( WaveDrawer* pDrawer )
{
	wxASSERT( pDrawer != NULL );

	m_wave_drawer.Add( pDrawer );
}

size_t ArrayWaveDrawer::GetCount() const
{
	return m_wave_drawer.GetCount();
}

WaveDrawer* ArrayWaveDrawer::GetDrawer( size_t i ) const
{
	wxASSERT( i >= 0 && i < m_wave_drawer.GetCount() );

	return m_wave_drawer[ i ];
}

void ArrayWaveDrawer::ProcessInitializer()
{
	for ( size_t i = 0, nSize = m_wave_drawer.GetCount(); i < nSize; i++ )
	{
		m_wave_drawer[ i ]->ProcessInitializer();
	}
}

void ArrayWaveDrawer::ProcessFrame( const wxFloat32* frames )
{
	for ( size_t i = 0, nSize = m_wave_drawer.GetCount(); i < nSize; i++ )
	{
		m_wave_drawer[ i ]->ProcessSample( frames[ i ] );
	}
}

void ArrayWaveDrawer::ProcessFinalizer()
{
	for ( size_t i = 0, nSize = m_wave_drawer.GetCount(); i < nSize; i++ )
	{
		m_wave_drawer[ i ]->ProcessFinalizer();
	}
}

