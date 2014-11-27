/*
 * wxApp.h
 */

#ifndef _WX_MY_APP_H_
#define _WX_MY_APP_H_

#ifndef _MY_APP_CONSOLE_H
#include <wxConsoleApp/MyAppConsole.h>
#endif

#ifndef _WX_INPUT_FILE_H_
class wxInputFile;
#endif

#ifndef _WX_CONFIGURATION_H_
#include "wxConfiguration.h"
#endif

#ifndef _WX_CUE_SHEET_H_
#include <wxCueFile/wxCueSheet.h>
#endif

#ifndef _WX_XML_CUE_SHEET_RENDERER_H_
class wxXmlCueSheetRenderer;
#endif;

#ifndef _WX_MKVMERGE_OPTS_RENDERER_H_
class wxMkvmergeOptsRenderer;
#endif

class wxMyApp:
	public MyAppConsole
{
	protected:

		wxConfiguration						  m_cfg;
		wxScopedPtr< wxMkvmergeOptsRenderer > m_pMkvmergeOptsRenderer;
		wxScopedPtr< wxCueSheet >			  m_pMergedCueSheet;

	protected:

		bool ShowInfo() const;

		int ProcessCueFile( const wxInputFile&, const wxTagSynonimsCollection&, const wxTagSynonimsCollection& );
		int ConvertCueSheet( const wxInputFile&, wxCueSheet& );
		int AppendCueSheet( wxCueSheet& );

		static void InfoVersion( wxMessageOutput& );
		static void InfoUsage( wxMessageOutput& );
		static void InfoFormatDescription( wxMessageOutput& );

		wxSharedPtr< wxXmlCueSheetRenderer > GetXmlRenderer( const wxInputFile& );
		wxMkvmergeOptsRenderer& GetMkvmergeOptsRenderer( bool = true );
		wxCueSheet&				GetMergedCueSheet();

		bool HasMkvmergeOptsRenderer() const;
		bool HasMergedCueSheet() const;

		bool RunMkvmerge( const wxFileName& );

	protected:

		virtual bool OnInit();
		virtual void OnInitCmdLine( wxCmdLineParser& );
		virtual bool OnCmdLineParsed( wxCmdLineParser& );
		virtual int OnRun();
		virtual int OnExit();

	public:

		static const char APP_NAME[];
		static const char APP_VERSION[];

	public:

		wxMyApp( void );
};

wxDECLARE_APP( wxMyApp );
#endif

