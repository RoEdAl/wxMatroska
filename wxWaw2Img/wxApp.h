/*
 * wxApp.h
 */

#ifndef _WX_MY_APP_H_
#define _WX_MY_APP_H_

#ifndef _MY_APP_CONSOLE_H
#include <wxConsoleApp/MyAppConsole.h>
#endif

#ifndef _WX_CONFIGURATION_H_
#include "wxConfiguration.h"
#endif

class wxMyApp:
	public MyAppConsole
{
	protected:

		wxConfiguration					m_cfg;
		wxScopedPtr< wxColourDatabase > m_pColourDatabase;

	protected:


		static void InitImageHandlers();

		static void InfoVersion( wxMessageOutput& );
		static void InfoColourFormat( wxMessageOutput& );
		static void InfoSystemSettings( wxMessageOutput& );
		static void InfoCuePointFormat( wxMessageOutput& );
		static void InfoCmdLineTemplate( wxMessageOutput& );

	protected:

		virtual bool OnInit();
		virtual void OnInitCmdLine( wxCmdLineParser& );
		virtual bool OnCmdLineParsed( wxCmdLineParser& );
		virtual int OnRun();

	public:

		static const wxChar APP_NAME[];
		static const wxChar APP_VERSION[];

		static const wxChar CMD_FFMPEG[];
		static const wxChar CMD_AUDIO[];
		static const wxChar CMD_INPUT[];
		static const wxChar CMD_INPUT_OVERLAY[];
		static const wxChar CMD_INPUT_DURATION[];
		static const wxChar CMD_INPUT_FRAMES[];
		static const wxChar CMD_INPUT_RATE[];
		static const wxChar CMD_OUTPUT[];
		static const wxChar CMD_KEY_FRAMES[];

		static const wxChar BACKGROUND_IMG[];

	public:

		wxMyApp( void );
};

wxDECLARE_APP( wxMyApp );

#endif

