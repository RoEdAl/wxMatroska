/*
 *      ArrayWaveDrawer.h
 */

#ifndef _ARRAY_WAVE_DRAWER_H_
#define _ARRAY_WAVE_DRAWER_H_

WX_DEFINE_ARRAY( WaveDrawer*, WaveDrawerArray );

class ArrayWaveDrawer:
	public MultiChannelWaveDrawer
{
	protected:

		WaveDrawerArray m_wave_drawer;

	public:

		ArrayWaveDrawer( wxUint16 nChannels );
		~ArrayWaveDrawer();

		void AddDrawer( WaveDrawer* );

		size_t GetCount() const;

		WaveDrawer* GetDrawer( size_t ) const;

	public:

		virtual void ProcessInitializer();
		virtual void ProcessFrame( const wxFloat32* );
		virtual void ProcessFinalizer();
};

#endif

