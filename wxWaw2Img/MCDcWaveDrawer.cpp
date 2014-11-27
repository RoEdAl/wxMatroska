/*
 *      MCDcWaveDrawer.cpp
 */
#include "StdWx.h"
#include "Arrays.h"
#include "LogarithmicScale.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "MultiChannelWaveDrawer.h"
#include "ArrayWaveDrawer.h"
#include "MCDcWaveDrawer.h"

McDcWaveDrawer::McDcWaveDrawer( wxUint16 nChannels ):
	ArrayWaveDrawer( nChannels )
{}

void McDcWaveDrawer::Initialize(
		wxDC* pDc
		)
{
	m_pDc.reset( pDc );
}

