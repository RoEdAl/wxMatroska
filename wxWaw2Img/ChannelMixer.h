/*
        ChannelMixer.h
 */

#ifndef _CHANNEL_MIXER_H_
#define _CHANNEL_MIXER_H_

class ChannelMixer:
	public McChainWaveDrawer
{
public:

	ChannelMixer( wxUint16, MultiChannelWaveDrawer*, bool );

	virtual void ProcessFrame( const wxFloat32* );

protected:

	wxFloat32 m_fFactor;
};

#endif

