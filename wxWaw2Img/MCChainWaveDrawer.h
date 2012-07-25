/*
 *      MCChainWaveDrawer.h
 */

#ifndef _MC_CHAIN_WAVE_DRAWER_H_
#define _MC_CHAIN_WAVE_DRAWER_H_

class McChainWaveDrawer:
	public MultiChannelWaveDrawer
{
	protected:

		MultiChannelWaveDrawer* m_pMcWaveDrawer;

	public:

		McChainWaveDrawer( wxUint16 nChannels, MultiChannelWaveDrawer* );
		~McChainWaveDrawer();

		MultiChannelWaveDrawer* GetWaveDrawer() const;

	public:

		virtual void ProcessInitializer();
		virtual void ProcessFrame( const wxFloat32* );
		virtual void ProcessFinalizer();
};

#endif

