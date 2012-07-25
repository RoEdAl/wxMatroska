/*
 *      WaveDrawer.h
 */

#ifndef _WAVE_DRAWER_H_
#define _WAVE_DRAWER_H_

class WaveDrawer:
	public SampleProcessor
{
	protected:

		WaveDrawer();

	public:

		virtual void ProcessSample( wxFloat32 fSample ) = 0;
};

#endif

