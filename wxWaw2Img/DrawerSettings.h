/*
 *      DrawerSettings.h
 */

#ifndef _DRAWER_SETTINGS_H
#define _DRAWER_SETTINGS_H

class DrawerSettings
{
	public:

		DrawerSettings( void );

	protected:

		bool	  m_bDrawWithGradient;
		bool	  m_bLogarithmicScale;
		bool	  m_bLogarithmicColorGradient;
		wxFloat32 m_fLogBase;
		wxColour  m_clrFrom;
		wxColour  m_clrTo;
		wxColour  m_clrBg;
		wxColour  m_clrBgSecond;
		wxUint16  m_nFrequency;
		wxFloat32 m_fBaselinePosition;

	public:

		bool DrawWithGradient() const;
		bool UseLogarithmicScale() const;
		bool UseLogarithmicColorGradient() const;
		wxFloat32 GetLogarithmBase() const;
		const wxColour& GetColourFrom() const;
		const wxColour& GetColourTo() const;
		const wxColour& GetBackgroundColour() const;
		const wxColour& GetSecondaryBackgroundColour() const;
		wxUint16 GetFrequency() const;
		wxFloat32 GetBaselinePosition() const;
		wxInt32 GetBaselinePositionPercent() const;

	public:

		bool& GetDrawWithGradient();
		bool& GetUseLogarithmicScale();
		bool& GetUseLogarithmicColorGradient();
		wxFloat32& GetLogarithmBase();
		wxColour&  GetColourFrom();
		wxColour&  GetColourTo();
		wxColour&  GetBackgroundColour();
		wxColour&  GetSecondaryBackgroundColour();
		wxUint16&  GetFrequency();
		wxFloat32& GetBaselinePosition();

	public:

		DrawerSettings& SetDrawWithGradient( bool );
		DrawerSettings& SetLogarithmicScale( bool );
		DrawerSettings& SetLogarithmicColourGradient( bool );

		DrawerSettings& SetLogarithmBase( wxFloat32 );
		DrawerSettings& SetColourFrom( const wxColour& );
		DrawerSettings& SetColourTo( const wxColour& );
		DrawerSettings& SetBackgroundColour( const wxColour& );
		DrawerSettings& SetSecondaryBackgroundColour( const wxColour& );

		DrawerSettings& SetFrequency( wxUint16 );
		DrawerSettings& SetBaselinePosition( wxFloat32 );
		DrawerSettings& SetBaselinePositionPercent( wxInt32 );
};

#endif

