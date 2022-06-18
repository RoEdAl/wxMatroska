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

#ifndef _WX_TEMPORARY_FILES_PROVIDER_H_
class wxTemporaryFilesCleaner;
#endif

#ifndef _WX_XML_CUE_SHEET_RENDERER_H_
class wxXmlCueSheetRenderer;
#endif;

#ifndef _WX_MKVMERGE_OPTS_RENDERER_H_
class wxMkvmergeOptsRenderer;
#endif

#ifndef _WX_CMD_TOOL_H_
#include <wxCmdTools/wxCmdTool.h>
#endif

class wxMyApp:
    public MyAppConsole
{
    protected:

    bool m_calcRg2Loudness;
    wxDouble m_rg2Value;

    wxDateTime m_dt;
    wxConfiguration m_cfg;
    wxScopedPtr< wxCueSheet > m_pMergedCueSheet;

    protected:

    bool ShowInfo() const;

    int ProcessCueFile(const wxInputFile&, const wxTagSynonimsCollection&, const wxTagSynonimsCollection&);
    int ConvertCueSheet(const wxInputFile&, wxCueSheet&);
    int AppendCueSheet(wxCueSheet&);

    static void InfoVersion(wxMessageOutput&);
    static void InfoUsage(wxMessageOutput&);
    void InfoTools(wxMessageOutput&) const;
    static void InfoAsciiToUnicode(wxMessageOutput&);
    static void InfoFormatDescription(wxMessageOutput&);

    wxXmlCueSheetRenderer* GetXmlRenderer(const wxInputFile&) const;
    wxXmlCueSheetRenderer* GetXmlRenderer(const wxInputFile&, const wxString&) const;
    wxCueSheet& GetMergedCueSheet();
    const wxCueSheet& GetMergedCueSheet() const;

    bool HasMergedCueSheet() const;

    void InfoTool(wxMessageOutput&, wxCmdTool::TOOL) const;
    void GetCmd(const wxFileName&, const wxString&, wxString&, wxString&) const;
    void GetCmd(const wxFileName&, const wxArrayString&, wxString&, wxString&) const;
    bool RunMkvmerge(const wxFileName&);
    bool RunCMakeScript(const wxFileName&);
    bool PreProcessAudio(const wxInputFile&, const wxString&, bool, bool, wxCueSheet&, wxFileName&, wxTemporaryFilesCleaner&) const;
    bool RunReplayGainScanner(const wxFileName&) const;
    bool ApplyTagsFromJson(wxCueSheet&, const wxFileName&) const;
    void ApplyTagsFromJson(wxCueSheet&, const wxJson&) const;

    bool PrepareExecuteEnv(wxExecuteEnv&) const;

    protected:

    virtual bool OnInit();
    virtual void OnInitCmdLine(wxCmdLineParser&);
    virtual bool OnCmdLineParsed(wxCmdLineParser&);
    virtual int OnRun();
    virtual int OnExit();

    public:

    static const char APP_NAME[];
    static const char APP_VERSION[];

    public:

    wxMyApp(void);
    const wxDateTime& GetNow() const;
};

wxDECLARE_APP(wxMyApp);
#endif

