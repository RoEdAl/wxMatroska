/*
 *      MyAppConsole.h
 */

#ifndef _MY_APP_CONSOLE_H
#define _MY_APP_CONSOLE_H

class MyAppConsole:
	public wxAppConsole
{
	public:

		MyAppConsole( void );

		static const char APP_VENDOR_NAME[];
		static const char APP_AUTHOR[];
		static const char LICENSE_FILE_NAME[];

	protected:

		static bool CheckLicense();
		static void ShowLicense( wxMessageOutput& );

	protected:

		virtual bool OnInit();
		virtual wxAppTraits* CreateTraits();
		virtual int OnExit();

		void AddSeparator( wxCmdLineParser& );

	protected:

		wxString m_sSeparator;
};
#endif

