/*
 *      MyConfiguration.cpp
 */
#include "StdWx.h"
#include <wxConsoleApp/MyConfiguration.h>

MyConfiguration::MyConfiguration( void )
{}

wxString MyConfiguration::BoolToStr( bool b )
{
	return b ? "yes" : "no";
}

wxString MyConfiguration::ToString( bool b )
{
	return b ? _( "on" ) : _( "off" );
}

bool MyConfiguration::ReadNegatableSwitchValue( const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal )
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

bool MyConfiguration::ReadNegatableSwitchValueAndNegate( const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal )
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

