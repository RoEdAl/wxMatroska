/*
 * wxTextCueSheetRenderer.cpp
 */

#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>

// ===============================================================================

wxTextCueSheetRenderer::wxTextCueSheetRenderer(wxTextOutputStream* textOutputStream, int dumpFlags):
    m_textOutputStream(textOutputStream),
    m_dumpFlags(dumpFlags),
    m_dataFileIdx(wxIndex::UnknownDataFileIdx),
    m_reSpace("\\p{Xps}+", wxRE_NOSUB)
{
    wxASSERT(m_reSpace.IsValid());
}

void wxTextCueSheetRenderer::Assign(wxTextOutputStream* textOutputStream, int dumpFlags)
{
    wxASSERT(textOutputStream != nullptr);

    m_textOutputStream = textOutputStream;
    m_dumpFlags = dumpFlags;
}

bool wxTextCueSheetRenderer::OnPreRenderDisc(const wxCueSheet& cueSheet)
{
    if (m_textOutputStream == nullptr)
    {
        wxLogError(_("wxTextCueSheetRenderer: output stream not specified"));
        return false;
    }

    return true;
}

bool wxTextCueSheetRenderer::OnRenderDisc(const wxCueSheet& cueSheet)
{
    InternalRenderCueSheet(cueSheet);
    return wxCueSheetRenderer::OnRenderDisc(cueSheet);
}

bool wxTextCueSheetRenderer::OnRenderTrack(const wxCueSheet& cueSheet, const wxTrack& track)
{
    InternalRenderTrack(cueSheet, track);
    return wxCueSheetRenderer::OnRenderTrack(cueSheet, track);
}

bool wxTextCueSheetRenderer::OnRenderIndex(const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& index)
{
    InternalRenderIndex(cueSheet, track, index);
    return wxCueSheetRenderer::OnRenderIndex(cueSheet, track, index);
}

bool wxTextCueSheetRenderer::OnRenderPreGap(const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& preGap)
{
    InternalRenderIndex(cueSheet, track, preGap, "PREGAP");
    return wxCueSheetRenderer::OnRenderPreGap(cueSheet, track, preGap);
}

bool wxTextCueSheetRenderer::OnRenderPostGap(const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& postGap)
{
    InternalRenderIndex(cueSheet, track, postGap, "POSTGAP");
    return wxCueSheetRenderer::OnRenderPostGap(cueSheet, track, postGap);
}

void wxTextCueSheetRenderer::DumpComponentTag(const wxCueComponent& component, const wxCueTag& tag)
{
    const wxString name = m_reSpace.Matches(tag.GetName()) ? wxCueTag::Quote(tag.GetName()) : tag.GetName();
    DumpComponentString(component, "REM", wxString::Format("%s %s", name, tag.GetFlattenValue()));
}

void wxTextCueSheetRenderer::DumpComponentString(
    const wxCueComponent& component,
    const wxString& entry,
    const wxString& text)
{
    if (!text.IsEmpty())
    {
        if (component.IsTrack()) *m_textOutputStream << '\t';
        *m_textOutputStream << ' ' << entry << ' ' << text << endl;
    }
}

void wxTextCueSheetRenderer::InternalRenderComponent(const wxCueComponent& component)
{
    // dump comments
    if ((m_dumpFlags & DUMP_COMMENTS) != 0)
    {
        const wxArrayString& comments = component.GetComments();
        for (wxArrayString::const_iterator i = comments.begin(), end = comments.end(); i != end; ++i)
        {
            DumpComponentString(component, "REM", *i);
        }
    }

    // dump tags
    if ((m_dumpFlags & DUMP_TAGS) != 0)
    {
        const wxArrayCueTag& tags = component.GetTags();
        for (size_t i = 0, cnt = tags.GetCount(); i < cnt; ++i)
        {
            DumpComponentTag(component, tags[i]);
        }
    }

    // dump CT-TEXT info
    const wxArrayCueTag& tags = component.GetCdTextTags();

    for (size_t i = 0, cnt = tags.GetCount(); i < cnt; ++i)
    {
        DumpComponentString(component,
                tags[i].GetName(),
                wxCueComponent::FormatCdTextData(tags[i].GetName(),
                            tags[i].GetValue()));
    }

    // dump garbage
    if ((m_dumpFlags & DUMP_GARBAGE) != 0)
    {
        const wxArrayString garbage = component.GetGarbage();
        for (wxArrayString::const_iterator i = garbage.begin(); i != garbage.end(); ++i)
        {
            *m_textOutputStream << *i << endl;
        }
    }
}

void wxTextCueSheetRenderer::InternalRenderCueSheet(const wxCueSheet& cueSheet)
{
    m_dataFileIdx = wxIndex::UnknownDataFileIdx;
    InternalRenderComponent(cueSheet);

    const wxArrayCueTag& catalogs = cueSheet.GetCatalogs();
    for (size_t i = 0, cnt = catalogs.GetCount(); i < cnt; ++i)
    {
        DumpComponentString(cueSheet, catalogs[i].GetName(), catalogs[i].GetValue());
    }

    const wxArrayFileName& cdtextfiles = cueSheet.GetCdTextFiles();

    for (size_t i = 0, cnt = cdtextfiles.GetCount(); i < cnt; ++i)
    {
        DumpComponentString(cueSheet, wxCueTag::Name::CDTEXTFILE, wxCueTag::Quote(cdtextfiles[i].GetFullName()));
    }
}

void wxTextCueSheetRenderer::InternalRenderTrack(const wxCueSheet& cueSheet, const wxTrack& track)
{
    if ((m_dumpFlags & DUMP_EMPTY_LINES) != 0)
    {
        *m_textOutputStream << endl;
    }

    if (track.GetNumber() == 1u)
    {
        size_t nDataFileIdx = track.GetMinDataFileIdx(true);
        InternalRenderDataFile(cueSheet, nDataFileIdx);
    }
    else if (!track.HasZeroIndex())
    {
        size_t nDataFileIdx = track.GetMinDataFileIdx(false);
        InternalRenderDataFile(cueSheet, nDataFileIdx);
    }

    const wxString line = wxString::Format("%02" wxSizeTFmtSpec "d %s", track.GetNumber(), track.GetModeAsString());
    DumpComponentString(track, "TRACK", line);
    InternalRenderComponent(track);

    // flags
    if (track.HasFlags())
    {
        DumpComponentString(track, "FLAGS", track.GetFlagsAsString());
    }
}

void wxTextCueSheetRenderer::InternalRenderIndex(const wxCueSheet& cueSheet, const wxTrack& WXUNUSED(track), const wxIndex& idx, wxString desc)
{
    if (idx.HasDataFileIdx())
    {
        InternalRenderDataFile(cueSheet, idx.GetDataFileIdx());
    }

    const wxString strIdx(m_si.GetIndexOffsetFramesStr(idx));
    *m_textOutputStream << "\t\t " << desc << ' ' << strIdx << endl;
}

void wxTextCueSheetRenderer::InternalRenderIndex(const wxCueSheet& cueSheet, const wxTrack& WXUNUSED(track), const wxIndex& idx)
{
    if (idx.HasDataFileIdx())
    {
        InternalRenderDataFile(cueSheet, idx.GetDataFileIdx());
    }

    const wxString strIdx = m_si.GetIndexOffsetFramesStr(idx);
    const wxString strIdxNo = wxString::Format("%02" wxSizeTFmtSpec "d", idx.GetNumber());
    *m_textOutputStream << "\t\t INDEX " << strIdxNo << ' ' << strIdx << endl;
}

void wxTextCueSheetRenderer::InternalRenderDataFile(const wxCueSheet& cueSheet, size_t dataFileIdx)
{
    if ((m_dataFileIdx == wxIndex::UnknownDataFileIdx && dataFileIdx != wxIndex::UnknownDataFileIdx) ||
         (dataFileIdx > m_dataFileIdx))
    {
        const wxDataFile& dataFile = cueSheet.GetDataFiles().Item(dataFileIdx);
        *m_textOutputStream <<
            "FILE \"" << dataFile.GetFileName().GetFullName() <<
            "\" " << dataFile.GetFileTypeAsString() << endl;
        m_dataFileIdx = dataFileIdx;
    }
}

wxString wxTextCueSheetRenderer::ToString(const wxCueSheet& cueSheet, int dumpFlags)
{
    wxTextOutputStreamOnString tos;
    wxTextCueSheetRenderer     renderer(&(tos.GetStream()), dumpFlags);

    if (renderer.Render(cueSheet))
    {
        return tos.GetString();
    }
    else
    {
        return wxEmptyString;
    }
}

bool wxTextCueSheetRenderer::ToString(wxTextOutputStream& tos, const wxCueSheet& cueSheet, int dumpFlags)
{
    wxTextCueSheetRenderer renderer(&tos, dumpFlags);

    return renderer.Render(cueSheet);
}

