/*
 * wxFfmpegCMakeScriptRenderer.cpp
 */

#include "wxConfiguration.h"
#include "wxFfmpegCMakeScriptRenderer.h"

// ===============================================================================

wxFfmpegCMakeScriptRenderer::wxFfmpegCMakeScriptRenderer(const wxConfiguration& cfg):
    wxCMakeScriptRenderer(cfg)
{
}

namespace
{
    wxString make_ffmpeg_filter(size_t cnt, bool opus, bool mono)
    {
        wxArrayString filters;

        wxString res;

        for (size_t i = 0; i < cnt; ++i)
        {
            res << wxString::Format("[%" wxSizeTFmtSpec "d:a:0]", i);
        }

        if (cnt > 1)
        {
            filters.Add(wxString::Format("concat=n=%" wxSizeTFmtSpec "d:v=0:a=1", cnt));
        }

        if (mono)
        { // pan - left channel
            filters.Add("pan=mono|c0=FL");
        }

        if (opus)
        { // resample
            filters.Add("aresample=48000:resampler=soxr:dither_method=shibata:precision=24:cutoff=0.91:osf=flt");
        }

        if (filters.IsEmpty())
        {
            wxLogWarning(_("Empty ffmpeg filter - using anull"));
            res << "anull[outa]";
        }
        else
        {
            for (wxArrayString::const_iterator i = filters.begin(), end = filters.end() - 1; i != end; ++i)
            {
                res << *i << ',';
            }
            res << filters.Last() << "[outa]";
        }
        return res;
    }

    wxTextOutputStream& put_audio_sample_fmt(
        wxTextOutputStream& os,
        const wxSamplingInfo& si,
        bool p)
    {
        wxASSERT(si.IsOK());

        wxUint16 bitsPerSample = si.GetBitsPerSample();
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

    wxString get_uncompressed_audio_codec(const wxSamplingInfo& si, bool bigEndian)
    {
        wxString res;

        res << "pcm_s" << si.GetBitsPerSample();
        if (bigEndian)
            res << "be";
        else
            res << "le";
        return res;
    }

    wxTextOutputStream& flac_codec(wxTextOutputStream& os, const wxSamplingInfo& si)
    {
        put_audio_sample_fmt(os << "        ", si, false) << endl;
        os << "        -lpc_type levinson " << endl;
        os << "        -ch_mode indep" << endl;
        os << "        -exact_rice_parameters 1" << endl;
        os << "        -c:a flac" << endl;

        return os;
    }

    wxTextOutputStream& wavpack_codec(wxTextOutputStream& os, const wxSamplingInfo& si)
    {
        put_audio_sample_fmt(os << "        ", si, true) << endl;
        os << "        -joint_stereo 0" << endl;
        os << "        -c:a wavpack" << endl;

        return os;
    }

    wxTextOutputStream& opus_codec(wxTextOutputStream& os, bool mono)
    {
        os << "        -b:a " << (mono? 64 : 96) << 'K' << endl;
        os << "        -vbr on" << endl;
        os << "        -packet_loss 1 -fec 1" << endl;
        os << "        -apply_phase_inv " << (mono? 0 : 1) << endl;
        os << "        -application audio" << endl;
        os << "        -c:a libopus" << endl;

        return os;
    }
}

void wxFfmpegCMakeScriptRenderer::render_ffmpeg_codec(bool manyDataFiles, const wxSamplingInfo& si, bool includeComments) const
{
    wxASSERT(si.IsOK());

    switch (m_cfg.GetFfmpegCodec())
    {
        case wxConfiguration::CODEC_PCM_LE:
        if (includeComments)
        {
            *m_os << "        # uncompressed audio" << endl;
        }
        *m_os << "        -c:a " << get_uncompressed_audio_codec(si, false) << endl;
        break;

        case wxConfiguration::CODEC_PCM_BE:
        if (includeComments)
        {
            *m_os << "        # uncompressed audio" << endl;
        }
        *m_os << "        -c:a " << get_uncompressed_audio_codec(si, true) << endl;
        break;

        case wxConfiguration::CODEC_FLAC:
        if (includeComments)
        {
            *m_os << "        # use FLAC codec" << endl;
        }
        flac_codec(*m_os, si);
        break;

        case wxConfiguration::CODEC_WAVPACK:
        if (includeComments)
        {
            *m_os << "        # use WavPack codec" << endl;
        }
        wavpack_codec(*m_os, si);
        break;

        case wxConfiguration::CODEC_OPUS:
        if (includeComments)
        {
            *m_os << "        # use Opus (lossy!) codec" << endl;
        }
        opus_codec(*m_os, m_cfg.IsDualMono());
        break;

        case wxConfiguration::CODEC_DEFAULT:
        default:
        if (manyDataFiles || m_cfg.IsDualMono() || !m_cfg.UseDefaultAudioSampleWidth())
        {
            if (includeComments)
            {
                *m_os << "        # reencode audio" << endl;
            }
            flac_codec(*m_os, si);
        }
        else
        {
            if (includeComments)
            {
                *m_os << "        # copy audio stream" << endl;
            }
            *m_os << "        -c:a copy" << endl;
        }
        break;
    }
}

void wxFfmpegCMakeScriptRenderer::RenderPre(
    const wxCueSheet& cueSheet,
    const wxFileName& workDir,
    const wxString& tmpStem) const
{
    wxFileName relDir;
    if (!m_cfg.UseFullPaths())
    {
        relDir = workDir;
    }

    *m_os << "CMAKE_MINIMUM_REQUIRED(VERSION 3.21)" << endl;
    RenderToolEnvCheck("ffmpeg");
    *m_os << "SET(CUE2MKC_STEM \"" << tmpStem << "\")" << endl;

    const wxSamplingInfo si = GetSamplingInfo(cueSheet);
    wxASSERT(si.IsOK());

    const wxArrayDataFile& dataFiles = cueSheet.GetDataFiles();
    for (size_t i = 0, cnt = dataFiles.GetCount(); i < cnt; ++i)
    {
        const wxDataFile& dataFile = dataFiles[i];
        wxASSERT(dataFile.HasRealFileName());
        const wxFileName fn = dataFile.GetRealFileName();
        WriteSizeT(*m_os << "CMAKE_PATH(SET CUE2MKC_AUDIO_", i) << " \"" << GetCMakePath(GetRelativeFileName(fn, relDir)) << "\")" << endl;
    }

    if (m_cfg.RunReplayGainScanner())
    {
        *m_os << "CMAKE_PATH(SET CUE2MKC_CHAPTERS \"${CUE2MKC_STEM}-" << wxConfiguration::TMP::CHAPTERS << '.' << wxConfiguration::EXT::JSON << "\")" << endl;
        *m_os << "CMAKE_PATH(SET CUE2MKC_DST \"${CUE2MKC_STEM}-" << wxConfiguration::TMP::RGSCAN << '.' << wxConfiguration::EXT::JSON << "\")" << endl;
    }

    *m_os << "CMAKE_PATH(SET CUE2MKC_MKA \"${CUE2MKC_STEM}-" << wxConfiguration::TMP::PRE << '.' << wxConfiguration::EXT::MKA << "\")" << endl;

    *m_os << "MESSAGE(STATUS \"Creating temporary MKA container (via ffmpeg)\")" << endl;
    *m_os << "EXECUTE_PROCESS(" << endl;
    *m_os << "    COMMAND ${FFMPEG}" << endl;
    *m_os << "        -y" << endl;
    *m_os << "        -hide_banner -nostdin -nostats" << endl;
#ifdef NDEBUG
    *m_os << "        -loglevel repeat+level+fatal" << endl;
#else
    *m_os << "        -loglevel repeat+level+" << (wxLog::GetVerbose() ? "info" : "warning") << endl;
#endif
    *m_os << "        -threads 1" << endl;

    for (size_t i = 0, cnt = dataFiles.GetCount(); i < cnt; ++i)
    {
        *m_os << "        -bitexact" << endl;
        WriteSizeT(*m_os << "        -i ${CUE2MKC_AUDIO_", i) << '}' << endl;
    }

    if (!cueSheet.HasSingleDataFile() || m_cfg.AudioFilteringRequired())
    {
        const bool opus = m_cfg.GetFfmpegCodec() == wxConfiguration::CODEC_OPUS;

        *m_os << "        -filter_complex_threads 1" << endl;
        *m_os << "        -filter_complex " << make_ffmpeg_filter(dataFiles.GetCount(), opus, m_cfg.IsDualMono()) << endl;
        *m_os << "        -map [outa]" << endl;
    }
    else
    {
        *m_os << "        -map 0:a:0" << endl;
    }

    *m_os << "        -map_metadata -1" << endl;
    *m_os << "        -map_chapters -1" << endl;
    render_ffmpeg_codec(cueSheet.HasManyDataFiles(), si, false);
    *m_os << "        -bitexact" << endl;
    *m_os << "        ${CUE2MKC_MKA}" << endl;
    *m_os << "    ENCODING UTF-8" << endl;
    *m_os << "    COMMAND_ECHO NONE" << endl;
    *m_os << "    COMMAND_ERROR_IS_FATAL ANY" << endl;
    if (relDir.IsOk())
    {
        *m_os << "    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}" << endl;
    }
    *m_os << ')' << endl;

    if (m_cfg.RunReplayGainScanner())
    {
        wxFileName ffScan(wxStandardPaths::Get().GetExecutablePath());
        ffScan.SetFullName("dr-scan.cmake");
        *m_os << "INCLUDE(\"" << GetCMakePath(ffScan) << "\")" << endl;
        *m_os << "CMAKE_PATH(APPEND CMAKE_CURRENT_BINARY_DIR ${CUE2MKC_CHAPTERS} OUTPUT_VARIABLE CUE2MKC_CHAPTERS_PATH)" << endl;
        *m_os << "FILE(REMOVE ${CUE2MKC_CHAPTERS_PATH})" << endl;
    }

    m_os->Flush();
}

void wxFfmpegCMakeScriptRenderer::RenderDisc(
    const wxInputFile& inputFile,
    const wxCueSheet& cueSheet,
    const wxString& tmpStem,
    const wxFileName& fnTmpMka,
    const wxFileName& metadataFile)
{
    RenderHeader();
    RenderMinimumVersion();
    RenderToolEnvCheck("ffmpeg");
    const bool convertCover = ConvertCover(cueSheet);
    if (convertCover)
    {
        RenderToolEnvCheck("imagick");
        if (m_cfg.CoverFromPdf())
        {
            RenderToolEnvCheck("mutool");
        }
    }
    *m_os << "SET(CUE2MKC_STEM " << tmpStem << ')' << endl;

    const wxFileName outputDir = m_cfg.GetOutputDir(inputFile);
    wxFileName outDir;

    if (!m_cfg.UseFullPaths())
    {
        outDir = m_cfg.GetOutputDir(inputFile);
    }

    if (fnTmpMka.IsOk())
    {
        // single temporary audio track
        *m_os << "CMAKE_PATH(SET CUE2MKC_AUDIO_0 \"" << GetCMakePath(GetRelativeFileName(fnTmpMka, outDir)) << "\")" << endl;
    }
    else
    {
        *m_os << endl << "# audio file(s)" << endl;
        const wxArrayDataFile& dataFiles = cueSheet.GetDataFiles();
        for (size_t i = 0, cnt = dataFiles.GetCount(); i < cnt; ++i)
        {
            const wxDataFile& dataFile = dataFiles[i];
            wxASSERT(dataFile.HasRealFileName());
            const wxFileName fn = dataFile.GetRealFileName();
            WriteSizeT(*m_os << "CMAKE_PATH(SET CUE2MKC_AUDIO_", i) << " \"" << GetCMakePath(GetRelativeFileName(fn, outDir)) << "\")" << endl;
        }
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
        AppendCoverAttachments(attachments, inputFile, tmpStem, cueSheet.GetCovers());
    }

    AppendCdTextFilesAttachments(attachments, inputFile, cueSheet.GetCdTextFiles());

    if (m_cfg.AttachEacLog())
    {
        AppendLogFilesAttachments(attachments, inputFile, cueSheet.GetLogs());
    }

    AppendEacFilesAttachments(attachments, inputFile, tmpStem, cueSheet);

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
        wxFileName dstMkaFile = m_cfg.GetOutputDir(inputFile);
        dstMkaFile.SetName(GetContainerFileName(cueSheet));
        dstMkaFile.SetExt(wxConfiguration::EXT::MATROSKA_AUDIO);
        
        *m_os << "CMAKE_PATH(SET MKA_FNAME \"" << GetCMakePath(GetRelativeFileName(dstMkaFile, outDir)) << "\")" << endl;
    }

    {
        const wxFileName mkaFile = m_cfg.GetTemporaryFile(inputFile, tmpStem, wxConfiguration::TMP::MKA, wxConfiguration::EXT::MATROSKA_AUDIO);
        *m_os << "CMAKE_PATH(SET TMP_MKA_FNAME \"" << GetCMakePath(GetRelativeFileName(mkaFile, outDir)) << "\")" << endl;
        m_temporaryFiles.Add(mkaFile);
    }

    wxFileName fnImg;
    if (convertCover)
    {
        fnImg = m_cfg.GetTemporaryImageFile(inputFile, tmpStem);

        *m_os << "SET(CUE2MKC_SRC_IMG ${CUE2MKC_ATTACHMENT_0})" << endl;
        *m_os << "CMAKE_PATH(SET CUE2MKC_DST_IMG \"" << GetCMakePath(GetRelativeFileName(fnImg, outDir)) << "\")" << endl;

        wxFileName mkcover(wxStandardPaths::Get().GetExecutablePath());
        mkcover.SetFullName("mkcover.cmake");
        *m_os << "INCLUDE(\"" << GetCMakePath(mkcover) << "\")" << endl << endl;

        m_temporaryFiles.Add(fnImg);
    }

    *m_os << "MESSAGE(STATUS \"Creating MKA container (via ffmpeg)\")" << endl;
    *m_os << "EXECUTE_PROCESS(" << endl;
    *m_os << "    # ffmpeg" << endl;
    *m_os << "    COMMAND ${FFMPEG}" << endl;
    *m_os << "        # global options" << endl;
    *m_os << "        -y" << endl;
    *m_os << "        -hide_banner -nostdin -nostats" << endl;
#ifdef NDEBUG
    *m_os << "        -loglevel repeat+level+fatal" << endl;
#else
    *m_os << "        -loglevel repeat+level+" << (wxLog::GetVerbose() ? "info" : "warning") << endl;
#endif
    *m_os << "        -threads 1" << endl;

    if (fnTmpMka.IsOk())
    {
        *m_os << "        # temporary input file" << endl;
        *m_os << "        -bitexact" << endl;
        *m_os << "        -i ${CUE2MKC_AUDIO_0}" << endl;
    }
    else
    {
        *m_os << "        # input file(s)" << endl;
        for (size_t i = 0, cnt = cueSheet.GetDataFilesCount(); i < cnt; ++i)
        {
            *m_os << "        -bitexact" << endl;
            WriteSizeT(*m_os << "        -i ${CUE2MKC_AUDIO_", i) << '}' << endl;
        }
    }

    *m_os << endl << "        # global metadata file (including chapters)" << endl;
    *m_os << "        -bitexact" << endl;
    *m_os << "        -i ${CUE2MKC_METADATA}" << endl << endl;

    if (!fnTmpMka.IsOk() && (!cueSheet.HasSingleDataFile() || m_cfg.AudioFilteringRequired()))
    {
        const bool opus = m_cfg.GetFfmpegCodec() == wxConfiguration::CODEC_OPUS;

        *m_os << "        # concatenating audio sources" << endl;
        *m_os << "        -filter_complex_threads 1" << endl;
        *m_os << "        -filter_complex " << make_ffmpeg_filter(cueSheet.GetDataFilesCount(), opus, m_cfg.IsDualMono()) << endl << endl;
    }

    *m_os << "        # attachment(s)" << endl;
    if (convertCover)
    {
        *m_os << "        -attach ${CUE2MKC_DST_IMG}" << endl;
    }
    for (size_t i = convertCover? 1 : 0, cnt = attachments.size(); i < cnt; ++i)
    {
        WriteSizeT(*m_os << "        -attach ${CUE2MKC_ATTACHMENT_", i) << '}' << endl;
    }
    *m_os << endl;

    if (!fnTmpMka.IsOk() && (!cueSheet.HasSingleDataFile() || m_cfg.IsDualMono()))
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
    if (fnTmpMka.IsOk())
    {
        *m_os << "        -map_metadata:g 1" << endl;
    }
    else
    {
        WriteSizeT(*m_os << "        -map_metadata:g ", cueSheet.GetDataFilesCount()) << endl;
    }
    *m_os << "        -metadata:g \"creation_time=${MKA_TS}\"" << endl << endl;

    *m_os << "        # audio track metadata" << endl;
    if (m_cfg.IsLangDefined())
    {
        *m_os << "        -metadata:s:a:0 language=" << m_cfg.GetLang() << endl;
    }
    *m_os << "        -metadata:s:a:0 \"title=" << GetTrackName(cueSheet) << '\"' << endl << endl;

    if (convertCover)
    {
        const wxMatroskaAttachment& a = attachments[0];

        *m_os << "        # attachment #0 metadata" << endl;
        *m_os << "        -metadata:s:t:0 \"filename=" << a.GetName(fnImg.GetExt()) << '\"' << endl;

        wxString mimeType;
        if (wxCoverFile::GetMimeFromExt(fnImg, mimeType))
        {
            *m_os << "        -metadata:s:t:0 \"mimetype=" << mimeType << '\"' << endl;
        }

        if (a.HasDescription())
        {
            *m_os << "        -metadata:s:t:0 \"title=" << a.GetDescription() << '\"' << endl << endl;
        }
        else
        {
            *m_os << endl << endl;
        }

    }
    for (size_t i = convertCover? 1 : 0, cnt = attachments.GetCount(); i < cnt; ++i)
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

    if (fnTmpMka.IsOk())
    {
        *m_os << "        # copy (temporary) audio stream" << endl;
        *m_os << "        -c:a copy" << endl << endl;
    }
    else
    {
        const wxSamplingInfo si = GetSamplingInfo(cueSheet);
        wxASSERT(si.IsOK());

        render_ffmpeg_codec(!cueSheet.HasManyDataFiles(), si, true);
    }

    *m_os << "        # output opitons" << endl;
    *m_os << "        -bitexact" << endl;
    *m_os << "        -cluster_time_limit 1000" << endl;
    *m_os << "        -default_mode infer_no_subs" << endl << endl;

    *m_os << "        # destination file" << endl;
    *m_os << "        ${TMP_MKA_FNAME}" << endl << endl;

    *m_os << "    ENCODING UTF-8" << endl;
    *m_os << "    COMMAND_ECHO NONE" << endl;
    *m_os << "    COMMAND_ERROR_IS_FATAL ANY" << endl;
    if (!m_cfg.UseFullPaths())
    {
        *m_os << "    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}" << endl;
    }
    *m_os << ')' << endl << endl;
    *m_os << "MESSAGE(STATUS \"Replacing MKA container\")" << endl;
    *m_os << "FILE(RENAME ${TMP_MKA_FNAME} ${MKA_FNAME})" << endl;
}

bool wxFfmpegCMakeScriptRenderer::SaveDraft(
    const wxFileName& workDir,
    const wxString& tmpStem,
    wxFileName& scriptFile,
    wxFileName& scanFile)
{
    scriptFile = wxConfiguration::GetTemporaryFile(workDir, tmpStem, wxConfiguration::TMP::PRE, wxConfiguration::EXT::CMAKE);
    scanFile = wxConfiguration::GetTemporaryFile(workDir, tmpStem, wxConfiguration::TMP::RGSCAN, wxConfiguration::EXT::JSON);

    if (SaveScript(scriptFile))
    {
        m_temporaryFiles.Add(scanFile);
        return true;
    }
    else
    {
        return false;
    }
}
