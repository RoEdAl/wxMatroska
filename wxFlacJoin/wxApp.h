/*
	wxApp.h
*/

#ifndef _WX_MY_APP_H_
#define _WX_MY_APP_H_

class wxMyApp :public wxAppConsole
{
protected:

	static bool CheckLicense();
	static void ShowLicense();

protected:

	virtual bool OnInit();
	//virtual void OnInitCmdLine(	wxCmdLineParser& );
	virtual bool OnCmdLineParsed( wxCmdLineParser& );
	virtual int OnRun();
	virtual int OnExit();

public:

	static const wxChar APP_VERSION[];
	static const wxChar APP_AUTHOR[];
	static const wxChar LICENSE_FILE_NAME[];

	wxMyApp(void);
	virtual ~wxMyApp(void);
};

wxDECLARE_APP(wxMyApp);

#endif
