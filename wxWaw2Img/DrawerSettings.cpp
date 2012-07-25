/*
 *      DrawerSettings.h
 */
#include "StdWx.h"
#include "ColourInterpolation.h"
#include "DrawerSettings.h"

DrawerSettings::DrawerSettings():
	m_bDrawWithGradient( true ),
	m_bLogarithmicScale( false ),
	m_bLogarithmicColorGradient( false ),
	m_fLogBase( exp( 1.0f ) ),
	m_clrCenter( 0, 0, 0, wxALPHA_OPAQUE ),
	m_clrTop( 0, 0, 0, wxALPHA_TRANSPARENT ),
	m_clrBottom( 0, 0, 0, wxALPHA_TRANSPARENT ),
	m_clrBgTop( wxTransparentColour ),
	m_clrBgTop2( 0, 0, 0, 15 ),
	m_clrBgBottom( wxTransparentColour ),
	m_clrBgBottom2( 0, 0, 0, 15 ),
	m_nFrequency( 50 ),
	m_fBaselinePosition( 0.5f )
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

const wxColour& DrawerSettings::GetColourCenter() const
{
	return m_clrCenter;
}

const wxColour& DrawerSettings::GetColourTop() const
{
	if ( m_bDrawWithGradient )
	{
		return m_clrTop;
	}
	else
	{
		return m_clrCenter;
	}
}

const wxColour& DrawerSettings::GetColourBottom() const
{
	if ( m_bDrawWithGradient )
	{
		return m_clrBottom;
	}
	else
	{
		return m_clrCenter;
	}
}

const wxColour& DrawerSettings::GetTopBackgroundColour() const
{
	return m_clrBgTop;
}

const wxColour& DrawerSettings::GetTopBackgroundColour2() const
{
	return m_clrBgTop2;
}

const wxColour& DrawerSettings::GetBottomBackgroundColour() const
{
	return m_clrBgBottom;
}

const wxColour& DrawerSettings::GetBottomBackgroundColour2() const
{
	return m_clrBgBottom2;
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

wxColour& DrawerSettings::GetColourCenter()
{
	return m_clrCenter;
}

wxColour& DrawerSettings::GetColourTop()
{
	return m_clrTop;
}

wxColour& DrawerSettings::GetColourBottom()
{
	return m_clrBottom;
}

wxColour& DrawerSettings::GetTopBackgroundColour()
{
	return m_clrBgTop;
}

wxColour& DrawerSettings::GetTopBackgroundColour2()
{
	return m_clrBgTop2;
}

wxColour& DrawerSettings::GetBottomBackgroundColour()
{
	return m_clrBgBottom;
}

wxColour& DrawerSettings::GetBottomBackgroundColour2()
{
	return m_clrBgBottom2;
}

wxUint16& DrawerSettings::GetFrequency()
{
	return m_nFrequency;
}

wxFloat32& DrawerSettings::GetBaselinePosition()
{
	return m_fBaselinePosition;
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

DrawerSettings& DrawerSettings::SetColourCenter( const wxColour& clr )
{
	m_clrCenter = clr;
	return *this;
}

const wxColour& DrawerSettings::CalcCenterColour()
{
	m_clrCenter = ColourInterpolation::linear_interpolation( m_clrBottom, m_clrTop, m_fBaselinePosition );
	return m_clrCenter;
}

DrawerSettings& DrawerSettings::SetSecondaryColour( const wxColour& clr )
{
	m_clrTop = clr;
	m_clrBottom = clr;
	return *this;
}

DrawerSettings& DrawerSettings::SetColourTop( const wxColour& clr )
{
	m_clrTop = clr;
	return *this;
}

DrawerSettings& DrawerSettings::SetColourBottom( const wxColour& clr )
{
	m_clrBottom = clr;
	return *this;
}

DrawerSettings& DrawerSettings::SetBackgroundColour( const wxColour& clr )
{
	m_clrBgTop = clr;
	m_clrBgBottom = clr;
	return *this;
}

DrawerSettings& DrawerSettings::SetBackgroundColour2( const wxColour& clr )
{
	m_clrBgTop2 = clr;
	m_clrBgBottom2 = clr;
	return *this;
}

DrawerSettings& DrawerSettings::SetTopBackgroundColour( const wxColour& clr )
{
	m_clrBgTop = clr;
	return *this;
}

DrawerSettings& DrawerSettings::SetTopBackgroundColour2( const wxColour& clr )
{
	m_clrBgTop2 = clr;
	return *this;
}

DrawerSettings& DrawerSettings::SetBottomBackgroundColour( const wxColour& clr )
{
	m_clrBgBottom = clr;
	return *this;
}

DrawerSettings& DrawerSettings::SetBottomBackgroundColour2( const wxColour& clr )
{
	m_clrBgBottom2 = clr;
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

