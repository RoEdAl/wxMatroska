/*
 *      ColourInterpolation.h
 */
#ifndef _COLOUR_INTERPOLATION_H_
#define _COLOUR_INTERPOLATION_H_

class ColourInterpolation
{
	public:

		static bool has_alpha( const wxColour& );

		static wxColour linear_interpolation( const wxColour&, const wxColour&, wxFloat32 );

	protected:

		static unsigned int linear_interpolation( unsigned int, unsigned int, wxFloat32 );
};

#endif

