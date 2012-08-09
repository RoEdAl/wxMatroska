/*
 *      AnimationSettings.cpp
 */
#include "StdWx.h"
#include "AnimationSettings.h"

// ======================================================================

AnimationSettings::AnimationSettings( void ):
	m_clrBorder( 0, 148, 255 ),
	m_clrFill( 94, 188, 255, 64 ),
	m_nBorderWidth( 0 )
{}

// ======================================================================

const wxColour& AnimationSettings::GetBorderColour() const
{
	return m_clrBorder;
}

const wxColour& AnimationSettings::GetFillColour() const
{
	return m_clrFill;
}

wxUint16 AnimationSettings::GetBorderWidth() const
{
	return m_nBorderWidth;
}

bool AnimationSettings::HasBitmap() const
{
	return m_progressBitmap.IsOk();
}

const wxFileName& AnimationSettings::GetBitmapFilename() const
{
	return m_progressBitmap;
}

// ======================================================================

wxColour& AnimationSettings::GetBorderColour()
{
	return m_clrBorder;
}

wxColour& AnimationSettings::GetFillColour()
{
	return m_clrFill;
}

wxUint16& AnimationSettings::GetBorderWidth()
{
	return m_nBorderWidth;
}

wxFileName& AnimationSettings::GetBitmapFilename()
{
	return m_progressBitmap;
}

