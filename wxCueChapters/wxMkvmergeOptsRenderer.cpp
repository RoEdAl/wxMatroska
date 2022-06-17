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

    void render_attachments(wxJson& opts, const wxArrayMatroskaAttachment& attachments)
    {
        for (size_t i = 0, cnt = attachments.GetCount(); i < cnt; ++i)
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

wxMkvmergeOptsRenderer::wxMkvmergeOptsRenderer(const wxConfiguration& cfg):
    wxPrimitiveRenderer(cfg)
{
}

void wxMkvmergeOptsRenderer::RenderDisc(
    const wxInputFile& inputFile,
    const wxCueSheet& cueSheet)
{
    wxFileName chaptersFile, tagsFile, mkaFile, matroskaOptsFile;
    wxJson opts;

    chaptersFile = m_cfg.GetOutputFile(inputFile, wxConfiguration::EXT::MATROSKA_CHAPTERS);
    if (m_cfg.GenerateTags())
    {
        tagsFile = m_cfg.GetOutputFile(inputFile, wxConfiguration::EXT::MATROSKA_TAGS);
    }
    mkaFile = m_cfg.GetOutputFile(inputFile, wxConfiguration::EXT::MATROSKA_AUDIO);
    matroskaOptsFile = m_cfg.GetOutputFile(inputFile, wxConfiguration::EXT::MKVMERGE_OPTIONS);

    const wxString trackName = GetTrackName(cueSheet);
    wxFileName outDir;

    if (!m_cfg.UseFullPaths())
    {
        outDir = m_cfg.GetOutputDir(inputFile);
    }

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

    wxArrayMatroskaAttachment attachments;

    // cover - must be a first attachment
    if (m_cfg.AttachCover())
    {
        AppendCoverAttachments(attachments, inputFile, cueSheet.GetCovers());
    }

    AppendCdTextFilesAttachments(attachments, inputFile, cueSheet.GetCdTextFiles());

    // log
    if (m_cfg.AttachEacLog())
    {
        AppendLogFilesAttachments(attachments, inputFile, cueSheet.GetLogs());
    }

    AppendEacFilesAttachments(attachments, inputFile, cueSheet);

    // accurip
    if (m_cfg.AttachAccurateRipLog())
    {
        AppendAccuripLogAttachments(attachments, cueSheet.GetAccurateRipLogs());
    }
    
    MakeRelativePaths(attachments, outDir);
    render_attachments(opts, attachments);

    // post
    if (!m_cfg.IsUnkLang())
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

    if (dataFiles.GetCount() > 1u)
    {
        add_string(opts, "--append-to");
        add_string(opts, get_mapping_str(cueSheet));
    }

    const wxString j = wxString::FromUTF8Unchecked(opts.dump(2));

    *m_os << j << endl;
}

bool wxMkvmergeOptsRenderer::Save(const wxFileName& matroskaOptsFile)
{
    wxFileOutputStream os(matroskaOptsFile.GetFullPath());

    if (os.IsOk())
    {
        wxLogInfo(_("Creating mkvmerge options file \u201C%s\u201D"), matroskaOptsFile.GetFullName());
        wxTextOutputStream stream(os, wxEOL_NATIVE, wxConvUTF8);
        m_os.SaveTo(stream);
        m_temporaryFiles.Add(matroskaOptsFile);
        return true;
    }
    else
    {
        wxLogError(_("Fail to save options to \u201C%s\u201D"), matroskaOptsFile.GetFullName());
        return false;
    }
}

