/*
 *      MultiChannelWaveDrawer.h
 */

#ifndef _MULTI_CHANNEL_WAVE_DRAWER_H_
#define _MULTI_CHANNEL_WAVE_DRAWER_H_

class MultiChannelWaveDrawer:
	public SampleProcessor
{
	protected:

		wxUint16 m_nChannels;

	public:

		MultiChannelWaveDrawer( wxUint16 nChannels );

		wxUint16 GetNumberOfChannels() const;

	public:

		virtual void ProcessFrame( const wxFloat32* ) = 0;

		void ProcessFames( const wxFloat32 *, size_t );
};

#endif

