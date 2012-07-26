/*
 *      DrawerSettings.h
 */
#include "StdWx.h"
#include "ColourInterpolation.h"
#include "DrawerSettings.h"

// ===============================================================

ColourSettings::ColourSettings( void ):
	m_clrMiddle( 0, 0, 0, wxALPHA_OPAQUE ),
	m_clrEdge( 0, 0, 0, wxALPHA_TRANSPARENT ),
	m_clrBg( wxTransparentColour ),
	m_clrBg2( 0, 0, 0, 15 )
{}

// ===============================================================

const wxColour& ColourSettings::GetMiddleColour() const
{
	return m_clrMiddle;
}

const wxColour& ColourSettings::GetEdgeColour() const
{
	return m_clrEdge;
}

const wxColour& ColourSettings::GetBackgroundColour() const
{
	return m_clrBg;
}

const wxColour& ColourSettings::GetBackgroundColour2() const
{
	return m_clrBg2;
}

// ===============================================================

wxColour& ColourSettings::GetMiddleColour()
{
	return m_clrMiddle;
}

wxColour& ColourSettings::GetEdgeColour()
{
	return m_clrEdge;
}

wxColour& ColourSettings::GetBackgroundColour()
{
	return m_clrBg;
}

wxColour& ColourSettings::GetBackgroundColour2()
{
	return m_clrBg2;
}

// ===============================================================

ColourSettings& ColourSettings::SetMiddleColour( const wxColour& clr )
{
	m_clrMiddle = clr;
	return *this;
}

ColourSettings& ColourSettings::SetEdgeColour( const wxColour& clr )
{
	m_clrEdge = clr;
	return *this;
}

ColourSettings& ColourSettings::SetBackgroundColour( const wxColour& clr )
{
	m_clrBg = clr;
	return *this;
}

ColourSettings& ColourSettings::SetBackgroundColour2( const wxColour& clr )
{
	m_clrBg2 = clr;
	return *this;
}

// ===============================================================

DrawerSettings::DrawerSettings():
	m_bDrawWithGradient( true ),
	m_bLogarithmicScale( false ),
	m_bLogarithmicColorGradient( false ),
	m_fLogBase( exp( 1.0f ) ),
	m_nFrequency( 50 ),
	m_fBaselinePosition( 0.5f ),
	m_clrBg( wxTransparentColour )
{}

// ===============================================================

bool DrawerSettings::DrawWithGradient() const
{
	return m_bDrawWithGradient;
}

bool DrawerSettings::UseLogarithmicScale() const
{
	return m_bLogarithmicScale;
}

bool DrawerSettings::UseLogarithmicColorGradient() const
{
	return m_bLogarithmicColorGradient;
}

wxFloat32 DrawerSettings::GetLogarithmBase() const
{
	return m_fLogBase;
}

wxUint16 DrawerSettings::GetFrequency() const
{
	return m_nFrequency;
}

wxFloat32 DrawerSettings::GetBaselinePosition() const
{
	return m_fBaselinePosition;
}

wxInt32 DrawerSettings::GetBaselinePositionPercent() const
{
	return ceil( m_fBaselinePosition * 100 );
}

const wxColour& DrawerSettings::GetBackgroundColour() const
{
	return m_clrBg;
}

const ColourSettings& DrawerSettings::GetTopColourSettings() const
{
	return m_topColourSettings;
}

const ColourSettings& DrawerSettings::GetBottomColourSettings() const
{
	return m_bottomColourSettings;
}

// ===============================================================

bool& DrawerSettings::GetDrawWithGradient()
{
	return m_bDrawWithGradient;
}

bool& DrawerSettings::GetUseLogarithmicScale()
{
	return m_bLogarithmicScale;
}

bool& DrawerSettings::GetUseLogarithmicColorGradient()
{
	return m_bLogarithmicColorGradient;
}

wxFloat32& DrawerSettings::GetLogarithmBase()
{
	return m_fLogBase;
}

wxUint16& DrawerSettings::GetFrequency()
{
	return m_nFrequency;
}

wxFloat32& DrawerSettings::GetBaselinePosition()
{
	return m_fBaselinePosition;
}

wxColour& DrawerSettings::GetBackgroundColour()
{
	return m_clrBg;
}

ColourSettings& DrawerSettings::GetTopColourSettings()
{
	return m_topColourSettings;
}

ColourSettings& DrawerSettings::GetBottomColourSettings()
{
	return m_bottomColourSettings;
}

// ===============================================================

DrawerSettings& DrawerSettings::SetDrawWithGradient( bool bDrawWithGradient )
{
	m_bDrawWithGradient = bDrawWithGradient;
	return *this;
}

DrawerSettings& DrawerSettings::SetLogarithmicScale( bool bLoarithmicScale )
{
	m_bLogarithmicScale = bLoarithmicScale;
	return *this;
}

DrawerSettings& DrawerSettings::SetLogarithmicColourGradient( bool bLogarithmicColorGradient )
{
	m_bLogarithmicColorGradient = bLogarithmicColorGradient;
	return *this;
}

DrawerSettings& DrawerSettings::SetLogarithmBase( wxFloat32 fLogBase )
{
	m_fLogBase = fLogBase;
	return *this;
}

DrawerSettings& DrawerSettings::SetMiddleColour( const wxColour& clr )
{
	m_topColourSettings.SetMiddleColour( clr );
	m_bottomColourSettings.SetMiddleColour( clr );
	return *this;
}

const wxColour& DrawerSettings::CalcMiddleColour()
{
	wxColour clr( ColourInterpolation::linear_interpolation( m_bottomColourSettings.GetEdgeColour(), m_topColourSettings.GetEdgeColour(), m_fBaselinePosition ) );
	SetMiddleColour( clr );
	return m_topColourSettings.GetMiddleColour();
}

DrawerSettings& DrawerSettings::SetEdgeColour( const wxColour& clr )
{
	m_topColourSettings.SetEdgeColour( clr );
	m_bottomColourSettings.SetEdgeColour( clr );
	return *this;
}

DrawerSettings& DrawerSettings::SetBackgroundColour( const wxColour& clr )
{
	m_clrBg = clr;
	m_topColourSettings.SetBackgroundColour( clr );
	m_bottomColourSettings.SetBackgroundColour( clr );
	return *this;
}

DrawerSettings& DrawerSettings::SetBackgroundColour2( const wxColour& clr )
{
	m_topColourSettings.SetBackgroundColour2( clr );
	m_bottomColourSettings.SetBackgroundColour2( clr );
	return *this;
}

DrawerSettings& DrawerSettings::SetFrequency( wxUint16 nFrequency )
{
	m_nFrequency = nFrequency;
	return *this;
}

DrawerSettings& DrawerSettings::SetBaselinePosition( wxFloat32 fBaseLineposition )
{
	m_fBaselinePosition = fBaseLineposition;
	return *this;
}

DrawerSettings& DrawerSettings::SetBaselinePositionPercent( wxInt32 nBaselinePosition )
{
	wxASSERT( nBaselinePosition >= 0 && nBaselinePosition <= 100 );
	m_fBaselinePosition = nBaselinePosition / 100.0f;
	return *this;
}
