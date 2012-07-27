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

	protected:

		static void create_log_stops( wxGraphicsGradientStops &, const wxColour &, const wxColour &, wxUint32, const LogarithmicScale & );
		static void create_log_stops( wxGraphicsGradientStops &, const wxColour &, const wxColour &, const wxColour &, wxUint32, wxFloat32, const LogarithmicScale & );
};

#endif

