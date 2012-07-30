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

const wxConfiguration::COMPOSITION_MODE_DESC wxConfiguration::CompositionModeDesc[] =
{
	{ wxCOMPOSITION_CLEAR, wxT( "clear" ) },
	{ wxCOMPOSITION_SOURCE, wxT( "source" ) },
	{ wxCOMPOSITION_OVER, wxT( "over" ) },
	{ wxCOMPOSITION_IN, wxT( "in" ) },
	{ wxCOMPOSITION_OUT, wxT( "out" ) },
	{ wxCOMPOSITION_ATOP, wxT( "atop" ) },
	{ wxCOMPOSITION_DEST, wxT( "dest" ) },
	{ wxCOMPOSITION_DEST_OVER, wxT( "dest_over" ) },
	{ wxCOMPOSITION_DEST_IN, wxT( "dest_in" ) },
	{ wxCOMPOSITION_DEST_OUT, wxT( "dest_out" ) },
	{ wxCOMPOSITION_DEST_ATOP, wxT( "dest_atop" ) },
	{ wxCOMPOSITION_XOR, wxT( "xor" ) },
	{ wxCOMPOSITION_ADD, wxT( "and" ) }
};

const size_t wxConfiguration::CompositionModeDescSize = sizeof ( wxConfiguration::CompositionModeDesc ) / sizeof ( wxConfiguration::COMPOSITION_MODE_DESC );

// ===============================================================================

wxConfiguration::wxConfiguration( void ):
	m_eDrawingMode( DRAWING_MODE_POLY ),
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
	cmdLine.AddParam( _( "Input audio file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "o", "output", _( "Output file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "m", "drawing-mode", wxString::Format( _( "Drawing mode [%s] (default: %s)" ), GetDrawingModeTexts(), GetDrawingModeAsText() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( "s", "take-display-properties", _( "Get image size and color depth from display properties (default: on)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "w", "width", wxString::Format( _( "Image width in pixels (default: %u)" ), m_imageSize.GetWidth() ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "y", "height", wxString::Format( _( "Image height in pixels (default: %u)" ), m_imageSize.GetHeight() ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( "g", "gradient", wxString::Format( _( "Draw with gradient (default: %s)" ), GetSwitchAsText( m_drawerSettings.DrawWithGradient() ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "bl", "baseline-position", wxString::Format( _( "Position of baseline (default %d%%)" ), m_drawerSettings.GetBaselinePositionPercent() ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( "c", "colour", wxString::Format( _( "Middle colour (default: %s)" ), m_drawerSettings.GetTopColourSettings().GetMiddleColour().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "c2", "edge-colour", wxString::Format( _( "Top and bottom colour to use when drawing gradient (default: %s)" ), m_drawerSettings.GetTopColourSettings().GetEdgeColour().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "ct", "top-middle-colour", wxString::Format( _( "Top middle colour (default: %s)" ), m_drawerSettings.GetTopColourSettings().GetMiddleColour().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "cb", "bottom-middle-colour", wxString::Format( _( "Bottom middle colour (default: %s)" ), m_drawerSettings.GetBottomColourSettings().GetMiddleColour().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "ct2", "top-edge-colour", wxString::Format( _( "Top edge colour to to use when drawing gradient (default: %s)" ), m_drawerSettings.GetTopColourSettings().GetEdgeColour().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "cb2", "bottom-edge-colour", wxString::Format( _( "Bottom edge colour to use when drawing gradient (default: %s)" ), m_drawerSettings.GetBottomColourSettings().GetEdgeColour().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( "cm", "calculate-middle-colour", _( "Calculate middle color from top and bottom colours (default: off)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	cmdLine.AddOption( "b", "background", _( "Background color (default: transparent or white)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "b2", "secondary-background", wxString::Format( _( "Secondary background color (default: %s)" ), m_drawerSettings.GetTopColourSettings().GetBackgroundColour2().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "bt", "top-background", wxString::Format( _( "Background color at top (default: %s)" ), m_drawerSettings.GetTopColourSettings().GetBackgroundColour().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "bb", "bottom-background", wxString::Format( _( "Background color at bottom (default: %s)" ), m_drawerSettings.GetBottomColourSettings().GetBackgroundColour().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "bt2", "secondary-top-background", wxString::Format( _( "Secondary background color at top (default: %s)" ), m_drawerSettings.GetTopColourSettings().GetBackgroundColour2().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "bb2", "secondary-bottom-background", wxString::Format( _( "Secondary background color at bottom (default: %s)" ), m_drawerSettings.GetBottomColourSettings().GetBackgroundColour2().GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "bp", "picture-background", _( "Picture background color (default: transparent or white)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
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
	cmdLine.AddSwitch( "mp", "power-mix", wxString::Format( _( "Power mixing of multichannel audio source (default: %s)" ), GetSwitchAsText( m_bPowerMix ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "cn", "number-of-columns", wxString::Format( _( "Number of columns in nultichannel mode (default %u)" ), m_nColumnNumber ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( "ma", "margin", wxString::Format( _( "Margin (horizontal and vertical) (default %u)" ), m_margins.x ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "mx", "margin-x", wxString::Format( _( "Horizontal margin (default %u)" ), m_margins.x ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "my", "margin-y", wxString::Format( _( "Vertical margin (default %u)" ), m_margins.y ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( "f", "frequency", wxString::Format( _( "Frequency (in Hz) of rendered audio file (default %u)" ), m_drawerSettings.GetFrequency() ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( "cp", "cue-point-file", _( "Cue point file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( "cg", "generate-cue-points", wxString::Format( _( "Multi channel mode (default: %s)" ), GetSwitchAsText( m_bGenerateCuePoints ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "ci", "cue-points-interval", wxString::Format( _( "Cue points interval (default: %s)" ), m_interval.AsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( "cs", "cue-strippes", wxString::Format( _( "Draw segments or lines (default: %s)" ), GetSwitchAsText( m_drawerSettings.GetDrawCueStrippes() ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	cmdLine.AddSwitch( wxEmptyString, "use-mlang", wxString::Format( _( "Use MLang library (default: %s)" ), GetSwitchAsText( m_bUseMLang ) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	cmdLine.AddOption( "dm", "composition-mode", wxString::Format( _( "Composition mode [%s] (default: %s)" ), GetCompositionModeTexts(), GetCompositionModeAsText() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
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

bool wxConfiguration::ConvertStringToCompositionMode( const wxString& s, wxCompositionMode& e )
{
	for ( size_t i = 0; i < CompositionModeDescSize; i++ )
	{
		if ( s.CmpNoCase( CompositionModeDesc[ i ].description ) == 0 )
		{
			e = CompositionModeDesc[ i ].compositionMode;
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

wxString wxConfiguration::GetCompositionModeAsText( wxCompositionMode e )
{
	for ( size_t i = 0; i < CompositionModeDescSize; i++ )
	{
		if ( CompositionModeDesc[ i ].compositionMode == e )
		{
			return CompositionModeDesc[ i ].description;
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

wxString wxConfiguration::GetCompositionModeTexts()
{
	wxString s;

	for ( size_t i = 0; i < CompositionModeDescSize; i++ )
	{
		s << CompositionModeDesc[ i ].description << "|";
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
			wxLogWarning( _( "Invalid drawing mode - %s" ), s );
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
		wxColour clr;

		if ( ParseColourString( s, clr ) )
		{
			m_drawerSettings.SetMiddleColour( clr );
		}
		else
		{
			wxLogWarning( _( "Invalid color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "c2", &s ) )
	{
		wxColour clr;

		if ( ParseColourString( s, clr ) )
		{
			m_drawerSettings.SetEdgeColour( clr );
		}
		else
		{
			wxLogWarning( _( "Invalid middle-top and middle-bottom color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "ct", &s ) )
	{
		if ( !ParseColourString( s, m_drawerSettings.GetTopColourSettings().GetMiddleColour() ) )
		{
			wxLogWarning( _( "Invalid top-middle color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "cb", &s ) )
	{
		if ( !ParseColourString( s, m_drawerSettings.GetBottomColourSettings().GetMiddleColour() ) )
		{
			wxLogWarning( _( "Invalid bottom-middle color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "ct2", &s ) )
	{
		if ( !ParseColourString( s, m_drawerSettings.GetTopColourSettings().GetEdgeColour() ) )
		{
			wxLogWarning( _( "Invalid top-edge color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "cb2", &s ) )
	{
		if ( !ParseColourString( s, m_drawerSettings.GetBottomColourSettings().GetEdgeColour() ) )
		{
			wxLogWarning( _( "Invalid bottom-edge color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "bl", &v ) )
	{
		if ( v <= 0 || v > 100 )
		{
			wxLogWarning( _( "Invalid baseline position - %d" ), v );
			return false;
		}

		m_drawerSettings.SetBaselinePositionPercent( v );
	}

	if ( ReadNegatableSwitchValue( cmdLine, "cm", bRes ) && bRes )
	{
		const wxColour& clr = m_drawerSettings.CalcMiddleColour();
		wxLogInfo( _( "Center color calculated to %s" ), clr.GetAsString() );
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
		m_nImageColorDepth = wxGetDisplayDepth();
	}
	m_drawerSettings.SetBackgroundColour( get_default_bg_color( m_nImageColorDepth ) );

	if ( ReadNegatableSwitchValue( cmdLine, "bs", bRes ) && bRes )
	{
		m_drawerSettings.SetBackgroundColour( wxSystemSettings::GetColour( COLOR_BACKGROUND ) );
		m_drawerSettings.SetBackgroundColour2( wxSystemSettings::GetColour( COLOR_BACKGROUND2 ) );
		wxLogInfo( _( "Background color: %s" ), m_drawerSettings.GetTopColourSettings().GetBackgroundColour().GetAsString() );
		wxLogInfo( _( "Secondary background color: %s" ), m_drawerSettings.GetTopColourSettings().GetBackgroundColour2().GetAsString() );
	}

	if ( cmdLine.Found( "b", &s ) )
	{
		wxColour clr;

		if ( ParseColourString( s, clr ) )
		{
			m_drawerSettings.SetBackgroundColour( clr );
		}
		else
		{
			wxLogWarning( _( "Invalid background color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "bt", &s ) )
	{
		if ( !ParseColourString( s, m_drawerSettings.GetTopColourSettings().GetBackgroundColour() ) )
		{
			wxLogWarning( _( "Invalid top background color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "bb", &s ) )
	{
		if ( !ParseColourString( s, m_drawerSettings.GetBottomColourSettings().GetBackgroundColour() ) )
		{
			wxLogWarning( _( "Invalid bottom background color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "b2", &s ) )
	{
		wxColour clr;

		if ( ParseColourString( s, clr ) )
		{
			m_drawerSettings.SetBackgroundColour2( clr );
		}
		else
		{
			wxLogWarning( _( "Invalid second background color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "bt2", &s ) )
	{
		if ( !ParseColourString( s, m_drawerSettings.GetTopColourSettings().GetBackgroundColour2() ) )
		{
			wxLogWarning( _( "Invalid second top background color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "bb2", &s ) )
	{
		if ( !ParseColourString( s, m_drawerSettings.GetBottomColourSettings().GetBackgroundColour2() ) )
		{
			wxLogWarning( _( "Invalid second bottom background color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "bp", &s ) )
	{
		if ( !ParseColourString( s, m_drawerSettings.GetBackgroundColour() ) )
		{
			wxLogWarning( _( "Invalid picture background color - %s" ), s );
			return false;
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

	ReadNegatableSwitchValue( cmdLine, "cs", m_drawerSettings.GetDrawCueStrippes() );

	ReadNegatableSwitchValue( cmdLine, "use-mlang", m_bUseMLang );

	if ( cmdLine.Found( "dm", &s ) )
	{
		if ( !ConvertStringToCompositionMode( s, m_drawerSettings.GetCompositionMode() ) )
		{
			wxLogWarning( _( "Invalid composition mode - %s" ), s );
			return false;
		}
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
	wxFileName fn( GetOutputFile() );

	if ( fn.GetExt().CmpNoCase( "emf" ) == 0 )
	{
		return -1;
	}
	else
	{
		return m_nImageColorDepth;
	}
}

bool wxConfiguration::MultiChannel() const
{
	return m_bMultiChannel;
}

wxUint16 wxConfiguration::GetNumberOfColumns() const
{
	return m_nColumnNumber;
}

void wxConfiguration::add_margin( wxRect2DInt& rc, bool bTop, bool bRight, bool bBottom, bool bLeft ) const
{
	wxPoint2DInt left_top( rc.GetLeftTop() );
	wxPoint2DInt right_bottom( rc.GetRightBottom() );

	wxSize margins( m_margins );

	margins.Scale( 0.5f, 0.5f );

	left_top.m_x += ( bLeft ? 2 : 1 ) * margins.x;
	left_top.m_y += ( bTop ? 2 : 1 ) * margins.y;

	right_bottom.m_x -= ( bRight ? 2 : 1 ) * margins.x;
	right_bottom.m_y -= ( bBottom ? 2 : 1 ) * margins.y;

	wxRect2DInt rcM( left_top, right_bottom );

	if ( rcM.IsEmpty() )
	{
		wxLogWarning( _( "Cannot set margins" ) );
	}
	else
	{
		rc = rcM;
	}
}

wxRect2DInt wxConfiguration::GetDrawerRect() const
{
	wxASSERT( !m_bMultiChannel );

	wxRect2DInt rc( 0, 0, m_imageSize.GetWidth(), m_imageSize.GetHeight() );
	add_margin( rc, true, true, true, true );
	return rc;
}

void wxConfiguration::GetDrawerRects( wxUint16 nChannels, wxRect2DIntArray& drawerRects ) const
{
	wxASSERT( m_bMultiChannel );
	drawerRects.Clear();

	wxUint16 nRowsNumber = nChannels / m_nColumnNumber;
	nRowsNumber += ( ( nChannels % m_nColumnNumber ) == 0 ) ? 0 : 1;

	wxUint16 nRowsNumber1	= nRowsNumber - 1U;
	wxUint16 nColumnNumber1 = m_nColumnNumber - 1U;

	wxUint32 nWidth	 = m_imageSize.GetWidth() / m_nColumnNumber;
	wxUint32 nHeight = m_imageSize.GetHeight() / nRowsNumber;

	for ( wxUint16 nChannel = 0; nChannel < nChannels; nChannel++ )
	{
		wxUint16 nRow	 = nChannel / m_nColumnNumber;
		wxUint16 nColumn = nChannel % m_nColumnNumber;

		wxRect2DInt rc( nColumn * m_imageSize.GetWidth() / m_nColumnNumber, nRow * m_imageSize.GetHeight() / nRowsNumber, nWidth, nHeight );
		add_margin( rc, ( nRow == 0 ), ( nColumn == nColumnNumber1 ), ( nRow == nRowsNumber1 ), ( nColumn == 0 ) );
		drawerRects.Add( rc );
	}
}

wxRegion wxConfiguration::GetDrawersRegion( wxUint16 nChannels ) const
{
	if ( m_bMultiChannel )
	{
		wxRect2DIntArray drawerRects;
		GetDrawerRects( nChannels, drawerRects );

		wxRegion rgn;
		for ( size_t i = 0, nCount = drawerRects.GetCount(); i < nCount; i++ )
		{
			const wxRect2DInt& rc = drawerRects[ i ];
			rgn.Union( rc.m_x, rc.m_y, rc.m_width, rc.m_height );
		}

		return rgn;
	}
	else
	{
		wxRect2DInt rc( GetDrawerRect() );
		wxRegion	rgn( rc.m_x, rc.m_y, rc.m_width, rc.m_height );
		return rgn;
	}
}

bool wxConfiguration::PowerMix() const
{
	return m_bPowerMix;
}

DRAWING_MODE wxConfiguration::GetDrawingMode() const
{
	return m_eDrawingMode;
}

wxString wxConfiguration::GetDrawingModeAsText() const
{
	return GetDrawingModeAsText( m_eDrawingMode );
}

bool wxConfiguration::UseMLang() const
{
	return m_bUseMLang;
}

wxString wxConfiguration::GetCompositionModeAsText() const
{
	return GetCompositionModeAsText( m_drawerSettings.GetCompositionMode() );
}
