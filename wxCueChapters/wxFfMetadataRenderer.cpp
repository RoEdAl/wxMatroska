/*
 * wxCuePointsRenderer.cpp
 */

#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#include <wxEncodingDetection/wxTextInputStreamOnString.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>
#include "wxConfiguration.h"
#include "wxInputFile.h"
#include "wxPrimitiveRenderer.h"
#include "wxFfMetadataRenderer.h"
#include "wxApp.h"

 // ===============================================================================

wxFfMetadataRenderer::wxFfMetadataRenderer(const wxConfiguration& cfg):
    wxPrimitiveRenderer(cfg)
{
    InitTagsSynonimsCollections();
}

wxString wxFfMetadataRenderer::get_matroska_title(const wxCueSheet& cueSheet, const wxStringProcessor& stringProcessor) const
{
    wxString matroskaNameFormat = m_cfg.GetMatroskaNameFormat();
    (stringProcessor)(matroskaNameFormat);
    return cueSheet.Format(m_cfg.GetTagSources(), matroskaNameFormat);
}

namespace
{
    wxString ff_escape(const wxString& s)
    {
        wxString res(s);
        res.Replace('=', "\\=");
        res.Replace(';', "\\;");
        res.Replace('#', "\\#");
        res.Replace('\\', "\\\\");
        return res;
    }
}

void wxFfMetadataRenderer::render_tags(const wxArrayCueTag& tags, bool album) const
{
    for (size_t i = 0, count = tags.GetCount(); i < count; ++i)
    {
        const wxCueTag& tag = tags[i];

        if (!m_cfg.RenderReplayGainTags())
        {
            if (tag.IsReplayGain())
            {
                wxLogInfo("FFM Renderer: skipping RG tag %s", tag.GetName());
                continue;
            }
        }

        if (!m_cfg.RenderMultilineTags())
        {
            if (tag.IsMultiline())
            {
                wxLogInfo("FFM renderer: skipping multiline tag %s", tag.GetName());
                continue;
            }
        }

        wxString tagName = tag.GetName();
        if (album)
        {
            if (tag == wxCueTag::Name::TITLE)
            {
                tagName = wxCueTag::Name::ALBUM;
            }
        }

        // tag name
        if (IsLanguageAgnostic(tag))
        {
            *m_os << tagName;
        }
        else
        {
            *m_os << tagName << '-' << m_cfg.GetLang();
        }

        *m_os << '=';

        // tag value - escaped
        if (tag.IsMultiline())
        {
            wxArrayString lines;
            tag.GetValue(lines);
            for (wxArrayString::iterator i = lines.begin(), end = lines.end(); i != end; ++i)
            {
                if ((i + 1) == end)
                {
                    *m_os << ff_escape(*i) << endl;
                }
                else
                {
                    *m_os << ff_escape(*i) << '\\' << endl;
                }
            }
        }
        else
        {
            *m_os << ff_escape(tag.GetValue()) << endl;
        }
    }
}

const wxIndex& wxFfMetadataRenderer::get_idx_from_first_track(const wxTrack& track) const
{
    if (!m_cfg.TrackOneIndexOne() && track.HasPreGap() && track.GetPreGap().HasDataFileIdx())
    {
        return track.GetPreGap();
    }
    else if (!m_cfg.TrackOneIndexOne() && track.HasZeroIndex())
    {
        return track.GetZeroIndex();
    }
    else
    {
        return track.GetFirstIndex();
    }
}

const wxIndex& wxFfMetadataRenderer::get_idx_from_second_track(const wxTrack& track) const
{
    return track.GetFirstIndex();
}

void wxFfMetadataRenderer::RenderDisc(const wxCueSheet& cueSheet)
{
    const wxScopedPtr< wxStringProcessor > stringProcessor(m_cfg.CreateStringProcessor());

    *m_os << ";FFMETADATA1" << endl;
    *m_os << "title=" << ff_escape(get_matroska_title(cueSheet, *stringProcessor)) << endl;

    wxArrayCueTag mappedTags;
    wxArrayCueTag rest;
    wxArrayCueTag artistTags;

    const wxArrayTrack& tracks = cueSheet.GetTracks();

    if (m_cfg.GenerateTags())
    {
        cueSheet.GetTags(m_cfg.GetTagSources(), m_discCdTextSynonims, m_discSynonims, mappedTags, rest);
        if (m_cfg.RenderArtistForTrack()) wxCueTag::GetTags(mappedTags, wxCueTag::Name::ARTIST, artistTags);

        render_tags(mappedTags, true);
        render_tags(rest, true);

        // extra tags
        WriteSizeT(*m_os << "TOTAL_PARTS=", tracks.GetCount()) << endl;
    }

    wxString trackNameFmt = m_cfg.GetTrackNameFormat();
    (*stringProcessor)(trackNameFmt);

    for (size_t i = 0, nCount = tracks.GetCount(); i < nCount; ++i)
    {
        const wxTrack& track = tracks[i];
        const wxString trackTitle = cueSheet.FormatTrack(m_cfg.GetTagSources(), i, trackNameFmt);

        wxDuration posStart, posEnd;

        posStart.Invalidate();
        posEnd.Invalidate();

        {
            // start position - easy
            const wxIndex& idx = (track.GetNumber() == 1u) ? get_idx_from_first_track(track) : get_idx_from_second_track(track);
            if (idx.HasDataFileIdx())
            {
                posStart = cueSheet.GetDuration(idx.GetDataFileIdx());
                const wxULongLong offset(posStart.GetSamplingInfo().GetIndexOffset(idx));
                posStart.Add(offset);
            }
        }

        // end position - more complex
        if (track.HasPostGap() && track.GetPostGap().HasDataFileIdx())
        { // from post-gap
            const wxIndex& idx = track.GetPostGap();
            posEnd = cueSheet.GetDuration(idx.GetDataFileIdx());
            const wxULongLong offset(posEnd.GetSamplingInfo().GetIndexOffset(idx));
            posEnd.Add(offset);
        }

        if (!posEnd.IsValid() && m_cfg.UseDataFiles())
        { // from media file
            const size_t nDataFileIdx = cueSheet.GetDataFileIdxIfLastForTrack(i);

            if (nDataFileIdx != wxIndex::UnknownDataFileIdx)
            {
                posEnd = cueSheet.GetDuration(nDataFileIdx + 1u);
            }
        }

        if (!posEnd.IsValid() && m_cfg.GetChapterEndTimeFromNextChapter() && ((i + 1u) < nCount))
        { // from next track (with offset)
            const wxTrack& nextTrack = cueSheet.GetTrack(i + 1u);
            if (nextTrack.HasZeroIndex())
            { // idx 00 - no offset
                const wxIndex& idx = nextTrack.GetZeroIndex();
                wxASSERT(idx.HasDataFileIdx());
                posEnd = cueSheet.GetDuration(idx.GetDataFileIdx());
                const wxULongLong offset(posEnd.GetSamplingInfo().GetIndexOffset(idx));
                posEnd.Add(offset);
            }
            else
            { // idx 01 - use offset from configuration
                const wxIndex& idx = nextTrack.GetFirstIndex();
                posEnd = cueSheet.GetDuration(idx.GetDataFileIdx());
                wxASSERT(idx.HasDataFileIdx());
                const wxULongLong offset1(posEnd.GetSamplingInfo().GetIndexOffset(idx));
                const wxULongLong offset2(posEnd.GetSamplingInfo().GetFramesFromCdFrames(m_cfg.GetChapterOffset()));
                posEnd.Add(offset1 - offset2);
            }
        }

        if (!posStart.IsValid())
        {
            wxLogWarning(_("Track/chapter %" wxSizeTFmtSpec "d without start time"), i);
            continue;
        }

        {
            const wxULongLong ts = posStart.GetSamplingInfo().ToNanoseconds(posStart.GetNumberOfSamples());
            *m_os << "[CHAPTER]" << endl;
            *m_os << "TIMEBASE=1/1000000000" << endl;
            *m_os << "START=" << ts << endl;
        }

        if (posEnd.IsValid())
        {
            const wxULongLong ts = posEnd.GetSamplingInfo().ToNanoseconds(posEnd.GetNumberOfSamples());
            *m_os << "END=" << ts << endl;
        }
        else
        {
            wxLogWarning(_("FFM Renderer: Track/chapter %" wxSizeTFmtSpec "d without end time - ffmpeg don't like that"), i);
            *m_os << ";END=<unknown>" << endl;
        }

        WriteSizeT(*m_os << "track=", track.GetNumber()) << endl;
        *m_os << "title=" << ff_escape(trackTitle) << endl;

        if (!m_cfg.GenerateTags())
        {
            continue;
        }

        track.GetTags(m_cfg.GetTagSources(), m_trackCdTextSynonims, m_trackSynonims, mappedTags, rest);
        if (m_cfg.RenderArtistForTrack())
        {
            wxArrayCueTag artists;
            const size_t res = wxCueTag::GetTags(mappedTags, wxCueTag::Name::ARTIST, artists);

            if (res == 0 && !artistTags.IsEmpty())
            {
                wxCueTag::AddTags(mappedTags, artistTags);
            }
        }

        render_tags(mappedTags, false);
        render_tags(rest, false);
    }
}

bool wxFfMetadataRenderer::Save(const wxFileName& outputFile)
{
    wxFileOutputStream os(outputFile.GetFullPath());

    if (os.IsOk())
    {
        wxLogInfo(_("Creating ffmpeg metadata file \u201C%s\u201D"), outputFile.GetFullName());
        // ENC:UTF-8 EOL:UNIX BOM:no
        wxSharedPtr< wxTextOutputStream > pStream(wxTextOutputStreamWithBOMFactory::CreateUTF8(os, wxEOL_UNIX, false, false));
        m_os.SaveTo(*pStream);
        return true;
    }
    else
    {
        wxLogError(_("Fail to save ffmpeg metadata to \u201C%s\u201D"), outputFile.GetFullName());
        return false;
    }
}

