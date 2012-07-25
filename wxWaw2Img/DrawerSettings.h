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
		wxColour  m_clrCenter;
		wxColour  m_clrTop;
		wxColour  m_clrBottom;
		wxColour  m_clrBgTop;
		wxColour  m_clrBgTop2;
		wxColour  m_clrBgBottom;
		wxColour  m_clrBgBottom2;
		wxUint16  m_nFrequency;
		wxFloat32 m_fBaselinePosition;

	public:

		bool DrawWithGradient() const;
		bool UseLogarithmicScale() const;
		bool UseLogarithmicColorGradient() const;
		wxFloat32 GetLogarithmBase() const;
		const wxColour& GetColourCenter() const;
		const wxColour& GetColourTop() const;
		const wxColour& GetColourBottom() const;
		const wxColour& GetTopBackgroundColour() const;
		const wxColour& GetTopBackgroundColour2() const;
		const wxColour& GetBottomBackgroundColour() const;
		const wxColour& GetBottomBackgroundColour2() const;
		wxUint16 GetFrequency() const;
		wxFloat32 GetBaselinePosition() const;
		wxInt32 GetBaselinePositionPercent() const;

	public:

		bool& GetDrawWithGradient();
		bool& GetUseLogarithmicScale();
		bool& GetUseLogarithmicColorGradient();
		wxFloat32& GetLogarithmBase();
		wxColour&  GetColourCenter();
		wxColour&  GetColourTop();
		wxColour&  GetColourBottom();
		wxColour&  GetTopBackgroundColour();
		wxColour&  GetTopBackgroundColour2();
		wxColour&  GetBottomBackgroundColour();
		wxColour&  GetBottomBackgroundColour2();
		wxUint16&  GetFrequency();
		wxFloat32& GetBaselinePosition();

	public:

		DrawerSettings& SetDrawWithGradient( bool );
		DrawerSettings& SetLogarithmicScale( bool );
		DrawerSettings& SetLogarithmicColourGradient( bool );

		DrawerSettings& SetLogarithmBase( wxFloat32 );

		DrawerSettings& SetColourCenter( const wxColour& );
		const wxColour& CalcCenterColour();
		DrawerSettings& SetSecondaryColour( const wxColour& );
		DrawerSettings& SetColourTop( const wxColour& );
		DrawerSettings& SetColourBottom( const wxColour& );

		DrawerSettings& SetBackgroundColour( const wxColour& );
		DrawerSettings& SetBackgroundColour2( const wxColour& );

		DrawerSettings& SetTopBackgroundColour( const wxColour& );
		DrawerSettings& SetTopBackgroundColour2( const wxColour& );
		DrawerSettings& SetBottomBackgroundColour( const wxColour& );
		DrawerSettings& SetBottomBackgroundColour2( const wxColour& );

		DrawerSettings& SetFrequency( wxUint16 );
		DrawerSettings& SetBaselinePosition( wxFloat32 );
		DrawerSettings& SetBaselinePositionPercent( wxInt32 );
};

#endif

