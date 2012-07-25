/*
 * wxConfiguration.h
 */

#ifndef _WX_CONFIGURATION_H_
#define _WX_CONFIGURATION_H_

enum DRAWING_MODE
{
	DRAWING_MODE_SIMPLE,
	DRAWING_MODE_RASTER1,
	DRAWING_MODE_RASTER2,
	DRAWING_MODE_POLY,
	DRAWING_MODE_AUDIO
};

enum INTERVAL_UNIT
{
	INTERVAL_UNIT_NONE,
	INTERVAL_UNIT_SECOND,
	INTERVAL_UNIT_MINUTE,
	INTERVAL_UNIT_HOUR,
	INTERVAL_UNIT_PERCENT
};

class Interval
{
	public:

		INTERVAL_UNIT m_eUnit;
		wxUint32	  m_nValue;

	public:

		Interval():
			m_eUnit( INTERVAL_UNIT_NONE ), m_nValue( 0 )
		{}

		Interval( INTERVAL_UNIT eUnit, wxUint32 nValue ):
			m_eUnit( eUnit ), m_nValue( nValue )
		{}

		Interval( const Interval& interval ):
			m_eUnit( interval.m_eUnit ), m_nValue( interval.m_nValue )
		{}

		Interval& operator =( const Interval& interval )
		{
			m_eUnit	 = interval.m_eUnit;
			m_nValue = interval.m_nValue;
			return *this;
		}

		operator bool() const
		{
			return m_eUnit != INTERVAL_UNIT_NONE;
		}

		bool Get( const wxTimeSpan& duration, wxTimeSpan& ts ) const
		{
			switch ( m_eUnit )
			{
				case INTERVAL_UNIT_NONE:
				default:
				{
					return false;
				}

				case INTERVAL_UNIT_SECOND:
				{
					ts = wxTimeSpan::Seconds( m_nValue );
					return true;
				}

				case INTERVAL_UNIT_MINUTE:
				{
					ts = wxTimeSpan::Minutes( m_nValue );
					return true;
				}

				case INTERVAL_UNIT_HOUR:
				{
					ts = wxTimeSpan::Hours( m_nValue );
					return true;
				}

				case INTERVAL_UNIT_PERCENT:
				ts = wxTimeSpan::Milliseconds( duration.GetMilliseconds().GetValue() * m_nValue / 100 );
				return true;
			}
		}

		static bool Parse( const wxString& _s, Interval& parsing )
		{
			wxString s( _s );

			if ( s.IsEmpty() )
			{
				return false;
			}

			bool   bTruncate = true;
			wxChar c		 = s.Last();

			switch ( c )
			{
				case wxT( 's' ):
				{
					parsing.m_eUnit = INTERVAL_UNIT_SECOND;
					break;
				}

				case wxT( 'm' ):
				{
					parsing.m_eUnit = INTERVAL_UNIT_MINUTE;
					break;
				}

				case wxT( 'h' ):
				{
					parsing.m_eUnit = INTERVAL_UNIT_HOUR;
					break;
				}

				case wxT( '%' ):
				{
					parsing.m_eUnit = INTERVAL_UNIT_PERCENT;
					break;
				}

				default:
				{
					parsing.m_eUnit = INTERVAL_UNIT_MINUTE;
					bTruncate		= false;
					break;
				}
			}

			if ( bTruncate )
			{
				s.RemoveLast();

				if ( s.IsEmpty() )
				{
					return false;
				}
			}

			unsigned long v;

			if ( s.ToCULong( &v ) || s.ToULong( &v ) )
			{
				parsing.m_nValue = v;
				return true;
			}

			return false;
		}
};

class wxConfiguration:
	public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxConfiguration );

	protected:

		DRAWING_MODE m_eDrawingMode;

		wxFileName m_inputFile;
		wxFileName m_outputFile;

		wxUint32 m_nWidth;
		wxUint32 m_nHeight;

		wxColour m_clrFrom;
		wxColour m_clrTo;
		bool	 m_bDrawWithGradient;

		wxColour m_clrBackground;
		wxColour m_clrBackground2;

		wxImageResolution m_imageResolutionUnits;
		wxSize			  m_imageResolution;
		wxUint16		  m_nImageQuality;

		int m_nImageColorDepth;

		bool	  m_bLogarithmicScale;
		bool	  m_bLogarithmicColorGradient;
		wxFloat32 m_fLogBase;

		bool	 m_bMultiChannel;
		wxUint16 m_nColumnNumber;
		wxSize	 m_margins;

		bool m_bPowerMix;

		wxUint16 m_nFrequency;

		wxFileName m_cuePointsFile;
		Interval   m_interval;

		bool m_bUseMLang;

	protected:

		bool ReadNegatableSwitchValue( const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal );
		bool ReadNegatableSwitchValueAndNegate( const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal );
		bool ParseColourString( const wxString&, wxColour& );
		void GetDefaultsFromDisplay();
		static bool ConvertStringToDrawingMode( const wxString&, DRAWING_MODE& );

		static wxString GetDrawingModeAsText( DRAWING_MODE );
		static wxString GetDrawingModeTexts();
		static wxString GetSwitchAsText( bool );

		struct DRAWING_MODE_DESC
		{
			DRAWING_MODE drawingMode;
			const wxChar* description;
		};

		static const DRAWING_MODE_DESC DrawingModeDesc[];
		static const size_t			   DrawingModeDescSize;

	private:

		void add_margin( wxRect2DInt& ) const;

	public:

		const wxFileName& GetInputFile() const;
		wxFileName GetOutputFile() const;

		wxUint32 GetWidth() const;
		wxUint32 GetHeight() const;

		const wxColour& GetColourFrom() const;
		const wxColour& GetColourTo() const;
		bool DrawWithGradient() const;

		const wxColour& GetBackgroundColor() const;
		const wxColour& GetSecondaryBackgroundColor() const;

		wxImageResolution GetImageResolutionUnits() const;
		const wxSize& GetImageResolution() const;
		wxUint16 GetImageQuality() const;

		int GetImageColorDepth() const;

		bool UseLogarithmicScale() const;
		bool UseLogarithmicColorPalette() const;
		wxFloat32 GetLogarithmBase() const;

		bool MultiChannel() const;
		wxUint16 GetNumberOfColumns() const;
		wxSize GetMargins() const;

		wxRect2DInt GetDrawerRect() const;

		wxRect2DInt GetDrawerRect( wxUint16, wxUint16 ) const;

		bool PowerMix() const;

		DRAWING_MODE GetDrawingMode() const;
		wxUint16 GetFrequency() const;

		bool HasCuePointsFile() const;
		const wxFileName& GetCuePointsFile() const;
		bool ReadCuePoints( wxTimeSpanArray& ) const;

		bool HasCuePointsInterval() const;
		bool GenerateCuePoints( const wxTimeSpan&, wxTimeSpanArray& ) const;

		bool UseMLang() const;

	public:

		wxConfiguration( void );

		void AddCmdLineParams( wxCmdLineParser& );
		bool Read( const wxCmdLineParser& );
};

#endif	// _WX_CONFIGURATION_H

