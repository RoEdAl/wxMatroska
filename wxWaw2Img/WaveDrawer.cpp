/*
 *      WaveDrawer.cpp
 */

#include "StdWx.h"
#include "LogarithmicScale.h"
#include "ColourInterpolation.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"

WaveDrawer::WaveDrawer()
{}

void WaveDrawer::create_log_stops(
		wxGraphicsGradientStops& stops,
		const wxColour& clrFrom, const wxColour& clrMiddle, const wxColour& clrTo,
		wxUint32 nHeight, wxFloat32 fBaselinePosition,
		const LogarithmicScale& logarithmicScale )
{
	wxASSERT( nHeight > 0 );

	wxFloat32 fBaselinePosition1 = 1.0f - fBaselinePosition;

	for ( wxUint32 i = 1; i < nHeight; i++ )
	{
		wxFloat32 p = 1.0f * i / nHeight;

		if ( p < fBaselinePosition1 )
		{
			wxFloat32 pp = ( fBaselinePosition1 - p ) / fBaselinePosition1;
			stops.Add( ColourInterpolation::linear_interpolation( clrMiddle, clrFrom, logarithmicScale( pp ) ), p );
		}
		else if ( fBaselinePosition != 0.0f )
		{
			wxFloat32 pp = ( p - fBaselinePosition1 ) / fBaselinePosition;
			stops.Add( ColourInterpolation::linear_interpolation( clrMiddle, clrTo, logarithmicScale( pp ) ), p );
		}
	}
}

void WaveDrawer::create_log_stops(
		wxGraphicsGradientStops& stops,
		const wxColour& clrFrom, const wxColour& clrTo,
		wxUint32 nHeight,
		const LogarithmicScale& logarithmicScale )
{
	wxASSERT( nHeight > 0 );

	for ( wxUint32 i = 1; i < nHeight; i++ )
	{
		wxFloat32 p = 1.0f * i / nHeight;
		stops.Add( ColourInterpolation::linear_interpolation( clrFrom, clrTo, logarithmicScale( p ) ), p );
	}
}

