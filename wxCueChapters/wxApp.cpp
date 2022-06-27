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
#include "wxTemporaryFilesProvider.h"
#include "wxPrimitiveRenderer.h"
#include "wxMkvmergeOptsRenderer.h"
#include "wxCuePointsRenderer.h"
#include "wxXmlCueSheetRenderer.h"
#include "wxFfMetadataRenderer.h"
#include "wxFfmpegCMakeScriptRenderer.h"
#include <wxWEBPHandler/imagwebp.h>
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

void wxMyApp::InfoTool(wxMessageOutput& out, wxCmdTool::TOOL tool)
{
    wxFileName exe;
    if (wxCmdTool::FindTool(tool, exe))
    {
        out.Printf(_("\t%-15s : %s"), exe.GetName(), exe.GetFullPath());
    }
    else
    {
        out.Printf(_("\t%-15s : <not found>"), exe.GetName());
    }
}

void wxMyApp::InfoTools(wxMessageOutput& out)
{
    wxFileName exe;

    InfoTool(out, wxCmdTool::TOOL_MKVMERGE);
    InfoTool(out, wxCmdTool::TOOL_FFMPEG);
    InfoTool(out, wxCmdTool::TOOL_FFPROBE);
    InfoTool(out, wxCmdTool::TOOL_CMAKE);
    InfoTool(out, wxCmdTool::TOOL_IMAGE_MAGICK);
    InfoTool(out, wxCmdTool::TOOL_MUTOOL);
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
    cmdline.SetLogo(_("This application converts cue sheet file to Matroska container in a more advanced way than standard Matroska tools."));
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
    wxImage::AddHandler(new wxWEBPHandler);

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

namespace
{
    wxString get_random_string(size_t length)
    {
        auto randchar = []() -> char {
            const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[rand() % max_index];
        };
        wxString str(length, '\000');
        std::generate_n(str.begin(), length, randchar);
        return str;
    }

    wxString get_stem()
    {
        return get_random_string(10).Prepend('-').Prepend(wxGetApp().GetAppName());
    }

    bool render_mkvmergeopts(
        const wxInputFile& inputFile,
        const wxConfiguration& cfg,
        const wxCueSheet& cueSheet,
        const wxString& tmpStem,
        const wxFileName& fnTmpMka,
        const wxFileName& fnChaptersFile,
        const wxFileName& fnTagsFile,
        wxTemporaryFilesCleaner& temporaryFilesCleaner,
        const wxFileName& optsFile,
        wxMatroskaAttachment& coverAttachment,
        wxFileName& fnImg)
    {
        // JSON with options
        const wxScopedPtr<wxMkvmergeOptsRenderer> optsRenderer(new wxMkvmergeOptsRenderer(cfg));
        optsRenderer->RenderDisc(inputFile, cueSheet, tmpStem, fnTmpMka, fnChaptersFile, fnTagsFile, coverAttachment, fnImg);
        if (!optsRenderer->Save(optsFile))
        {
            return false;
        }
        temporaryFilesCleaner.Feed(*optsRenderer);
        return true;
    }
}

int wxMyApp::ConvertCueSheet(const wxInputFile& inputFile, wxCueSheet& cueSheet)
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

    if (!ApplyApplicationTags(cueSheet))
    {
        wxLogWarning(_("Unable to apply application tags"));
    }

    switch (m_cfg.GetRenderMode())
    {
        case wxConfiguration::RENDER_CUESHEET:
        {
            wxString sOutputFile(m_cfg.GetOutputFile(inputFile).GetFullPath());
            wxLogInfo(_wxS("Saving cue scheet to " ENQUOTED_STR_FMT), sOutputFile);
            wxFileOutputStream fos(sOutputFile);

            if (!fos.IsOk())
            {
                wxLogError(_wxS("Fail to open " ENQUOTED_STR_FMT), sOutputFile);
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
            const wxScopedPtr<wxTemporaryFilesCleaner> temporaryFilesCleaner(new wxTemporaryFilesCleaner(m_cfg.RunTool()));
            const wxString tmpStem(get_stem());
            const bool tmpMka = cueSheet.HasFlacDataFile() || m_cfg.RunReplayGainScanner() || !cueSheet.HasSingleDataFile() || m_cfg.AudioFilteringRequired();
            wxFileName fnTmpMka;

            if (tmpMka)
            {
                if (!PreProcessAudio(
                    inputFile,
                    cueSheet,
                    tmpStem,
                    fnTmpMka,
                    *temporaryFilesCleaner))
                {
                    return 1;
                }
            }

            wxLogInfo(_("Converting cue scheet to XML format"));
            wxFileName fnXmlChapters;
            wxFileName fnXmlTags;
            {
                // chapters & tags
                const wxScopedPtr<wxXmlCueSheetRenderer> xmlRenderer(GetXmlRenderer(inputFile, tmpStem));

                if (!xmlRenderer->Render(cueSheet))
                {
                    wxLogError(_("Fail to export cue sheet to Matroska chapters"));
                }

                if (!xmlRenderer->Save())
                {
                    return 1;
                }
                temporaryFilesCleaner->Feed(*xmlRenderer);
                fnXmlChapters = xmlRenderer->GetChaptersFile();
                fnXmlTags = xmlRenderer->GetTagsFile();
            }

            {
                const wxFileName optsFile = m_cfg.GetTemporaryFile(inputFile, tmpStem, wxConfiguration::TMP::CMD, wxConfiguration::EXT::JSON);
                wxFileName fnImg;
                wxMatroskaAttachment coverAttachment;

                if (!render_mkvmergeopts(inputFile, m_cfg, cueSheet, 
                    tmpStem, fnTmpMka,
                    fnXmlChapters, fnXmlTags,
                    *temporaryFilesCleaner,
                    optsFile,
                    coverAttachment, fnImg))
                {
                    return 1;
                }

                const wxFileName scriptPath = m_cfg.GetTemporaryFile(inputFile, tmpStem, wxConfiguration::TMP::CMD, wxConfiguration::EXT::CMAKE);
                {
                    const wxScopedPtr<wxMkvmergeOptsRenderer> scriptRenderer(new wxMkvmergeOptsRenderer(m_cfg));
                    scriptRenderer->RenderScript(inputFile, cueSheet, tmpStem, optsFile, coverAttachment, fnImg);
                    if (!scriptRenderer->SaveScript(scriptPath))
                    {
                        return 1;
                    }
                    temporaryFilesCleaner->Feed(*scriptRenderer);
                }

                if (m_cfg.RunTool())
                {
                    wxASSERT(optsFile.IsOk());
                    if (!RunCMakeScript(scriptPath, fnImg.IsOk()))
                    {
                        return 1;
                    }
                }
            }
            break;
        }

        case wxConfiguration::RENDER_FFMPEG_CHAPTERS:
        case wxConfiguration::RENDER_FFMPEG:
        {
            const wxScopedPtr<wxTemporaryFilesCleaner> temporaryFilesCleaner(new wxTemporaryFilesCleaner(m_cfg.RunTool()));
            const wxString tmpStem(get_stem());
            const bool tmpMka = m_cfg.RunReplayGainScanner() || !cueSheet.HasSingleDataFile() || m_cfg.AudioFilteringRequired();
            wxFileName fnTmpMka;

            if (tmpMka)
            {
                if (!PreProcessAudio(
                    inputFile,
                    cueSheet,
                    tmpStem,
                    fnTmpMka,
                    *temporaryFilesCleaner))
                {
                    return 1;
                }
            }

            wxLogInfo(_("Converting cue scheet to ffmetadata format"));
            const wxFileName ffmetaPath = m_cfg.GetTemporaryFile(inputFile, tmpStem, wxConfiguration::TMP::FFM, wxConfiguration::EXT::TXT);
            const wxFileName scriptPath = m_cfg.GetTemporaryFile(inputFile, tmpStem, wxConfiguration::TMP::CMD, wxConfiguration::EXT::CMAKE);

            {
                // metadata
                const wxScopedPtr< wxFfMetadataRenderer > metadataRenderer(new wxFfMetadataRenderer(m_cfg));
                metadataRenderer->RenderDisc(cueSheet);
                if (!metadataRenderer->Save(ffmetaPath))
                {
                    return 1;
                }
                temporaryFilesCleaner->Feed(*metadataRenderer);
            }

            {
                const wxScopedPtr< wxFfmpegCMakeScriptRenderer > scriptRenderer(new wxFfmpegCMakeScriptRenderer(m_cfg));
                scriptRenderer->RenderDisc(inputFile, cueSheet, tmpStem, fnTmpMka, ffmetaPath);
                if (!scriptRenderer->SaveScript(scriptPath))
                {
                    return 1;
                }
                temporaryFilesCleaner->Feed(*scriptRenderer);
            }

            if (m_cfg.RunTool())
            {
                if (!RunCMakeScript(scriptPath, m_cfg.ConvertCoverFile() && cueSheet.HasCover()))
                {
                    return 1;
                }
            }
            break;
        }

        case wxConfiguration::RENDER_WAV2IMG_CUE_POINTS:
        {
            wxFileName outputFile(m_cfg.GetOutputFile(inputFile));

            wxCuePointsRenderer renderer(m_cfg);
            renderer.RenderDisc(cueSheet);

            wxLogInfo(_wxS("Saving cue points to " ENQUOTED_STR_FMT), outputFile.GetFullName());

            if (!renderer.Save(outputFile))
            {
                return 1;
            }

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

    const wxString inputFilePath(inputFile.GetInputFile().GetFullPath());

    wxLogMessage(_wxS("Processing " ENQUOTED_STR_FMT), inputFilePath);

    if (!reader.ReadCueSheetEx(inputFilePath, m_cfg.UseMLang()))
    {
        wxLogError(_wxS("Fail to read or parse input cue file " ENQUOTED_STR_FMT), inputFilePath);
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
            wxLogMessage(_wxS("Directory " ENQUOTED_STR_FMT " doesn't exists"), fn.GetPath());
            res = 1;

            if (m_cfg.AbortOnError()) break;
            else continue;
        }

        wxDir dir(fn.GetPath());

        if (!dir.IsOpened())
        {
            wxLogError(_wxS("Cannot open directory " ENQUOTED_STR_FMT), fn.GetPath());
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

wxXmlCueSheetRenderer* wxMyApp::GetXmlRenderer(const wxInputFile& inputFile, const wxString& tmpStem) const
{
    return new wxXmlCueSheetRenderer(m_cfg, inputFile, tmpStem);
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

        return res.RemoveLast();
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

    wxArrayString params;
    wxString optionsPath;
    wxFileName exe;

    if (!wxCmdTool::FindTool(wxCmdTool::TOOL_MKVMERGE, exe))
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
    optionsPath.Prepend('@');

    if (!wxLog::GetVerbose())
    {
        params.Add("--quiet");
    }

    params.Add("--ui-language");
    params.Add("en");

#if defined( __WXMSW__ ) && defined( __VISUALC__ ) && defined( UNICODE )
    switch (GetTranslationMode())
    {
        case _O_U8TEXT:
        params.Add("--output-charset");
        params.Add("utf-8");
        break;

        case _O_U16TEXT:
        params.Add("--output-charset");
        params.Add("utf-16");
        break;
    }
#endif

    params.Add(optionsPath);

    wxString cmd, cmdDesc;
    GetCmd(exe, params, cmd, cmdDesc);

    wxLogMessage(_("Invoking mkvmerge"));
    wxLogInfo(cmdDesc);

    wxExecuteEnv env;
    if (!PrepareExecuteEnv(env))
    {
        return false;
    }

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

bool wxMyApp::RunCMakeScript(const wxFileName& scriptFile, bool convertImage)
{
    wxASSERT(m_cfg.RunTool());
    wxString scriptPath;

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

    wxFileName mkvmerge;

    if (!wxCmdTool::FindTool(wxCmdTool::TOOL_MKVMERGE, mkvmerge))
    {
        wxLogError(_("Unable to find mkvmerge tool"));
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

    wxArrayString params;

    {
        const wxString logLevel(wxLog::GetVerbose() ? "STATUS" : "WARNING");
        params.Add(wxString::Format("--log-level=%s", logLevel));
    }

    params.Add("--log-context");
    params.Add("-D");
    params.Add("CMAKE_MESSAGE_CONTEXT=cue2mkc");

    params.Add("-P");
    params.Add(scriptPath);

    wxString cmd, cmdDesc;
    GetCmd(cmake, params, cmd, cmdDesc);

    wxLogMessage(_("Invoking CMake script"));
    wxLogInfo(cmdDesc);

    wxExecuteEnv env;
    if (!PrepareExecuteEnv(env))
    {
        return false;
    }

    env.env["FFMPEG"] = ffmpeg.GetFullPath();
    env.env["MKVMERGE"] = mkvmerge.GetFullPath();

    if (convertImage)
    {
        wxFileName imagick;
        if (wxCmdTool::FindTool(wxCmdTool::TOOL_IMAGE_MAGICK, imagick))
        {
            env.env["IMAGICK"] = imagick.GetFullPath();
        }
        else
        {
            wxLogWarning(_("ImageMagick package not found"));
        }

        wxFileName mutool;

        if (wxCmdTool::FindTool(wxCmdTool::TOOL_MUTOOL, mutool))
        {
            env.env["MUTOOL"] = mutool.GetFullPath();
        }
        else
        {
            wxLogWarning(_("MuPDF package not found"));
        }
    }

    if (!m_cfg.UseFullPaths())
    {
        env.cwd = scriptFile.GetPath();
    }

    long nRes = wxExecute(cmd, wxEXEC_SYNC | wxEXEC_NOEVENTS, nullptr, &env);

    if (nRes == -1)
    {
        wxLogError(_("Fail to execute CMake script"));
        return false;
    }
    else
    {
        if (nRes == 0)
        {
            wxLogInfo(_("CMake exit code: %ld (%08lX)"), nRes, nRes);
            return true;
        }
        else
        {
            wxLogError(_("CMake exit code: %ld (%08lX)"), nRes, nRes);
            return false;
        }
    }
}

bool wxMyApp::PreProcessAudio(
    const wxInputFile& inputFile,
    wxCueSheet& cueSheet,
    const wxString& tmpStem,
    wxFileName& fnTmpMka,
    wxTemporaryFilesCleaner& temporaryFilesCleaner) const
{
    const wxFileName workDir = m_cfg.GetOutputDir(inputFile);

    /*
    if (m_cfg.UseFullPaths())
    {
        workDir = wxFileName::DirName(wxStandardPaths::Get().GetTempDir());
    }

    const wxString tmpStem(get_stem());
    const bool rgScan = m_cfg.RunReplayGainScanner();
    const bool tmpMka = (m_cfg.UseMkvmerge() && cueSheet.HasFlacDataFile()) || (cueSheet.GetDataFilesCount() > 1u);
    */

    fnTmpMka = wxConfiguration::GetTemporaryFile(workDir, tmpStem, wxConfiguration::TMP::PRE, wxConfiguration::EXT::MKA);
    temporaryFilesCleaner.Add(fnTmpMka);

    if (m_cfg.RunReplayGainScanner())
    {
        const wxFileName chaptersFile = wxConfiguration::GetTemporaryFile(workDir, tmpStem, wxConfiguration::TMP::CHAPTERS, wxConfiguration::EXT::JSON);

        wxScopedPtr<wxFfMetadataRenderer> metadataRenderer(new wxFfMetadataRenderer(m_cfg));
        const wxJson chapters = metadataRenderer->RenderChapters(cueSheet);
        if (!metadataRenderer->SaveChapters(chapters, chaptersFile))
        {
            return false;
        }
        temporaryFilesCleaner.Feed(*metadataRenderer);
    }

    wxFileName scriptFile;
    wxFileName scanFile;
    {
        wxScopedPtr<wxFfmpegCMakeScriptRenderer> scriptRenderer(new wxFfmpegCMakeScriptRenderer(m_cfg));
        scriptRenderer->RenderPre(cueSheet, workDir, tmpStem);
        if (!scriptRenderer->SaveDraft(workDir, tmpStem, scriptFile, scanFile))
        {
            return false;
        }
        temporaryFilesCleaner.Feed(*scriptRenderer);
    }

    bool res = RunPreScript(scriptFile);

    if (!res)
    {
        return false;
    }

    if (m_cfg.RunReplayGainScanner())
    {
        if (!scanFile.IsFileReadable())
        {
            return false;
        }

        if (!cueSheet.ApplyTagsFromJson(scanFile))
        {
            return false;
        }
    }

    return true;
}

bool wxMyApp::RunPreScript(const wxFileName& cmakeScriptFile) const
{
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

    wxFileName outDir(cmakeScriptFile);
    outDir.SetFullName(wxEmptyString);

    wxArrayString params;

    {
        const wxString logLevel(wxLog::GetVerbose() ? "STATUS" : "WARNING");
        params.Add(wxString::Format("--log-level=%s", logLevel));
    }

    params.Add("--log-context");
    params.Add("-D");
    params.Add("CMAKE_MESSAGE_CONTEXT=pre");

    params.Add("-P");
    if (m_cfg.UseFullPaths())
        params.Add(cmakeScriptFile.GetFullPath());
    else
        params.Add(cmakeScriptFile.GetFullName());

    wxString cmd, cmdDesc;
    GetCmd(cmake, params, cmd, cmdDesc);

    wxLogMessage(_("Invoking pre-process script"));
    wxLogInfo(cmdDesc);

    wxExecuteEnv env;
    if (!PrepareExecuteEnv(env))
    {
        return false;
    }

    env.env["CUE2MKC"] = wxStandardPaths::Get().GetExecutablePath();
    env.env["FFMPEG"] = ffmpeg.GetFullPath();

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
        if (nRes == 0)
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

bool wxMyApp::ApplyApplicationTags(wxCueSheet& cueSheet) const
{
    wxFileName tagsFile = wxFileName::FileName(wxStandardPaths::Get().GetExecutablePath());
    tagsFile.SetExt("tags.json");

    if (tagsFile.IsFileReadable())
    {
        return cueSheet.ApplyTagsFromJson(tagsFile);
    }
    else
    {
        wxLogDebug("Tags file not found");
        return false;
    }
}