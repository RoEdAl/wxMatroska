/*
 * wxCueSheetReader.cpp
 */

#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheetContent.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxEncodingDetection/wxEncodingDetection.h>
#include <wxEncodingDetection/wxTextInputStreamOnString.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>
#include "wxStringCorrector.h"
 
// ===============================================================================

const char wxCueSheetReader::eac_log::EXT[] = "log";
const char wxCueSheetReader::eac_log::MASK[] = "*.log";

const char wxCueSheetReader::accurip_log::EXT[] = "accurip";
const char wxCueSheetReader::accurip_log::MASK[] = "*.accurip";

const char wxCueSheetReader::tags_file::EXT[] = "tags.json";
const char wxCueSheetReader::tags_file::MASK[] = "*.tags.json";

// ===============================================================================

const wxCueSheetReader::PARSE_STRUCT wxCueSheetReader::parseArray[] =
{
    { "REM", &wxCueSheetReader::ParseComment },
    { "INDEX", &wxCueSheetReader::ParseIndex },
    { "PREGAP", &wxCueSheetReader::ParsePreGap },
    { "POSTGAP", &wxCueSheetReader::ParsePostGap },
    { "FILE", &wxCueSheetReader::ParseFile },
    { "FLAGS", &wxCueSheetReader::ParseFlags },
    { "TRACK", &wxCueSheetReader::ParseTrack },
    { "CATALOG", &wxCueSheetReader::ParseCatalog },
    { "CDTEXTFILE", &wxCueSheetReader::ParseCdTextFile }
};

// ===============================================================================

namespace
{
    template< size_t FMTS, typename G >
    inline wxString build_regex_str(const char(&fmt)[FMTS], const G& getter)
    {
        wxString se(getter());
        wxString s;

        s.Printf(fmt, se);
        return s;
    }

    template< typename G >
    inline wxString build_component_regex_str(const G& getter)
    {
        return wxString::Format(wxCueComponent::REG_EXP_FMT, getter());
    }

    struct reg_exp
    {
        static inline wxString keywords()
        {
            return build_component_regex_str(wxCueComponent::GetKeywordsRegExp);
        }

        static inline wxString data_mode()
        {
            return build_regex_str("^(\\p{Nd}{1,2})(?:\\p{Xps}+%s)?$", wxTrack::GetDataModeRegExp);
        }

        static inline wxString cd_text_info()
        {
            return build_component_regex_str(wxCueComponent::GetCdTextInfoRegExp);
        }

        static inline wxString data_file()
        {
            return build_regex_str("^((?:\".*\")|(?:'.*'))(?:\\p{Xps}+%s)?$", wxDataFile::GetFileTypeRegExp);
        }
    };
}

template< typename T >
bool wxCueSheetReader::GetLogFile(const wxFileName& inputFile, bool bAnyLog, wxFileName& logFile)
{
    wxASSERT(inputFile.IsOk());

    if (bAnyLog)	// any log file in the same directory
    {
        wxFileName sourceDirFn(inputFile);

        sourceDirFn.SetName(wxEmptyString);
        sourceDirFn.ClearExt();
        wxASSERT(sourceDirFn.IsDirReadable());

        const wxDir sourceDir(sourceDirFn.GetPath());

        if (!sourceDir.IsOpened())
        {
            wxLogError(_wxS("Fail to open directory \u201C%s\u201D"), sourceDirFn.GetPath());
            return false;
        }

        wxString sFileName;

        if (sourceDir.GetFirst(&sFileName, T::MASK, wxDIR_FILES))
        {
            while (true)
            {
                const wxFileName fileName(sourceDir.GetName(), sFileName);
                logFile = fileName;
                return true;
            }
        }

        return false;
    }
    else
    {
        logFile = inputFile;
        logFile.SetExt(T::EXT);
        wxASSERT(logFile.IsOk());
        return logFile.IsFileReadable();
    }
}

wxString wxCueSheetReader::GetOneTrackCue()
{
    wxTextOutputStreamOnString tos;

    (*tos) <<
        "REM one-track CUE sheet" << endl <<
        "FILE \"%source%\" WAVE" << endl <<
        "\tTRACK 01 AUDIO" << endl <<
        "\tINDEX 01 00:00:00" << endl;
    tos->Flush();

    return tos.GetString();
}

bool wxCueSheetReader::TestReadFlags(ReadFlags flags, ReadFlags mask)
{
    return (flags & mask) == mask;
}

bool wxCueSheetReader::TestReadFlags(ReadFlags mask) const
{
    return TestReadFlags(m_readFlags, mask);
}

wxString wxCueSheetReader::GetTagLibVersion()
{
    return wxString::Format(wxS("TagLib version: %d.%d.%d"), TAGLIB_MAJOR_VERSION, TAGLIB_MINOR_VERSION, TAGLIB_PATCH_VERSION);
}

#ifndef NDEBUG

void TagLibDebugListener::printMessage(const TagLib::String& msg)
{
    wxLogDebug(msg.toCWString());
}

#endif

wxCueSheetReader::wxCueSheetReader(void):
    m_reKeywords(reg_exp::keywords()),
    m_reCdTextInfo(reg_exp::cd_text_info()),
    m_reEmpty("^\\p{Xps}*$"),
    m_reIndex("^\\p{Xps}*(\\d{1,2})\\p{Xps}+(\\P{Xps}.*\\P{Xps})$"),
    m_reMsf("^(\\p{Nd}{1,4}):(\\p{Nd}{1,2}):(\\p{Nd}{1,2})$"),
    m_reFlags("\\p{Xps}+"),
    m_reDataMode(reg_exp::data_mode()),
    m_reDataFile(reg_exp::data_file()),
    m_reCatalog("\\d{13}", wxRE_NOSUB),
    m_reIsrc("^(\\p{Lu}{2}|00)-?[\\p{Lu}\\p{Nd}]{3}\\-?[\\p{Nd}]{2}\\-?[\\p{Nd}]{5}$", wxRE_NOSUB),
    m_reTrackComment("cue[\\-_]track(\\p{Nd}{1,2})_([\\pL\\-_\\p{Xps}]+)", wxRE_ICASE),
    m_reCommentMeta("^(\"?)([\\p{Lu}\\-_\\p{Xps}]+)\\1\\p{Xps}+(\\P{Xps}.+)$"),
    m_errorsAsWarnings(true),
    m_readFlags(EC_PARSE_COMMENTS | EC_ELLIPSIZE_TAGS | EC_REMOVE_EXTRA_SPACES | EC_MEDIA_READ_TAGS | EC_FIND_COVER | EC_FIND_LOG | EC_FIND_ACCURIP_LOG | EC_CONVERT_UPPER_ROMAN_NUMERALS),
    m_oneTrackCue(GetOneTrackCue())
{
    wxASSERT(m_reKeywords.IsValid());
    wxASSERT(m_reCdTextInfo.IsValid());
    wxASSERT(m_reEmpty.IsValid());
    wxASSERT(m_reIndex.IsValid());
    wxASSERT(m_reMsf.IsValid());
    wxASSERT(m_reFlags.IsValid());
    wxASSERT(m_reDataMode.IsValid());
    wxASSERT(m_reDataFile.IsValid());
    wxASSERT(m_reCatalog.IsValid());
    wxASSERT(m_reIsrc.IsValid());
    wxASSERT(m_reTrackComment.IsValid());
    wxASSERT(m_reCommentMeta.IsValid());

    m_stringProcessor.reset(CreateStringProcessor(m_readFlags));

#ifndef NDEBUG
    TagLib::setDebugListener(&m_debugListener);
#endif
}

#ifndef NDEBUG
wxCueSheetReader::~wxCueSheetReader()
{
    TagLib::setDebugListener(nullptr);
}

#endif

const wxCueSheet& wxCueSheetReader::GetCueSheet() const
{
    return m_cueSheet;
}

bool wxCueSheetReader::ErrorsAsWarnings() const
{
    return m_errorsAsWarnings;
}

wxCueSheetReader& wxCueSheetReader::SetErrorsAsWarnings(bool errorsAsWarnings)
{
    m_errorsAsWarnings = errorsAsWarnings;
    return *this;
}

wxCueSheetReader& wxCueSheetReader::CorrectQuotationMarks(bool correctQuotationMarks, const wxString& lang)
{
    m_quoteCorrector.SetLang(correctQuotationMarks ? lang : wxEmptyString);
    return *this;
}

const wxString& wxCueSheetReader::GetAlternateExt() const
{
    return m_alternateExt;
}

wxCueSheetReader& wxCueSheetReader::SetAlternateExt(const wxString& alternateExt)
{
    m_alternateExt = alternateExt;
    return *this;
}

wxCueSheetReader::ReadFlags wxCueSheetReader::GetReadFlags() const
{
    return m_readFlags;
}

wxCueSheetReader& wxCueSheetReader::SetReadFlags(wxCueSheetReader::ReadFlags readFlags)
{
    m_readFlags = readFlags;
    m_stringProcessor.reset(CreateStringProcessor(readFlags));
    return *this;
}

bool wxCueSheetReader::FindLog(const wxCueSheetContent& content)
{
    wxASSERT(TestReadFlags(EC_FIND_LOG));
    wxFileName logFile;

    if (GetLogFile< eac_log >(content.GetSource().GetFileName(), TestReadFlags(EC_SINGLE_MEDIA_FILE), logFile))
    {
        m_cueSheet.AddLog(logFile);
        return true;
    }
    else
    {
        return false;
    }
}

bool wxCueSheetReader::FindAccurateRipLog(const wxCueSheetContent& content)
{
    wxASSERT(TestReadFlags(EC_FIND_ACCURIP_LOG));
    wxFileName logFile;

    if (GetLogFile< accurip_log >(content.GetSource().GetFileName(), TestReadFlags(EC_SINGLE_MEDIA_FILE), logFile))
    {
        m_cueSheet.AddAccuripLog(logFile);
        return true;
    }
    else
    {
        return false;
    }
}

bool wxCueSheetReader::ApplyTagsFromFile(const wxCueSheetContent& content)
{
    wxASSERT(TestReadFlags(EC_APPLY_TAGS_FROM_FILE));
    wxFileName tagsFile;

    if (GetLogFile< tags_file >(content.GetSource().GetFileName(), TestReadFlags(EC_SINGLE_MEDIA_FILE), tagsFile))
    {
        return m_cueSheet.ApplyTagsFromJson(tagsFile);
    }
    else
    {
        return false;
    }
}

bool wxCueSheetReader::FindCover(const wxCueSheetContent& content)
{
    wxASSERT(TestReadFlags(EC_FIND_COVER));
    wxFileName coverFile;

    if (wxCoverFile::Find(content.GetSource().GetFileName(), coverFile))
    {
        m_cueSheet.AddCover(coverFile);
        return true;
    }
    else
    {
        return false;
    }
}

bool wxCueSheetReader::ReadCueSheet(const wxString& cueFile, bool useMLang)
{
    wxString cpDescription;

    wxEncodingDetection::wxMBConvSharedPtr pConv(wxEncodingDetection::GetFileEncoding(cueFile, useMLang, cpDescription));

    if (pConv)
    {
        wxLogInfo(_wxS("Detected encoding of file \u201C%s\u201D file is \u201C%s\u201D"), cueFile, cpDescription);
        return ReadCueSheet(cueFile, *pConv);
    }
    else
    {
        return false;
    }
}

bool wxCueSheetReader::ReadCueSheet(const wxString& cueFile, wxMBConv& conv)
{
    wxFileName cueFileName(cueFile);

    cueFileName.MakeAbsolute();

    if (!cueFileName.IsFileReadable() || cueFileName.IsDir())
    {
        wxLogError(_wxS("Invalid path to CUE file \u201C%s\u201D"), cueFile);
        return false;
    }

    wxFileInputStream fis(cueFileName.GetFullPath());

    if (!fis.IsOk())
    {
        wxLogError(_wxS("Unable to open CUE file \u201C%s\u201D"), cueFileName.GetFullPath());
        return false;
    }

    wxDataFile dataFile(cueFileName);

    dataFile.FindFile();

    return ParseCue(wxCueSheetContent(internalReadCueSheet(fis, conv), dataFile, false));
}

bool wxCueSheetReader::ReadCueSheet(wxInputStream& stream)
{
    return ReadCueSheet(stream, wxConvLocal);
}

bool wxCueSheetReader::ReadCueSheet(wxInputStream& stream, wxMBConv& conv)
{
    return ParseCue(wxCueSheetContent(internalReadCueSheet(stream, conv)));
}

void wxCueSheetReader::AppendTags(const wxArrayCueTag& comments, bool singleMediaFile)
{
    for (size_t i = 0, nComments = comments.GetCount(); i < nComments; ++i)
    {
        wxCueTag tag(comments[i]);

        tag.Correct(m_unquoter);
        CorrectTag(tag);

        if (singleMediaFile)	// just add to first track
        {
            if (tag == wxCueTag::Name::TRACKNUMBER) continue;

            wxASSERT(m_cueSheet.HasTrack(1));
            wxTrack& firstTrack = m_cueSheet.GetTrackByNumber(1);
            firstTrack.AddTag(tag);
        }
        else
        {
            if (tag == wxCueTag::Name::TOTALTRACKS) continue;

            if (m_reTrackComment.Matches(tag.GetName()))
            {
                const wxString      tagNumber(m_reTrackComment.GetMatch(tag.GetName(), 1));
                const wxString      tagName(m_reTrackComment.GetMatch(tag.GetName(), 2));
                unsigned long trackNumber;

                if (tagNumber.ToULong(&trackNumber))
                {
                    if (m_cueSheet.HasTrack(trackNumber))
                    {
                        wxTrack& track = m_cueSheet.GetTrackByNumber(trackNumber);
                        const wxCueTag trackTag = tag.Rename(tagName);
                        track.AddTag(trackTag);
                    }
                    else
                    {
                        wxLogInfo(_("Skipping track comment %s - track %d not found"), tag.GetName(), trackNumber);
                    }
                }
                else
                {
                    wxLogDebug("Invalid track comment regular expression");
                }
            }
            else
            {
                m_cueSheet.AddTag(tag);
            }
        }
    }
}

void wxCueSheetReader::AppendTags(const wxArrayCueTag& tags, size_t trackFrom, size_t trackTo)
{
    for (size_t i = 0, cnt = tags.GetCount(); i < cnt; ++i)
    {
        wxCueTag tag(tags[i]);

        if (tag == wxCueTag::Name::TOTALTRACKS) continue;
        if (tag == wxCueTag::Name::TRACKNUMBER) continue;

        tag.Correct(m_unquoter);
        CorrectTag(tag);

        if (m_reTrackComment.Matches(tag.GetName()))
        {
            const wxString tagNumber(m_reTrackComment.GetMatch(tag.GetName(), 1));
            const wxString tagName(m_reTrackComment.GetMatch(tag.GetName(), 2));
            unsigned long trackNumber;

            if (tagNumber.ToULong(&trackNumber))
            {
                if (m_cueSheet.HasTrack(trackNumber))
                {
                    wxTrack& track = m_cueSheet.GetTrackByNumber(trackNumber);
                    const wxCueTag ntag = tag.Rename(tagName);
                    track.AddTag(ntag);
                }
                else
                {
                    wxLogInfo(_("Skipping track comment %s - track %d not found"), tag.GetName(), trackNumber);
                }
            }
            else
            {
                wxLogDebug("Invalid track comment regular expression");
            }
        }
        else
        {
            for (size_t j = trackFrom; j <= trackTo; ++j)
            {
                m_cueSheet.GetTrack(j).AddTag(tag);
            }
        }
    }
}

bool wxCueSheetReader::ReadCueSheetEx(const wxString& fileName, bool useMLang)
{
    wxDataFile dataFile(fileName, wxDataFile::WAVE);

    if (dataFile.GetInfo(m_alternateExt))
    {	// embedded or single
        const bool singleMediaFile = TestReadFlags(EC_SINGLE_MEDIA_FILE);
        const bool cueSheetPresent = dataFile.HasCueSheet();

        if (singleMediaFile)
        {
            return BuildFromSingleMediaFile(dataFile);
        }
        else
        {
            if (cueSheetPresent)
            {
                return ParseCue(wxCueSheetContent(dataFile));
            }
            else
            {
                wxLogWarning(_("Cannot find cue sheet"));
                return false;
            }
        }
    }
    else
    {	// assuming normal text file
        return ReadCueSheet(fileName, useMLang);
    }
}

/* one track, one index*/
bool wxCueSheetReader::BuildFromSingleMediaFile(const wxDataFile& mediaFile)
{
    m_cueSheet.Clear();
    wxString     sOneTrackCue(m_oneTrackCue);
    const size_t nRepl = sOneTrackCue.Replace("%source%", mediaFile.GetRealFileName().GetFullPath());

    wxASSERT(nRepl > 0);

    if (ParseCue(wxCueSheetContent(sOneTrackCue, mediaFile, true)))
    {
        wxASSERT(m_cueSheet.GetTracksCount() == 1u);
        wxASSERT(m_cueSheet.HasSingleDataFile());
        return true;
    }
    else
    {
        return false;
    }
}

wxString wxCueSheetReader::internalReadCueSheet(wxInputStream& stream, wxMBConv& conv)
{
    wxTextInputStream          tis(stream, " \t", conv);
    wxTextOutputStreamOnString tos;

    m_cueSheet.Clear();

    wxString line;

    while (!stream.Eof())
    {
        line = tis.ReadLine();

        if (line.IsEmpty()) *tos << endl;
        else *tos << line << endl;
    }

    tos->Flush();
    return tos.GetString();
}

void wxCueSheetReader::AddError0(const wxString& msg)
{
    m_errors.Add(msg);
}

void wxCueSheetReader::AddError(const wxString format, ...)
{
    va_list  argptr;
    wxString s;

    va_start(argptr, format);
    const int res = s.PrintfV(format, argptr);
    va_end(argptr);

    if (res > 0) AddError0(s);
}

void wxCueSheetReader::DumpErrors(size_t lineNo) const
{
    if (!m_errors.IsEmpty())
    {
        for (wxArrayString::const_iterator i = m_errors.begin(), end = m_errors.end(); i != end; ++i)
        {
            if (m_errorsAsWarnings)
            {
                wxLogWarning(_("Line %" wxSizeTFmtSpec "d: %s"), lineNo, i->GetData());
            }
            else
            {
                wxLogError(_("Line %" wxSizeTFmtSpec "d: %s"), lineNo, i->GetData());
            }
        }
    }
}

bool wxCueSheetReader::CheckEntryType(wxCueComponent::ENTRY_TYPE et) const
{
    if (m_cueSheet.HasTracks())
    {
        return m_cueSheet.GetLastTrack().CheckEntryType(et);
    }
    else
    {
        return m_cueSheet.CheckEntryType(et);
    }
}

wxString wxCueSheetReader::Unquote(const wxString& qs) const
{
    return wxCueTag::UnEscape(m_quoteCorrector(m_unquoter(qs)));
}

wxString wxCueSheetReader::UnquoteFs(const wxString& qs) const
{
    return wxCueTag::UnEscape(m_unquoter(qs));
}

template< size_t SIZE >
bool wxCueSheetReader::ParseLine(const wxString& token, const wxString& rest, const PARSE_STRUCT(&pa)[SIZE])
{
    for (size_t i = 0; i < SIZE; ++i)
    {
        if (token.CmpNoCase(pa[i].token) == 0)
        {
            wxCueComponent::ENTRY_TYPE et;
            wxCueComponent::GetEntryType(pa[i].token, et);
            m_errors.Clear();

            if (CheckEntryType(et))
            {
                PARSE_METHOD method = pa[i].method;
                (this->*method)(token, rest);
            }
            else
            {
                AddError(_("Keyword %s is not allowed here"), token);
            }

            return true;
        }
    }

    return false;
}

void wxCueSheetReader::ParseLine(size_t WXUNUSED(lineNo), const wxString& token, const wxString& rest)
{
    if (!ParseLine(token, rest, parseArray))
    {
        m_errors.Clear();
        AddError(_("Unknown token %s"), token);
    }
}

void wxCueSheetReader::ParseCdTextInfo(size_t WXUNUSED(lineNo), const wxString& token, const wxString& body)
{
    wxCueComponent::ENTRY_FORMAT fmt;

    wxCueComponent::GetCdTextInfoFormat(token, fmt);
    wxString s;

    if (fmt == wxCueComponent::CHARACTER) s = Unquote(body);
    else s = body;

    bool add = true;

    if (token.CmpNoCase(wxCueTag::Name::ISRC) == 0)
    {
        if (!m_reIsrc.Matches(body))
        {
            AddError0(_("Invalid ISRC code"));
            add = false;
        }
    }

    if (add)
        if (!AddCdTextInfo(token, s)) AddError(_("Keyword %s is not allowed here"), token);
}

void wxCueSheetReader::ParseGarbage(const wxString& line)
{
    if (m_cueSheet.HasTracks())
    {
        m_cueSheet.GetLastTrack().AddGarbage(line);
    }
    else
    {
        m_cueSheet.AddGarbage(line);
    }
}

void wxCueSheetReader::CorrectTag(wxCueTag& tag) const
{
    tag.Correct(*m_stringProcessor);
}

void wxCueSheetReader::CorrectString(wxString& str) const
{
    (*m_stringProcessor)(str);
}

void wxCueSheetReader::ParseComment(wxCueComponent& component, const wxString& comment)
{
    if (!TestReadFlags(EC_PARSE_COMMENTS))
    {
        component.AddComment(comment);
        return;
    }

    if (m_reCommentMeta.Matches(comment))
    {
        const wxString tagName(m_reCommentMeta.GetMatch(comment, 2));
        const wxString rawTagValue(m_reCommentMeta.GetMatch(comment, 3));
        const wxString tagValue(m_genericQuoteCorrector(m_unquoter(rawTagValue)));

        wxCueTag tag(wxCueTag::TAG_CUE_COMMENT, tagName, tagValue);
        CorrectTag(tag);

        if (tag.IsRipperComment())
        {
            tag = tag.Rename(wxCueTag::Name::RIPPER);
        }

        component.AddTag(tag);
    }
    else
    {
        component.AddComment(comment);
    }
}

void wxCueSheetReader::ParseComment(const wxString& WXUNUSED(token), const wxString& comment)
{
    if (m_cueSheet.HasTracks()) ParseComment(m_cueSheet.GetLastTrack(), comment);
    else ParseComment(m_cueSheet, comment);
}

bool wxCueSheetReader::ParseCue(const wxCueSheetContent& content)
{
    m_cueSheetContent = content;
    size_t   lineNo = 1;
    wxString line;

    wxTextInputStreamOnString tis(content.GetValue());

    while (!tis.Eof())
    {
        line = (*tis).ReadLine();

        if (m_reEmpty.Matches(line))
        {
            wxLogDebug("Skipping empty line %" wxSizeTFmtSpec "u", lineNo);
            continue;
        }

        if (!ParseCueLine(line, lineNo)) return false;
        lineNo += 1;
    }

    m_cueSheet.SortTracks();
    m_cueSheet.AddContent(content);

    if (TestReadFlags(EC_MEDIA_READ_TAGS))
    {
        ReadTagsFromRelatedFiles();
    }

    if (content.HasSource())
    {
        if (TestReadFlags(EC_FIND_COVER))
        {
            if (!FindCover(content))
            {
                if (TestReadFlags(EC_MEDIA_READ_TAGS))
                {
                    FindCoversInRelatedFiles();
                }
            }
        }

        if (TestReadFlags(EC_FIND_LOG))
        {
            FindLog(content);
        }

        if (TestReadFlags(EC_FIND_ACCURIP_LOG))
        {
            FindAccurateRipLog(content);
        }

        if (TestReadFlags(EC_APPLY_TAGS_FROM_FILE))
        {
            ApplyTagsFromFile(content);
        }
    }

    return true;
}

bool wxCueSheetReader::ParseCueLine(const wxString& line, size_t lineNo)
{
    bool res = true;

    if (m_reKeywords.Matches(line))
    {
        wxASSERT(m_reKeywords.GetMatchCount() == 3);
        wxString token = m_reKeywords.GetMatch(line, 1);
        wxString rest = m_reKeywords.GetMatch(line, 2);
        m_errors.Clear();
        ParseLine(lineNo, token, rest);

        if (m_errors.GetCount() > 0)
        {
            DumpErrors(lineNo);
            ParseGarbage(line);
            res = false;
        }
    }
    else if (m_reCdTextInfo.Matches(line))
    {
        wxASSERT(m_reCdTextInfo.GetMatchCount() == 3);
        const wxString token = m_reCdTextInfo.GetMatch(line, 1);
        const wxString rest = m_reCdTextInfo.GetMatch(line, 2);
        m_errors.Clear();
        ParseCdTextInfo(lineNo, token, rest);

        if (m_errors.GetCount() > 0)
        {
            DumpErrors(lineNo);
            ParseGarbage(line);
            res = false;
        }
    }
    else
    {
        wxLogWarning(_("Incorrect line %" wxSizeTFmtSpec "d: %s"), lineNo, line);
        ParseGarbage(line);
        res = false;
    }

    return res;
}

bool wxCueSheetReader::AddCdTextInfo(const wxString& token, const wxString& body)
{
    wxString modifiedBody(body);

    CorrectString(modifiedBody);

    if (m_cueSheet.HasTracks()) return m_cueSheet.GetLastTrack().AddCdTextInfoTag(token, modifiedBody);
    else return m_cueSheet.AddCdTextInfoTag(token, modifiedBody);
}

bool wxCueSheetReader::ParseMsf(const wxString& body, wxIndex& idx, bool preOrPost)
{
    bool          res = true;
    unsigned long min, sec, frames;

    if (m_reMsf.Matches(body))
    {
        if (!m_reMsf.GetMatch(body, 1).ToULong(&min)) res = false;
        if (!m_reMsf.GetMatch(body, 2).ToULong(&sec)) res = false;
        if (!m_reMsf.GetMatch(body, 3).ToULong(&frames)) res = false;
    }
    else
    {
        res = false;
    }

    if (res) res = idx.Assign(0, min, sec, frames).IsValid(preOrPost);

    return res;
}

void wxCueSheetReader::ParsePreGap(const wxString& WXUNUSED(token), const wxString& body)
{
    wxIndex idx;

    if (ParseMsf(body, idx, true))
    {
        if (m_cueSheet.GetTracksCount() > 1u)
        {
            idx.SetDataFileIdx(m_cueSheet.GetBeforeLastTrack().GetMaxDataFileIdx(false));
            m_cueSheet.GetLastTrack().SetPreGap(idx);
        }
        else
        {
            AddError0(_("Fail to add pre-gap - no previous track"));
        }
    }
    else
    {
        AddError0(_("Invalid index specification"));
    }
}

void wxCueSheetReader::ParsePostGap(const wxString& WXUNUSED(token), const wxString& body)
{
    wxIndex idx;

    if (ParseMsf(body, idx, true))
    {
        if (m_cueSheet.HasDataFiles())
        {
            idx.SetDataFileIdx(m_cueSheet.GetLastDataFileIdx());
            m_cueSheet.GetLastTrack().SetPostGap(idx);
        }
        else
        {
            AddError0(_("Cannot add post-gap - no data files"));
        }
    }
    else
    {
        AddError0(_("Invalid index specification"));
    }
}

void wxCueSheetReader::ParseIndex(const wxString& WXUNUSED(token), const wxString& body)
{
    if (m_reIndex.Matches(body))
    {
        unsigned long number;

        if (!m_reIndex.GetMatch(body, 1).ToULong(&number))
        {
            AddError0(_("Invalid index specification"));
        }
        else
        {
            const wxString msf = m_reIndex.GetMatch(body, 2);
            wxIndex        idx;

            if (ParseMsf(msf, idx))
            {
                idx.SetNumber(number);

                if (idx.IsValid())
                {
                    if (m_cueSheet.HasDataFiles())
                    {
                        idx.SetDataFileIdx(m_cueSheet.GetLastDataFileIdx());
                        m_cueSheet.GetLastTrack().AddIndex(idx);
                    }
                    else
                    {
                        AddError0(_("Fail to add index - no data file"));
                    }
                }
                else
                {
                    AddError0(_("Invalid index specification"));
                }
            }
            else
            {
                AddError0(_("Invalid index specification"));
            }
        }
    }
    else
    {
        AddError0(_("Invalid index specification"));
    }
}

void wxCueSheetReader::ParseFile(const wxString& WXUNUSED(token), const wxString& body)
{
    if (m_reDataFile.Matches(body))
    {
        const wxString file(m_reDataFile.GetMatch(body, 1));
        const wxString strType(m_reDataFile.GetMatch(body, 2));
        wxDataFile::FileType ftype = wxDataFile::BINARY;

        if (!strType.IsEmpty()) wxDataFile::FromString(strType, ftype);

        wxFileName fn(UnquoteFs(file));

        if (m_cueSheetContent.HasSource())
        {
            if (m_cueSheetContent.IsEmbedded())	// the only source
            {
                m_cueSheet.AddDataFile(m_cueSheetContent.GetSource());
            }
            else
            {
                fn.SetPath(m_cueSheetContent.GetSource().GetFileName().GetPath());
                m_cueSheet.AddDataFile(wxDataFile(fn, ftype));
            }
        }
        else
        {
            m_cueSheet.AddDataFile(wxDataFile(fn, ftype));
        }
    }
}

void wxCueSheetReader::ParseFlags(const wxString& WXUNUSED(token), const wxString& body)
{
    wxString strFlags(body);

    m_reFlags.ReplaceAll(&strFlags, '|');
    wxStringTokenizer tokenizer(strFlags, '|');

    while (tokenizer.HasMoreTokens())
    {
        const wxString flagStr(tokenizer.GetNextToken());
        if (!m_cueSheet.GetLastTrack().AddFlag(flagStr)) AddError(_("Invalid flag %s"), flagStr);
    }
}

void wxCueSheetReader::ParseTrack(const wxString& token, const wxString& body)
{
    if (m_reDataMode.Matches(body))
    {
        unsigned long trackNo;

        if (!m_reDataMode.GetMatch(body, 1).ToULong(&trackNo))
        {
            AddError(_("Invalid track number %s"),
                    m_reDataMode.GetMatch(body, 1));
        }
        else
        {
            const wxString sMode(m_reDataMode.GetMatch(body, 2));
            wxTrack        newTrack(trackNo);

            if (newTrack.IsValid() && newTrack.SetMode(sMode)) m_cueSheet.AddTrack(newTrack);
            else AddError0(_("Invalid track specification"));
        }
    }
    else
    {
        AddError0(_("Invalid track specification"));
    }
}

void wxCueSheetReader::ParseCatalog(const wxString& WXUNUSED(token), const wxString& body)
{
    if (m_reCatalog.Matches(body)) m_cueSheet.AddCatalog(body);
    else AddError0(_("Invalid catalog number"));
}

void wxCueSheetReader::ParseCdTextFile(const wxString& WXUNUSED(token), const wxString& body)
{
    wxFileName cdTextFile(UnquoteFs(body));

    if (cdTextFile.GetPath().IsEmpty() && m_cueSheetContent.HasSource()) cdTextFile.SetPath(m_cueSheetContent.GetSource().GetFileName().GetPath());

    m_cueSheet.AddCdTextFile(cdTextFile);
}

bool wxCueSheetReader::ReadTagsFromRelatedFiles()
{
    if (!m_cueSheet.CalculateDuration(m_alternateExt)) return false;

    const wxArrayDataFile& dataFiles = m_cueSheet.GetDataFiles();
    size_t                 trackFrom, trackTo;
    bool                   res = true;

    for (size_t i = 0, cnt = dataFiles.GetCount(); i < cnt; ++i)
    {
        wxASSERT(dataFiles[i].HasRealFileName());

        if (m_cueSheet.GetRelatedTracks(i, trackFrom, trackTo))
        {
            if (!ReadTagsFromMediaFile(dataFiles[i], trackFrom, trackTo)) res = false;
        }
        else
        {
            wxLogWarning(_wxS("Data file \u201C%s\u201D is not related to any track"), dataFiles[i].GetRealFileName().GetName());
        }
    }

    return res;
}

bool wxCueSheetReader::FindCoversInRelatedFiles()
{
    if (!m_cueSheet.CalculateDuration(m_alternateExt)) return false;

    const wxArrayDataFile& dataFiles = m_cueSheet.GetDataFiles();
    wxArrayCoverFile covers;

    for (size_t i = 0, cnt = dataFiles.GetCount(); i < cnt; ++i)
    {
        ExtractCoversFromDataFile(dataFiles[i], covers);
    }
    if (covers.IsEmpty())
    {
        return false;
    }

    wxCoverFile::Sort(covers);
    m_cueSheet.AddCover(covers[0]); // add smallest image
    return true;
}

bool wxCueSheetReader::ReadTagsFromMediaFile(const wxDataFile& _dataFile, size_t trackFrom, size_t trackTo)
{
    if (_dataFile.HasRealFileName())
    {
        AppendTags(_dataFile.GetTags(), trackFrom, trackTo);
        return true;
    }
    else
    {
        wxDataFile dataFile(_dataFile);

        if (dataFile.GetInfo())
        {
            AppendTags(dataFile.GetTags(), trackFrom, trackTo);
            return true;
        }
        else
        {
            wxLogError(_wxS("Cannot read metadata from \u201C%s\u201D"), dataFile.GetFileName().GetFullName());
            return false;
        }
    }
}

size_t wxCueSheetReader::ExtractCoversFromDataFile(const wxDataFile& dataFile, wxArrayCoverFile& covers) const
{
    if (!dataFile.HasRealFileName())
    {
        return 0;
    }

    return wxCoverFile::Extract(dataFile.GetRealFileName(), covers);
}

wxStringProcessor* const wxCueSheetReader::CreateStringProcessor(wxCueSheetReader::ReadFlags readFlags)
{
    return wxStringCorrector::Configurator()
        .RemoveExtraSpaces(wxCueSheetReader::TestReadFlags(readFlags, wxCueSheetReader::EC_REMOVE_EXTRA_SPACES))
        .Ellipsize(wxCueSheetReader::TestReadFlags(readFlags, wxCueSheetReader::EC_ELLIPSIZE_TAGS))
        .RomanNumeralsUpper(wxCueSheetReader::TestReadFlags(readFlags, wxCueSheetReader::EC_CONVERT_UPPER_ROMAN_NUMERALS))
        .RomanNumeralsLower(wxCueSheetReader::TestReadFlags(readFlags, wxCueSheetReader::EC_CONVERT_LOWER_ROMAN_NUMERALS))
        .Dashes(wxCueSheetReader::TestReadFlags(readFlags, wxCueSheetReader::EC_CORRECT_DASHES))
        .SmallEmDash(wxCueSheetReader::TestReadFlags(readFlags, wxCueSheetReader::EC_SMALL_EM_DASH))
        .NumberFullStop(wxCueSheetReader::TestReadFlags(readFlags, wxCueSheetReader::EC_NUMBER_FULL_STOP))
        .SmallLetterParenthesized(wxCueSheetReader::TestReadFlags(readFlags, wxCueSheetReader::EC_SMALL_LETTER_PARENTHESIZED))
        .AsciiToUnicode(wxCueSheetReader::TestReadFlags(readFlags, wxCueSheetReader::EC_ASCII_TO_UNICODE))
        .Create();
}

