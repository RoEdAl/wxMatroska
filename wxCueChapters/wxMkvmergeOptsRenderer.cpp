/*
 * wxMkvmergeOptsRenderer.cpp
 */

#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxCueFile/wxStringProcessor.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#include <wxEncodingDetection/wxTextInputStreamOnString.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>
#include "wxConfiguration.h"
#include "wxInputFile.h"
#include "wxPrimitiveRenderer.h"
#include "wxMkvmergeOptsRenderer.h"
#include "wxApp.h"

 // ===============================================================================

namespace
{
    struct attachment
    {
        static const char name[];
        static const char mime[];
        static const char desc[];
        static const char file[];
    };

    const char attachment::name[] = "--attachment-name";
    const char attachment::mime[] = "--attachment-mime-type";
    const char attachment::desc[] = "--attachment-description";
    const char attachment::file[] = "--attach-file";

    template< size_t N >
    void add_string(wxJson& j, const char(&s)[N])
    {
        j.push_back(s);
    }

    void add_string(wxJson& j, const wxString& s)
    {
        j.push_back(s.utf8_string());
    }

    wxString get_mapping_str(const wxCueSheet& cueSheet)
    {
        wxASSERT(cueSheet.GetDataFilesCount() > 1u);
        wxString sRes;

        for (size_t i = 1u, nCount = cueSheet.GetDataFilesCount(); i < nCount; ++i)
        {
            sRes += wxString::Format("%" wxSizeTFmtSpec "u:0:%" wxSizeTFmtSpec "u:0,", i, i - 1);
        }

        return sRes.RemoveLast();
    }

    void render_attachments(wxJson& opts, const wxArrayMatroskaAttachment& attachments, const wxFileName& fnImg)
    {
        if (fnImg.IsOk())
        {
            const wxMatroskaAttachment& a = attachments[0];

            add_string(opts, attachment::name);
            add_string(opts, a.GetName(fnImg.GetExt()));
            if (a.HasDescription())
            {
                add_string(opts, attachment::desc);
                add_string(opts, a.GetDescription());
            }
            wxString mimeType;
            if (wxCoverFile::GetMimeFromExt(fnImg, mimeType))
            {
                add_string(opts, attachment::mime);
                add_string(opts, mimeType);
            }
            add_string(opts, attachment::file);
            add_string(opts, fnImg.GetFullPath());
        }

        for (size_t i = fnImg.IsOk()? 1 : 0, cnt = attachments.GetCount(); i < cnt; ++i)
        {
            const wxMatroskaAttachment& a = attachments[i];

            add_string(opts, attachment::name);
            add_string(opts, a.GetName());
            if (a.HasDescription())
            {
                add_string(opts, attachment::desc);
                add_string(opts, a.GetDescription());
            }
            if (a.HasMimeType())
            {
                add_string(opts, attachment::mime);
                add_string(opts, a.GetMimeType());
            }
            add_string(opts, attachment::file);
            add_string(opts, a.GetFileName().GetFullPath());
        }
    }
}

// ===============================================================================

wxMkvmergeOptsRenderer::wxMkvmergeOptsRenderer(const wxConfiguration& cfg)
    :wxCMakeScriptRenderer(cfg)
{
}

void wxMkvmergeOptsRenderer::RenderOptions(
    const wxInputFile& inputFile,
    const wxCueSheet& cueSheet,
    const wxString& tmpStem,
    const wxFileName& fnTmpMka,
    const wxFileName& chaptersFile,
    const wxFileName& tagsFile,
    wxFileName& mkaFile,
    wxMatroskaAttachment& coverAttachment,
    wxFileName& fnImg)
{
    mkaFile = m_cfg.GetTemporaryFile(inputFile, tmpStem, wxConfiguration::TMP::MKA, wxConfiguration::EXT::MATROSKA_AUDIO);
    m_temporaryFiles.Add(mkaFile);

    const wxString trackName = GetTrackName(cueSheet);

    wxFileName outDir;

    if (!m_cfg.UseFullPaths())
    {
        outDir = m_cfg.GetOutputDir(inputFile);
    }

    wxJson opts;

    // pre
    add_string(opts, "--deterministic");
    add_string(opts, wxString(wxMyApp::APP_NAME));
    add_string(opts, "--disable-track-statistics-tags");
    add_string(opts, "--no-date");
    add_string(opts, "--disable-language-ietf");
    add_string(opts, "--no-cues");
    add_string(opts, "-o");
    add_string(opts, GetRelativeFileName(mkaFile, outDir).GetFullPath());
    add_string(opts, "--language");
    add_string(opts, wxString::Format("0:%s", m_cfg.GetLang()));
    add_string(opts, "--default-track");
    add_string(opts, "0:yes");
    add_string(opts, "--track-name");
    add_string(opts, wxString::Format("0:%s", trackName));

    if (fnTmpMka.IsOk())
    {
        // single temporary audio file
        add_string(opts, "-a");
        add_string(opts, '0');
        add_string(opts, "-D");
        add_string(opts, "-S");
        add_string(opts, "-B");
        add_string(opts, "-T");
        add_string(opts, "-M");
        add_string(opts, "--no-global-tags");
        add_string(opts, "--no-chapters");

        wxFileName fn(fnTmpMka);
        if (outDir.IsOk())
        {
            fn.MakeRelativeTo(outDir.GetFullPath());
        }
        const wxString dataFilePath = fn.GetFullPath();
        add_string(opts, wxString::Format("=%s", dataFilePath));
    }
    else
    {
        // tracks
        const wxArrayDataFile& dataFiles = cueSheet.GetDataFiles();
        bool                   bFirst = true;

        for (size_t nTracks = dataFiles.GetCount(), i = 0; i < nTracks; ++i)
        {
            add_string(opts, "-a");
            add_string(opts, '0');
            add_string(opts, "-D");
            add_string(opts, "-S");
            add_string(opts, "-B");
            add_string(opts, "-T");
            add_string(opts, "-M");
            add_string(opts, "--no-global-tags");
            add_string(opts, "--no-chapters");

            const wxDataFile& dataFile = dataFiles[i];
            wxASSERT(dataFile.HasRealFileName());

            wxFileName fn = dataFile.GetRealFileName();
            if (outDir.IsOk())
            {
                fn.MakeRelativeTo(outDir.GetFullPath());
            }
            const wxString dataFilePath = fn.GetFullPath();

            if (!bFirst)
            {
                add_string(opts, wxString::Format("+%s", dataFilePath));
            }
            else
            {
                add_string(opts, wxString::Format("=%s", dataFilePath));
                bFirst = false;
            }
        }
    }

    wxArrayMatroskaAttachment attachments;

    // cover - must be a first attachment
    if (m_cfg.AttachCover())
    {
        AppendCoverAttachments(attachments, inputFile, tmpStem, cueSheet.GetCovers());
    }

    AppendCdTextFilesAttachments(attachments, inputFile, cueSheet.GetCdTextFiles());

    // log
    if (m_cfg.AttachEacLog())
    {
        AppendLogFilesAttachments(attachments, inputFile, cueSheet.GetLogs());
    }

    AppendEacFilesAttachments(attachments, inputFile, tmpStem, cueSheet);

    // accurip
    if (m_cfg.AttachAccurateRipLog())
    {
        AppendAccuripLogAttachments(attachments, cueSheet.GetAccurateRipLogs());
    }
    
    MakeRelativePaths(attachments, outDir);

    if (ConvertCover(cueSheet))
    {
        fnImg = m_cfg.GetTemporaryImageFile(inputFile, tmpStem);
        bool rendered = false;
        if (outDir.IsOk())
        {
            wxFileName fnImgRel(fnImg);
            if (fnImgRel.MakeRelativeTo(outDir.GetFullPath()))
            {
                render_attachments(opts, attachments, fnImgRel);
                rendered = true;
            }
        }
        if (!rendered)
        {
            render_attachments(opts, attachments, fnImg);
        }
        coverAttachment = attachments[0];
        m_temporaryFiles.Add(fnImg);
    }
    else
    {
        render_attachments(opts, attachments, fnImg);
    }

    // post
    if (m_cfg.IsLangDefined())
    {
        add_string(opts, "--default-language");
        add_string(opts, m_cfg.GetLang());
    }
    add_string(opts, "--title");
    add_string(opts, trackName);
    add_string(opts, "--chapters");
    add_string(opts, GetRelativeFileName(chaptersFile, outDir).GetFullPath());

    if (m_cfg.GenerateTags())
    {
        add_string(opts, "--global-tags");
        add_string(opts, GetRelativeFileName(tagsFile, outDir).GetFullPath());
    }

    if (!fnTmpMka.IsOk() && (cueSheet.GetDataFilesCount() > 1u))
    {
        add_string(opts, "--append-to");
        add_string(opts, get_mapping_str(cueSheet));
    }

    const wxString j = wxString::FromUTF8Unchecked(opts.dump(2));
    *m_os << j << endl;
}

void wxMkvmergeOptsRenderer::RenderScript(
    const wxInputFile& inputFile,
    const wxCueSheet& cueSheet,
    const wxString& tmpStem,
    const wxFileName& optionsFile,
    const wxFileName& mkaFile,
    const wxMatroskaAttachment& coverAttachment,
    const wxFileName& fnImg)
{
    RenderHeader();
    RenderMinimumVersion();
    RenderToolEnvCheck("mkvmerge");
    if (fnImg.IsOk())
    {
        RenderToolEnvCheck("imagick");
        if (coverAttachment.IsPdf())
        {
            RenderToolEnvCheck("mutool");
        }
    }
    *m_os << "SET(CUE2MKC_STEM " << tmpStem << ')' << endl << endl;

    wxFileName outDir;
    if (!m_cfg.UseFullPaths())
    {
        outDir = m_cfg.GetOutputDir(inputFile);
    }

    {
        wxFileName dstMkaFile = m_cfg.GetOutputDir(inputFile);
        dstMkaFile.SetName(GetContainerFileName(cueSheet));
        dstMkaFile.SetExt(wxConfiguration::EXT::MATROSKA_AUDIO);

        *m_os << endl << "CMAKE_PATH(SET MKA_FNAME \"" << GetCMakePath(GetRelativeFileName(dstMkaFile, outDir)) << "\")" << endl << endl;
    }

    *m_os << endl << "CMAKE_PATH(SET TMP_MKA_FNAME \"" << GetCMakePath(GetRelativeFileName(mkaFile, outDir)) << "\")" << endl << endl;

    if (fnImg.IsOk())
    {
        *m_os << endl;
        *m_os << "CMAKE_PATH(SET CUE2MKC_SRC_IMG \"" << GetCMakePath(coverAttachment.GetFileName()) << "\")" << endl;
        *m_os << "CMAKE_PATH(SET CUE2MKC_DST_IMG \"" << GetCMakePath(GetRelativeFileName(fnImg, outDir)) << "\")" << endl;

        wxFileName mkcover(wxStandardPaths::Get().GetExecutablePath());
        mkcover.SetFullName("mkcover.cmake");
        *m_os << "INCLUDE(\"" << GetCMakePath(mkcover) << "\")" << endl << endl;
    }

    *m_os << "MESSAGE(STATUS \"Creating MKA container (via mkvmerge)\")" << endl;
    *m_os << "EXECUTE_PROCESS(" << endl;
    *m_os << "    COMMAND ${MKVMERGE}" << endl;
    *m_os << "        --ui-language en --output-charset utf-8" << endl;
    if (!wxLog::GetVerbose())
    {
        *m_os << "        --quiet" << endl;
    }
    *m_os << "        \"@" << GetCMakePath(GetRelativeFileName(optionsFile, outDir)) << '\"' << endl;
    *m_os << "    ENCODING UTF-8" << endl;
    *m_os << "    COMMAND_ECHO NONE" << endl;
    *m_os << "    COMMAND_ERROR_IS_FATAL ANY" << endl;
    if (!m_cfg.UseFullPaths())
    {
        *m_os << "    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}" << endl;
    }
    *m_os << ')' << endl;
    *m_os << "MESSAGE(STATUS \"Replacing MKA container\")" << endl;
    *m_os << "FILE(RENAME ${TMP_MKA_FNAME} ${MKA_FNAME})" << endl;
}

bool wxMkvmergeOptsRenderer::Save(const wxFileName& matroskaOptsFile)
{
    wxFileOutputStream os(matroskaOptsFile.GetFullPath());

    if (os.IsOk())
    {
        wxLogInfo(_wxS("Creating mkvmerge options file " ENQUOTED_STR_FMT), matroskaOptsFile.GetFullName());
        wxTextOutputStream stream(os, wxEOL_NATIVE, wxConvUTF8);
        m_os.SaveTo(stream);
        m_temporaryFiles.Add(matroskaOptsFile);
        return true;
    }
    else
    {
        wxLogError(_wxS("Fail to save options to " ENQUOTED_STR_FMT), matroskaOptsFile.GetFullName());
        return false;
    }
}


