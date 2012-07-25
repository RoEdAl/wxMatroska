/*
	ColourInterpolation.cpp
*/
#include "StdWx.h"
#include "ColourInterpolation.h"

bool ColourInterpolation::has_alpha( const wxColour& clr )
{
	return clr.Alpha() != wxALPHA_OPAQUE;
}

unsigned int ColourInterpolation::linear_interpolation( unsigned int v1, unsigned int v2, wxFloat32 f )
{
	int v = ceil( f * ( (int)v2 - (int)v1 ) + v1 );

	if ( v < 0 )
	{
		v = 0;
	}
	else if ( v > 255 )
	{
		v = 255;
	}

	return v;
}

wxColour ColourInterpolation::linear_interpolation( const wxColour& c1, const wxColour& c2, wxFloat32 f )
{
	unsigned int r = linear_interpolation( c1.Red(), c2.Red(), f );
	unsigned int g = linear_interpolation( c1.Green(), c2.Green(), f );
	unsigned int b = linear_interpolation( c1.Blue(), c2.Blue(), f );
	unsigned int a = linear_interpolation( c1.Alpha(), c2.Alpha(), f );

	return wxColour( r, g, b, a );
}

