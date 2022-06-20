/*
 * wxCueSheet.cpp
 */

#include <wxCueFile/wxSamplingInfo.h>
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#include <wxCueFile/wxDuration.h>
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheetContent.h>
#include <wxCueFile/wxCueSheet.h>
#include <app_config.h>

 // ===============================================================================

namespace
{
    static const char* const CD_ALIASES[] =
    {
        "cd",
        "vol",
        "volume",
        "disc",
        "disk",
        "dysk",
        "disque"
    };

    template< size_t S >
    wxString get_cd_aliases_reg_exp(const char* const (&aliases)[S])
    {
        wxTextOutputStreamOnString tos;

        for (size_t i = 0; i < S; ++i)
        {
            *tos << aliases[i] << '|';
        }

        const wxString s = tos.GetString();

        wxASSERT(!s.IsEmpty());
        return wxString::Format("(%s)", s.Left(s.Length() - 1));
    }
}

// ===============================================================================

const char wxCueSheet::ALBUM_REG_EX1[] =
"^(.*\\P{Xps})\\p{Xps}*(\\p{P}\\p{Xps})*%s[\\p{Xps}\\p{P}]*(\\p{Nd}{1,2}\\p{Xps}*\\p{P})\\p{Xps}*(\\P{Xps}.*)?$";

const char wxCueSheet::ALBUM_REG_EX2[] =
"^(.*\\P{Xps})\\p{Xps}*(%s[\\p{Xps}\\p{P}]*(\\p{Nd}{1,2}))\\p{Xps}*(\\P{Xps}.*)?$";

// ===============================================================================

wxString wxCueSheet::GetCdAliasesRegExp()
{
    return get_cd_aliases_reg_exp(CD_ALIASES);
}

// ===============================================================================

wxCueSheet::wxCueSheet(void)
{
}

wxCueSheet::wxCueSheet(const wxCueSheet& cs)
{
    copy(cs);
}

wxCueSheet& wxCueSheet::operator =(const wxCueSheet& cs)
{
    copy(cs);
    return *this;
}

size_t wxCueSheet::GetCatalogsCount() const
{
    return m_catalogs.GetCount();
}

const wxArrayCueTag& wxCueSheet::GetCatalogs() const
{
    return m_catalogs;
}

wxCueSheet& wxCueSheet::AddCatalog(const wxString& sCatalog)
{
    m_catalogs.Add(wxCueTag(wxCueTag::TAG_UNKNOWN, wxCueTag::Name::CATALOG, sCatalog));
    return *this;
}

size_t wxCueSheet::GetCdTextFilesCount() const
{
    return m_cdTextFiles.GetCount();
}

const wxArrayFileName& wxCueSheet::GetCdTextFiles() const
{
    return m_cdTextFiles;
}

wxCueSheet& wxCueSheet::AddCdTextFile(const wxFileName& cdTextFile)
{
    m_cdTextFiles.Add(cdTextFile);
    return *this;
}

size_t wxCueSheet::GetContentsCount() const
{
    return m_content.GetCount();
}

const wxArrayCueSheetContent& wxCueSheet::GetContents() const
{
    return m_content;
}

wxCueSheet& wxCueSheet::AddContent(const wxCueSheetContent& content)
{
    m_content.Add(content);
    return *this;
}

wxCueSheet& wxCueSheet::AddContent(const wxString& sContent)
{
    m_content.Add(wxCueSheetContent(sContent));
    return *this;
}

size_t wxCueSheet::GetLogsCount() const
{
    return m_logs.GetCount();
}

const wxArrayFileName& wxCueSheet::GetLogs() const
{
    return m_logs;
}

size_t wxCueSheet::GetAccurateRipLogsCount() const
{
    return m_accuripLogs.GetCount();
}

const wxArrayFileName& wxCueSheet::GetAccurateRipLogs() const
{
    return m_accuripLogs;
}

size_t wxCueSheet::GetCoversCount() const
{
    return m_covers.GetCount();
}

const wxArrayCoverFile& wxCueSheet::GetCovers() const
{
    return m_covers;
}

wxCueSheet& wxCueSheet::AddLog(const wxFileName& logFile)
{
    m_logs.Add(logFile);
    return *this;
}

wxCueSheet& wxCueSheet::AddAccuripLog(const wxFileName& logFile)
{
    m_accuripLogs.Add(logFile);
    return *this;
}

void wxCueSheet::AddCover(const wxFileName& coverFn)
{
    AddCover(wxCoverFile(coverFn, wxCoverFile::FrontCover));
}

void wxCueSheet::AddCover(const wxCoverFile& cover)
{
    wxCoverFile::Append(m_covers, cover);
}

void wxCueSheet::AddCovers(const wxArrayCoverFile& covers)
{
    wxCoverFile::Append(m_covers, covers);
}

bool wxCueSheet::HasTracks() const
{
    return !m_tracks.IsEmpty();
}

size_t wxCueSheet::GetTracksCount() const
{
    return m_tracks.GetCount();
}

const wxArrayTrack& wxCueSheet::GetTracks() const
{
    return m_tracks;
}

bool wxCueSheet::AddTrack(const wxTrack& track)
{
    if (m_dataFiles.IsEmpty() && track.HasIndexes()) return false;

    m_tracks.Add(track);
    return true;
}

wxTrack& wxCueSheet::GetTrack(size_t idx)
{
    return m_tracks[idx];
}

bool wxCueSheet::HasTrack(size_t nTrackNo) const
{
    for (size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; ++i)
    {
        if (m_tracks[i].GetNumber() == nTrackNo) return true;
    }

    return false;
}

wxTrack& wxCueSheet::GetTrackByNumber(size_t nTrackNo)
{
    wxASSERT(HasTrack(nTrackNo));

    for (size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; ++i)
    {
        if (m_tracks[i].GetNumber() == nTrackNo) return m_tracks[i];
    }

    wxASSERT(false);
    return m_tracks[0];
}

size_t wxCueSheet::GetTrackIdxFromNumber(size_t nTrackNo) const
{
    wxASSERT(HasTrack(nTrackNo));

    for (size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; ++i)
    {
        if (m_tracks[i].GetNumber() == nTrackNo) return i;
    }

    wxASSERT(false);
    return wxIndex::UnknownDataFileIdx;
}

wxTrack& wxCueSheet::GetLastTrack()
{
    wxASSERT(!m_tracks.IsEmpty());
    return m_tracks.Last();
}

wxTrack& wxCueSheet::GetBeforeLastTrack()
{
    size_t nCount = m_tracks.GetCount();

    wxASSERT(nCount > 1u);
    return m_tracks[nCount - 2];
}

const wxTrack& wxCueSheet::GetTrack(size_t idx) const
{
    return m_tracks[idx];
}

const wxTrack& wxCueSheet::GetLastTrack() const
{
    return m_tracks.Last();
}

bool wxCueSheet::HasGarbage() const
{
    bool   bRes = (m_garbage.GetCount() > 0);
    size_t nTracks = m_tracks.GetCount();
    size_t i = 0;

    while (!bRes && (i < nTracks))
    {
        bRes = bRes && m_tracks[i].HasGarbage();
        i++;
    }

    return bRes;
}

bool wxCueSheet::HasDataFiles() const
{
    return !m_dataFiles.IsEmpty();
}

size_t wxCueSheet::GetDataFilesCount() const
{
    return m_dataFiles.GetCount();
}

bool wxCueSheet::HasFlacDataFile() const
{
    for (size_t i = 0, cnt = m_dataFiles.GetCount(); i < cnt; ++i)
    {
        const wxDataFile& dataFile = m_dataFiles[i];
        // if (!dataFile.HasRealFileName()) continue;
        if (dataFile.GetMediaType() == wxDataFile::MEDIA_TYPE_FLAC) return true;
    }
    return false;
}

size_t wxCueSheet::GetLastDataFileIdx() const
{
    if (m_dataFiles.IsEmpty()) return wxIndex::UnknownDataFileIdx;
    else return m_dataFiles.GetCount() - 1;
}

bool wxCueSheet::GetRelatedTracks(size_t nDataFileIdx, size_t& nTrackFrom, size_t& nTrackTo) const
{
    wxASSERT(nDataFileIdx != wxIndex::UnknownDataFileIdx);
    bool bTrackFrom = false, bTrackTo = false;

    for (size_t i = 0, nCount = m_tracks.GetCount(); (i < nCount) && !(bTrackFrom && bTrackTo); ++i)
    {
        if (m_tracks[i].IsRelatedToDataFileIdx(nDataFileIdx, false))
        {
            if (!bTrackFrom)
            {
                nTrackFrom = i;
                bTrackFrom = true;
            }
        }
        else if (bTrackFrom)
        {
            wxASSERT(i > 0u);
            nTrackTo = i - 1;
            bTrackTo = true;
        }
    }

    if (bTrackFrom && !bTrackTo)
    {
        wxASSERT(!m_tracks.IsEmpty());
        nTrackTo = m_tracks.GetCount() - 1;
        bTrackTo = true;
    }

    return (bTrackFrom && bTrackTo);
}

const wxArrayDataFile& wxCueSheet::GetDataFiles() const
{
    return m_dataFiles;
}

wxCueSheet& wxCueSheet::AddDataFile(const wxDataFile& dataFile)
{
    m_dataFiles.Add(dataFile);
    return *this;
}

void wxCueSheet::Clear(void)
{
    m_content.Clear();
    m_logs.Clear();
    m_accuripLogs.Clear();
    m_covers.Clear();
    wxCueComponent::Clear();
    m_catalogs.Clear();
    m_cdTextFiles.Clear();
    m_tracks.Clear();
    m_dataFiles.Clear();
}

void wxCueSheet::copy(const wxCueSheet& cs)
{
    wxCueComponent::copy(cs);

    m_content = cs.m_content;
    m_logs = cs.m_logs;
    m_accuripLogs = cs.m_accuripLogs;
    m_covers = cs.m_covers;
    m_catalogs = cs.m_catalogs;
    m_cdTextFiles = cs.m_cdTextFiles;
    m_tracks = cs.m_tracks;
    m_dataFiles = cs.m_dataFiles;
}

void wxCueSheet::AddCdTextInfoTagToAllTracks(const wxCueTag& tag)
{
    for (size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; ++i)
    {
        m_tracks[i].AddCdTextInfoTag(tag);
    }
}

void wxCueSheet::AddTagToAllTracks(const wxCueTag& tag)
{
    for (size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; ++i)
    {
        m_tracks[i].AddTag(tag);
    }
}

void wxCueSheet::PrepareToAppend()
{
    for (size_t i = 0, nCount = m_cdTextTags.GetCount(); i < nCount; ++i)
    {
        if (m_cdTextTags[i] == wxCueTag::Name::TITLE)
        {
            wxCueTag albumTag(m_cdTextTags[i].GetSource(), wxCueTag::Name::ALBUM, m_cdTextTags[i].GetValue());
            AddTagToAllTracks(albumTag);
        }
        else
        {
            AddCdTextInfoTagToAllTracks(m_cdTextTags[i]);
        }
    }

    for (size_t i = 0, nCount = m_tags.GetCount(); i < nCount; ++i)
    {
        if (m_tags[i] == wxCueTag::Name::ARTIST)
        {
            wxCueTag albumArtistTag(m_tags[i].GetSource(), wxCueTag::Name::ALBUM_ARTIST, m_tags[i].GetValue());
            AddTagToAllTracks(albumArtistTag);
        }
        else
        {
            AddTagToAllTracks(m_tags[i]);
        }
    }

    for (size_t i = 0, nCount = m_catalogs.GetCount(); i < nCount; ++i)
    {
        AddTagToAllTracks(m_catalogs[i]);
    }

    m_catalogs.Clear();
    m_cdTextTags.Clear();
    m_tags.Clear();
}

void wxCueSheet::AppendFileNames(wxArrayFileName& dest, const wxArrayFileName& source)
{
    bool bAdd;

    for (size_t i = 0, nSourceCount = source.GetCount(); i < nSourceCount; ++i)
    {
        bAdd = true;
        for (size_t j = 0, nDestCount = dest.GetCount(); j < nDestCount; ++j)
        {
            if (dest[j] == source[i])
            {
                bAdd = false;
                break;
            }
        }

        if (bAdd) dest.Add(source[i]);
    }
}

wxCueSheet& wxCueSheet::Append(const wxCueSheet& _cs)
{
    wxCueSheet cs(_cs);

    cs.PrepareToAppend();

    wxCueComponent::Append(cs);

    WX_APPEND_ARRAY(m_content, cs.m_content);
    AppendFileNames(m_logs, cs.m_logs);
    AppendFileNames(m_accuripLogs, cs.m_accuripLogs);
    wxCoverFile::Append(m_covers, cs.m_covers);
    WX_APPEND_ARRAY(m_catalogs, cs.m_catalogs);
    WX_APPEND_ARRAY(m_cdTextFiles, cs.m_cdTextFiles);

    size_t nDataFilesCount = m_dataFiles.GetCount();

    WX_APPEND_ARRAY(m_dataFiles, cs.m_dataFiles);

    size_t nNumberOffset = 0;

    if (!m_tracks.IsEmpty())
    {
        wxTrack& lastTrack = GetLastTrack();
        nNumberOffset = lastTrack.GetNumber();
    }

    wxArrayTrack tracks(cs.m_tracks);

    for (size_t i = 0, nCount = tracks.GetCount(); i < nCount; ++i)
    {
        tracks[i].SetNumber(nNumberOffset + tracks[i].GetNumber());
        tracks[i].ShiftDataFileIdx(nDataFilesCount);
    }

    WX_APPEND_ARRAY(m_tracks, tracks);
    return *this;
}

wxArrayTrack& wxCueSheet::SortTracks()
{
    for (size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; ++i)
    {
        m_tracks[i].SortIndicies();
    }

    m_tracks.Sort(wxTrack::CompareFn);
    return m_tracks;
}

size_t wxCueSheet::GetDataFileIdxIfLastForTrack(size_t nTrackNo) const
{
    size_t nTracksCount = m_tracks.GetCount();

    wxASSERT(nTrackNo < nTracksCount);

    if ((nTrackNo + 1u) == nTracksCount)
    {
        return GetTrack(nTrackNo).GetMinDataFileIdx(false);
    }
    else
    {
        size_t nCurDataFileIdx = GetTrack(nTrackNo).GetMaxDataFileIdx(true);
        size_t nNextDataFileIdx = GetTrack(nTrackNo + 1u).GetMinDataFileIdx(false);

        if (nNextDataFileIdx > nCurDataFileIdx) return GetTrack(nTrackNo).GetMinDataFileIdx(false);
        else return wxIndex::UnknownDataFileIdx;
    }
}

wxString wxCueSheet::FormatTrack(wxCueTag::TagSources sources, size_t trackNo, const wxString& sFmt) const
{
    wxHashString replacements;

    GetReplacements(sources, replacements);
    const wxTrack& track = GetTrack(trackNo);

    track.GetReplacements(sources, replacements);

    wxString s(sFmt);

    for (wxHashString::const_iterator i = replacements.begin(); i != replacements.end(); ++i)
    {
        wxString sFind(i->first);
        sFind.Prepend('%').Append('%');
        s.Replace(sFind, i->second, true);
    }

    return s;
}

wxString wxCueSheet::Format(wxCueTag::TagSources sources, const wxString& sFmt) const
{
    wxHashString replacements;

    GetReplacements(sources, replacements);

    wxString s(sFmt);

    for (wxHashString::const_iterator i = replacements.begin(); i != replacements.end(); ++i)
    {
        wxString sFind(i->first);
        sFind.Prepend('%').Append('%');
        s.Replace(sFind, i->second, true);
    }

    return s;
}

bool wxCueSheet::HasSingleDataFile(wxDataFile& dataFile) const
{
    size_t nDataFiles = m_dataFiles.GetCount();

    if (nDataFiles == 1u)
    {
        dataFile = m_dataFiles[0];
        return true;
    }
    else
    {
        return false;
    }
}

bool wxCueSheet::HasSingleDataFile() const
{
    return (m_dataFiles.GetCount() == 1u);
}

wxCueSheet& wxCueSheet::SetSingleDataFile(const wxDataFile& dataFile)
{
    m_dataFiles.Clear();
    m_dataFiles.Add(dataFile);
    for (size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; ++i)
    {
        m_tracks[i].SetDataFileIdx(0u);
    }

    return *this;
}

wxCueSheet& wxCueSheet::SetDataFiles(const wxArrayDataFile& dataFiles)
{
    size_t nDataFilesCount = m_dataFiles.GetCount();
    size_t nNewDataFilesCount = dataFiles.GetCount();

    if (nDataFilesCount > nNewDataFilesCount)
    {
        wxLogWarning(_("Not all data files in cue sheet are replaced"));
        wxLogWarning(_("%d data file(s) left"), (nDataFilesCount - nNewDataFilesCount));
        for (size_t i = 0; i < nNewDataFilesCount; i++)
        {
            m_dataFiles[i] = dataFiles[i];
        }
    }
    else if (nDataFilesCount == nNewDataFilesCount)
    {
        m_dataFiles.Clear();
        WX_APPEND_ARRAY(m_dataFiles, dataFiles);
    }
    else
    {
        wxLogWarning(_("Too many data files"));
        wxLogWarning(_("%d data file(s) too much"), (nNewDataFilesCount - nDataFilesCount));
        for (size_t i = 0; i < nDataFilesCount; i++)
        {
            m_dataFiles[i] = dataFiles[i];
        }
    }

    return *this;
}

bool wxCueSheet::HasDuration() const
{
    bool bRes = true;

    for (size_t i = 0, nCount = m_dataFiles.GetCount(); (i < nCount) && bRes; ++i)
    {
        bRes = bRes && m_dataFiles[i].HasDuration();
    }

    return bRes;
}

wxDuration wxCueSheet::GetDuration(size_t nDataFileIdx) const
{
    wxDuration duration;

    if (nDataFileIdx == wxIndex::UnknownDataFileIdx || nDataFileIdx == 0u) return duration;

    if (nDataFileIdx == 0)
    {
        if (!m_dataFiles.IsEmpty())
        {
            duration = wxDuration(m_dataFiles[0].GetDuration().GetSamplingInfo(), wxULL(0));
        }
        return duration;
    }

    m_dataFiles[0].GetDuration().GetSamplingInfo();

    wxASSERT(HasDuration());
    wxASSERT(nDataFileIdx <= m_dataFiles.GetCount());

    bool   bFirst = true;
    bool   bStop = false;
    size_t nFirstTrack, nLastTrack;

    for (size_t i = 0; i < nDataFileIdx; ++i)
    {
        if (!GetRelatedTracks(i, nFirstTrack, nLastTrack))
        {
            wxLogDebug("wxCueSheet::GetDuration - skipping unused data file %" wxSizeTFmtSpec "d", i);
            continue;
        }

        const wxDuration& dfDuration = m_dataFiles[i].GetDuration();

        if (bFirst)
        {
            duration = dfDuration;
            bFirst = false;
        }
        else if (!duration.Add(dfDuration))
        {
            bStop = true;
        }
    }

    if (bStop)
    {
        wxLogDebug("Fail to calculate duration of cue sheet");
        duration.Invalidate();
    }

    return duration;
}

wxDuration wxCueSheet::GetDuration() const
{
    return GetDuration(m_dataFiles.GetCount());
}

bool wxCueSheet::CalculateDuration(const wxString& sAlternateExt)
{
    bool bRes = true;

    for (size_t i = 0, nCount = m_dataFiles.GetCount(); i < nCount; ++i)
    {
        if (!m_dataFiles[i].HasDuration())
        {
            if (!m_dataFiles[i].GetInfo(sAlternateExt))
            {
                wxLogDebug("Fail to calculate duration for track %" wxSizeTFmtSpec "d", i);
                bRes = false;
            }
        }
    }

    return bRes;
}

namespace
{
    size_t only_suitable_tags(wxArrayCueTag& tags)
    {
        wxCueComponent::ENTRY_TYPE eEntryType;
        size_t                     nCounter = 0;

        for (size_t i = 0, nCount = tags.GetCount(); i < nCount; ++i)
        {
            if (wxCueComponent::GetCdTextInfoType(tags[i].GetName(), eEntryType) &&
                 eEntryType == wxCueComponent::TRACK)
            {
                tags.RemoveAt(i);
                nCounter += 1;
                nCount -= 1;
                i -= 1;
            }
        }

        return nCounter;
    }

    wxString concatenate(const wxString& s1, const wxString& s2)
    {
        if (s1.IsEmpty() && s2.IsEmpty()) return wxEmptyString;
        else if (s1.IsEmpty() && !s2.IsEmpty()) return s2;
        else if (!s1.IsEmpty() && s2.IsEmpty()) return s1;
        else return s1 + ' ' + s2;
    }
}

void wxCueSheet::SanitizeTags(const wxTagSynonimsCollection& discSynonims, const wxTagSynonimsCollection& trackSynonims, bool bMerge, bool bIncludeDiscNumber)
{
    const size_t nTracks = m_tracks.GetCount();

    MoveCdTextInfoTags(discSynonims);
    for (size_t i = 0; i < nTracks; ++i)
    {
        m_tracks[i].MoveCdTextInfoTags(trackSynonims);
    }

    if (nTracks > 1)
    {
        wxArrayCueTag commonTags;
        wxArrayCueTag group;

        // CD-TEXT
        group = m_tracks[0].GetCdTextTags();
        for (size_t i = 1; i < nTracks; ++i)
        {
            wxCueTag::CommonTags(commonTags, group, m_tracks[i].GetCdTextTags());
            group = commonTags;
        }

        only_suitable_tags(commonTags);

        AddCdTextInfoTags(commonTags);
        for (size_t i = 0; i < nTracks; ++i)
        {
            m_tracks[i].RemoveCdTextInfoTags(commonTags);
        }

        // TAGS
        commonTags.Clear();
        group = m_tracks[0].GetTags();
        for (size_t i = 1; i < nTracks; ++i)
        {
            wxCueTag::CommonTags(commonTags, group, m_tracks[i].GetTags());
            group = commonTags;
        }

        AddTags(commonTags);
        for (size_t i = 0; i < nTracks; ++i)
        {
            m_tracks[i].RemoveTags(commonTags);
        }
    }

    // ALBUM -> TITLE
    {
        wxArrayCueTag albumTags;
        MoveTags(wxCueTag::Name::ALBUM, albumTags);
        for (size_t i = 0, nCount = albumTags.GetCount(); i < nCount; ++i)
        {
            AddCdTextInfoTag(albumTags[i].Rename(wxCueTag::Name::TITLE));
        }
    }

    if (!bIncludeDiscNumber)
    {
        RemoveTag(wxCueTag::Name::DISCNUMBER);
        RemoveTag(wxCueTag::Name::TOTALDISCS);

        for (size_t j = 0; j < nTracks; ++j)
        {
            m_tracks[j].RemoveTag(wxCueTag::Name::DISCNUMBER);
            m_tracks[j].RemoveTag(wxCueTag::Name::TOTALDISCS);
        }
    }

    // Trying to find common part of album tag
    if (bMerge)
    {
        if (bIncludeDiscNumber)
        {
            const wxCueTag totalDiscsTag(
                wxCueTag::TAG_AUTO_GENERATED,
                wxCueTag::Name::TOTALDISCS,
                wxString::Format("%" wxSizeTFmtSpec "u", m_content.GetCount()));
            AddTag(totalDiscsTag);
        }

        wxArrayCueTag albumTags;
        size_t        nElements = 0;

        for (size_t i = 0; i < nTracks; ++i)
        {
            nElements += m_tracks[i].GetTags(wxCueTag::Name::ALBUM, albumTags);
        }

        if (nElements > 0)
        {
            wxASSERT(albumTags.GetCount() >= 1);

            WX_DECLARE_STRING_HASH_MAP(unsigned long, wxHashMapStringToULong);
            wxHashMapStringToULong albumNumbers;

            bool     bFirst = true;
            bool     bIsCommon = true;
            wxString sCommonAlbum;
            wxRegEx  reAlbum1(wxString::Format(ALBUM_REG_EX1, GetCdAliasesRegExp()), wxRE_ICASE);
            wxRegEx  reAlbum2(wxString::Format(ALBUM_REG_EX2, GetCdAliasesRegExp()), wxRE_ICASE);

            wxASSERT(reAlbum1.IsValid());
            wxASSERT(reAlbum2.IsValid());

            for (size_t j = 0, nCount = albumTags.GetCount(); j < nCount && bIsCommon; ++j)
            {
                wxString sAlbum(albumTags[j].GetValue());

                if (reAlbum1.Matches(sAlbum))
                {
                    wxASSERT(reAlbum1.GetMatchCount() > 5);
                    wxString sLocalAlbum1(reAlbum1.GetMatch(sAlbum, 1));
                    wxString sDiscNumber(reAlbum1.GetMatch(sAlbum, 4));
                    wxString sLocalAlbum2(reAlbum1.GetMatch(sAlbum, 5));

                    unsigned long u;

                    if (sDiscNumber.ToULong(&u)) albumNumbers[albumTags[j].GetValue()] = u;

                    sAlbum = concatenate(sLocalAlbum1, sLocalAlbum2);
                }
                else if (reAlbum2.Matches(sAlbum))
                {
                    wxASSERT(reAlbum1.GetMatchCount() > 5);
                    wxString sLocalAlbum1(reAlbum2.GetMatch(sAlbum, 1));
                    wxString sDiscNumber(reAlbum2.GetMatch(sAlbum, 4));
                    wxString sLocalAlbum2(reAlbum2.GetMatch(sAlbum, 5));

                    unsigned long u;

                    if (sDiscNumber.ToULong(&u)) albumNumbers[albumTags[j].GetValue()] = u;

                    sAlbum = concatenate(sLocalAlbum1, sLocalAlbum2);
                }

                if (bFirst)
                {
                    sCommonAlbum = sAlbum;
                    bFirst = false;
                }
                else if (sCommonAlbum.Cmp(sAlbum) != 0)
                {
                    bIsCommon = false;
                }
            }

            if (bIsCommon)
            {
                wxLogInfo(_(wxS("Album name: " ENQUOTED_STR_FMT)), sCommonAlbum);

                RemoveCdTextInfoTag(wxCueTag::Name::TITLE);
                const wxCueTag albumNameTag(wxCueTag::TAG_AUTO_GENERATED, wxCueTag::Name::TITLE, sCommonAlbum);
                AddCdTextInfoTag(albumNameTag);

                for (wxHashMapStringToULong::const_iterator i = albumNumbers.begin(), iend = albumNumbers.end(); i != iend; ++i)
                {
                    const wxCueTag albumTag(wxCueTag::TAG_AUTO_GENERATED, wxCueTag::Name::ALBUM, i->first);

                    if (bIncludeDiscNumber)
                    {
                        const wxCueTag discNumberTag(wxCueTag::TAG_AUTO_GENERATED, wxCueTag::Name::DISCNUMBER, wxString::Format("%u", i->second));

                        for (size_t j = 0; j < nTracks; ++j)
                        {
                            m_tracks[j].AddTagIfAndRemove(discNumberTag, albumTag);
                        }
                    }
                    else
                    {
                        for (size_t j = 0; j < nTracks; ++j)
                        {
                            m_tracks[j].RemoveTag(albumTag);
                        }
                    }
                }
            }
        }
    }
}

bool wxCueSheet::ApplyTagsFromJson(const wxFileName& jsonFile)
{
    wxLogInfo(_(wxS("Applying tags from " ENQUOTED_STR_FMT)), jsonFile.GetFullName());
    wxTextOutputStreamOnString tos;

    {
        wxFileInputStream is(jsonFile.GetFullPath());
        if (!is.IsOk())
        {
            wxLogError(_(wxS("Fail to open " ENQUOTED_STR_FMT)), jsonFile.GetFullName());
            return false;
        }

        wxTextInputStream tis(is, wxEmptyString, wxConvUTF8);
        while (!tis.GetInputStream().Eof())
        {
            *tos << tis.ReadLine() << endl;
        }
    }
    tos->Flush();

    try
    {
        ApplyTagsFromJson(wxJson::parse(tos.GetString().utf8_string()));
        return true;
    }
    catch (nlohmann::detail::exception err)
    {
        wxLogError(err.what());
        return false;
    }
}

void wxCueSheet::ApplyTagsFromJson(const wxJson& tags)
{
    if (tags.contains("album"))
    {
        const wxJson& album = tags["album"];
        if (album.is_object())
        {
            for (auto i = album.cbegin(), end = album.cend(); i != end; ++i)
            {
                const wxString tagName = wxString::FromUTF8(i.key());
                if (!i.value().is_string())
                {
                    wxLogWarning(_("json[album]: expecting string value for %s"), tagName);
                    continue;
                }

                const wxString tagVal = wxString::FromUTF8(i.value());
                const wxCueTag tag(wxCueTag::TAG_AUTO_GENERATED, tagName, tagVal);
                RemoveTag(tagName);
                AddTag(tag);
            }
        }
    }

    if (tags.contains("chapters"))
    {
        const wxJson& chapters = tags["chapters"];
        if (chapters.is_array())
        {
            size_t chapterNo = 0;
            for (auto i = chapters.cbegin(), end = chapters.cend(); i != end; ++i, ++chapterNo)
            {
                if (!i->is_object()) continue;
                if (chapterNo >= GetTracksCount()) continue;

                wxTrack& track = GetTrack(chapterNo);
                for (auto j = i->cbegin(), jend = i->cend(); j != jend; ++j)
                {
                    const wxString tagName = wxString::FromUTF8(j.key());
                    if (!j.value().is_string())
                    {
                        wxLogWarning(_("json[chapter]: expecting string value for %s"), tagName);
                        continue;
                    }
                    const wxString tagVal = wxString::FromUTF8(j.value());
                    const wxCueTag tag(wxCueTag::TAG_AUTO_GENERATED, tagName, tagVal);
                    RemoveTag(tagName);
                    AddTag(tag);
                }
            }
        }
        else
        {
            wxLogWarning(_("json: expecting array value for 'chapters' key"));
        }
    }
}
