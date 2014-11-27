/*
 *      MCChainWaveDrawer.h
 */

#ifndef _MC_CHAIN_WAVE_DRAWER_H_
#define _MC_CHAIN_WAVE_DRAWER_H_

class McChainWaveDrawer:
	public MultiChannelWaveDrawer
{
	private:

		wxScopedPtr< MultiChannelWaveDrawer > m_pMcWaveDrawer;

	public:

		McChainWaveDrawer( wxUint16 nChannels, MultiChannelWaveDrawer* );
		MultiChannelWaveDrawer* GetWaveDrawer() const;

	public:

		virtual void ProcessInitializer();
		virtual void ProcessFrame( const wxFloat32* );
		virtual void ProcessFinalizer();
};
#endif

