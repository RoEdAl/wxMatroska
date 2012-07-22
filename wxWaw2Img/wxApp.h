/*
   wxApp.h
 */

#ifndef _WX_MY_APP_H_
#define _WX_MY_APP_H_

#ifndef _WX_CONFIGURATION_H_
#include "wxConfiguration.h"
#endif

class wxMyApp:
	public wxAppConsole
{
protected:

	wxString m_sSeparator;
	wxConfiguration m_cfg;
	wxScopedPtr<wxColourDatabase> m_pColourDatabase;

protected:

	void AddSeparator( wxCmdLineParser& );
	static void AddVersionInfos( wxCmdLineParser& );
	static void AddColourFormatDescription( wxCmdLineParser& );
	static void AddDisplayDescription( wxCmdLineParser& );
	static void InitImageHandlers();

protected:

	virtual bool OnInit();
	virtual void OnInitCmdLine( wxCmdLineParser& );
	virtual bool OnCmdLineParsed( wxCmdLineParser& );
	virtual int OnRun();
	virtual int OnExit();

public:

	static const wxChar APP_NAME[];
	static const wxChar APP_VENDOR_NAME[];
	static const wxChar APP_VERSION[];
	static const wxChar APP_AUTHOR[];

public:

	wxMyApp( void );
};

wxDECLARE_APP( wxMyApp );

#endif

