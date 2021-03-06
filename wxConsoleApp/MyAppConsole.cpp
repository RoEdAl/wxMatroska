/*
 *      MyAppConsole.cpp
 */
#include <wxConsoleApp/MyMessageOutputStderr.h>
#include <wxConsoleApp/MyAppTraits.h>
#include <wxConsoleApp/MyAppConsole.h>
#include <wxConsoleApp/MyConfiguration.h>

 // ================================================================================

const char MyAppConsole::APP_VENDOR_NAME[] = "Edmunt Pienkowsky";
const char MyAppConsole::APP_AUTHOR[] = "Edmunt Pienkowsky - roed@onet.eu";
const char MyAppConsole::LICENSE_FILE_NAME[] = "license.txt";

namespace
{
    const size_t MAX_LICENSE_FILE_SIZE = 4 * 1024;

#if defined(__WXMSW__) && defined(UNICODE)
    bool str_2_translation_mode(const wxString& outputCharset, int& res)
    {
        if (outputCharset.CmpNoCase("utf8") == 0 || outputCharset.CmpNoCase("utf-8") == 0)
        {
            res = _O_U8TEXT;
            return true;
        }

        if (outputCharset.CmpNoCase("utf16") == 0 || outputCharset.CmpNoCase("utf-16") == 0)
        {
            res = _O_U16TEXT;
            return true;
        }

        if (outputCharset.CmpNoCase("text") == 0)
        {
            res = _O_WTEXT;
            return true;
        }

        return false;
    }
#endif
}

// ================================================================================

MyAppConsole::MyAppConsole(void):
    m_sSeparator('=', 75)
#if defined(__WXMSW__) && defined(UNICODE)
    ,m_translationMode(_O_WTEXT)
#endif
{
}

wxDateTime MyAppConsole::GetNow()
{
    wxString sourceDateEpoch;
    if (wxGetEnv("SOURCE_DATE_EPOCH", &sourceDateEpoch))
    {
        wxULongLong_t unixEpoch;
        if (sourceDateEpoch.ToULongLong(&unixEpoch))
        {
            return wxDateTime(static_cast<time_t>(unixEpoch));
        }
    }

    return wxDateTime::Now();
}

void MyAppConsole::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxAppConsole::OnInitCmdLine(parser);

    parser.AddLongSwitch("log-timestamps", _("Show/hide log timestamps"), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE | wxCMD_LINE_HIDDEN);

#if defined(__WXMSW__) && defined(UNICODE)
    parser.AddLongOption("output-charset",
                         _("Sets the character set to which strings are converted that are to be output (default: system's current locale) [utf8, utf16, text]"),
                         wxCMD_LINE_VAL_STRING,
                         wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_HIDDEN);
#endif
}

#if defined(__WXMSW__) && defined(UNICODE)
bool MyAppConsole::OnCmdLineHelp(wxCmdLineParser& parser)
{
    const bool res = ProcessOutputCharset(parser);
    wxASSERT(res);
    return wxAppConsole::OnCmdLineHelp(parser);
}

bool MyAppConsole::ProcessOutputCharset(wxCmdLineParser& parser)
{
    wxString outputCharset;
    if (parser.Found("output-charset", &outputCharset))
    {
        if (str_2_translation_mode(outputCharset, m_translationMode))
        {
            int res = _setmode(_fileno(stderr), m_translationMode);
            wxASSERT(res != -1);
            res = _setmode(_fileno(stdout), m_translationMode);
            wxASSERT(res != -1);

            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}
#endif

bool MyAppConsole::OnCmdLineParsed(wxCmdLineParser& parser)
{
#if defined(__WXMSW__) && defined(UNICODE)
    if (!ProcessOutputCharset(parser))
    {
        return false;
    }
#endif

    {
        bool beVerbose;
        if (MyConfiguration::ReadNegatableSwitchValue(parser, "verbose", beVerbose) && beVerbose)
        {
            wxLog::SetLogLevel(wxLOG_Info);
            // wxLog::SetVerbose(true);
        }
        else
        {
            wxLog::SetLogLevel(wxLOG_Message);
            // wxLog::SetVerbose(false);
        }
    }

    {
        bool logTimestamps;
        if (MyConfiguration::ReadNegatableSwitchValue(parser, "log-timestamps", logTimestamps) && !logTimestamps)
        {
            wxLog::DisableTimestamp();
        }
    }

    return wxAppConsole::OnCmdLineParsed(parser);
}

#if defined(__WXMSW__) && defined(UNICODE)
int MyAppConsole::GetTranslationMode() const
{
    return m_translationMode;
}
#endif

void MyAppConsole::OnAssertFailure(
    const wxChar* file,
    int line,
    const wxChar* func,
    const wxChar* cond,
    const wxChar* msg
)
{
    const wxScopedPtr<wxMessageOutput> msgErr(new MyMessageOutputStderr());
    msgErr->Output(wxEmptyString);
    msgErr->Output("======== <ASSERT> =======");
    msgErr->Printf("ASSERT - file: %s", file);
    msgErr->Printf("ASSERT - line: %d", line);
    msgErr->Printf("ASSERT - fn  : %s", func);
    msgErr->Printf("ASSERT - cond: %s", cond);
    if (msg != nullptr && wxStrlen(msg) > 0)
    {
        msgErr->Printf("ASSERT - msg : %s", msg);
    }
    else
    {
        msgErr->Output("ASSERT - msg: <none>");
    }
    msgErr->Output("======== <ASSERT> =======");
}

#if defined(__WXMSW__)
namespace
{
    HRESULT co_initialize()
    {
        return CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
    }

    bool co_initialize_ex()
    {
        HRESULT hres = co_initialize();
        if (hres == S_OK)
        {
            return true;
        }
        else if (hres == RPC_E_CHANGED_MODE)
        {
            OleUninitialize();
            hres = co_initialize();
            return hres == S_OK;
        }
        else
        {
            return false;
        }
    }
}
#endif

bool MyAppConsole::OnInit()
{
    SetVendorName(APP_VENDOR_NAME);
    SetVendorDisplayName(APP_AUTHOR);

    if (!CheckLicense())
    {
        wxLogError(_("Cannot find or load license file"));
        return false;
    }

#if defined(__WXMSW__)
    wxCHECK(co_initialize_ex(), false);
#endif

    if (!wxAppConsole::OnInit()) return false;

    return true;
}

wxAppTraits* MyAppConsole::CreateTraits()
{
    wxAppTraits* pAppTraits = wxAppConsole::CreateTraits();

    return new MyAppTraits(pAppTraits);
}

int MyAppConsole::OnExit()
{
    wxLog::FlushActive();
    int res = wxAppConsole::OnExit();

    CoUninitialize();
    wxLogInfo(_("Done"));
    return res;
}

void MyAppConsole::AddSeparator(wxCmdLineParser& cmdline)
{
    cmdline.AddUsageText(m_sSeparator);
}

bool MyAppConsole::CheckLicense()
{
#ifdef _DEBUG
    return true;
#else
    const wxStandardPaths& paths = wxStandardPaths::Get();
    wxFileName             fn(paths.GetExecutablePath());
    fn.SetFullName(LICENSE_FILE_NAME);

    if (!fn.IsFileReadable()) return false;

    wxULongLong fs(fn.GetSize());

    if (fs == wxInvalidSize)
    {
        wxLogInfo(_wxS("Unable to read license " ENQUOTED_STR_FMT), fn.GetFullPath());
        return false;
    }

    wxULongLong maxSize(0, MAX_LICENSE_FILE_SIZE);

    if (fs > maxSize)
    {
        wxLogInfo(_wxS("License file " ENQUOTED_STR_FMT " is too big"), fn.GetFullPath());
        return false;
    }
    return true;
#endif
}

void MyAppConsole::ShowLicense(wxMessageOutput& out)
{
    const wxStandardPaths& paths = wxStandardPaths::Get();
    wxFileName             fn(paths.GetExecutablePath());

    fn.SetFullName(LICENSE_FILE_NAME);

    if (!fn.IsFileReadable())
    {
        wxLogError(_wxS("Cannot find license file " ENQUOTED_STR_FMT), fn.GetFullPath());
        return;
    }

    wxULongLong fs(fn.GetSize());

    if (fs == wxInvalidSize)
    {
        wxLogError(_wxS("Unable to read license " ENQUOTED_STR_FMT), fn.GetFullPath());
        return;
    }

    wxULongLong maxSize(0, MAX_LICENSE_FILE_SIZE);

    if (fs > maxSize)
    {
        wxLogError(_wxS("License file " ENQUOTED_STR_FMT " is too big"), fn.GetFullPath());
        return;
    }

    wxFileInputStream fis(fn.GetFullPath());

    if (!fis.IsOk())
    {
        wxLogError(_wxS("Cannot open license file " ENQUOTED_STR_FMT), fn.GetFullPath());
        return;
    }

    wxTextInputStream tis(fis, wxEmptyString, wxConvUTF8);

    while (!fis.Eof())
    {
        out.Output(tis.ReadLine());
    }
}

