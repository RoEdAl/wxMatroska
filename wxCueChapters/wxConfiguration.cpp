/*
 * wxConfiguration.cpp
 */

#include <enum2str.h>
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxEncodingDetection/wxEncodingDetection.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include "wxConfiguration.h"
#include "wxApp.h"

 // ===============================================================================

const unsigned long wxConfiguration::DEF_CHAPTER_OFFSET_FFMPEG = 0u;
const unsigned long wxConfiguration::DEF_CHAPTER_OFFSET_MKVMERGE = 150u;

const wxInt16 wxConfiguration::DEF_AUDIO_SAMPLE_WIDTH = -1;

const size_t wxConfiguration::EXT::MAX_LEN = 20;
const char wxConfiguration::EXT::MATROSKA_CHAPTERS[] = "mkc.xml";
const char wxConfiguration::EXT::MATROSKA_TAGS[] = "mkt.xml";
const char wxConfiguration::EXT::MKVMERGE_OPTIONS[] = "cmd.json";
const char wxConfiguration::EXT::MATROSKA_AUDIO[] = "mka";
const char wxConfiguration::EXT::CUESHEET[] = "cue";
const char wxConfiguration::EXT::CMAKE_SCRIPT[] = "cmd.cmake";
const char wxConfiguration::EXT::FFMPEG_METADATA[] = "ffm.txt";
const char wxConfiguration::EXT::TXT[] = "txt";
const char wxConfiguration::EXT::XML[] = "xml";
const char wxConfiguration::EXT::JSON[] = "json";
const char wxConfiguration::EXT::CMAKE[] = "cmake";
const char wxConfiguration::EXT::MKA[] = "mka";
const char wxConfiguration::EXT::UNK[] = "unk";
const char wxConfiguration::EXT::JPEG[] = "jpg";
const char wxConfiguration::EXT::WEBP[] = "webp";
const char wxConfiguration::TMP::CMD[] = "cmd";
const char wxConfiguration::TMP::MKC[] = "mkc";
const char wxConfiguration::TMP::MKT[] = "mkt";
const char wxConfiguration::TMP::FFM[] = "ffm";
const char wxConfiguration::TMP::PRE[] = "pre";
const char wxConfiguration::TMP::CHAPTERS[] = "ctr";
const char wxConfiguration::TMP::RGSCAN[] = "rg2";
const char wxConfiguration::TMP::IMG[] = "img";
const char wxConfiguration::TMP::EMBEDDED[] = "mbd";
const char wxConfiguration::TMP::RENDERED[] = "rnr";
const char wxConfiguration::TMP::CONVERTED[] = "cnv";
const char wxConfiguration::TMP::MKA[] = "mka";

const char wxConfiguration::FMT::MKA_CHAPTER[] = "%dp% - %dt% - %tt%";
const char wxConfiguration::FMT::MKA_CONTAINER[] = "%dp% - %dt%";

const char   wxConfiguration::LANG::FILE_URL[] = "http://www.loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt";
const char   wxConfiguration::LANG::FILE_NAME[] = "ISO-639-2_utf-8.txt";
const char   wxConfiguration::LANG::UND[] = "und";

// ===============================================================================

const wxConfiguration::CuesheetAttachModeName wxConfiguration::AttachModeNames[] =
{
    { CUESHEET_ATTACH_NONE, "none" },
    { CUESHEET_ATTACH_SOURCE, "source" },
    { CUESHEET_ATTACH_DECODED, "decoded" },
    { CUESHEET_ATTACH_RENDERED, "rendered" }
};

// ===============================================================================

const wxConfiguration::RenderModeName wxConfiguration::RenderModeNames[] =
{
    { RENDER_CUESHEET, "cuesheet" },
    { RENDER_MKVMERGE_CHAPTERS, "mkvmerge-chapters" },
    { RENDER_MKVMERGE, "mkvmerge" },
    { RENDER_FFMPEG_CHAPTERS, "ffmpeg-chapters" },
    { RENDER_FFMPEG, "ffmpeg" },
    { RENDER_WAV2IMG_CUE_POINTS, "wav2img" }
};

// ===============================================================================

const wxConfiguration::FfmpegCodecName wxConfiguration::FfmpegCodecNames[] = {
    { CODEC_DEFAULT, "default" },
    { CODEC_PCM_LE, "pcmle" },
    { CODEC_PCM_BE, "pcmbe" },
    { CODEC_FLAC, "flac" },
    { CODEC_WAVPACK, "wavpack" },
    { CODEC_OPUS, "opus" },
};

// ===============================================================================

wxString wxConfiguration::GetRenderingModes()
{
    return get_texts(RenderModeNames);
}

// ===============================================================================

const wxConfiguration::INFO_SUBJECT_DESC wxConfiguration::InfoSubjectDesc[] =
{
    { INFO_VERSION, "version" },
    { INFO_USAGE, "usage" },
    { INFO_TOOLS, "tools" },
    { INFO_FORMATTING_DIRECTIVES, "formatting" },
    { INFO_ASCII_TO_UNICODE, "ascii-to-unicode" },
    { INFO_LICENSE, "license" }
};

// ===============================================================================

wxString wxConfiguration::GetInfoSubjectTexts()
{
    return get_texts(InfoSubjectDesc);
}

bool wxConfiguration::FromString(const wxString& s, wxConfiguration::INFO_SUBJECT& e)
{
    return from_string(s, e, InfoSubjectDesc);
}

wxString wxConfiguration::ToString(wxConfiguration::INFO_SUBJECT e)
{
    return to_string(e, InfoSubjectDesc);
}

wxString wxConfiguration::ToString(wxConfiguration::RENDER_MODE e)
{
    return to_string(e, RenderModeNames);
}

bool wxConfiguration::FromString(const wxString& s, wxConfiguration::RENDER_MODE& e)
{
    return from_string(s, e, RenderModeNames);
}

wxString wxConfiguration::ToString(wxConfiguration::FILE_ENCODING eFileEncoding)
{
    wxString s;

    switch (eFileEncoding)
    {
        case ENCODING_LOCAL:
        {
            s = "LOCAL";
            break;
        }

        case ENCODING_UTF8:
        {
            s = "UTF-8";
            break;
        }

        case ENCODING_UTF8_WITH_BOM:
        {
            s = "UTF-8 (BOM)";
            break;
        }

        case ENCODING_UTF16_LE:
        {
            s = "UTF-16LE";
            break;
        }

        case ENCODING_UTF16_LE_WITH_BOM:
        {
            s = "UTF-16LE (BOM)";
            break;
        }

        case ENCODING_UTF16_BE:
        {
            s = "UTF-16BE";
            break;
        }

        case ENCODING_UTF16_BE_WITH_BOM:
        {
            s = "UTF-16BE (BOM)";
            break;
        }

        default:
        {
            s.Printf("UNKNOWN %d", eFileEncoding);
            break;
        }
    }

    return s;
}

bool wxConfiguration::FromString(const wxString& sFileEncoding_, wxConfiguration::FILE_ENCODING& eFileEncoding)
{
    wxString sFileEncoding(sFileEncoding_);

    sFileEncoding.Replace('-', wxEmptyString);
    sFileEncoding.Replace('_', wxEmptyString);

    if (
        sFileEncoding.CmpNoCase("local") == 0 ||
        sFileEncoding.CmpNoCase("default") == 0
        )
    {
        eFileEncoding = ENCODING_LOCAL;
        return true;
    }
    else if (sFileEncoding.CmpNoCase("utf8") == 0)
    {
        eFileEncoding = ENCODING_UTF8;
        return true;
    }
    else if (sFileEncoding.CmpNoCase("utf8bom") == 0)
    {
        eFileEncoding = ENCODING_UTF8_WITH_BOM;
        return true;
    }
    else if (
        sFileEncoding.CmpNoCase("utf16") == 0 ||
        sFileEncoding.CmpNoCase("utf16le") == 0
        )
    {
        eFileEncoding = ENCODING_UTF16_LE;
        return true;
    }
    else if (
        sFileEncoding.CmpNoCase("utf16bom") == 0 ||
        sFileEncoding.CmpNoCase("utf16lebom") == 0
        )
    {
        eFileEncoding = ENCODING_UTF16_LE_WITH_BOM;
        return true;
    }
    else if (sFileEncoding.CmpNoCase("utf16be") == 0)
    {
        eFileEncoding = ENCODING_UTF16_BE;
        return true;
    }
    else if (sFileEncoding.CmpNoCase("utf16bebom") == 0)
    {
        eFileEncoding = ENCODING_UTF16_BE_WITH_BOM;
        return true;
    }
    else
    {
        return false;
    }
}

wxString wxConfiguration::ToString(wxConfiguration::CUESHEET_ATTACH_MODE e)
{
    return to_string(e, AttachModeNames);
}

bool wxConfiguration::FromString(const wxString& sCsAttachMode, wxConfiguration::CUESHEET_ATTACH_MODE& eCsAttachMode, bool& bDefault)
{
    if (sCsAttachMode.CmpNoCase("default") == 0)
    {
        bDefault = true;
        return true;
    }
    else
    {
        bDefault = false;
        return from_string(sCsAttachMode, eCsAttachMode, AttachModeNames);
    }
}

wxString wxConfiguration::ToString(wxConfiguration::FFMPEG_CODEC e)
{
    return to_string(e, FfmpegCodecNames);
}

bool wxConfiguration::FromString(const wxString& ffmpegCodecStr, wxConfiguration::FFMPEG_CODEC& ffmpegCodec)
{
    return from_string(ffmpegCodecStr, ffmpegCodec, FfmpegCodecNames);
}

bool wxConfiguration::ReadLanguagesStrings(wxSortedArrayString& as)
{
    const wxStandardPaths& paths = wxStandardPaths::Get();
    wxFileName             fn(paths.GetExecutablePath());

    fn.SetFullName(LANG::FILE_NAME);

    if (!fn.IsFileReadable())
    {
        wxLogWarning(_wxS("Cannot find language file " ENQUOTED_STR_FMT), fn.GetFullPath());
        wxLogWarning(_("You can find this file at %s"), LANG::FILE_URL);
        return false;
    }

    wxFileInputStream fis(fn.GetFullPath());

    if (!fis.IsOk())
    {
        wxLogDebug(wxS("Cannot open language file " ENQUOTED_STR_FMT), fn.GetFullPath());
        return false;
    }

    as.Clear();
    wxTextInputStream tis(fis);
    size_t            n = 0;

    while (!fis.Eof())
    {
        wxString sLine(tis.ReadLine());

        if (sLine.IsEmpty()) continue;

        wxStringTokenizer tokenizer(sLine, '|');

        if (tokenizer.HasMoreTokens())
        {
            wxString sLang(tokenizer.GetNextToken());

            if (sLang.IsEmpty() || (sLang.Length() > 3)) wxLogDebug("Skipping language %s", sLang);
            else as.Add(sLang);
        }

        if (n++ > 5000)
        {
            wxLogError(_wxS("Too many languages. File " ENQUOTED_STR_FMT " is corrupt"), fn.GetFullName());
            as.Clear();
            return false;
        }
    }

    return true;
}

wxConfiguration::wxConfiguration(void):
    m_infoSubject(INFO_NONE),
    m_sAlternateExtensions(wxEmptyString),
    m_sTrackNameFormat(FMT::MKA_CHAPTER),
    m_sMatroskaNameFormat(FMT::MKA_CONTAINER),
    m_bCorrectQuotationMarks(true),
    m_eRenderMode(RENDER_MKVMERGE),
    m_eFileEncoding(ENCODING_UTF8_WITH_BOM),
    m_bGenerateEditionUID(false),
    m_bRunTool(true),
    m_bTrackOneIndexOne(true),
    m_bAbortOnError(true),
    m_bHiddenIndexes(false),
    m_bJoinMode(false),
    m_bIncludeDiscNumberTag(false),
    m_nReadFlags(wxCueSheetReader::DEF_READ_FLAGS),
    m_nTagSources(wxCueTag::TAG_CD_TEXT | wxCueTag::TAG_CUE_COMMENT | wxCueTag::TAG_MEDIA_METADATA | wxCueTag::TAG_AUTO_GENERATED),
    m_bUseMLang(false),
    m_bUseFullPaths(false),
    m_eCsAttachMode(CUESHEET_ATTACH_NONE),
    m_bRenderArtistForTrack(false),
    m_bRenderMultilineTags(false),
    m_bRenderReplayGainTags(true),
    m_eFfmpegCodec(CODEC_DEFAULT),
    m_bSingleAudioChannel(false),
    m_bRunReplayGainScanner(false),
    m_nAudioSampleWidth(DEF_AUDIO_SAMPLE_WIDTH),
    m_convertCoverFile(false),
    m_convertedCoverFileExt(EXT::JPEG)
{
}

bool wxConfiguration::ReadLanguagesStrings()
{
    return ReadLanguagesStrings(m_asLang);
}

void wxConfiguration::AddCmdLineParams(wxCmdLineParser& cmdLine) const
{
    cmdLine.AddOption("i", "info", wxString::Format(_("Display additional information about [%s]"), GetInfoSubjectTexts()), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);

    cmdLine.AddSwitch("a", "abort-on-error", wxString::Format(_("Abort when conversion errors occurs (default: %s)"), BoolToStr(m_bAbortOnError)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddOption("o", "output", _("Output Matroska chapter file or cue sheet file (see -c option)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    cmdLine.AddOption("od", "output-directory", _("Output directory (default: input directory)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    cmdLine.AddSwitch("ce", "chapter-time-end", wxString::Format(_("Calculate end time of chapters if possible (default: %s)"), BoolToStr(m_chapterTimeEnd)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch("cn", "unknown-chapter-end-to-next-track", wxString::Format(_("If track's end time is unknown set it to next track position using frame offset (default: %s)"), BoolToStr(m_chapterEndTimeFromNextChapter)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddOption("fo", "frame-offset", wxString::Format(_("Offset in frames to use with -cn option (default: %ld for mkvmerge and %ld for ffmpeg)"), DEF_CHAPTER_OFFSET_MKVMERGE, DEF_CHAPTER_OFFSET_FFMPEG), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
    cmdLine.AddSwitch("df", "use-data-files", wxString::Format(_("Use data file(s) to calculate end time of chapters (default: %s)"), BoolToStr(m_useDataFiles)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddOption("x", "alternate-extensions", _("Comma-separated list of alternate extensions of data files (default: none)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    cmdLine.AddOption("mf", "matroska-title-format", wxString::Format(_("Mtroska container's title format (default: %s)"), FMT::MKA_CONTAINER), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    cmdLine.AddOption("f", "track-title-format", wxString::Format(_("Track/chapter title format (default: %s)"), FMT::MKA_CHAPTER), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    cmdLine.AddOption("l", "language", wxString::Format(_("Set language of chapters and tags in Matroska container (default: %s)"), m_lang.value_or(LANG::UND)), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    cmdLine.AddOption("m", "rendering-method", wxString::Format(_("Rendering method [%s] (default: %s)"), GetRenderingModes(), ToString(m_eRenderMode)), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    cmdLine.AddSwitch("xt", "run-selected-tool", wxString::Format(_("Run selected tool (default: %s)"), BoolToStr(m_bRunTool)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch("eu", "generate-edition-uid", wxString::Format(_("Generate edition UID (default: %s)"), BoolToStr(m_bGenerateEditionUID)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddOption("e", "output-encoding", _("Output cue sheet file encoding [local|utf8|utf8_bom|utf16le|utf16le_bom|utf16be|utf16be_bom] (default: utf8_bom)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    cmdLine.AddSwitch("j", "join", wxString::Format(_("Join mode (default: %s)"), BoolToStr(m_bJoinMode)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch("dn", "include-discnumber", wxString::Format(_("Copy DISCNUMBER and TOTALDISCS tags (default: %s)"), BoolToStr(m_bIncludeDiscNumberTag)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);

    // read flags
    cmdLine.AddSwitch("tc", "parse-tags-from-comments", wxString::Format(_("Parse tags from cue sheet comments (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_PARSE_COMMENTS)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "single-media-file", wxString::Format(_("Embedded mode - assume input as single media file without cuesheet (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_SINGLE_MEDIA_FILE)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "read-media-tags", wxString::Format(_("Embedded mode - read tags from media file (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_MEDIA_READ_TAGS)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "attach-eac-log", wxString::Format(_("Attach EAC log file to mkvmerge options file (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_FIND_LOG)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "attach-cover", wxString::Format(_("Attach cover image (cover.*;front.*;album.*) to mkvmerge options file (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_FIND_COVER)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "attach-accurip-log", wxString::Format(_("Attach AccurateRip log (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_FIND_ACCURIP_LOG)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "parent-dir", wxString::Format(_("Search attachments also in parent dir (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_PARENT_DIR)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "apply-tags", wxString::Format(_("Apply tags from related JSON files (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_APPLY_TAGS_FROM_FILE)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddOption(wxEmptyString, "audio-sample-width", _("Set audio sample width (default: auto, accepted values: 16, 24)"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);

    cmdLine.AddSwitch("et", "ellipsize-tags", wxString::Format(_("Tags processing - ellipsize tags - convert last three dots to ellipsis (U+2026) character (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_ELLIPSIZE_TAGS)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch("rs", "remove-extra-spaces", wxString::Format(_("Tags processing - remove extra spaces (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_REMOVE_EXTRA_SPACES)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch("ru", "upper-roman-numerals", wxString::Format(_("Tags processing - convert roman numerals - upper case (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_CONVERT_UPPER_ROMAN_NUMERALS)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch("rl", "lower-roman-numerals", wxString::Format(_("Tags processing - convert roman numerals - lower case (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_CONVERT_LOWER_ROMAN_NUMERALS)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "correct-dashes", wxString::Format(_("Tags processing - correct dashes (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_CORRECT_DASHES)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "small-em-dash", wxString::Format(_("Tags processing - use small em dash (U+FE58) character instead of normal em dash char (U+2014) (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_SMALL_EM_DASH)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch("cq", "correct-quotation-marks", wxString::Format(_("Tags processing - correct quotation marks (default: %s)"), BoolToStr(m_bCorrectQuotationMarks)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch("cf", "number-full-stop", wxString::Format(_("Tags processing - use '<number> full stop' Unicode characters (U-2488 to U-249B, default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_NUMBER_FULL_STOP)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "small-letter-parenthesized", wxString::Format(_("Tags processing - use '<small letter> parenthesized' Unicode characters (U-249C to U-24B5, default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_SMALL_LETTER_PARENTHESIZED)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "ascii-to-unicode", wxString::Format(_("Tags processing - convert some ASCII sequences to Unicode character (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_ASCII_TO_UNICODE)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);

    // advanced options
    cmdLine.AddSwitch(wxEmptyString, "use-mlang", wxString::Format(_("Use MLang library (default: %s)"), BoolToStr(m_bUseMLang)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch("t1i0", "track-01-index-00", wxString::Format(_("For first track use index 00 as beginning of track (default: %s)"), BoolToStr(!m_bTrackOneIndexOne)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "hidden-indexes", wxString::Format(_("Convert indexes greater than one to hidden (sub)chapters (default %s)"), BoolToStr(m_bHiddenIndexes)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddOption(wxEmptyString, "cue-sheet-attach-mode", _("Mode of attaching cue sheet to mkvmerge options file - possible values are: none (default), source, decoded, rendered and default"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    cmdLine.AddSwitch(wxEmptyString, "use-full-paths", wxString::Format(_("Use full paths in mkvmerge options file (default: %s)"), BoolToStr(m_bUseFullPaths)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch("ra", "render-artist-for-track", wxString::Format(_("Render artist for track (default: %s)"), BoolToStr(m_bRenderArtistForTrack)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch("rm", "render-multiline-tags", wxString::Format(_("Render multiline tags (default: %s)"), BoolToStr(m_bRenderMultilineTags)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch("rg", "render-replaygain-tags", wxString::Format(_("Render ReplayGain tags (default: %s)"), BoolToStr(m_bRenderReplayGainTags)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "run-replaygain-scanner", wxString::Format(_("Run ReplayGain scanner on created Matroska container (default: %s)"), BoolToStr(m_bRunReplayGainScanner)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "convert-cover-file", wxString::Format(_("Convert cover file (default: %s)"), BoolToStr(m_convertCoverFile)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddOption(wxEmptyString, "cover-file-ext", wxString::Format(_("Extension of converted cover file - possible values are: default, jpg, jpeg, webp (default: %s)"), m_convertedCoverFileExt), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    cmdLine.AddSwitch(wxEmptyString, "cover-from-pdf", wxString::Format(_("Create cover from PDF (default: %s)"), ReadFlagTestStr(wxCueSheetReader::EC_FIND_PDF)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);

    // tags usage
    cmdLine.AddSwitch(wxEmptyString, "use-cdtext-tags", wxString::Format(_("Use CD-TEXT tags (default: %s)"), TagSourcesTestStr(wxCueTag::TAG_CD_TEXT)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "use-cue-comments-tags", wxString::Format(_("Use tags from cuesheet comments (default: %s)"), TagSourcesTestStr(wxCueTag::TAG_CUE_COMMENT)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
    cmdLine.AddSwitch(wxEmptyString, "use-media-tags", wxString::Format(_("Use tags from media file(s) (default: %s)"), TagSourcesTestStr(wxCueTag::TAG_MEDIA_METADATA)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);

    cmdLine.AddOption(wxEmptyString, "ffmpeg-codec", wxString::Format(_("Use specific FFMPEG codec - possible values are: default, pcmle, pcmbe (default: %s)"), ToString(m_eFfmpegCodec)), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    cmdLine.AddSwitch(wxEmptyString, "mono", wxString::Format(_("Assume input audio as dual mono, use only left channel (default: %s"), BoolToStr(m_bSingleAudioChannel)), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);

    // input files
    cmdLine.AddParam(_("<cue sheet>"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL);
}

bool wxConfiguration::check_ext(const wxString& sExt)
{
    return !sExt.IsEmpty() && (sExt.Length() < EXT::MAX_LEN);
}

bool wxConfiguration::CheckLang(const wxString& sLang) const
{
    if (m_asLang.IsEmpty())
    {
        return !sLang.IsEmpty() && (sLang.Length() <= 3) && sLang.IsAscii();
    }
    else
    {
        int idx = m_asLang.Index(sLang.Lower());
        return (idx != wxNOT_FOUND);
    }
}

bool wxConfiguration::ReadReadFlags(const wxCmdLineParser& cmdLine, const wxString& name, wxCueSheetReader::ReadFlags nReadFlags)
{
    wxCmdLineSwitchState state = cmdLine.FoundSwitch(name);
    bool                 bSwitchValue;

    if (ReadNegatableSwitchValue(cmdLine, name, bSwitchValue))
    {
        m_nReadFlags &= ~nReadFlags;
        m_nReadFlags |= bSwitchValue ? nReadFlags : 0u;
        return true;
    }
    else
    {
        return false;
    }
}

bool wxConfiguration::ReadTagSources(const wxCmdLineParser& cmdLine, const wxString& name, wxCueTag::TagSources nTagSources)
{
    wxCmdLineSwitchState state = cmdLine.FoundSwitch(name);
    bool                 bSwitchValue;

    if (ReadNegatableSwitchValue(cmdLine, name, bSwitchValue))
    {
        m_nTagSources &= ~nTagSources;
        m_nTagSources |= bSwitchValue ? nTagSources : 0u;
        return true;
    }
    else
    {
        return false;
    }
}

bool wxConfiguration::Read(const wxCmdLineParser& cmdLine)
{
    bool     bRes = true;
    wxString s;
    long     v;

    if (cmdLine.Found("i", &s))
    {
        if (!FromString(s, m_infoSubject))
        {
            wxLogWarning(_("Invalid info subject - %s"), s);
            bRes = false;
        }
    }

    ReadNegatableSwitchValue(cmdLine, "ce", m_chapterTimeEnd);
    ReadNegatableSwitchValue(cmdLine, "cn", m_chapterEndTimeFromNextChapter);

    if (cmdLine.Found("fo", &v))
    {
        if ((v < 0) || (v > 10000))
        {
            wxLogWarning(_("Wrong frame offset - %d"), v);
            bRes = false;
        }
        else
        {
            m_chapterOffset = (unsigned long)v;
        }
    }

    ReadNegatableSwitchValue(cmdLine, "df", m_useDataFiles);
    ReadNegatableSwitchValue(cmdLine, "cq", m_bCorrectQuotationMarks);

    if (cmdLine.Found("m", &s))
    {
        if (!FromString(s, m_eRenderMode))
        {
            wxLogWarning(_("Wrong rendering method - %s "), s);
            bRes = false;
        }
    }

    ReadNegatableSwitchValueAndNegate(cmdLine, "t1i0", m_bTrackOneIndexOne);
    ReadNegatableSwitchValue(cmdLine, "a", m_bAbortOnError);
    ReadNegatableSwitchValue(cmdLine, "hidden-indexes", m_bHiddenIndexes);
    ReadNegatableSwitchValue(cmdLine, "xt", m_bRunTool);
    ReadNegatableSwitchValue(cmdLine, "use-full-paths", m_bUseFullPaths);
    ReadNegatableSwitchValue(cmdLine, "ra", m_bRenderArtistForTrack);
    ReadNegatableSwitchValue(cmdLine, "rm", m_bRenderMultilineTags);
    ReadNegatableSwitchValue(cmdLine, "rg", m_bRenderReplayGainTags);
    if (ReadNegatableSwitchValue(cmdLine, "run-replaygain-scanner", m_bRunReplayGainScanner))
    {
        switch (m_eRenderMode)
        {
            case RENDER_MKVMERGE_CHAPTERS:
            case RENDER_MKVMERGE:
            case RENDER_FFMPEG_CHAPTERS:
            case RENDER_FFMPEG:
            break;

            default:
            wxLogWarning(_("RG scanner works only in ffmpeg and mkvmerge rendering mode"));
            break;
        }
    }

    if (cmdLine.Found("e", &s))
    {
        if (!FromString(s, m_eFileEncoding))
        {
            wxLogWarning(_("Wrong output encoding %s"), s);
            bRes = false;
        }
    }

    ReadNegatableSwitchValue(cmdLine, "eu", m_bGenerateEditionUID);
    ReadNegatableSwitchValue(cmdLine, "j", m_bJoinMode);
    ReadNegatableSwitchValue(cmdLine, "dn", m_bIncludeDiscNumberTag);
    ReadNegatableSwitchValue(cmdLine, "mono", m_bSingleAudioChannel);

    if (cmdLine.Found("x", &s)) m_sAlternateExtensions = s;
    if (cmdLine.Found("f", &s)) m_sTrackNameFormat = s;
    if (cmdLine.Found("mf", &s)) m_sMatroskaNameFormat = s;

    if (cmdLine.GetParamCount() > 0)
    {
        for (size_t i = 0; i < cmdLine.GetParamCount(); ++i)
        {
            wxInputFile inputFile(cmdLine.GetParam(i));

            if (inputFile.IsOk())
            {
                m_inputFile.Add(inputFile);
            }
            else
            {
                wxLogWarning(_wxS("Invalid input file " ENQUOTED_STR_FMT), cmdLine.GetParam(i));
                bRes = false;
            }
        }
    }

    if (cmdLine.Found("l", &s))
    {
        if (CheckLang(s))
        {
            const wxString lang = s.Lower();
            if (lang.CmpNoCase(LANG::UND) != 0) m_lang = lang;
        }
        else
        {
            wxLogWarning(_("Invalid laguage %s"), s);
            bRes = false;
        }
    }

    if (cmdLine.Found("o", &s))
    {
        m_outputFile.Assign(s);

        if (!m_outputFile.MakeAbsolute())
        {
            wxLogInfo(_wxS("Fail to normalize path " ENQUOTED_STR_FMT), s);
            bRes = false;
        }
    }

    if (cmdLine.Found("od", &s))
    {
        m_outputFile.AssignDir(s);

        if (!m_outputFile.MakeAbsolute())
        {
            wxLogInfo(_wxS("Fail to normalize path " ENQUOTED_STR_FMT), s);
            bRes = false;
        }
    }

    ReadTagSources(cmdLine, "use-cdtext-tags", wxCueTag::TAG_CD_TEXT);
    ReadTagSources(cmdLine, "use-cue-comments-tags", wxCueTag::TAG_CUE_COMMENT);
    ReadTagSources(cmdLine, "use-media-tags", wxCueTag::TAG_MEDIA_METADATA);

    ReadReadFlags(cmdLine, "tc", wxCueSheetReader::EC_PARSE_COMMENTS);
    ReadReadFlags(cmdLine, "et", wxCueSheetReader::EC_ELLIPSIZE_TAGS);
    ReadReadFlags(cmdLine, "rs", wxCueSheetReader::EC_REMOVE_EXTRA_SPACES);
    ReadReadFlags(cmdLine, "single-media-file", wxCueSheetReader::EC_SINGLE_MEDIA_FILE);
    ReadReadFlags(cmdLine, "read-media-tags", wxCueSheetReader::EC_MEDIA_READ_TAGS);
    ReadReadFlags(cmdLine, "attach-eac-log", wxCueSheetReader::EC_FIND_LOG);
    ReadReadFlags(cmdLine, "attach-accurip-log", wxCueSheetReader::EC_FIND_ACCURIP_LOG);
    ReadReadFlags(cmdLine, "attach-cover", wxCueSheetReader::EC_FIND_COVER);
    ReadReadFlags(cmdLine, "apply-tags", wxCueSheetReader::EC_APPLY_TAGS_FROM_FILE);
    ReadReadFlags(cmdLine, "ru", wxCueSheetReader::EC_CONVERT_UPPER_ROMAN_NUMERALS);
    ReadReadFlags(cmdLine, "rl", wxCueSheetReader::EC_CONVERT_LOWER_ROMAN_NUMERALS);
    ReadReadFlags(cmdLine, "correct-dashes", wxCueSheetReader::EC_CORRECT_DASHES);
    ReadReadFlags(cmdLine, "small-em-dash", wxCueSheetReader::EC_SMALL_EM_DASH);
    ReadReadFlags(cmdLine, "number-full-stop", wxCueSheetReader::EC_NUMBER_FULL_STOP);
    ReadReadFlags(cmdLine, "small-letter-parenthesized", wxCueSheetReader::EC_SMALL_LETTER_PARENTHESIZED);
    ReadReadFlags(cmdLine, "ascii-to-unicode", wxCueSheetReader::EC_ASCII_TO_UNICODE);
    ReadReadFlags(cmdLine, "cover-from-pdf", wxCueSheetReader::EC_FIND_PDF);
    ReadReadFlags(cmdLine, "parent-dir", wxCueSheetReader::EC_PARENT_DIR);

    // MLang
    ReadNegatableSwitchValue(cmdLine, "use-mlang", m_bUseMLang);

    if (cmdLine.Found("cue-sheet-attach-mode", &s))
    {
        bool bDefault;
        if (!FromString(s, m_eCsAttachMode, bDefault))
        {
            wxLogWarning(_("Wrong cue sheet attaching mode %s"), s);
            bRes = false;
        }
    }

    if (cmdLine.Found("ffmpeg-codec", &s))
    {
        if (!FromString(s, m_eFfmpegCodec))
        {
            wxLogWarning(_("Wrong ffmpeg codec %s"), s);
            bRes = false;
        }
    }

    if (cmdLine.Found("audio-sample-width", &v))
    {
        switch (v)
        {
            case 16:
            m_nAudioSampleWidth = 16;
            break;

            case 24:
            m_nAudioSampleWidth = 24;
            break;

            case -1:
            m_nAudioSampleWidth = DEF_AUDIO_SAMPLE_WIDTH;
            break;

            default:
            wxLogWarning(_("Invalid audio sample width: %ld"), v);
            bRes = false;
            break;
        }
    }

    ReadNegatableSwitchValue(cmdLine, "convert-cover-file", m_convertCoverFile);
    if (cmdLine.Found("cover-file-ext", &s))
    {
        if (s.CmpNoCase("default") == 0 || s.CmpNoCase("jpg") == 0 || s.CmpNoCase("jpeg") == 0)
        {
            m_convertedCoverFileExt = EXT::JPEG;
        }
        else if (s.CmpNoCase("webp") == 0)
        {
            m_convertedCoverFileExt = EXT::WEBP;
        }
        else
        {
            wxLogWarning(_("Unknown cover file extension: %s"), s);
            bRes = false;
        }
    }

    return bRes;
}

wxString wxConfiguration::BoolToIdx(bool b)
{
    return b ? "01" : "00";
}

wxString wxConfiguration::ReadFlagTestStr(wxCueSheetReader::ReadFlags n) const
{
    return BoolToStr((m_nReadFlags & n) == n);
}

wxString wxConfiguration::TagSourcesTestStr(wxCueTag::TagSources n) const
{
    return BoolToStr((m_nTagSources & n) == n);
}

void wxConfiguration::AddFlag(wxArrayString& as, wxCueSheetReader::ReadFlags flags, wxCueSheetReader::ReadFlags mask, const wxString& sText)
{
    if ((flags & mask) == mask) as.Add(sText);
}

wxString wxConfiguration::GetReadFlagsDesc(wxCueSheetReader::ReadFlags flags)
{
    wxArrayString as;

    AddFlag(as, flags, wxCueSheetReader::EC_PARSE_COMMENTS, "generate-tags-from-comments");
    AddFlag(as, flags, wxCueSheetReader::EC_ELLIPSIZE_TAGS, "ellipsize-tags");
    AddFlag(as, flags, wxCueSheetReader::EC_REMOVE_EXTRA_SPACES, "remove-extra-spaces");
    AddFlag(as, flags, wxCueSheetReader::EC_SINGLE_MEDIA_FILE, "single-media-file");
    AddFlag(as, flags, wxCueSheetReader::EC_MEDIA_READ_TAGS, "media-tags");
    AddFlag(as, flags, wxCueSheetReader::EC_FIND_COVER, "find-cover");
    AddFlag(as, flags, wxCueSheetReader::EC_FIND_LOG, "find-log");
    AddFlag(as, flags, wxCueSheetReader::EC_FIND_ACCURIP_LOG, "find-accurip-log");
    AddFlag(as, flags, wxCueSheetReader::EC_PARENT_DIR, "parent-dir");
    AddFlag(as, flags, wxCueSheetReader::EC_APPLY_TAGS_FROM_FILE, "apply-tags-from-file");
    AddFlag(as, flags, wxCueSheetReader::EC_CONVERT_UPPER_ROMAN_NUMERALS, "upper-roman-numerals");
    AddFlag(as, flags, wxCueSheetReader::EC_CONVERT_LOWER_ROMAN_NUMERALS, "lower-roman-numerals");
    AddFlag(as, flags, wxCueSheetReader::UNUSED_EC_CONVERT_COVER_TO_JPEG, "convert-cover-to-jpeg");
    AddFlag(as, flags, wxCueSheetReader::EC_CORRECT_DASHES, "correct-dashes");
    AddFlag(as, flags, wxCueSheetReader::EC_SMALL_EM_DASH, "small-em-dash");
    AddFlag(as, flags, wxCueSheetReader::EC_NUMBER_FULL_STOP, "number-full-stop");
    AddFlag(as, flags, wxCueSheetReader::EC_SMALL_LETTER_PARENTHESIZED, "small-letter-parenthesized");
    AddFlag(as, flags, wxCueSheetReader::EC_ASCII_TO_UNICODE, "ascii-to-unicode");
    AddFlag(as, flags, wxCueSheetReader::EC_FIND_PDF, "cover-from-pdf");

    wxString s;

    for (size_t i = 0, nCount = as.GetCount(); i < nCount; ++i)
    {
        s << as[i] << ',';
    }

    return s.RemoveLast();
}

void wxConfiguration::FillArray(wxArrayString& as) const
{
    as.Add(wxString::Format("Rendering method: %s", ToString(m_eRenderMode)));
    as.Add(wxString::Format("Run tool: %s", BoolToStr(m_bRunTool)));
    as.Add(wxString::Format("Generate edition UID: %s", BoolToStr(m_bGenerateEditionUID)));
    as.Add(wxString::Format("Tag sources: %s", wxCueTag::SourcesToString(m_nTagSources)));
    as.Add(wxString::Format("Output file encoding: %s", ToString(m_eFileEncoding)));
    as.Add(wxString::Format("Calculate end time of chapters: %s", BoolToStr(m_chapterTimeEnd)));
    as.Add(wxString::Format("Use data files to calculate end time of chapters: %s", BoolToStr(m_useDataFiles)));
    as.Add(wxString::Format("Alternate media file extensions: %s", m_sAlternateExtensions));
    as.Add(wxString::Format("Set chapter's end time to beginning of next chapter if track's end time cannot be calculated: %s", BoolToStr(m_chapterEndTimeFromNextChapter)));
    as.Add(wxString::Format("Chapter offset (frames): %d", GetChapterOffset()));
    as.Add(wxString::Format("Matroska container title format: %s", m_sMatroskaNameFormat));
    as.Add(wxString::Format("Chapter/track title format: %s", m_sTrackNameFormat));
    as.Add(wxString::Format("Chapter string language: %s", m_lang.value_or(LANG::UND)));
    as.Add(wxString::Format("For track 01 assume index %s as beginning of track", BoolToIdx(m_bTrackOneIndexOne)));
    as.Add(wxString::Format("Join mode: %s", BoolToStr(m_bJoinMode)));
    as.Add(wxString::Format("Dual mono: %s", BoolToStr(m_bSingleAudioChannel)));
    as.Add(wxString::Format("Include DISCNUMBER tag: %s", BoolToStr(m_bIncludeDiscNumberTag)));
    as.Add(wxString::Format("Convert indexes to hidden subchapters: %s", BoolToStr(m_bHiddenIndexes)));
    as.Add(wxString::Format("Correct quotation marks: %s", BoolToStr(m_bCorrectQuotationMarks)));
    as.Add(wxString::Format("Render artist for tracks: %s", BoolToStr(m_bRenderArtistForTrack)));
    as.Add(wxString::Format("Render multiline tags: %s", BoolToStr(m_bRenderMultilineTags)));
    as.Add(wxString::Format("Render ReplayGain tags: %s", BoolToStr(m_bRenderReplayGainTags)));
    as.Add(wxString::Format("Run ReplayGain scanner: %s", BoolToStr(m_bRunReplayGainScanner)));
    as.Add(wxString::Format("Read flags: %s", GetReadFlagsDesc(m_nReadFlags)));
    as.Add(wxString::Format("Use MLang library: %s", BoolToStr(m_bUseMLang)));
    as.Add(wxString::Format("ffmpeg codec: %s", ToString(m_eFfmpegCodec)));
    if (m_convertCoverFile || CoverFromPdf())
    {
        as.Add(wxString::Format("Convert cover file: %s", m_convertedCoverFileExt));
    }
    else
    {
        as.Add(wxString::Format("Convert cover file: %s", BoolToStr(false)));
    }
}

void wxConfiguration::Dump() const
{
    if (wxLog::IsLevelEnabled(wxLOG_Info, wxLOG_COMPONENT) && wxLog::GetVerbose())
    {
        wxString      sSeparator('=', 65);
        wxArrayString as;
        as.Add(sSeparator);
        as.Add(_("Configuration:"));
        FillArray(as);
        as.Add(wxString::Format(_wxS("Output path: " ENQUOTED_STR_FMT), m_outputFile.GetFullPath()));
        as.Add(sSeparator);
        size_t     strings = as.GetCount();
        wxDateTime dt(wxGetApp().GetNow());
        wxLog* pLog = wxLog::GetActiveTarget();
        for (size_t i = 0; i < strings; ++i)
        {
            pLog->OnLog(wxLOG_Info, as[i], dt.GetTicks());
        }
    }
}

void wxConfiguration::ToArray(wxArrayString& as) const
{
    as.Empty();

    const wxDateTime dtNow(wxGetApp().GetNow());

    as.Add(wxString::Format("Creation time: %s %s", dtNow.FormatISODate(), dtNow.FormatISOTime()));
    as.Add(wxString::Format("Application version: %s", wxGetApp().APP_VERSION));
    as.Add(wxString::Format("Application vendor: %s", wxGetApp().GetVendorDisplayName()));

    FillArray(as);
}

void wxConfiguration::BuildXmlComments(const wxFileName& outputFile, wxXmlNode* pNode) const
{
    wxString sInit;

    sInit.Printf("This file was created by %s", wxGetApp().GetAppDisplayName());
    wxXmlNode* pComment = new wxXmlNode(nullptr, wxXML_COMMENT_NODE, wxEmptyString, sInit);

    pNode->AddChild(pComment);

    wxArrayString as;
    ToArray(as);

    for (wxArrayString::iterator i = as.begin(), end = as.end(); i != end; ++i)
    {
        i->Prepend("CFG ");
    }

    for(wxArrayString::const_iterator i = as.begin(), end = as.end(); i != end; ++i)
    {
        wxXmlNode* const pComment = new wxXmlNode(nullptr, wxXML_COMMENT_NODE, wxEmptyString, *i);
        pNode->AddChild(pComment);
    }
}

wxConfiguration::INFO_SUBJECT wxConfiguration::GetInfoSubject() const
{
    return m_infoSubject;
}

bool wxConfiguration::GetChapterTimeEnd() const
{
    if (m_chapterTimeEnd.has_value())
    {
        return m_chapterTimeEnd.value();
    }
    else
    {
        switch (m_eRenderMode)
        {
            case RENDER_MKVMERGE_CHAPTERS:
            case RENDER_MKVMERGE:
            return true;

            case RENDER_FFMPEG_CHAPTERS:
            case RENDER_FFMPEG:
            return true;

            default:
            return false;
        }
    }
}

bool wxConfiguration::GetChapterEndTimeFromNextChapter() const
{
    if (!m_chapterEndTimeFromNextChapter.has_value())
    {
        switch (m_eRenderMode)
        {
            case RENDER_MKVMERGE_CHAPTERS:
            case RENDER_MKVMERGE:
            default:
            return false;

            case RENDER_FFMPEG_CHAPTERS:
            case RENDER_FFMPEG:
            return true;
        }
    }
    else
    {
        return m_chapterEndTimeFromNextChapter.value();
    }
}

unsigned long wxConfiguration::GetChapterOffset() const
{
    if (!m_chapterOffset.has_value())
    {
        switch (m_eRenderMode)
        {
            case RENDER_MKVMERGE_CHAPTERS:
            case RENDER_MKVMERGE:
            default:
            return DEF_CHAPTER_OFFSET_MKVMERGE;

            case RENDER_FFMPEG_CHAPTERS:
            case RENDER_FFMPEG:
            return DEF_CHAPTER_OFFSET_FFMPEG;
        }
    }
    else
    {
        return m_chapterOffset.value();
    }
}

bool wxConfiguration::UseDataFiles() const
{
    if (m_useDataFiles.has_value())
    {
        return m_useDataFiles.value();
    }
    else
    {
        switch (m_eRenderMode)
        {
            case RENDER_FFMPEG_CHAPTERS:
            case RENDER_FFMPEG:
            return true;

            default:
            return false;
        }
    }
}

const wxString& wxConfiguration::GetAlternateExtensions() const
{
    return m_sAlternateExtensions;
}

bool wxConfiguration::HasAlternateExtensions() const
{
    return !m_sAlternateExtensions.IsEmpty();
}

const wxString& wxConfiguration::GetTrackNameFormat() const
{
    return m_sTrackNameFormat;
}

const wxString& wxConfiguration::GetMatroskaNameFormat() const
{
    return m_sMatroskaNameFormat;
}

bool wxConfiguration::IsLangUndefined() const
{
    return !m_lang.has_value();
}

bool wxConfiguration::IsLangDefined() const
{
    return m_lang.has_value();
}

wxString wxConfiguration::GetLang() const
{
    return m_lang.value_or(LANG::UND);
}

const wxArrayInputFile& wxConfiguration::GetInputFiles() const
{
    return m_inputFile;
}

bool wxConfiguration::RenderArtistForTrack() const
{
    return m_bRenderArtistForTrack;
}

bool wxConfiguration::RenderMultilineTags() const
{
    return m_bRenderMultilineTags;
}

bool wxConfiguration::RenderReplayGainTags() const
{
    return m_bRenderReplayGainTags;
}

namespace
{
    template<size_t N>
    wxConfiguration::CharBufferType create_non_owned(const char(&var)[N])
    {
        return wxConfiguration::CharBufferType::CreateNonOwned(var, N - 1);
    }
}

wxConfiguration::CharBufferType wxConfiguration::GetExt() const
{
    switch (m_eRenderMode)
    {
        case RENDER_CUESHEET:
        return create_non_owned(EXT::CUESHEET);

        case RENDER_MKVMERGE_CHAPTERS:
        case RENDER_MKVMERGE:
        return create_non_owned(EXT::MATROSKA_CHAPTERS);

        case RENDER_FFMPEG_CHAPTERS:
        case RENDER_FFMPEG:
        return create_non_owned(EXT::FFMPEG_METADATA);

        case RENDER_WAV2IMG_CUE_POINTS:
        return create_non_owned(EXT::TXT);
    }

    wxASSERT(false);
    return create_non_owned(EXT::UNK);
}

wxFileName wxConfiguration::GetOutputDir(const wxInputFile& inputFile) const
{
    wxFileName res(inputFile.GetInputFile());
    res.SetFullName(wxEmptyString);

    if (m_outputFile.IsOk())
    {
        res = m_outputFile;
        if (!m_outputFile.IsDir())
        {
            res.SetFullName(wxEmptyString);
        }
    }

    return res;
}

wxFileName wxConfiguration::GetOutputFile(const wxInputFile& inputFile, const wxString& ext) const
{
    wxFileName res(inputFile.GetInputFile());

    if (m_outputFile.IsOk())
    {
        if (m_outputFile.IsDir())
        {
            res.SetPath(m_outputFile.GetPath());
        }
        else
        {
            res = m_outputFile;
        }
    }

    res.SetExt(ext);
    return res;
}

wxFileName wxConfiguration::GetTemporaryFile(const wxFileName& dir, const wxString& tmpStem, const wxString& tmpPostFix, const wxString& ext)
{
    wxASSERT(dir.IsDir());
    wxFileName res(dir);

    wxString fileName(tmpStem);
    fileName.Append('-').Append(tmpPostFix);

    res.SetName(fileName);
    res.SetExt(ext);
    return res;
}

wxFileName wxConfiguration::GetTemporaryFile(const wxInputFile& inputFile, const wxString& tmpStem, const wxString& tmpPostFix, const wxString& ext) const
{
    wxFileName res(inputFile.GetInputFile());

    if (m_outputFile.IsOk())
    {
        if (m_outputFile.IsDir())
        {
            res.SetPath(m_outputFile.GetPath());
        }
        else
        {
            res = m_outputFile;
        }
    }

    wxString fileName(tmpStem);
    fileName.Append('-').Append(tmpPostFix);

    res.SetName(fileName);
    res.SetExt(ext);
    return res;
}

wxFileName wxConfiguration::GetTemporaryImageFile(const wxInputFile& inputFile, const wxString& tmpStem) const
{
    wxASSERT(m_convertCoverFile || CoverFromPdf());
    return GetTemporaryFile(inputFile, tmpStem, TMP::CONVERTED, m_convertedCoverFileExt);
}

wxFileName wxConfiguration::GetOutputFile(const wxInputFile& inputFile) const
{
    const wxString ext(GetExt());
    wxFileName res(inputFile.GetInputFile());

    if (m_outputFile.IsOk())
    {
        if (m_outputFile.IsDir())
        {
            res.SetPath(m_outputFile.GetPath());
            res.SetExt(ext);
        }
        else
        {
            res = m_outputFile;
        }
    }
    else
    {
        res.SetExt(ext);
    }

    return res;
}

bool wxConfiguration::GetOutputFile(
    const wxInputFile& inputFile,
    const wxString& postFix,
    const wxString& ext,
    wxFileName& fn) const
{
    wxFileName res(inputFile.GetInputFile());
    if (!res.IsOk())
    {
        return false;
    }

    if (m_outputFile.IsOk())
    {
        if (m_outputFile.IsDir())
        {
            res.SetPath(m_outputFile.GetPath());
        }
        else
        {
            res = m_outputFile;
        }
    }

    res.SetName(wxString::Format("%s.%s", res.GetName(), postFix));
    res.SetExt(ext);
    res.MakeAbsolute();
    fn = res;
    return true;
}

bool wxConfiguration::GetOutputCueSheetFile(
    const wxInputFile& inputFile, const wxString& postFix, wxFileName& cueFile) const
{
    return GetOutputFile(inputFile, postFix, EXT::CUESHEET, cueFile);
}

bool wxConfiguration::CorrectQuotationMarks() const
{
    return m_bCorrectQuotationMarks;
}

wxConfiguration::RENDER_MODE wxConfiguration::GetRenderMode() const
{
    return m_eRenderMode;
}

bool wxConfiguration::UseMkvmerge() const
{
    switch (m_eRenderMode)
    {
        case RENDER_MKVMERGE_CHAPTERS:
        case RENDER_MKVMERGE:
        return true;

        default:
        return false;
    }
}

bool wxConfiguration::TrackOneIndexOne() const
{
    return m_bTrackOneIndexOne;
}

bool wxConfiguration::AbortOnError() const
{
    return m_bAbortOnError;
}

bool wxConfiguration::HiddenIndexes() const
{
    return m_bHiddenIndexes;
}

bool wxConfiguration::GenerateTags() const
{
    return (m_eRenderMode == RENDER_MKVMERGE) || (m_eRenderMode == RENDER_FFMPEG);
}

bool wxConfiguration::GenerateEditionUID() const
{
    return m_bGenerateEditionUID;
}

wxConfiguration::FILE_ENCODING wxConfiguration::GetFileEncoding() const
{
    return m_eFileEncoding;
}

wxString wxConfiguration::GetXmlFileEncoding() const
{
    switch (m_eFileEncoding)
    {
        case ENCODING_UTF8:
        case ENCODING_UTF8_WITH_BOM:
        {
            return "UTF-8";
        }

        case ENCODING_UTF16_LE:
        case ENCODING_UTF16_LE_WITH_BOM:
        {
            return "UTF-16LE";
        }

        case ENCODING_UTF16_BE:
        case ENCODING_UTF16_BE_WITH_BOM:
        {
            return "UTF-16BE";
        }

        default:
        {
            return "UTF-8";
        }
    }
}

namespace
{
    void enc_2_cp(wxConfiguration::FILE_ENCODING enc, wxUint32& nCodePage, bool& bBom)
    {
        bBom = false;

        switch (enc)
        {
            case wxConfiguration::ENCODING_UTF8_WITH_BOM:
            {
                bBom = true;
            }

            case wxConfiguration::ENCODING_UTF8:
            {
                nCodePage = wxEncodingDetection::CP::UTF8;
                break;
            }

            case wxConfiguration::ENCODING_UTF16_LE_WITH_BOM:
            {
                bBom = true;
            }

            case wxConfiguration::ENCODING_UTF16_LE:
            {
                nCodePage = wxEncodingDetection::CP::UTF16_LE;
                break;
            }

            case wxConfiguration::ENCODING_UTF16_BE_WITH_BOM:
            {
                bBom = true;
            }

            case wxConfiguration::ENCODING_UTF16_BE:
            {
                nCodePage = wxEncodingDetection::CP::UTF16_BE;
                break;
            }

            default:
            {
                nCodePage = wxEncodingDetection::GetDefaultEncoding();
                break;
            }
        }
    }
}

wxSharedPtr< wxTextOutputStream > wxConfiguration::GetOutputTextStream(wxOutputStream& os) const
{
    wxSharedPtr< wxTextOutputStream > pRes;
    wxUint32                          nCodePage;
    bool                              bBom;

    enc_2_cp(m_eFileEncoding, nCodePage, bBom);

    return wxTextOutputStreamWithBOMFactory::Create(os, wxEOL_NATIVE, bBom, nCodePage, m_bUseMLang);
}

wxSharedPtr< wxMBConv > wxConfiguration::GetXmlEncoding() const
{
    wxString sDescription;

    wxSharedPtr< wxMBConv > pRes;
    wxUint32                nCodePage;
    bool                    bBom;

    enc_2_cp(m_eFileEncoding, nCodePage, bBom);

    pRes = wxEncodingDetection::GetStandardMBConv(nCodePage, m_bUseMLang, sDescription);

    if (!pRes) pRes = wxEncodingDetection::GetDefaultEncoding(m_bUseMLang, sDescription);

    return pRes;
}

bool wxConfiguration::JoinMode() const
{
    return m_bJoinMode;
}

bool wxConfiguration::IncludeDiscNumberTag() const
{
    return m_bIncludeDiscNumberTag;
}

wxCueSheetReader::ReadFlags wxConfiguration::GetReadFlags() const
{
    return m_nReadFlags;
}

wxCueTag::TagSources wxConfiguration::GetTagSources() const
{
    return m_nTagSources;
}

bool wxConfiguration::UseMLang() const
{
    return m_bUseMLang;
}

bool wxConfiguration::RunTool() const
{
    return m_bRunTool;
}

bool wxConfiguration::UseFullPaths() const
{
    return m_bUseFullPaths;
}

bool wxConfiguration::AttachEacLog() const
{
    return (m_nReadFlags & wxCueSheetReader::EC_FIND_LOG) != 0;
}

bool wxConfiguration::AttachAccurateRipLog() const
{
    return (m_nReadFlags & wxCueSheetReader::EC_FIND_ACCURIP_LOG) != 0;
}

bool wxConfiguration::AttachCover() const
{
    return (m_nReadFlags & wxCueSheetReader::EC_FIND_COVER) != 0;
}

wxConfiguration::FFMPEG_CODEC wxConfiguration::GetFfmpegCodec() const
{
    return m_eFfmpegCodec;
}

bool wxConfiguration::UseDefaultFfmpegCodec() const
{
    return m_eFfmpegCodec == CODEC_DEFAULT;
}

bool wxConfiguration::IsDualMono() const
{
    switch (m_eRenderMode)
    {
        case RENDER_FFMPEG_CHAPTERS:
        case RENDER_FFMPEG:
        return m_bSingleAudioChannel;

        default:
        return false;
    }
}

bool wxConfiguration::RunReplayGainScanner() const
{
    return m_bRunReplayGainScanner;
}

bool wxConfiguration::UseDefaultAudioSampleWidth() const
{
    switch (m_eFfmpegCodec)
    {
        case CODEC_DEFAULT:
        return true;

        default:
        return m_nAudioSampleWidth == DEF_AUDIO_SAMPLE_WIDTH;
    }
}

wxInt16 wxConfiguration::GetAudioSampleWidth() const
{
    switch (m_eFfmpegCodec)
    {
        case CODEC_DEFAULT:
        return DEF_AUDIO_SAMPLE_WIDTH;

        default:
        return m_nAudioSampleWidth;
    }    
}

bool wxConfiguration::ConvertCoverFile() const
{
    return m_convertCoverFile;
}

bool wxConfiguration::CoverFromPdf() const
{
    return (m_nReadFlags & wxCueSheetReader::EC_FIND_PDF) != 0;
}

wxString wxConfiguration::GetConvertedCoverFileExt() const
{
    if (m_convertCoverFile || CoverFromPdf())
    {
        return m_convertedCoverFileExt;
    }
    else
    {
        return wxEmptyString;
    }
}

bool wxConfiguration::AudioFilteringRequired() const
{
    return IsDualMono() || !UseDefaultAudioSampleWidth() || !UseDefaultFfmpegCodec();
}

wxConfiguration::CUESHEET_ATTACH_MODE wxConfiguration::GetCueSheetAttachMode() const
{
    if (m_eCsAttachMode == CUESHEET_ATTACH_NONE)
    {
        switch (m_eRenderMode)
        {
            case RENDER_MKVMERGE_CHAPTERS:
            case RENDER_MKVMERGE:
            case RENDER_FFMPEG_CHAPTERS:
            case RENDER_FFMPEG:
            return CUESHEET_ATTACH_SOURCE;
        }
    }

    return m_eCsAttachMode;
}

wxStringProcessor* const wxConfiguration::CreateStringProcessor() const
{
    return wxCueSheetReader::CreateStringProcessor(m_nReadFlags);
}

#include <wx/arrimpl.cpp>	// this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(wxArrayInputFile);

