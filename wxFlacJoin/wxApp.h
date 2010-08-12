/*
	wxApp.h
*/

#ifndef _WX_MY_APP_H_
#define _WX_MY_APP_H_

#ifndef _WX_CONFIGURATION_H_
class wxConfiguration;
#endif

class wxMyApp :public wxAppConsole
{
protected:

	wxString m_sSeparator;
	wxConfiguration m_cfg;

protected:

	static bool CheckLicense();
	static void ShowLicense( bool = false );
	void AddSeparator( wxCmdLineParser& );
	static void AddVersionInfos( wxCmdLineParser& );

protected:

	virtual bool OnInit();
	virtual void OnInitCmdLine(	wxCmdLineParser& );
	virtual bool OnCmdLineParsed( wxCmdLineParser& );
	virtual int OnRun();
	virtual int OnExit();

public:

	static const wxChar APP_VERSION[];
	static const wxChar APP_AUTHOR[];
	static const wxChar LICENSE_FILE_NAME[];
	static const wxChar FLAC_LICENSE_FILE_NAME[];

	wxMyApp(void);
	virtual ~wxMyApp(void);

	const wxConfiguration& GetConfiguration() const;
};

wxDECLARE_APP(wxMyApp);

#endif
