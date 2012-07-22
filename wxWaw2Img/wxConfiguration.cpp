/*
   wxConfiguration.cpp
 */

#include "StdWx.h"
#include "wxConfiguration.h"
#include "wxApp.h"

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxConfiguration, wxObject );

// ===============================================================================

const wxConfiguration::DRAWING_MODE_DESC wxConfiguration::DrawingModeDesc[] = {
	{ DRAWING_MODE_SIMPLE, wxT("simple") },
	{ DRAWING_MODE_RASTER1, wxT("raster1") },
	{ DRAWING_MODE_RASTER2, wxT("raster2") },
	{ DRAWING_MODE_POLY, wxT("poly") },
	{ DRAWING_MODE_AUDIO, wxT("audio") },
};

const size_t wxConfiguration::DrawingModeDescSize = sizeof(wxConfiguration::DrawingModeDesc)/sizeof(wxConfiguration::DRAWING_MODE_DESC);

// ===============================================================================

wxConfiguration::wxConfiguration( void ):
	m_eDrawingMode( DRAWING_MODE_RASTER1 ),
	m_nWidth(800),
	m_nHeight(300),
	m_clrFrom( 0,0,0,wxALPHA_OPAQUE  ),
	m_clrTo( 0,0,0, wxALPHA_TRANSPARENT ),
	m_bDrawWithGradient( true ),
	m_clrBackground( wxTransparentColour ),
	m_imageResolutionUnits( wxIMAGE_RESOLUTION_INCHES ),
	m_imageResolution( 150, 150 ),
	m_nImageQuality(75),
	m_nImageColorDepth( wxBITMAP_SCREEN_DEPTH ),
	m_bLogarithmicScale( false ),
	m_bLogarithmicColorGradient( false ),
	m_fLogBase( 100 ),
	m_bPowerMix( true ),
	m_nFrequency(50)
{
}

wxString wxConfiguration::GetSwitchAsText( bool b )
{
	return b? _("on") : _("off");
}

void wxConfiguration::AddCmdLineParams( wxCmdLineParser& cmdLine )
{
	cmdLine.AddParam( _("Input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "o", "output", _( "Output file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "m", "drawing-mode", wxString::Format( _( "Drawing mode. [%s] (default: %s)" ), GetDrawingModeTexts(), GetDrawingModeAsText( m_eDrawingMode ) ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( "s", "take-display-properties", _( "Get image size and color depth from display properties (default: on)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	cmdLine.AddOption( "w", "width", wxString::Format( _( "Image width in pixels (default: %u)" ), m_nWidth ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "y", "height", wxString::Format( _( "Image height in pixels (default: %u)" ), m_nHeight ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( "g", "gradient", wxString::Format( _( "Draw gradient (default: %s)" ), GetSwitchAsText(m_bDrawWithGradient) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	cmdLine.AddOption( "c", "colour", wxString::Format( _( "Colour to draw (default: %s)"), m_clrFrom.GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "c2", "second-colour", wxString::Format( _( "Second colour to draw if drawing gradient (default: %s)" ), m_clrTo.GetAsString() ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( "b", "background", _( "Background color (default: transparent or white)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( "bs", "background-from-system", _( "Take background color from system settings (default: off)" ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );

	cmdLine.AddOption( "r", "resolution", wxString::Format( _( "Image horizontal and vertical resolution (default: %d)" ), m_imageResolution.x ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "rx", "x-resolution", wxString::Format( _( "Image horizontal resolution (default: %d)" ), m_imageResolution.x ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "ry", "y-resolution", wxString::Format( _( "Image vertical resolution (default: %d)" ), m_imageResolution.y ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( wxEmptyString, "resolution-units", _( "Image resolution units [inches|cm] (default: inches)" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddOption( "q", "image-quality", wxString::Format( _( "Image quality [0-100] (default: %d)" ), m_nImageQuality ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( "d", "color-depth", _( "Image color depth [8,16,24,32] (default: display color depth)" ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( "l", "logaritmic-scale", wxString::Format( _( "Draw using logarithmic scale (default: %s)" ), GetSwitchAsText(m_bLogarithmicScale) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddSwitch( "lc", "logarithmic-color-gradient", wxString::Format( _( "Use logarithmic color gradient (default: %s)" ), GetSwitchAsText(m_bLogarithmicColorGradient) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "lb", "logarithm-base", _( "Logarithm base used in logarithmic scale (default: 100)" ), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL );

	cmdLine.AddSwitch( "mp", "power-mix", wxString::Format( _( "Power mixing of multichannel audio source (default: %s)" ), GetSwitchAsText(m_bPowerMix) ), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE );
	cmdLine.AddOption( "f", "frequency", wxString::Format( _( "Frequency (in Hz) of rendered audio file (default %u)" ), m_nFrequency ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
}

bool wxConfiguration::ReadNegatableSwitchValue( const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal )
{
	wxCmdLineSwitchState state = cmdLine.FoundSwitch( name );
	bool				 res   = true;

	switch ( state )
	{
		case wxCMD_SWITCH_ON:
		switchVal = true;
		break;

		case wxCMD_SWITCH_OFF:
		switchVal = false;
		break;

		default:
		res = false;
		break;
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
	int nDepth = wxDisplayDepth();
	wxSize res = wxGetDisplayPPI();

	wxLogInfo( _("Display size in pixels: %dx%d"), dplRect.GetWidth(), dplRect.GetHeight() );
	wxLogInfo( _("Display color depth: %d bits"), nDepth );
	wxLogInfo( _("Display resolution in pixels per inch: %dx%d"), res.x, res.y );

	m_nWidth = dplRect.GetWidth();
	m_nHeight = dplRect.GetHeight();
	m_nImageColorDepth = nDepth;
	m_imageResolution = res;
}

static wxColour get_default_bg_color( int nDepth )
{
	switch( nDepth )
	{
		case 8:
		case 16:
		case 24:
		return wxColour( "white" );

		case 32:
		return wxTransparentColour;

		default:
		wxFAIL_MSG( "Invalid image color depth" );
		return wxColour();
	}
}

bool wxConfiguration::ConvertStringToDrawingMode( const wxString& s, DRAWING_MODE& e )
{
	for( size_t i=0; i < DrawingModeDescSize; i++ )
	{
		if ( s.CmpNoCase( DrawingModeDesc[i].description ) == 0 )
		{
			e = DrawingModeDesc[i].drawingMode;
			return true;
		}
	}

	return false;
}

wxString wxConfiguration::GetDrawingModeAsText( DRAWING_MODE e )
{
	for( size_t i=0; i < DrawingModeDescSize; i++ )
	{
		if ( DrawingModeDesc[i].drawingMode == e )
		{
			return DrawingModeDesc[i].description;
		}
	}

	return wxString::Format( "<%d>", static_cast<int>(e) );
}

wxString wxConfiguration::GetDrawingModeTexts()
{
	wxString s;
	for( size_t i=0; i < DrawingModeDescSize; i++ )
	{
		s << DrawingModeDesc[i].description << "|";
	}

	return s.RemoveLast();
}

bool wxConfiguration::Read( const wxCmdLineParser& cmdLine )
{
	bool	 bRes = true;
	wxString s;
	long	 v;
	wxDouble vd;


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
		m_nWidth = v;
	}

	if ( cmdLine.Found( "y", &v ) )
	{
		if ( v <= 0 || v > 10000 )
		{
			wxLogWarning( _( "Invalid height - %d" ), v );
			return false;
		}

		m_nHeight = v;
	}

	if ( cmdLine.Found( "c", &s ) )
	{
		if ( !ParseColourString( s, m_clrFrom ) )
		{
			wxLogWarning( _( "Invalid color - %s" ), s );
			return false;
		}
	}

	if ( cmdLine.Found( "c2", &s ) )
	{
		if ( !ParseColourString( s, m_clrTo ) )
		{
			wxLogWarning( _( "Invalid second color - %s" ), s );
			return false;
		}
		m_bDrawWithGradient = true;
	}

	ReadNegatableSwitchValue( cmdLine, "g", m_bDrawWithGradient );

	if ( cmdLine.Found( "d", &v ) )
	{
		switch( v )
		{
			case 8:
			case 16:
			case 24:
			case 32:
			m_nImageColorDepth = v;
			break;

			default:
			wxLogWarning( _( "Invalid image color depth - %d" ), v );
			return false;
		}
	}

	if ( m_nImageColorDepth == wxBITMAP_SCREEN_DEPTH )
	{
		m_clrBackground = get_default_bg_color( wxGetDisplayDepth() );
	}
	else
	{
		m_clrBackground = get_default_bg_color( m_nImageColorDepth );
	}

	if ( ReadNegatableSwitchValue( cmdLine, "bs", bRes ) && bRes )
	{
		m_clrBackground = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW  );
		wxLogInfo( _("Background color: %s"), m_clrBackground.GetAsString() );
	}
	else if ( cmdLine.Found( "b", &s ) )
	{
		if ( !ParseColourString( s, m_clrBackground ) )
		{
			wxLogWarning( _( "Invalid background color - %s" ), s );
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

		m_imageResolution = wxSize(v,v);
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

		m_fLogBase = vd;
	}

	ReadNegatableSwitchValue( cmdLine, "l", m_bLogarithmicScale );
	ReadNegatableSwitchValue( cmdLine, "lc", m_bLogarithmicColorGradient );

	ReadNegatableSwitchValue( cmdLine, "mp", m_bPowerMix );

	if ( cmdLine.Found( "f", &v ) )
	{
		if ( v < 10 || v > 20000 )
		{
			wxLogWarning( _( "Invalid audio frequency - %d" ), v );
			return false;
		}

		m_nFrequency = v;
	}

	return true;
}

const wxFileName& wxConfiguration::GetInputFile() const
{
	return m_inputFile;
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
			switch( m_eDrawingMode )
			{
				case DRAWING_MODE_AUDIO:
				fn.SetExt( "wav" );
				break;

				default:
				fn.SetExt( "png" );
				break;
			}

			return fn;
		}
		else
		{
			return wxFileName();
		}
	}
}

wxUint32 wxConfiguration::GetWidth() const
{
	return m_nWidth;
}

wxUint32 wxConfiguration::GetHeight() const
{
	return m_nHeight;
}

const wxColour& wxConfiguration::GetColourFrom() const
{
	return m_clrFrom;
}

const wxColour& wxConfiguration::GetColourTo() const
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

bool wxConfiguration::DrawWithGradient() const
{
	return m_bDrawWithGradient;
}

const wxColour& wxConfiguration::GetBackgroundColor() const
{
	return m_clrBackground;
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

bool wxConfiguration::UseLogarithmicScale() const
{
	return m_bLogarithmicScale;
}

bool wxConfiguration::UseLogarithmicColorPalette() const
{
	return m_bLogarithmicColorGradient;
}

wxFloat32 wxConfiguration::GetLogarithmBase() const
{
	return m_fLogBase;
}

bool wxConfiguration::PowerMix() const
{
	return m_bPowerMix;
}

DRAWING_MODE wxConfiguration::GetDrawingMode() const
{
	return m_eDrawingMode;
}

wxUint16 wxConfiguration::GetFrequency() const
{
	return m_nFrequency;
}
