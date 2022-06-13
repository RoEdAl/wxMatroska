/*
 * wxApp.cpp
 */

#include <app_config.h>
#include <wxCueFile/wxTagSynonims.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxCueFile/wxCueSheetRenderer.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>
#include <wxCueFile/wxAsciiToUnicode.h>
#include "wxConfiguration.h"
#include "wxPrimitiveRenderer.h"
#include "wxMkvmergeOptsRenderer.h"
#include "wxCuePointsRenderer.h"
#include "wxXmlCueSheetRenderer.h"
#include "wxFfMetadataRenderer.h"
#include "wxFfmpegCMakeScriptRenderer.h"
#include "wxApp.h"

 // ===============================================================================

const char wxMyApp::APP_NAME[] = "cue2mkc";
const char wxMyApp::APP_VERSION[] = WXMATROSKA_VERSION_STR;

namespace
{
    constexpr wxDouble RG2_REF_R128_LOUDNESS_DBFS = -18;
}

// ===============================================================================

wxIMPLEMENT_APP_CONSOLE(wxMyApp);

wxMyApp::wxMyApp(void)
    :m_calcRg2Loudness(false),m_rg2Value(0.0)
{
}

void wxMyApp::InfoVersion(wxMessageOutput& out)
{
    out.Printf(_("Application version: %s"), APP_VERSION);
    out.Printf(_("Author: %s"), APP_AUTHOR);
    out.Output(_("License: Simplified BSD License - http://www.opensource.org/licenses/bsd-license.php"));
    out.Output(wxVERSION_STRING);
    out.Output(wxCueSheetReader::GetTagLibVersion());
    out.Printf(_("Operating system: %s"), wxPlatformInfo::Get().GetOperatingSystemDescription());
    out.Printf(_("Compiler: %s %s"), INFO_CXX_COMPILER_ID, INFO_CXX_COMPILER_VERSION);
    out.Printf(_("Compiled on: %s %s (%s)"), INFO_HOST_SYSTEM_NAME, INFO_HOST_SYSTEM_VERSION, INFO_HOST_SYSTEM_PROCESSOR);
}

void wxMyApp::InfoUsage(wxMessageOutput& out)
{
    out.Output(_("Input file format specification:"));
    out.Output(_("Input file may be a wildcard:"));
    out.Output(_("\t*.cue"));
    out.Printf(_("You may also specify data files after cue file using %c as separator."), wxInputFile::SEPARATOR);
    out.Printf(_("\t\"test.cue%ctest.flac\""), wxInputFile::SEPARATOR);
    out.Output(_("This allow you to override data file specification in cue sheet file."));
}

void wxMyApp::InfoTool(wxMessageOutput& out, wxCmdTool::TOOL tool) const
{
    wxFileName exe;
    if (wxCmdTool::FindTool(tool, exe))
    {
        out.Printf(_("\t%-15s : %s"), exe.GetName(), exe.GetFullPath());
    }
    else
    {
        out.Printf(_("\t%s\t<not found>"), exe.GetName());
    }
}

void wxMyApp::InfoTools(wxMessageOutput& out) const
{
    wxFileName exe;

    InfoTool(out, wxCmdTool::TOOL_MKVMERGE);
    InfoTool(out, wxCmdTool::TOOL_FFMPEG);
    InfoTool(out, wxCmdTool::TOOL_CMAKE);
}

void wxMyApp::InfoAsciiToUnicode(wxMessageOutput& out)
{
    wxAsciiToUnicode::ShowReplTable(out);
}

void wxMyApp::InfoFormatDescription(wxMessageOutput& out)
{
    out.Output(_("Formating directives:"));
    out.Output(wxEmptyString);

    out.Output(_("\t%da%\tdisc arranger"));
    out.Output(_("\t%dc%\tdisc composer"));
    out.Output(_("\t%dp%\tdisc performer"));
    out.Output(_("\t%ds%\tdisc songwriter"));
    out.Output(_("\t%dt%\tdisc title"));

    out.Output(_("\t%n%\ttrack number"));
    out.Output(_("\t%ta%\ttrack arranger"));
    out.Output(_("\t%tc%\ttrack composer"));
    out.Output(_("\t%tp%\ttrack performer"));
    out.Output(_("\t%ts%\ttrack songwriter"));
    out.Output(_("\t%tt%\ttrack title"));

    out.Output(_("\t%aa%\ttrack or disc arranger"));
    out.Output(_("\t%ac%\ttrack or disc composer"));
    out.Output(_("\t%ap%\ttrack or disc performer"));
    out.Output(_("\t%as%\ttrack or disc songwriter"));
    out.Output(_("\t%at%\ttrack or disc title"));

    out.Output(wxEmptyString);
}

bool wxMyApp::ShowInfo() const
{
    switch (m_cfg.GetInfoSubject())
    {
        case wxConfiguration::INFO_VERSION:
        {
            InfoVersion(*wxMessageOutput::Get());
            return true;
        }

        case wxConfiguration::INFO_USAGE:
        {
            InfoUsage(*wxMessageOutput::Get());
            return true;
        }

        case wxConfiguration::INFO_TOOLS:
        {
            InfoTools(*wxMessageOutput::Get());
            return true;
        }

        case wxConfiguration::INFO_ASCII_TO_UNICODE:
        {
            InfoAsciiToUnicode(*wxMessageOutput::Get());
            return true;
        }

        case wxConfiguration::INFO_FORMATTING_DIRECTIVES:
        {
            InfoFormatDescription(*wxMessageOutput::Get());
            return true;
        }

        case wxConfiguration::INFO_LICENSE:
        {
            ShowLicense(*wxMessageOutput::Get());
            return true;
        }

        default:
        {
            return false;
        }
    }
}

void wxMyApp::OnInitCmdLine(wxCmdLineParser& cmdline)
{
    MyAppConsole::OnInitCmdLine(cmdline);
    cmdline.AddOption(wxEmptyString, "calc-rg2-loudness", wxEmptyString, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_HIDDEN);
    m_cfg.AddCmdLineParams(cmdline);
    cmdline.SetLogo(_("This application converts cue sheet files to Matroska XML chapter files in a more advanced way than standard Matroska tools."));
}

bool wxMyApp::OnCmdLineParsed(wxCmdLineParser& cmdline)
{
    if (!MyAppConsole::OnCmdLineParsed(cmdline)) return false;

    {
        wxDouble val;
        if (cmdline.Found("calc-rg2-loudness", &val))
        {
            m_calcRg2Loudness = true;
            m_rg2Value = val;

            return true;
        }
    }

    m_cfg.ReadLanguagesStrings();

    if (!m_cfg.Read(cmdline)) return false;

    wxLogInfo(_("%s ver. %s"), GetAppDisplayName(), APP_VERSION);
    m_cfg.Dump();

    return true;
}

bool wxMyApp::OnInit()
{
    SetAppName(APP_NAME);

    m_dt = MyAppConsole::GetNow();
    srand(m_dt.GetTicks());

    if (!MyAppConsole::OnInit()) return false;

    wxInitAllImageHandlers();

    if (!m_cfg.InitJpegHandler()) wxLogWarning(_("Unable to initialize JPEG image handler."));

    return true;
}

const wxDateTime& wxMyApp::GetNow() const
{
    return m_dt;
}

bool wxMyApp::PrepareExecuteEnv(wxExecuteEnv& env) const
{
    if (!wxGetEnvMap(&env.env))
    {
        wxLogError(_("Fail to get environment variables"));
        return false;
    }

    env.env["SOURCE_DATE_EPOCH"] = wxString::Format("%" wxSizeTFmtSpec "d", m_dt.GetTicks());
    return true;
}

int wxMyApp::AppendCueSheet(wxCueSheet& cueSheet)
{
    wxASSERT(m_cfg.JoinMode());

    if (!(cueSheet.HasDuration() || cueSheet.CalculateDuration(m_cfg.GetAlternateExtensions())))
    {
        wxLogError(_("Fail to calculate duration of cue sheet"));
        return 1;
    }

    wxCueSheet& mergedCueSheet = GetMergedCueSheet();

    mergedCueSheet.Append(cueSheet);
    return 0;
}

int wxMyApp::ConvertCueSheet(const wxInputFile& inputFile, const wxCueSheet& cueSheet)
{
    if (cueSheet.GetDataFilesCount() > 1u)
    {
        if (!m_cfg.UseDataFiles())
        {
            wxLogError(_("Cue sheet has more than one data file."));
            wxLogError(_("Please use -df (or --use-data-files) option."));
            return 1;
        }
    }

    if (m_cfg.UseDataFiles())
    {
        // if ( !( cueSheet.HasDuration() || cueSheet.CalculateDuration( m_cfg.GetAlternateExtensions() ) ) )
        if (!cueSheet.HasDuration())
        {
            wxLogError(_("Fail to calculate duration of cue sheet"));
            return 1;
        }
    }

    switch (m_cfg.GetRenderMode())
    {
        case wxConfiguration::RENDER_CUESHEET:
        {
            wxString sOutputFile(m_cfg.GetOutputFile(inputFile).GetFullPath());
            wxLogInfo(_("Saving cue scheet to \u201C%s\u201D"), sOutputFile);
            wxFileOutputStream fos(sOutputFile);

            if (!fos.IsOk())
            {
                wxLogError(_("Fail to open \u201C%s\u201D"), sOutputFile);
                return 1;
            }

            wxSharedPtr< wxTextOutputStream > pTos(m_cfg.GetOutputTextStream(fos));
            wxTextCueSheetRenderer            renderer(pTos.get());

            if (!renderer.Render(cueSheet)) return 1;

            break;
        }

        case wxConfiguration::RENDER_MKVMERGE_CHAPTERS:
        case wxConfiguration::RENDER_MKVMERGE:
        {
            wxLogInfo(_("Converting cue scheet to XML format"));
            wxScopedPtr<wxXmlCueSheetRenderer> pXmlRenderer(GetXmlRenderer(inputFile));

            if (pXmlRenderer->Render(cueSheet))
            {
                if (!pXmlRenderer->SaveXmlDoc()) return 1;

                wxScopedPtr<wxMkvmergeOptsRenderer> optsRenderer( new wxMkvmergeOptsRenderer(m_cfg));
                optsRenderer->RenderDisc(inputFile, cueSheet);
                if (!optsRenderer->Save()) return 1;

                if (m_cfg.RunTool())
                {
                    if (!RunMkvmerge(optsRenderer->GetMkvmergeOptsFile()))
                    {
                        return 1;
                    }

                    // TODO: Fix RG scanner
                    if (m_cfg.RunReplayGainScanner())
                    {
                        wxFileName mka = m_cfg.GetOutputFile(inputFile, wxConfiguration::EXT::MATROSKA_AUDIO);
                        if (!RunReplayGainScanner(mka))
                        {
                            return 1;
                        }
                    }
                }
            }
            else
            {
                wxLogError(_("Fail to export cue sheet to Matroska chapters"));
                return 1;
            }
            break;
        }

        case wxConfiguration::RENDER_FFMPEG_CHAPTERS:
        case wxConfiguration::RENDER_FFMPEG:
        {
            wxLogInfo(_("Converting cue scheet to ffmetadata format"));
            const wxFileName ffmetaPath = m_cfg.GetOutputFile(inputFile, wxConfiguration::EXT::FFMPEG_METADATA);
            const wxFileName scriptPath = m_cfg.GetOutputFile(inputFile, wxConfiguration::EXT::CMAKE_SCRIPT);
            {
                // metadata
                wxScopedPtr< wxFfMetadataRenderer > metadataRenderer(new wxFfMetadataRenderer(m_cfg));
                metadataRenderer->RenderDisc(cueSheet);
                if (!metadataRenderer->Save(ffmetaPath)) return 1;
            }

            {
                wxScopedPtr< wxFfmpegCMakeScriptRenderer > scriptRenderer( new wxFfmpegCMakeScriptRenderer(m_cfg) );
                scriptRenderer->RenderDisc(cueSheet, inputFile, ffmetaPath);
                if (!scriptRenderer->Save(scriptPath)) return 1;
                if (m_cfg.RunTool())
                {
                    if (!RunCMakeScript(scriptPath))
                    {
                        return 1;
                    }

                    if (m_cfg.RunReplayGainScanner())
                    {
                        wxFileName mka = m_cfg.GetOutputFile(inputFile, wxConfiguration::EXT::MATROSKA_AUDIO);
                        if (!RunReplayGainScanner(mka))
                        {
                            return 1;
                        }
                    }
                }
            }
            break;
        }

        case wxConfiguration::RENDER_WAV2IMG_CUE_POINTS:
        {
            wxFileName outputFile(m_cfg.GetOutputFile(inputFile));

            wxCuePointsRenderer renderer(m_cfg);
            renderer.RenderDisc(cueSheet);

            wxLogInfo(_("Saving cue points to \u201C%s\u201D"), outputFile.GetFullName());

            if (!renderer.Save(outputFile)) return 1;

            break;
        }
    }

    return 0;
}

int wxMyApp::ProcessCueFile(const wxInputFile& inputFile, const wxTagSynonimsCollection& discSynonims, const wxTagSynonimsCollection& trackSynonims)
{
    wxCueSheetReader reader;

    reader
        .CorrectQuotationMarks(m_cfg.CorrectQuotationMarks(), m_cfg.GetLang())
        .SetAlternateExt(m_cfg.GetAlternateExtensions())
        .SetReadFlags(m_cfg.GetReadFlags());

    wxString sInputFile(inputFile.GetInputFile().GetFullPath());

    wxLogMessage(_("Processing \u201C%s\u201D"), sInputFile);

    if (!reader.ReadCueSheetEx(sInputFile, m_cfg.UseMLang()))
    {
        wxLogError(_("Fail to read or parse input cue file \u201C%s\u201D"), sInputFile);
        return 1;
    }

    wxCueSheet cueSheet(reader.GetCueSheet());

    if (inputFile.HasDataFiles())
    {
        // obsolete code
        wxArrayDataFile dataFiles;
        inputFile.GetDataFiles(dataFiles, wxDataFile::WAVE);
        cueSheet.SetDataFiles(dataFiles);
    }

    if (m_cfg.JoinMode())
    {
        return AppendCueSheet(cueSheet);
    }
    else
    {
        cueSheet.SanitizeTags(discSynonims, trackSynonims, false, m_cfg.IncludeDiscNumberTag());
        return ConvertCueSheet(inputFile, cueSheet);
    }
}

int wxMyApp::OnRun()
{
    if (m_calcRg2Loudness)
    {
        wxMessageOutput* const out = wxMessageOutput::Get();
        const wxDouble loudness = RG2_REF_R128_LOUDNESS_DBFS - m_rg2Value;
        wxString loudnessStr = wxString::FromCDouble(loudness, 1);
        if (loudness > 0.0)
        {
            loudnessStr.Prepend('+');
        }

        out->Output(loudnessStr);
        return 0;
    }

    if (ShowInfo()) return 0;

    wxInputFile firstInputFile;
    bool        bFirst = true;

    wxTagSynonimsCollection discSynonims;
    wxTagSynonimsCollection trackSynonims;

    wxCueComponent::GetSynonims(discSynonims, false);
    wxCueComponent::GetSynonims(trackSynonims, true);

    int                     res = 0;
    const wxArrayInputFile& inputFile = m_cfg.GetInputFiles();

    for (size_t i = 0, nCount = inputFile.GetCount(); i < nCount; ++i)
    {
        wxFileName fn(inputFile[i].GetInputFile());

        if (!wxDir::Exists(fn.GetPath()))
        {
            wxLogMessage(_("Directory \u201C%s\u201D doesn't exists"), fn.GetPath());
            res = 1;

            if (m_cfg.AbortOnError()) break;
            else continue;
        }

        wxDir dir(fn.GetPath());

        if (!dir.IsOpened())
        {
            wxLogError(_("Cannot open directory \u201C%s\u201D"), fn.GetPath());
            res = 1;

            if (m_cfg.AbortOnError()) break;
            else continue;
        }

        wxString sFileSpec(fn.GetFullName());
        wxString sInputFile;

        if (dir.GetFirst(&sInputFile, sFileSpec, wxDIR_FILES))
        {
            wxInputFile singleFile(inputFile[i]);
            while (true)
            {
                fn.SetFullName(sInputFile);
                singleFile.SetInputFile(fn);

                if (bFirst)
                {
                    firstInputFile = singleFile;
                    bFirst = false;
                }

                res = ProcessCueFile(singleFile, discSynonims, trackSynonims);

                if ((res != 0) && (m_cfg.AbortOnError() || m_cfg.JoinMode())) break;

                if (!dir.GetNext(&sInputFile)) break;
            }
        }
    }

    if (m_cfg.JoinMode() && (res == 0))
    {
        wxASSERT(!bFirst);
        GetMergedCueSheet().SanitizeTags(discSynonims, trackSynonims, true, m_cfg.IncludeDiscNumberTag());
        res = ConvertCueSheet(firstInputFile, GetMergedCueSheet());
    }

    return (m_cfg.AbortOnError() || m_cfg.JoinMode()) ? res : 0;
}

int wxMyApp::OnExit()
{
    int res = MyAppConsole::OnExit();

    m_pMergedCueSheet.reset();
    return res;
}

wxXmlCueSheetRenderer* wxMyApp::GetXmlRenderer(const wxInputFile& inputFile) const
{
    return wxXmlCueSheetRenderer::CreateObject(m_cfg, inputFile);
}

bool wxMyApp::HasMergedCueSheet() const
{
    return m_pMergedCueSheet;
}

wxCueSheet& wxMyApp::GetMergedCueSheet()
{
    if (!HasMergedCueSheet())
    {
        wxLogDebug("Creating empty cue sheet for merging");
        m_pMergedCueSheet.reset(new wxCueSheet());
    }

    wxASSERT(HasMergedCueSheet());
    return *m_pMergedCueSheet;
}

const wxCueSheet& wxMyApp::GetMergedCueSheet() const
{
    wxASSERT(HasMergedCueSheet());
    return *m_pMergedCueSheet;
}

namespace
{
    bool have_spaces(const wxString& str, const wxRegEx& spaceChecker)
    {
        if (str.StartsWith("--") || str.StartsWith('-')) return false;

        return spaceChecker.Matches(str);
    }

    wxString options_to_str(const wxArrayString& options)
    {
        if (options.IsEmpty()) return wxEmptyString;

        const wxRegEx spaceChecker("\\p{Xps}", wxRE_NOSUB);

        wxASSERT(spaceChecker.IsValid());

        wxString res;

        for (wxArrayString::const_iterator i = options.begin(), end = options.end(); i != end; ++i)
        {
            if (have_spaces(*i, spaceChecker))
            {
                res.Append('\"').Append(*i).Append("\" ");
            }
            else
            {
                res.Append(*i).Append(' ');
            }
        }

        res.RemoveLast();
        return res;
    }
}

void wxMyApp::GetCmd(const wxFileName& exe, const wxArrayString& params, wxString& cmd, wxString& cmdDesc) const
{
    GetCmd(exe, options_to_str(params), cmd, cmdDesc);
}

void wxMyApp::GetCmd(const wxFileName& exe, const wxString& params, wxString& cmd, wxString& cmdDesc) const
{
    cmd.Empty();
    cmd << '"' << exe.GetFullPath() << "\" " << params;

    cmdDesc.Empty();
    cmdDesc << exe.GetName() << ' ' << params;
}

bool wxMyApp::RunMkvmerge(const wxFileName& optionsFile)
{
    wxASSERT(m_cfg.RunTool());

    wxString params;
    wxString optionsPath;
    wxFileName exe;

    if (!wxCmdTool::FindExecutable("mkvmerge", "MKVToolNix", exe))
    {
        wxLogError(_("Unable to find mkvmerge tool"));
        return false;
    }

    if (m_cfg.UseFullPaths())
    {
        optionsPath = optionsFile.GetFullPath();
    }
    else
    {
        optionsPath = optionsFile.GetFullName();
    }

    wxString outputCharset;
#if defined( __WXMSW__ ) && defined( __VISUALC__ ) && defined( UNICODE )
    switch (GetTranslationMode())
    {
        case _O_U8TEXT:
        outputCharset = "--output-charset utf-8";
        break;

        case _O_U16TEXT:
        outputCharset = "--output-charset utf-16";
        break;
    }
#endif
    if (!outputCharset.IsEmpty())
    {
        outputCharset.Prepend(' ').Append(' ');
    }

    if (wxLog::GetVerbose())
    {
        params.Printf("--ui-language en %s\"@%s\"", outputCharset, optionsPath);
    }
    else
    {
        params.Printf("--quiet --ui-language en %s\"@%s\"", outputCharset, optionsPath);
    }

    wxString cmd, cmdDesc;
    GetCmd(exe, params, cmd, cmdDesc);

    wxLogMessage(cmdDesc);

    wxExecuteEnv env;
    PrepareExecuteEnv(env);

    if (!m_cfg.UseFullPaths())
    {
        env.cwd = optionsFile.GetPath();
    }

    long nRes = wxExecute(cmd, wxEXEC_SYNC | wxEXEC_NOEVENTS, nullptr, &env);

    if (nRes == -1)
    {
        wxLogError(_("Fail to execute mkvmerge tool"));
        return false;
    }
    else
    {
        if (nRes <= 1)
        {
            wxLogInfo(_("mkvmerge exit code: %ld (%08lX)"), nRes, nRes);
            return true;
        }
        else
        {
            wxLogError(_("mkvmerge exit code: %ld (%08lX)"), nRes, nRes);
            return false;
        }
    }
}

bool wxMyApp::RunCMakeScript(const wxFileName& scriptFile)
{
    wxASSERT(m_cfg.RunTool());

    wxString params;
    wxString scriptPath;
    wxFileName exe;

    if (!wxCmdTool::FindExecutable("cmake", "CMake/bin", exe))
    {
        wxLogError(_("Unable to find cmake tool"));
        return false;
    }

    if (m_cfg.UseFullPaths())
    {
        scriptPath = scriptFile.GetFullPath();
    }
    else
    {
        scriptPath = scriptFile.GetFullName();
    }

    params.Printf("-P \"%s\"", scriptPath);

    wxString cmd, cmdDesc;
    GetCmd(exe, params, cmd, cmdDesc);

    wxLogMessage(cmdDesc);

    wxExecuteEnv env;
    PrepareExecuteEnv(env);

    if (!m_cfg.UseFullPaths())
    {
        env.cwd = scriptFile.GetPath();
    }

    long nRes = wxExecute(cmd, wxEXEC_SYNC | wxEXEC_NOEVENTS, nullptr, &env);

    if (nRes == -1)
    {
        wxLogError(_("Fail to execute cmake tool"));
        return false;
    }
    else
    {
        if (nRes <= 1)
        {
            wxLogInfo(_("cmake exit code: %ld (%08lX)"), nRes, nRes);
            return true;
        }
        else
        {
            wxLogError(_("cmake exit code: %ld (%08lX)"), nRes, nRes);
            return false;
        }
    }
}

bool wxMyApp::RunReplayGainScanner(const wxFileName& mka)
{
    wxASSERT(m_cfg.RunReplayGainScanner());

    wxFileName cmake;

    if (!wxCmdTool::FindTool(wxCmdTool::TOOL_CMAKE, cmake))
    {
        wxLogError(_("Unable to find cmake tool"));
        return false;
    }

    wxFileName ffmpeg;
    if (!wxCmdTool::FindTool(wxCmdTool::TOOL_FFMPEG, ffmpeg))
    {
        wxLogError(_("Unable to find ffmpeg tool"));
        return false;
    }

    wxFileName ffprobe;
    if (!wxCmdTool::FindTool(wxCmdTool::TOOL_FFPROBE, ffprobe))
    {
        wxLogError(_("Unable to find ffprobe tool"));
        return false;
    }

    wxFileName fnScanner(wxStandardPaths::Get().GetExecutablePath());
    fnScanner.SetFullName("ff-scan.cmake");

    wxFileName outDir(mka);
    outDir.SetFullName(wxEmptyString);

    wxArrayString params;

    {
        const wxString logLevel(wxLog::GetVerbose() ? "STATUS" : "WARNING");

        params.Add(wxString::Format("--log-level=%s", logLevel));

        params.Add("-D");
        params.Add(wxString::Format("CUE2MKC_MESSAGE_LOG_LEVEL=%s", logLevel));
    }

    params.Add("--log-context");
    params.Add("-D");
    params.Add("CMAKE_MESSAGE_CONTEXT=ffscan");

    params.Add("-D");
    params.Add(wxString::Format("FFMPEG=%s", ffmpeg.GetFullPath()));

    params.Add("-D");
    params.Add(wxString::Format("FFPROBE=%s", ffprobe.GetFullPath()));

    params.Add("-D");
    params.Add(wxString::Format("CUE2MKC=%s", wxStandardPaths::Get().GetExecutablePath()));

    if (m_cfg.UseFullPaths())
    {
        params.Add("-D");
        params.Add(wxString::Format("CUE2MKC_MKA=%s", mka.GetFullPath()));
    }
    else
    {
        params.Add("-D");
        params.Add(wxString::Format("CUE2MKC_MKA=%s", mka.GetFullName()));

        params.Add("-D");
        params.Add(wxString::Format("CUE2MKC_WORKDIR=%s", outDir.GetFullPath().RemoveLast()));
    }

    params.Add("-P");
    params.Add(fnScanner.GetFullPath());

    wxString cmd, cmdDesc;
    GetCmd(cmake, params, cmd, cmdDesc);

    wxLogMessage(cmdDesc);

    wxExecuteEnv env;
    PrepareExecuteEnv(env);

    if (!m_cfg.UseFullPaths())
    {
        env.cwd = outDir.GetFullPath();
    }

    long nRes = wxExecute(cmd, wxEXEC_SYNC | wxEXEC_NOEVENTS, nullptr, &env);

    if (nRes == -1)
    {
        wxLogError(_("Fail to execute cmake tool"));
        return false;
    }
    else
    {
        if (nRes <= 1)
        {
            wxLogInfo(_("cmake exit code: %ld (%08lX)"), nRes, nRes);
            return true;
        }
        else
        {
            wxLogError(_("cmake exit code: %ld (%08lX)"), nRes, nRes);
            return false;
        }
    }
}
