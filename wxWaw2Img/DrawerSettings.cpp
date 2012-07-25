/*
	DrawerSettings.h
*/
#include "StdWx.h"
#include "DrawerSettings.h"

DrawerSettings::DrawerSettings():
	m_bDrawWithGradient( true ),
	m_bLogarithmicScale( false ),
	m_bLogarithmicColorGradient( false ),
	m_fLogBase( exp(1.0f) ),
	m_clrFrom( 0, 0, 0, wxALPHA_OPAQUE ),
	m_clrTo( 0, 0, 0, wxALPHA_TRANSPARENT ),
	m_clrBg( wxTransparentColour ),
	m_clrBgSecond(  0, 0, 0, 15 ),
	m_nFrequency( 50 )
{}

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

const wxColour& DrawerSettings::GetColourFrom() const
{
	return m_clrFrom;
}

const wxColour& DrawerSettings::GetColourTo() const
{
	if ( m_bDrawWithGradient )
	{
		return m_clrTo;
	}
	else
	{
		return m_clrFrom;
	}
}

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

wxColour& DrawerSettings::GetColourFrom()
{
	return m_clrFrom;
}

wxColour& DrawerSettings::GetColourTo()
{
	return m_clrTo;
}

wxColour& DrawerSettings::GetBackgroundColour()
{
	return m_clrBg;
}

wxColour& DrawerSettings::GetSecondaryBackgroundColour()
{
	return m_clrBgSecond;
}

wxUint16& DrawerSettings::GetFrequency()
{
	return m_nFrequency;
}

const wxColour& DrawerSettings::GetBackgroundColour() const
{
	return m_clrBg;
}

const wxColour& DrawerSettings::GetSecondaryBackgroundColour() const
{
	return m_clrTo;
}

wxUint16 DrawerSettings::GetFrequency() const
{
	return m_nFrequency;
}

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

DrawerSettings& DrawerSettings::SetColourFrom( const wxColour& clrFrom )
{
	m_clrFrom = clrFrom;
	return *this;
}

DrawerSettings& DrawerSettings::SetColourTo( const wxColour& clrTo )
{
	m_clrTo = clrTo;
	return *this;
}

DrawerSettings& DrawerSettings::SetBackgroundColour( const wxColour& clrBg )
{
	m_clrBg = clrBg;
	return *this;
}

DrawerSettings& DrawerSettings::SetSecondaryBackgroundColour( const wxColour& clrBgSecond )
{
	m_clrBgSecond = clrBgSecond;
	return *this;
}

DrawerSettings& DrawerSettings::SetFrequency( wxUint16 nFrequency )
{
	m_nFrequency = nFrequency;
	return *this;
}
