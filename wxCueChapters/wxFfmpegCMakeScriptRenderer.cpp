/*
 * wxFfmpegCMakeScriptRenderer.cpp
 */

#include "wxConfiguration.h"
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#include "wxFfmpegCMakeScriptRenderer.h"

// ===============================================================================

wxFfmpegCMakeScriptRenderer::wxFfmpegCMakeScriptRenderer(const wxConfiguration& cfg):
    wxCMakeScriptRenderer(cfg)
{
}

namespace
{
    wxString make_ffmpeg_concat_filter(size_t cnt, bool mono)
    {
        if (cnt == 1 && mono)
        {
            return "[0:a:0]pan=mono|c0=FL[outa]";
        }

        wxString res;
        for (size_t i = 0; i < cnt; ++i)
        {
            res << wxString::Format("[%" wxSizeTFmtSpec "d:a:0]", i);
        }
        res << wxString::Format("concat=n=%" wxSizeTFmtSpec "d:v=0:a=1", cnt);
        if (mono)
        {
            res << ",pan=mono|c0=FL"; // left channel
        }
        res << "[outa]";
        return res;
    }

    unsigned short get_audio_sample_depth(const wxArrayDataFile& dataFiles)
    {
        unsigned short bitsPerSample = 16;
        for (size_t i = 0, cnt = dataFiles.GetCount(); i < cnt; ++i)
        {
            if (!dataFiles[i].HasDuration()) continue;
            const unsigned short bps = dataFiles[i].GetDuration().GetSamplingInfo().GetBitsPerSample();
            if (bps > bitsPerSample) bitsPerSample = bps;
        }
        return bitsPerSample;
    }

    wxTextOutputStream& put_audio_sample_fmt(wxTextOutputStream& os, const wxArrayDataFile& dataFiles, bool p)
    {
        unsigned short bitsPerSample = get_audio_sample_depth(dataFiles);
        os << "-sample_fmt:a ";
        if (bitsPerSample <= 16)
        {
            os << "s16";
            if (p) os << 'p';
        }
        else if (bitsPerSample > 16 && bitsPerSample < 32)
        {
            os << "s32";
            if (p) os << 'p';
            os << " -bits_per_raw_sample:a " << bitsPerSample;
        }
        else if (bitsPerSample == 32)
        {
            os << "s32";
            if (p) os << 'p';
        }
        else if (bitsPerSample > 32 && bitsPerSample < 64)
        {
            os << "s64";
            if (p) os << 'p';
            os << " -bits_per_raw_sample:a " << bitsPerSample;
        }
        else
        {
            os << "s64";
        }

        return os;
    }

    wxString get_uncompressed_audio_codec(const wxArrayDataFile& dataFiles, bool bigEndian)
    {
        wxString res;

        res << "pcm_s" << get_audio_sample_depth(dataFiles);
        if (bigEndian)
            res << "be";
        else
            res << "le";
        return res;
    }

    wxTextOutputStream& flac_codec(wxTextOutputStream& os, const wxArrayDataFile& dataFiles)
    {
        put_audio_sample_fmt(os << "        ", dataFiles, false) << endl;
        os << "        -lpc_type levinson " << endl;
        os << "        -ch_mode indep" << endl;
        os << "        -exact_rice_parameters 1" << endl;
        os << "        -c:a flac" << endl << endl;

        return os;
    }

    wxTextOutputStream& flac0_codec(wxTextOutputStream& os, const wxArrayDataFile& dataFiles)
    {
        put_audio_sample_fmt(os << "        ", dataFiles, false) << endl;
        os << "        -compression_level 0 " << endl;
        os << "        -c:a flac" << endl << endl;

        return os;
    }

    wxTextOutputStream& wavpack_codec(wxTextOutputStream& os, const wxArrayDataFile& dataFiles)
    {
        put_audio_sample_fmt(os << "        ", dataFiles, true) << endl;
        os << "        -joint_stereo 0" << endl;
        os << "        -c:a wavpack" << endl << endl;

        return os;
    }
}


wxString wxFfmpegCMakeScriptRenderer::RenderDiscDraft(const wxCueSheet& cueSheet, const wxFileName& workDir, const wxString& tmpStem) const
{
    wxFileName relDir;
    if (!m_cfg.UseFullPaths())
    {
        relDir = workDir;
    }

    wxTextOutputStreamOnString os;

    *os << "CMAKE_MINIMUM_REQUIRED(VERSION 3.21)" << endl;
    *os << "SET(FFSTEM \"" << tmpStem << "\")" << endl;
    *os << "CMAKE_PATH(SET CUE2MKC_WORKDIR \"" << GetCMakePath(workDir) << "\")" << endl;

    const wxArrayDataFile& dataFiles = cueSheet.GetDataFiles();
    for (size_t i = 0, cnt = dataFiles.GetCount(); i < cnt; ++i)
    {
        const wxDataFile& dataFile = dataFiles[i];
        wxASSERT(dataFile.HasRealFileName());
        const wxFileName fn = dataFile.GetRealFileName();
        WriteSizeT(*os << "CMAKE_PATH(SET CUE2MKC_AUDIO_", i) << " \"" << GetCMakePath(GetRelativeFileName(fn, relDir)) << "\")" << endl;
    }

    *os << "CMAKE_PATH(SET CUE2MKC_CHAPTERS \"${FFSTEM}-chapters.json\")" << endl;
    *os << "CMAKE_PATH(SET CUE2MKC_MKA \"${FFSTEM}.mka\")" << endl;
    *os << "CMAKE_PATH(SET CUE2MKC_DST \"${FFSTEM}-rg.json\")" << endl;
    *os << "MESSAGE(STATUS \"Creating temporary MKA container\")" << endl;
    *os << "EXECUTE_PROCESS(" << endl;
    *os << "    COMMAND ${FFMPEG}" << endl;
    *os << "        -y" << endl;
    *os << "        -hide_banner -nostdin -nostats" << endl;
    *os << "        -loglevel repeat+level+warning" << endl;
    *os << "        -threads 1" << endl;

    for (size_t i = 0, cnt = dataFiles.GetCount(); i < cnt; ++i)
    {
        WriteSizeT(*os << "        -i ${CUE2MKC_AUDIO_", i) << '}' << endl;
    }

    if (m_cfg.JoinMode() || m_cfg.IsDualMono())
    {
        *os << "        -filter_complex_threads 1" << endl;
        *os << "        -filter_complex " << make_ffmpeg_concat_filter(dataFiles.GetCount(), m_cfg.IsDualMono()) << endl;
        *os << "        -map [outa]" << endl;
    }
    else
    {
        *os << "        -map 0:a:0" << endl;
    }

    *os << "        -map_metadata -1" << endl;
    *os << "        -map_chapters -1" << endl;
    flac0_codec(*os, dataFiles);
    *os << "        ${CUE2MKC_MKA}" << endl;
    *os << "    ENCODING UTF-8" << endl;
    *os << "    COMMAND_ECHO NONE" << endl;
    *os << "    COMMAND_ERROR_IS_FATAL ANY" << endl;
    if (!m_cfg.UseFullPaths())
    {
        *os << "    WORKING_DIRECTORY ${CUE2MKC_WORKDIR}" << endl;
    }
    *os << ')' << endl;

    {
        wxFileName ffScan(wxStandardPaths::Get().GetExecutablePath());
        ffScan.SetFullName("ff-scan.cmake");
        *os << "INCLUDE(\"" << GetCMakePath(ffScan) << "\")" << endl;
    }
    *os << "CMAKE_PATH(APPEND CUE2MKC_WORKDIR ${CUE2MKC_MKA} OUTPUT_VARIABLE CUE2MKC_MKA_PATH)" << endl;
    *os << "CMAKE_PATH(APPEND CUE2MKC_WORKDIR ${CUE2MKC_CHAPTERS} OUTPUT_VARIABLE CUE2MKC_CHAPTERS_PATH)" << endl;
    *os << "FILE(REMOVE ${CUE2MKC_MKA_PATH} ${CUE2MKC_CHAPTERS_PATH})" << endl;

    os->Flush();
    return os.GetString();
}

void wxFfmpegCMakeScriptRenderer::RenderDisc(const wxCueSheet& cueSheet, const wxInputFile& inputFile, const wxFileName& metadataFile)
{
    RenderHeader(inputFile);
    RenderMinimumVersion();
    RenderFfmpegFinder();

    const wxFileName outputDir = m_cfg.GetOutputDir(inputFile);
    RenderWorkingDirectoryVariable(outputDir);

    wxFileName outDir;

    if (!m_cfg.UseFullPaths())
    {
        outDir = m_cfg.GetOutputDir(inputFile);
    }

    *m_os << endl << "# audio file(s)" << endl;
    const wxArrayDataFile& dataFiles = cueSheet.GetDataFiles();
    for (size_t i = 0, cnt = dataFiles.GetCount(); i < cnt; ++i)
    {
        const wxDataFile& dataFile = dataFiles[i];
        wxASSERT(dataFile.HasRealFileName());
        const wxFileName fn = dataFile.GetRealFileName();
        WriteSizeT(*m_os << "CMAKE_PATH(SET CUE2MKC_AUDIO_", i) << " \"" << GetCMakePath(GetRelativeFileName(fn, outDir)) << "\")" << endl;
    }

    *m_os << endl << "# timestamp" << endl;
    *m_os << "IF(DEFINED ENV{SOURCE_DATE_EPOCH})" << endl;
    *m_os << "    STRING(TIMESTAMP MKA_TS UTC)" << endl;
    *m_os << "ELSE()" << endl;
    *m_os << "    FILE(TIMESTAMP ${CUE2MKC_AUDIO_0} MKA_TS UTC)" << endl;
    *m_os << "    FILE(TIMESTAMP ${CUE2MKC_AUDIO_0} MKA_EPOCH \"%s\" UTC)" << endl;
    *m_os << "    SET(ENV{SOURCE_DATE_EPOCH} ${MKA_EPOCH})" << endl;
    *m_os << "ENDIF()" << endl << endl;

    *m_os << "# metadata file, chapters" << endl;
    *m_os << "CMAKE_PATH(SET CUE2MKC_METADATA \"" << GetCMakePath(GetRelativeFileName(metadataFile, outDir)) << "\")" << endl << endl;

    wxArrayMatroskaAttachment attachments;

    if (m_cfg.AttachCover())
    {
        AppendCoverAttachments(attachments, inputFile, cueSheet.GetCovers());
    }

    AppendCdTextFilesAttachments(attachments, inputFile, cueSheet.GetCdTextFiles());

    if (m_cfg.AttachEacLog())
    {
        AppendLogFilesAttachments(attachments, inputFile, cueSheet.GetLogs());
    }

    AppendEacFilesAttachments(attachments, inputFile, cueSheet);

    if (m_cfg.AttachAccurateRipLog())
    {
        AppendAccuripLogAttachments(attachments, cueSheet.GetAccurateRipLogs());
    }

    MakeRelativePaths(attachments, outDir);

    *m_os << "# attachments" << endl;
    for (size_t i = 0, cnt = attachments.size(); i < cnt; ++i)
    {
        const wxMatroskaAttachment& a = attachments[i];
        WriteSizeT(*m_os << "CMAKE_PATH(SET CUE2MKC_ATTACHMENT_", i) << " \"" <<
            GetCMakePath(a.GetFileName()) << "\")" << endl;
    }

    {
        const wxFileName mkaFile = m_cfg.GetOutputFile(inputFile, wxConfiguration::EXT::MATROSKA_AUDIO);
        *m_os << endl << "CMAKE_PATH(SET MKA_FNAME \"" << GetCMakePath(GetRelativeFileName(mkaFile, outDir)) << "\")" << endl << endl;
    }

    *m_os << "EXECUTE_PROCESS(" << endl;
    *m_os << "    # ffmpeg" << endl;
    *m_os << "    COMMAND ${FFMPEG}" << endl;
    *m_os << "        # global options" << endl;
    *m_os << "        -y" << endl;
    *m_os << "        -hide_banner -nostdin -nostats" << endl;
    *m_os << "        -loglevel repeat+level+" << (wxLog::GetVerbose() ? "info" : "warning") << endl << endl;
    *m_os << "        -threads 1" << endl;

    *m_os << "        # input file(s)" << endl;
    for (size_t i = 0, cnt = dataFiles.GetCount(); i < cnt; ++i)
    {
        *m_os << "        -bitexact" << endl;
        WriteSizeT(*m_os << "        -i ${CUE2MKC_AUDIO_", i) << '}' << endl;
    }

    *m_os << endl << "        # global metadata file (including chapters)" << endl;
    *m_os << "        -bitexact" << endl;
    *m_os << "        -i ${CUE2MKC_METADATA}" << endl << endl;

    if (m_cfg.JoinMode() || m_cfg.IsDualMono())
    {
        *m_os << "        # concatenating audio sources" << endl;
        *m_os << "        -filter_complex_threads 1" << endl;
        *m_os << "        -filter_complex " << make_ffmpeg_concat_filter(dataFiles.GetCount(), m_cfg.IsDualMono()) << endl << endl;
    }

    *m_os << "        # attachment(s)" << endl;
    for (size_t i = 0, cnt = attachments.size(); i < cnt; ++i)
    {
        WriteSizeT(*m_os << "        -attach ${CUE2MKC_ATTACHMENT_", i) << '}' << endl;
    }
    *m_os << endl;

    if (m_cfg.JoinMode() || m_cfg.IsDualMono())
    {
        *m_os << "        # copy processed autio stream" << endl;
        *m_os << "        -map [outa]" << endl << endl;
    }
    else
    {
        *m_os << "        # copy first autio track" << endl;
        *m_os << "        -map 0:a:0" << endl << endl;
    }

    *m_os << "        # global metadata" << endl;
    WriteSizeT(*m_os << "        -map_metadata:g ", dataFiles.GetCount()) << endl;
    *m_os << "        -metadata:g \"creation_time=${MKA_TS}\"" << endl << endl;

    *m_os << "        # audio track metadata" << endl;
    if (!m_cfg.IsUnkLang())
    {
        *m_os << "        -metadata:s:a:0 language=" << m_cfg.GetLang() << endl;
    }
    *m_os << "        -metadata:s:a:0 \"title=" << GetTrackName(cueSheet) << '\"' << endl << endl;

    for (size_t i = 0, cnt = attachments.size(); i < cnt; ++i)
    {
        const wxMatroskaAttachment& a = attachments[i];
        WriteSizeT(*m_os << "        # attachment #", i) << " metadata" << endl;
        WriteSizeT(*m_os << "        -metadata:s:t:", i) << " \"filename=" << a.GetName() << '\"' << endl;

        wxASSERT(a.HasMimeType());
        WriteSizeT(*m_os << "        -metadata:s:t:", i) << " \"mimetype=" << a.GetMimeType() << '\"' << endl;

        if (a.HasDescription())
        {
            WriteSizeT(*m_os << "        -metadata:s:t:", i) << " \"title=" << a.GetDescription() << '\"' << endl << endl;
        }
        else
        {
            *m_os << endl << endl;
        }
    }

    switch (m_cfg.GetFfmpegCodec())
    {
        case wxConfiguration::CODEC_PCM_LE:
        *m_os << "        # uncompressed audio" << endl;
        *m_os << "        -c:a " << get_uncompressed_audio_codec(dataFiles, false) << endl << endl;
        break;

        case wxConfiguration::CODEC_PCM_BE:
        *m_os << "        # uncompressed audio" << endl;
        *m_os << "        -c:a " << get_uncompressed_audio_codec(dataFiles, true) << endl << endl;
        break;

        case wxConfiguration::CODEC_FLAC:
        *m_os << "        # use FLAC codec" << endl;
        flac_codec(*m_os, dataFiles);
        break;

        case wxConfiguration::CODEC_WAVPACK:
        *m_os << "        # use WavPack codec" << endl;
        wavpack_codec(*m_os, dataFiles);
        break;

        case wxConfiguration::CODEC_DEFAULT:
        default:
        if (m_cfg.JoinMode() || m_cfg.IsDualMono())
        {
            *m_os << "        # reencode audio" << endl;
            flac_codec(*m_os, dataFiles);
        }
        else
        {
            *m_os << "        # copy audio stream" << endl;
            *m_os << "        -c:a copy" << endl << endl;
        }
        break;
    }

    *m_os << "        # output opitons" << endl;
    *m_os << "        -bitexact" << endl;
    *m_os << "        -cluster_time_limit 1000" << endl;
    *m_os << "        -default_mode infer_no_subs" << endl << endl;

    *m_os << "        # destination file" << endl;
    *m_os << "        ${MKA_FNAME}" << endl << endl;

    *m_os << "    ENCODING UTF-8" << endl;
    *m_os << "    COMMAND_ECHO NONE" << endl;
    *m_os << "    COMMAND_ERROR_IS_FATAL ANY" << endl;
    if (!m_cfg.UseFullPaths())
    {
        *m_os << "    WORKING_DIRECTORY ${CUE2MKC_WORKDIR}" << endl;
    }
    *m_os << ')' << endl;
}

bool wxFfmpegCMakeScriptRenderer::SaveDraft(const wxString& script, const wxFileName& workDir, const wxString& tmpStem, wxFileName& scriptFile, wxFileName& scanFile) const
{
    scriptFile = workDir;
    scriptFile.SetName(tmpStem);
    scriptFile.SetExt("cmake");

    scanFile = workDir;

    wxString scanFileName(tmpStem);
    scanFileName += "-rg";

    scanFile.SetName(scanFileName);
    scanFile.SetExt("json");

    wxFileOutputStream os(scriptFile.GetFullPath());
    if (os.IsOk())
    {
        wxLogInfo(_("Creating temporary CMake script \u201C%s\u201D"), scriptFile.GetFullName());
        wxSharedPtr< wxTextOutputStream > pStream(wxTextOutputStreamWithBOMFactory::CreateUTF8(os, wxEOL_NATIVE, true, false));
        pStream->WriteString(script);
        pStream->Flush();

        return true;
    }
    else
    {
        wxLogError(_("Fail to save temporary CMake script to \u201C%s\u201D"), scriptFile.GetFullName());
        return false;
    }
}

bool wxFfmpegCMakeScriptRenderer::Save(const wxFileName& outputFile)
{
    wxFileOutputStream os(outputFile.GetFullPath());

    if (os.IsOk())
    {
        wxLogInfo(_("Creating CMake script \u201C%s\u201D"), outputFile.GetFullName());
        wxSharedPtr< wxTextOutputStream > pStream(wxTextOutputStreamWithBOMFactory::CreateUTF8(os, wxEOL_NATIVE, true, false));
        m_os.SaveTo(*pStream);
        return true;
    }
    else
    {
        wxLogError(_("Fail to save CMake script to \u201C%s\u201D"), outputFile.GetFullName());
        return false;
    }
}

