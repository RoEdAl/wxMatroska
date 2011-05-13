/*
   wxApp.h
 */

#ifndef _WX_MY_APP_H_
#define _WX_MY_APP_H_

#ifndef _WX_INPUT_FILE_H_
class wxInputFile;
#endif

#ifndef _WX_CONFIGURATION_H_
#include "wxConfiguration.h"
#endif

#ifndef _WX_CUE_SHEET_H_
#include <wxCueFile/wxCueSheet.h>
#endif

#ifndef _WX_CUE_SHEET_READER_H_
class wxCueSheetReader;
#endif

#ifndef _WX_XML_CUE_SHEET_RENDERER_H_
class wxXmlCueSheetRenderer;
#endif;

#ifndef _WX_MKVMERGE_OPTS_RENDERER_H_
class wxMkvmergeOptsRenderer;
#endif

class wxMyApp:
	public wxAppConsole
{
protected:

	wxConfiguration						m_cfg;
	wxString							m_sSeparator;
	wxScopedPtr<wxMkvmergeOptsRenderer> m_pMkvmergeOptsRenderer;
	wxScopedPtr<wxCueSheet>				m_pMergedCueSheet;

protected:

	int ProcessCueFile( wxCueSheetReader&, const wxInputFile& );
	int ConvertCueSheet( const wxInputFile&, const wxCueSheet& );
	int AppendCueSheet( wxCueSheet& );

	void AddSeparator( wxCmdLineParser& );
	static void AddVersionInfos( wxCmdLineParser& );
	static void AddFormatDescription( wxCmdLineParser& );
	static void AddInputFileFormatDescription( wxCmdLineParser& );

	static bool CheckLicense();
	static void ShowLicense();

	wxSharedPtr<wxXmlCueSheetRenderer> GetXmlRenderer( const wxInputFile& );
	wxMkvmergeOptsRenderer& GetMkvmergeOptsRenderer( bool = true );
	wxCueSheet&				GetMergedCueSheet();

	bool HasMkvmergeOptsRenderer() const;
	bool HasMergedCueSheet() const;

	bool RunMkvmerge( const wxString& );

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
	static const wxChar LICENSE_FILE_NAME[];

	static wxString GetFileName( const wxString& );

public:

	wxMyApp( void );
};

wxDECLARE_APP( wxMyApp );

#endif