/*
 * wxConfiguration.h
 */

#ifndef _WX_CONFIGURATION_H_
#define _WX_CONFIGURATION_H_

#ifndef _MY_CONFIGURATION_H_
#include <wxConsoleApp/MyConfiguration.h>
#endif

#ifndef _WX_INPUT_FILE_H_
#include "wxInputFile.h"
#endif

#ifndef _WX_CUE_COMPONENT_H
#include <wxCueFile/wxCueComponent.h>
#endif

#ifndef _WX_CUE_SHEET_READER_H_
#include <wxCueFile/wxCueSheetReader.h>
#endif

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

#include <enum2str.h>

WX_DECLARE_OBJARRAY(wxInputFile, wxArrayInputFile);

class wxConfiguration: public MyConfiguration
{
    public:

    typedef wxCharTypeBuffer< char > CharBufferType;

    enum RENDER_MODE
    {
        RENDER_CUESHEET,
        RENDER_MKVMERGE_CHAPTERS,
        RENDER_MKVMERGE,
        RENDER_FFMPEG_CHAPTERS,
        RENDER_FFMPEG,
        RENDER_WAV2IMG_CUE_POINTS
    };

    enum INFO_SUBJECT
    {
        INFO_NONE,
        INFO_VERSION,
        INFO_USAGE,
        INFO_TOOLS,
        INFO_ASCII_TO_UNICODE,
        INFO_FORMATTING_DIRECTIVES,
        INFO_LICENSE
    };

    static wxString GetRenderingModes();

    static wxString ToString(RENDER_MODE);
    static bool FromString(const wxString&, RENDER_MODE&);

    enum FILE_ENCODING
    {
        ENCODING_LOCAL,
        ENCODING_UTF8,
        ENCODING_UTF8_WITH_BOM,
        ENCODING_UTF16_LE,
        ENCODING_UTF16_LE_WITH_BOM,
        ENCODING_UTF16_BE,
        ENCODING_UTF16_BE_WITH_BOM,
    };

    static wxString ToString(FILE_ENCODING);
    static bool FromString(const wxString&, FILE_ENCODING&);

    enum CUESHEET_ATTACH_MODE
    {
        CUESHEET_ATTACH_NONE,
        CUESHEET_ATTACH_SOURCE,
        CUESHEET_ATTACH_DECODED,
        CUESHEET_ATTACH_RENDERED
    };

    static wxString ToString(CUESHEET_ATTACH_MODE);
    static bool FromString(const wxString&, CUESHEET_ATTACH_MODE&, bool&);

    enum FFMPEG_CODEC
    {
        CODEC_DEFAULT, // copy or flac for joined stream
        CODEC_PCM_LE,
        CODEC_PCM_BE,
        CODEC_FLAC,
        CODEC_WAVPACK,
        CODEC_OPUS,
    };

    static wxString ToString(FFMPEG_CODEC);
    static bool FromString(const wxString&, FFMPEG_CODEC&);

    protected:

    typedef VALUE_NAME_PAIR< CUESHEET_ATTACH_MODE > CuesheetAttachModeName;
    typedef VALUE_NAME_PAIR< RENDER_MODE > RenderModeName;
    typedef VALUE_NAME_PAIR< INFO_SUBJECT > INFO_SUBJECT_DESC;
    typedef VALUE_NAME_PAIR< FFMPEG_CODEC > FfmpegCodecName;

    static const CuesheetAttachModeName AttachModeNames[];
    static const RenderModeName RenderModeNames[];
    static const INFO_SUBJECT_DESC InfoSubjectDesc[];
    static const FfmpegCodecName FfmpegCodecNames[];

    protected:

    INFO_SUBJECT m_infoSubject;
    std::optional<bool> m_chapterTimeEnd;	// default=true
    std::optional<bool> m_chapterEndTimeFromNextChapter;	// default=false
    std::optional<unsigned long> m_chapterOffset; // in frames
    std::optional<bool> m_useDataFiles;	// default=true
    bool m_bCorrectQuotationMarks;
    RENDER_MODE m_eRenderMode;
    bool m_bRunTool;
    bool m_bGenerateEditionUID;
    FILE_ENCODING m_eFileEncoding;
    bool m_bTrackOneIndexOne;	// or zero
    bool m_bAbortOnError;
    bool m_bHiddenIndexes;
    bool m_bJoinMode;
    bool m_bIncludeDiscNumberTag;
    wxCueSheetReader::ReadFlags m_nReadFlags;
    wxCueTag::TagSources m_nTagSources;
    bool m_bUseMLang;
    bool m_bUseFullPaths;
    CUESHEET_ATTACH_MODE m_eCsAttachMode;

    wxString m_sAlternateExtensions;
    std::optional<wxString> m_lang;
    wxString m_sTrackNameFormat;
    wxString m_sMatroskaNameFormat;

    wxArrayInputFile m_inputFile;
    wxFileName m_outputFile;

    wxSortedArrayString m_asLang;

    bool m_bRenderMultilineTags;
    bool m_bRenderReplayGainTags;
    bool m_bRenderArtistForTrack;
    FFMPEG_CODEC m_eFfmpegCodec;
    bool m_bSingleAudioChannel;
    bool m_bRunReplayGainScanner;
    wxInt16 m_nAudioSampleWidth;
    bool m_convertCoverFile;
    wxString m_convertedCoverFileExt;

    protected:

    static bool ReadLanguagesStrings(wxSortedArrayString&);
    static bool check_ext(const wxString&);
    static wxString BoolToIdx(bool);
    static wxString GetReadFlagsDesc(wxCueSheetReader::ReadFlags);
    static void AddFlag(wxArrayString&, wxCueSheetReader::ReadFlags, wxCueSheetReader::ReadFlags, const wxString&);

    static bool FromString(const wxString&, INFO_SUBJECT&);

    static wxString ToString(INFO_SUBJECT);
    static wxString GetInfoSubjectTexts();

    wxString ReadFlagTestStr(wxCueSheetReader::ReadFlags) const;
    wxString TagSourcesTestStr(wxCueTag::TagSources) const;

    bool CheckLang(const wxString&) const;
    void FillArray(wxArrayString& as) const;
    bool ReadReadFlags(const wxCmdLineParser&, const wxString&, wxCueSheetReader::ReadFlags);
    bool ReadTagSources(const wxCmdLineParser&, const wxString&, wxCueTag::TagSources);

    public:

    INFO_SUBJECT GetInfoSubject() const;
    bool GetChapterTimeEnd() const;
    bool GetChapterEndTimeFromNextChapter() const;
    unsigned long GetChapterOffset() const;
    bool UseDataFiles() const;
    const wxString& GetAlternateExtensions() const;
    bool HasAlternateExtensions() const;
    bool IsLangUndefined() const;
    bool IsLangDefined() const;
    wxString GetLang() const;
    const wxString& GetTrackNameFormat() const;
    const wxString& GetMatroskaNameFormat() const;
    const wxArrayInputFile& GetInputFiles() const;
    bool CorrectQuotationMarks() const;
    RENDER_MODE GetRenderMode() const;
    bool UseMkvmerge() const;
    bool TrackOneIndexOne() const;
    bool AbortOnError() const;
    bool HiddenIndexes() const;
    bool GenerateTags() const;
    bool RunTool() const;
    bool GenerateEditionUID() const;
    FILE_ENCODING GetFileEncoding() const;
    wxString GetXmlFileEncoding() const;
    bool RenderArtistForTrack() const;
    bool RenderMultilineTags() const;
    bool RenderReplayGainTags() const;
    FFMPEG_CODEC GetFfmpegCodec() const;
    bool UseDefaultFfmpegCodec() const;
    bool IsDualMono() const;
    bool RunReplayGainScanner() const;
    bool UseDefaultAudioSampleWidth() const;
    wxInt16 GetAudioSampleWidth() const;
    bool AudioFilteringRequired() const;
    bool ConvertCoverFile() const;
    wxString GetConvertedCoverFileExt() const;

    wxSharedPtr< wxMBConv > GetXmlEncoding() const;

    wxSharedPtr< wxTextOutputStream > GetOutputTextStream(wxOutputStream&) const;
    bool JoinMode() const;
    bool IncludeDiscNumberTag() const;
    wxCueSheetReader::ReadFlags GetReadFlags() const;
    wxCueTag::TagSources GetTagSources() const;
    bool UseMLang() const;
    bool UseFullPaths() const;
    bool AttachEacLog() const;
    bool AttachAccurateRipLog() const;
    CUESHEET_ATTACH_MODE GetCueSheetAttachMode() const;
    bool AttachCover() const;
    CharBufferType GetExt() const;
    wxFileName GetOutputDir(const wxInputFile&) const;
    wxFileName GetOutputFile(const wxInputFile&) const;
    wxFileName GetOutputFile(const wxInputFile&, const wxString&) const;
    wxFileName GetTemporaryFile(const wxInputFile&, const wxString&, const wxString&, const wxString&) const;
    wxFileName GetTemporaryImageFile(const wxInputFile&, const wxString&) const;
    static wxFileName GetTemporaryFile(const wxFileName&, const wxString&, const wxString&, const wxString&);
    bool GetOutputFile(const wxInputFile&, const wxString&, const wxString&, wxFileName&) const;
    bool GetOutputCueSheetFile(const wxInputFile&, const wxString&, wxFileName&) const;
    wxStringProcessor* const CreateStringProcessor() const;

    public:

    struct EXT
    {
        static const size_t MAX_LEN;

        static const char MATROSKA_AUDIO[];
        static const char MATROSKA_CHAPTERS[];
        static const char MATROSKA_TAGS[];
        static const char MKVMERGE_OPTIONS[];

        static const char CMAKE_SCRIPT[];
        static const char FFMPEG_METADATA[];

        static const char CUESHEET[];
        static const char TXT[];
        static const char XML[];
        static const char JSON[];
        static const char CMAKE[];
        static const char MKA[];
        static const char UNK[];
        static const char JPEG[];
        static const char WEBP[];
    };

    struct TMP
    {
        static const char CMD[];
        static const char MKC[];
        static const char MKT[];
        static const char FFM[];
        static const char PRE[];
        static const char CHAPTERS[];
        static const char RGSCAN[];
        static const char IMG[];
        static const char EMBEDDED[];
        static const char RENDERED[];
        static const char CONVERTED[];
    };

    struct FMT
    {
        static const char MKA_CONTAINER[];
        static const char MKA_CHAPTER[];
    };

    struct LANG
    {
        static const char FILE_URL[];
        static const char FILE_NAME[];
        static const char UND[];
    };

    static const unsigned long  DEF_CHAPTER_OFFSET_MKVMERGE;
    static const unsigned long  DEF_CHAPTER_OFFSET_FFMPEG;

    static const wxInt16 DEF_AUDIO_SAMPLE_WIDTH;

    public:

    wxConfiguration(void);
    bool ReadLanguagesStrings();

    void AddCmdLineParams(wxCmdLineParser&) const;
    bool Read(const wxCmdLineParser&);

    void Dump() const;
    void BuildXmlComments(const wxFileName&, wxXmlNode*) const;
    void ToArray(wxArrayString&) const;
};

#endif  // _WX_CONFIGURATION_H

