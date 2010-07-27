/*
	wxApp.h
*/

#ifndef _WX_MY_APP_H_
#define _WX_MY_APP_H_

#ifndef _WX_CONFIGURATION_H_
class wxConfiguration;
#endif

#ifndef _WX_CUE_SHEET_READER_H_
class wxCueSheetReader;
#endif

#ifndef _WX_CUE_SHEET_H_
class wxCueSheet;
#endif

class wxMyApp :public wxAppConsole
{
protected:

	wxConfiguration m_cfg;
	wxString m_sSeparator;

protected:

	int ProcessCueFile( wxCueSheetReader&, const wxString& );
	int ConvertCueSheet( const wxString&, const wxCueSheet& );

	void AddSeparator( wxCmdLineParser& );
	static void AddVersionInfos( wxCmdLineParser& );
	static void AddFormatDescription( wxCmdLineParser& );

protected:

	virtual bool OnInit();
	virtual void OnInitCmdLine(	wxCmdLineParser& );
	virtual bool OnCmdLineParsed( wxCmdLineParser& );
	virtual int OnRun();
	virtual int OnExit();

public:

	static const wxChar APP_VERSION[];
	static const wxChar APP_AUTHOR[];

	wxMyApp(void);
	virtual ~wxMyApp(void);
};

wxDECLARE_APP(wxMyApp);

#endif
