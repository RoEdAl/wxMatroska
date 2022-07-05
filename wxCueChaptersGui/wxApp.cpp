/*
 * wxApp.cpp
 */

#include <app_config.h>
#include <wxCmdTools/wxCmdTool.h>
#include "wxApp.h"
#include "wxMainFrame.h"

 // ===============================================================================

const char wxMyApp::APP_NAME[] = "cue2mkc-frontend";
const char wxMyApp::APP_VERSION[] = WXMATROSKA_VERSION_STR;
const char wxMyApp::APP_VENDOR_NAME[] = "Edmunt Pienkowsky";
const char wxMyApp::APP_AUTHOR[] = "Edmunt Pienkowsky - roed@onet.eu";
const char wxMyApp::LICENSE_FILE_NAME[] = "license.txt";

// ===============================================================================

wxIMPLEMENT_APP(wxMyApp);

wxMyApp::wxMyApp(void)
{
}

void wxMyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxApp::OnInitCmdLine(parser);

    parser.AddLongSwitch("log-timestamps", _("Show/hide log timestamps"), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
}

bool wxMyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    wxASSERT(parser.AreLongOptionsEnabled());

    const bool res = wxApp::OnCmdLineParsed(parser);

    if (res)
    {
        const wxCmdLineSwitchState state = parser.FoundSwitch("verbose");
        switch (state)
        {
            case wxCMD_SWITCH_ON:
            {
                // wxLog::SetLogLevel(wxLOG_Info);
                wxLog::SetVerbose(true);
                break;
            }

            default:
            {
                // wxLog::SetLogLevel(wxLOG_Message);
                wxLog::SetVerbose(false);
                break;
            }
        }

        if (parser.FoundSwitch("log-timestamps") == wxCMD_SWITCH_OFF) wxLog::DisableTimestamp();
    }

    return res;
}

bool wxMyApp::OnInit()
{
    SetAppName(APP_NAME);
    SetVendorName(APP_VENDOR_NAME);
    SetVendorDisplayName(APP_AUTHOR);

    if (!wxApp::OnInit()) return false;

    wxLog::EnableLogging(false);
#ifdef NDEBUG
    wxDisableAsserts();
#endif

    // cue2mkc tool

    m_cue2MkcPath.Assign(wxStandardPaths::Get().GetExecutablePath());
    m_cue2MkcPath.SetName("cue2mkc");

    if (!m_cue2MkcPath.IsFileExecutable())
    {
    #ifdef NDEBUG
        wxLogDebug(_("Using cue2mkc tool from current directory"));

        const wxString fullName(m_cue2MkcPath.GetFullName());
        m_cue2MkcPath.AssignCwd();
        m_cue2MkcPath.SetFullName(fullName);
    #else
        const wxArrayString& dirs = m_cue2MkcPath.GetDirs();
        const wxString       lastDir(dirs.Last());
        m_cue2MkcPath.RemoveLastDir();
        m_cue2MkcPath.RemoveLastDir();
        m_cue2MkcPath.AppendDir("wxCueChapters");
        m_cue2MkcPath.AppendDir(lastDir);
    #endif
    }

    if (!wxCmdTool::FindTool(wxCmdTool::TOOL_MKVMERGE, m_mkvmergePath))
    {
        wxLogWarning(_("mkvmerge tool not found."));
    }

    if (!wxCmdTool::FindTool(wxCmdTool::TOOL_MKVPROPEDIT, m_mkvPropEditPath))
    {
        wxLogWarning(_("mkvpropedit tool not found."));
    }

    if (!wxCmdTool::FindTool(wxCmdTool::TOOL_FFMPEG, m_ffmpegPath))
    {
        wxLogWarning(_("ffmpeg tool not found."));
    }

    if (!wxCmdTool::FindTool(wxCmdTool::TOOL_FFPROBE, m_ffprobePath))
    {
        wxLogWarning(_("ffprobe tool not found."));
    }

    wxFrame* const pFrame = new wxMainFrame();
    pFrame->Show(true);

    return true;
}

const wxFileName& wxMyApp::GetCue2MkcPath() const
{
    return m_cue2MkcPath;
}

const wxFileName& wxMyApp::GetMkvPropEditPath() const
{
    return m_mkvPropEditPath;
}

const wxFileName& wxMyApp::GetMkvmergePath() const
{
    return m_mkvmergePath;
}

const wxFileName& wxMyApp::GetFfmpegPath() const
{
    return m_ffmpegPath;
}

const wxFileName& wxMyApp::GetFfprobePath() const
{
    return m_ffprobePath;
}

namespace
{
    void show_tool_path(const wxFileName& tool)
    {
        if (tool.IsOk() && tool.IsAbsolute())
        {
            wxLogMessage(_("[tool] %-15s: %s"), tool.GetName(), tool.GetFullPath());
        }
        else
        {
            wxLogMessage(_("[tool] %-15s: <not found>"), tool.GetName());
        }
    }
}

void wxMyApp::ShowToolPaths() const
{
    show_tool_path(m_cue2MkcPath);
    show_tool_path(m_mkvmergePath);
    show_tool_path(m_mkvPropEditPath);
    show_tool_path(m_ffmpegPath);
    show_tool_path(m_ffprobePath);
}