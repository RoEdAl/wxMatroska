/*
 *      DrawerSettings.h
 */

#ifndef _DRAWER_SETTINGS_H
#define _DRAWER_SETTINGS_H

class ColourSettings
{
	public:

		ColourSettings( void );

	public:

		const wxColour& GetMiddleColour() const;
		const wxColour& GetEdgeColour() const;
		const wxColour& GetBackgroundColour() const;
		const wxColour& GetBackgroundColour2() const;

	public:

		wxColour& GetMiddleColour();
		wxColour& GetEdgeColour();
		wxColour& GetBackgroundColour();
		wxColour& GetBackgroundColour2();

	public:

		ColourSettings& SetMiddleColour( const wxColour& );
		ColourSettings& SetEdgeColour( const wxColour& );
		ColourSettings& SetBackgroundColour( const wxColour& );
		ColourSettings& SetBackgroundColour2( const wxColour& );

	protected:

		wxColour m_clrMiddle;
		wxColour m_clrEdge;
		wxColour m_clrBg;
		wxColour m_clrBg2;
};

class DrawerSettings
{
	public:

		DrawerSettings( void );

	protected:

		bool			  m_bDrawWithGradient;
		bool			  m_bLogarithmicScale;
		bool			  m_bLogarithmicColorGradient;
		wxFloat32		  m_fLogBase;
		wxUint16		  m_nFrequency;
		wxFloat32		  m_fBaselinePosition;
		wxColour		  m_clrBg;
		ColourSettings	  m_topColourSettings;
		ColourSettings	  m_bottomColourSettings;
		bool			  m_bDrawChapters;
		wxCompositionMode m_eCompositionMode;

	public:

		bool DrawWithGradient() const;
		bool UseLogarithmicScale() const;
		bool UseLogarithmicColorGradient() const;
		wxFloat32 GetLogarithmBase() const;
		wxUint16 GetFrequency() const;
		wxFloat32 GetBaselinePosition() const;
		wxInt32 GetBaselinePositionPercent() const;
		const wxColour&		  GetBackgroundColour() const;
		const ColourSettings& GetTopColourSettings() const;
		const ColourSettings& GetBottomColourSettings() const;
		bool OneMiddleColour() const;
		bool OneEdgeColour() const;
		bool OneBackgroundColour() const;
		bool OneBackgroundColour2() const;
		bool GetDrawChapters() const;
		wxCompositionMode GetCompositionMode() const;

	public:

		bool& GetDrawWithGradient();
		bool& GetUseLogarithmicScale();
		bool& GetUseLogarithmicColorGradient();
		wxFloat32&		GetLogarithmBase();
		wxUint16&		GetFrequency();
		wxFloat32&		GetBaselinePosition();
		wxColour&		GetBackgroundColour();
		ColourSettings& GetTopColourSettings();
		ColourSettings& GetBottomColourSettings();
		bool& GetDrawChapters();
		wxCompositionMode& GetCompositionMode();

	public:

		DrawerSettings& SetDrawWithGradient( bool );
		DrawerSettings& SetLogarithmicScale( bool );
		DrawerSettings& SetLogarithmicColourGradient( bool );

		DrawerSettings& SetLogarithmBase( wxFloat32 );
		DrawerSettings& SetFrequency( wxUint16 );
		DrawerSettings& SetBaselinePosition( wxFloat32 );
		DrawerSettings& SetBaselinePositionPercent( wxInt32 );

		DrawerSettings& SetMiddleColour( const wxColour& );
		DrawerSettings& SetEdgeColour( const wxColour& );
		DrawerSettings& SetBackgroundColour( const wxColour& );
		DrawerSettings& SetBackgroundColour2( const wxColour& );
		const wxColour& CalcMiddleColour();
		DrawerSettings& SetDrawChapters( bool );

		DrawerSettings& SetCompositionMode( wxCompositionMode );
};

#endif

