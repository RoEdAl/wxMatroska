/*
 * wxConfiguration.cpp
 */

#include "StdWx.h"
#include "FloatArray.h"
#include "CuePointsReader.h"
#include "Interval.h"
#include "DrawerSettings.h"
#include "wxConfiguration.h"
#include "wxApp.h"

// ===============================================================================

const wxSystemColour wxConfiguration::COLOR_BACKGROUND	= wxSYS_COLOUR_WINDOW;
const wxSystemColour wxConfiguration::COLOR_BACKGROUND2 = wxSYS_COLOUR_MENUBAR;

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxConfiguration, wxObject );

// ===============================================================================

const wxConfiguration::DRAWING_MODE_DESC wxConfiguration::DrawingModeDesc[] =
{
	{ DRAWING_MODE_SIMPLE, wxT( "simple" ) },
	{ DRAWING_MODE_RASTER1, wxT( "raster1" ) },
	{ DRAWING_MODE_RASTER2, wxT( "raster2" ) },
	{ DRAWING_MODE_POLY, wxT( "poly" ) },
	{ DRAWING_MODE_AUDIO, wxT( "audio" ) },
};

const size_t wxConfiguration::DrawingModeDescSize = sizeof ( wxConfiguration::DrawingModeDesc ) / sizeof ( wxConfiguration::DRAWING_MODE_DESC );

// ===============================================================================

wxConfiguration::wxConfiguration( void ):
	m_eDrawingMode( DRAWING_MODE_RASTER1 ),
	m_imageSize( 800, 300 ),
	m_imageResolutionUnits( wxIMAGE_RESOLUTION_INCHES ),
	m_imageResolution( 150, 150 ),
	m_nImageQuality( 75 ),
	m_nImageColorDepth( wxBITMAP_SCREEN_DEPTH ),
	m_bMultiChannel( false ),
	m_nColumnNumber( 1 ),
	m_margins( 4, 4 ),
	m_bPowerMix( true ),
	m_bGenerateCuePoints( false ),
	m_interval( INTERVAL_UNIT_PERCENT, 10 ),
	m_bUseMLang( true )
{}

wxString wxConfiguration::GetSwitchAsText( bool b )
{
	return b ? _( "on" ) : _( "off" );
}

void wxConfiguration::AddCmdLineParams( wxCmdLineParser& cmdLine ) const
{
	cmdLine.AddParam( _( "Input file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "o", "output", _( "Output file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "m", "drawing-mode", wxString::Format( _( "Drawing mode. [%s] (default: %s)" ), GetDrawingModeTexts(), GetDrawingModeAsText( m_eDrawingMode ) ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( "s", "take-display-properties", _( "Get image size and color depth from display properties (default: on)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "w", "width", wxString::Format( _( "Image width in pixels (default: %u)" ), m_imageSize.GetWidth() ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "y", "height", wxString::Format( _( "Image height in pixels (default: %u)" ), m_imageSize.GetHeight() ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( "g", "gradient", wxString::Format( _( "Draw gradient (default: %s)" ), GetSwitchAsText( m_drawerSettings.DrawWithGradient() ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	cmdLine.AddOption( "c", "colour", wxString::Format( _( "Colour to draw (default: %s)" ), m_drawerSettings.GetColourFrom().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "c2", "second-colour", wxString::Format( _( "Second colour to draw if drawing gradient (default: %s)" ), m_drawerSettings.GetColourTo().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( "b", "background", _( "Background color (default: transparent or white)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "b2", "secondary-background", wxString::Format( _( "Secondary background color (default: %s)" ), m_drawerSettings.GetSecondaryBackgroundColour().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( "bs", "background-from-system", _( "Take background colors from system settings (default: off)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	cmdLine.AddOption( "r", "resolution", wxString::Format( _( "Image horizontal and vertical resolution (default: %d)" ), m_imageResolution.x ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "rx", "x-resolution", wxString::Format( _( "Image horizontal resolution (default: %d)" ), m_imageResolution.x ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "ry", "y-resolution", wxString::Format( _( "Image vertical resolution (default: %d)" ), m_imageResolution.y ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( wxEmptyString, "resolution-units", _( "Image resolution units [inches|cm] (default: inches)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( "q", "image-quality", wxString::Format( _( "Image quality [0-100] (default: %d)" ), m_nImageQuality ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "d", "color-depth", _( "Image color depth [8,16,24,32] (default: display color depth)" ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( "l", "logaritmic-scale", wxString::Format( _( "Draw using logarithmic scale (default: %s)" ), GetSwitchAsText( m_drawerSettings.UseLogarithmicScale() ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( "lc", "logarithmic-color-gradient", wxString::Format( _( "Use logarithmic color gradient (default: %s)" ), GetSwitchAsText( m_drawerSettings.UseLogarithmicColorGradient() ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "lb", "logarithm-base", _( "Logarithm base used in logarithmic scale (default: 10)" ), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( "mc", "multi-channel", wxString::Format( _( "Multi channel mode (default: %s)" ), GetSwitchAsText( m_bMultiChannel ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "cn", "number-of-columns", wxString::Format( _( "Number of columns in nultichannel mode (default %u)" ), m_nColumnNumber ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( "mp", "power-mix", wxString::Format( _( "Power mixing of multichannel audio source (default: %s)" ), GetSwitchAsText( m_bPowerMix ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	cmdLine.AddOption( "ma", "margin", wxString::Format( _( "Margin (horizontal and vertical) (default %u)" ), m_margins.x ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "mx", "margin-x", wxString::Format( _( "Horizontal margin (default %u)" ), m_margins.x ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "my", "margin-y", wxString::Format( _( "Vertical margin (default %u)" ), m_margins.y ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( "bl", "baseline-position", wxString::Format( _( "Position of baseline (default %d%%)" ), m_drawerSettings.GetBaselinePositionPercent() ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( "f", "frequency", wxString::Format( _( "Frequency (in Hz) of rendered audio file (default %u)" ), m_drawerSettings.GetFrequency() ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "cp", "cue-point-file", _( "Cue point file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( "cg", "generate-cue-points", wxString::Format( _( "Multi channel mode (default: %s)" ), GetSwitchAsText( m_bGenerateCuePoints ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "ci", "cue-points-interval", wxString::Format( _( "Cue points interval (default: %s)" ), m_interval.AsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( wxEmptyString, "use-mlang", wxString::Format( _( "Use MLang library (default: %s)" ), GetSwitchAsText( m_bUseMLang ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
}

bool wxConfiguration::ReadNegatableSwitchValue( const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal )
{
	wxCmdLineSwitchState state = cmdLine.FoundSwitch( name );
	bool				 res   = true;

	switch ( state )
	{
		case wxCMD_SWITCH_ON:
		{
			switchVal = true;
			break;
		}

		case wxCMD_SWITCH_OFF:
		{
			switchVal = false;
			break;
		}

		default:
		{
			res = false;
			break;
		}
	}

	return res;
}

bool wxConfiguration::ReadNegatableSwitchValueAndNegate( const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal )
{
	if ( ReadNegatableSwitchValue( cmdLine, name, switchVal ) )
	{
		switchVal = !switchVal;
		return true;
	}
	else
	{
		return false;
	}
}

void wxConfiguration::GetDefaultsFromDisplay()
{
	wxRect dplRect = wxGetClientDisplayRect();
	int	   nDepth  = wxDisplayDepth();
	wxSize res	   = wxGetDisplayPPI();

	wxLogInfo( _( "Display size in pixels: %dx%d (%dx%d)" ), dplRect.GetWidth(), dplRect.GetHeight(), dplRect.x, dplRect.y );
	wxLogInfo( _( "Display color depth: %d bits" ), nDepth );
	wxLogInfo( _( "Display resolution in pixels per inch: %dx%d" ), res.x, res.y );

	m_imageSize		   = dplRect.GetSize();
	m_nImageColorDepth = nDepth;
	m_imageResolution  = res;
}

static int time_span_compare_fn( wxTimeSpan* ts1, wxTimeSpan* ts2 )
{
	if ( ts1->GetValue() < ts2->GetValue() )
	{
		return -1;
	}
	else if ( ts1->GetValue() > ts2->GetValue() )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool wxConfiguration::ReadCuePoints( wxTimeSpanArray& cuePoints ) const
{
	CuePointsReader reader;

	if ( reader.Read( cuePoints, m_cuePointsFile, m_bUseMLang ) )
	{
		cuePoints.Sort( time_span_compare_fn );
		return true;
	}
	else
	{
		return false;
	}
}

static wxColour get_default_bg_color( int nDepth )
{
	switch ( nDepth )
	{
		case 8:
		case 16:
		case 24:
		{
			return wxColour( "white" );
		}

		case 32:
		{
			return wxTransparentColour;
		}

		default:
		wxFAIL_MSG( "Invalid image color depth" );
		return wxColour();
	}
}

bool wxConfiguration::ConvertStringToDrawingMode( const wxString& s, DRAWING_MODE& e )
{
	for ( size_t i = 0; i < DrawingModeDescSize; i++ )
	{
		if ( s.CmpNoCase( DrawingModeDesc[ i ].description ) == 0 )
		{
			e = DrawingModeDesc[ i ].drawingMode;
			return true;
		}
	}

	return false;
}

wxString wxConfiguration::GetDrawingModeAsText( DRAWING_MODE e )
{
	for ( size_t i = 0; i < DrawingModeDescSize; i++ )
	{
		if ( DrawingModeDesc[ i ].drawingMode == e )
		{
			return DrawingModeDesc[ i ].description;
		}
	}

	return wxString::Format( "<%d>", static_cast< int >( e ) );
}

wxString wxConfiguration::GetDrawingModeTexts()
{
	wxString s;

	for ( size_t i = 0; i < DrawingModeDescSize; i++ )
	{
		s << DrawingModeDesc[ i ].description << "|";
	}

	return s.RemoveLast();
}

bool wxConfiguration::Read( const wxCmdLineParser& cmdLine )
{
	bool	 bRes = true;
	wxString s;
	long	 v;
	wxDouble vd;
	wxColour clr;

	if ( ReadNegatableSwitchValue( cmdLine, "s", bRes ) && bRes )
	{
		GetDefaultsFromDisplay();
	}

	if ( cmdLine.GetParamCount() > 0 )
	{
		m_inputFile = cmdLine.GetParam();
	}

	if ( cmdLine.Found( "o", &s ) )
	{
		m_outputFile = s;
	}

	if ( cmdLine.Found( "m", &s ) )
	{
		if ( !ConvertStringToDrawingMode( s, m_eDrawingMode ) )
		{
			wxLogWarning( _( "Invalid draw mode - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "w", &v ) )
	{
		if ( v <= 0 || v > 10000 )
		{
			wxLogWarning( _( "Wrong width - %d" ), v );
			return false;
		}

		m_imageSize.SetWidth( v );
	}

	if ( cmdLine.Found( "y", &v ) )
	{
		if ( v <= 0 || v > 10000 )
		{
			wxLogWarning( _( "Invalid height - %d" ), v );
			return false;
		}

		m_imageSize.SetHeight( v );
	}

	if ( cmdLine.Found( "c", &s ) )
	{
		if ( !ParseColourString( s, m_drawerSettings.GetColourFrom() ) )
		{
			wxLogWarning( _( "Invalid color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "c2", &s ) )
	{
		if ( !ParseColourString( s, m_drawerSettings.GetColourTo() ) )
		{
			wxLogWarning( _( "Invalid second color - %s" ), s );
			return false;
		}

		m_drawerSettings.SetDrawWithGradient( true );
	}

	ReadNegatableSwitchValue( cmdLine, "g", m_drawerSettings.GetDrawWithGradient() );

	if ( cmdLine.Found( "d", &v ) )
	{
		switch ( v )
		{
			case 8:
			case 16:
			case 24:
			case 32:
			{
				m_nImageColorDepth = v;
				break;
			}

			default:
			wxLogWarning( _( "Invalid image color depth - %d" ), v );
			return false;
		}
	}

	if ( m_nImageColorDepth == wxBITMAP_SCREEN_DEPTH )
	{
		m_drawerSettings.SetBackgroundColour( get_default_bg_color( wxGetDisplayDepth() ) );
	}
	else
	{
		m_drawerSettings.SetBackgroundColour( get_default_bg_color( m_nImageColorDepth ) );
	}

	if ( ReadNegatableSwitchValue( cmdLine, "bs", bRes ) && bRes )
	{
		m_drawerSettings.SetBackgroundColour( wxSystemSettings::GetColour( COLOR_BACKGROUND ) );
		m_drawerSettings.SetSecondaryBackgroundColour( wxSystemSettings::GetColour( COLOR_BACKGROUND2 ) );
		wxLogInfo( _( "Background color: %s" ), m_drawerSettings.GetBackgroundColour().GetAsString() );
		wxLogInfo( _( "Secondary background color: %s" ), m_drawerSettings.GetSecondaryBackgroundColour().GetAsString() );
	}
	else
	{
		if ( cmdLine.Found( "b", &s ) )
		{
			if ( !ParseColourString( s, m_drawerSettings.GetBackgroundColour() ) )
			{
				wxLogWarning( _( "Invalid background color - %s" ), s );
				return false;
			}
		}

		if ( cmdLine.Found( "b2", &s ) )
		{
			if ( !ParseColourString( s, m_drawerSettings.GetSecondaryBackgroundColour() ) )
			{
				wxLogWarning( _( "Invalid second background color - %s" ), s );
				return false;
			}
		}
	}

	if ( cmdLine.Found( "r", &v ) )
	{
		if ( v <= 0 || v > 10000 )
		{
			wxLogWarning( _( "Invalid resolution - %d" ), v );
			return false;
		}

		m_imageResolution = wxSize( v, v );
	}

	if ( cmdLine.Found( "rx", &v ) )
	{
		if ( v <= 0 || v > 10000 )
		{
			wxLogWarning( _( "Invalid X resolution - %d" ), v );
			return false;
		}

		m_imageResolution.SetWidth( v );
	}

	if ( cmdLine.Found( "ry", &v ) )
	{
		if ( v <= 0 || v > 10000 )
		{
			wxLogWarning( _( "Invalid Y resolution - %d" ), v );
			return false;
		}

		m_imageResolution.SetHeight( v );
	}

	if ( cmdLine.Found( "resolution-units", &s ) )
	{
		if ( s.CmpNoCase( "inches" ) == 0 )
		{
			m_imageResolutionUnits = wxIMAGE_RESOLUTION_INCHES;
		}
		else if ( s.CmpNoCase( "cm" ) == 0 )
		{
			m_imageResolutionUnits = wxIMAGE_RESOLUTION_CM;
		}
		else
		{
			wxLogWarning( _( "Invalid image resolution units - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "q", &v ) )
	{
		if ( v < 0 || v > 100 )
		{
			wxLogWarning( _( "Invalid image quality - %d" ), v );
			return false;
		}

		m_nImageQuality = v;
	}

	if ( cmdLine.Found( "lb", &vd ) )
	{
		if ( vd <= 1.0 || vd > 100000.0f )
		{
			wxLogWarning( _( "Invalid logarithm base - %f" ), vd );
			return false;
		}

		m_drawerSettings.SetLogarithmBase( vd );
	}

	ReadNegatableSwitchValue( cmdLine, "l", m_drawerSettings.GetDrawWithGradient() );
	ReadNegatableSwitchValue( cmdLine, "lc", m_drawerSettings.GetUseLogarithmicColorGradient() );

	ReadNegatableSwitchValue( cmdLine, "mc", m_bMultiChannel );

	if ( cmdLine.Found( "cn", &v ) )
	{
		if ( v < 1 || v > 10 )
		{
			wxLogWarning( _( "Invalid number of columns - %d" ), v );
			return false;
		}

		m_nColumnNumber = v;
	}

	if ( cmdLine.Found( "ma", &v ) )
	{
		if ( v < 0 || v > 10000 )
		{
			wxLogWarning( _( "Invalid margin - %d" ), v );
			return false;
		}

		m_margins.Set( v, v );
	}

	if ( cmdLine.Found( "mx", &v ) )
	{
		if ( v < 0 || v > 10000 )
		{
			wxLogWarning( _( "Invalid horizontal margin - %d" ), v );
			return false;
		}

		m_margins.SetWidth( v );
	}

	if ( cmdLine.Found( "my", &v ) )
	{
		if ( v < 0 || v > 10000 )
		{
			wxLogWarning( _( "Invalid vertical margin - %d" ), v );
			return false;
		}

		m_margins.SetHeight( v );
	}

	ReadNegatableSwitchValue( cmdLine, "mp", m_bPowerMix );

	if ( cmdLine.Found( "f", &v ) )
	{
		if ( v < 10 || v > 20000 )
		{
			wxLogWarning( _( "Invalid audio frequency - %d" ), v );
			return false;
		}

		m_drawerSettings.SetFrequency( v );
	}

	if ( cmdLine.Found( "cp", &s ) )
	{
		m_cuePointsFile = s;
	}

	ReadNegatableSwitchValue( cmdLine, "cg", m_bGenerateCuePoints );

	if ( cmdLine.Found( "ci", &s ) )
	{
		if ( !Interval::Parse( s, m_interval ) )
		{
			wxLogWarning( _( "Invalid interval - %s" ), s );
			return false;
		}
	}

	ReadNegatableSwitchValue( cmdLine, "use-mlang", m_bUseMLang );

	if ( cmdLine.Found( "bl", &v ) )
	{
		if ( v <= 0 || v > 100 )
		{
			wxLogWarning( _( "Invalid baseline position - %d" ), v );
			return false;
		}

		m_drawerSettings.SetBaselinePositionPercent( v );
	}

	return true;
}

const wxFileName& wxConfiguration::GetInputFile() const
{
	return m_inputFile;
}

bool wxConfiguration::HasCuePointsFile() const
{
	return m_cuePointsFile.IsOk();
}

const wxFileName& wxConfiguration::GetCuePointsFile() const
{
	wxASSERT( HasCuePointsFile() );
	return m_cuePointsFile;
}

bool wxConfiguration::GenerateCuePoints() const
{
	return m_bGenerateCuePoints;
}

bool wxConfiguration::GenerateCuePoints( const wxTimeSpan& duration, wxTimeSpanArray& cuePoints ) const
{
	wxASSERT( GenerateCuePoints() );

	wxTimeSpan step;
	m_interval.Get( duration, step );
	wxTimeSpan pos		= step;
	wxUint32   nCounter = 0;

	while ( pos < duration )
	{
		cuePoints.Add( pos );
		pos		 += step;
		nCounter += 1U;
	}

	if ( nCounter == 0 )
	{
		wxLogWarning( _( "No cue points generated. Propably too big interval." ) );
	}

	cuePoints.Sort( time_span_compare_fn );

	return ( nCounter > 0 );
}

wxFileName wxConfiguration::GetOutputFile() const
{
	if ( m_outputFile.IsOk() )
	{
		return m_outputFile;
	}
	else
	{
		if ( m_inputFile.IsOk() )
		{
			wxFileName fn( m_inputFile );
			switch ( m_eDrawingMode )
			{
				case DRAWING_MODE_AUDIO:
				{
					fn.SetExt( "wav" );
					break;
				}

				default:
				{
					fn.SetExt( "png" );
					break;
				}
			}

			return fn;
		}
		else
		{
			return wxFileName();
		}
	}
}

const wxSize& wxConfiguration::GetImageSize() const
{
	return m_imageSize;
}

const DrawerSettings& wxConfiguration::GetDrawerSettings() const
{
	return m_drawerSettings;
}

bool wxConfiguration::ParseColourString( const wxString& s, wxColour& clr )
{
	if ( s.CmpNoCase( "transparent" ) == 0 )
	{
		clr = wxTransparentColour;
		return true;
	}
	else
	{
		clr = wxColour( s );
		return clr.IsOk();
	}
}

wxImageResolution wxConfiguration::GetImageResolutionUnits() const
{
	return m_imageResolutionUnits;
}

const wxSize& wxConfiguration::GetImageResolution() const
{
	return m_imageResolution;
}

wxUint16 wxConfiguration::GetImageQuality() const
{
	return m_nImageQuality;
}

int wxConfiguration::GetImageColorDepth() const
{
	return m_nImageColorDepth;
}

bool wxConfiguration::MultiChannel() const
{
	return m_bMultiChannel;
}

wxUint16 wxConfiguration::GetNumberOfColumns() const
{
	return m_nColumnNumber;
}

void wxConfiguration::add_margin( wxRect2DInt& rc ) const
{
	if ( rc.m_width > ( 2 * m_margins.x ) )
	{
		rc.m_x	   += m_margins.x;
		rc.m_width -= 2 * m_margins.x;
	}
	else
	{
		wxLogWarning( _( "Horizontal margin too big" ) );
	}

	if ( rc.m_height > ( 2 * m_margins.y ) )
	{
		rc.m_y		+= m_margins.y;
		rc.m_height -= 2 * m_margins.y;
	}
	else
	{
		wxLogWarning( _( "Vertical margin too big" ) );
	}
}

wxRect2DInt wxConfiguration::GetDrawerRect() const
{
	wxASSERT( !m_bMultiChannel );

	wxRect2DInt rc( 0, 0, m_imageSize.GetWidth(), m_imageSize.GetHeight() );
	add_margin( rc );
	return rc;
}

wxRect2DInt wxConfiguration::GetDrawerRect( wxUint16 nChannel, wxUint16 nChannels ) const
{
	wxASSERT( m_bMultiChannel );

	wxUint16 nRow	 = nChannel / m_nColumnNumber;
	wxUint16 nColumn = nChannel % m_nColumnNumber;

	wxUint16 nRowsNumber = nChannels / m_nColumnNumber;
	nRowsNumber += ( ( nChannels % m_nColumnNumber ) == 0 ) ? 0 : 1;

	wxUint32 nWidth	 = m_imageSize.GetWidth() / m_nColumnNumber;
	wxUint32 nHeight = m_imageSize.GetWidth() / nRowsNumber;

	wxRect2DInt rc( nColumn * m_imageSize.GetWidth() / m_nColumnNumber, nRow * m_imageSize.GetWidth() / nRowsNumber, nWidth, nHeight );
	add_margin( rc );
	return rc;
}

bool wxConfiguration::PowerMix() const
{
	return m_bPowerMix;
}

DRAWING_MODE wxConfiguration::GetDrawingMode() const
{
	return m_eDrawingMode;
}

bool wxConfiguration::UseMLang() const
{
	return m_bUseMLang;
}

