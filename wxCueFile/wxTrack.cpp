/*
 * wxTrack.cpp
 */

#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxDuration.h>

 // ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxTrack, wxCueComponent);

// ===============================================================================

const wxTrack::FLAG_STR wxTrack::FlagString[] =
{
    { DCP, "DCP" },
    { CH4, "CH4" },
    { PRE, "PRE" },
    { SCMS, "SCMS" },
    { DATA, "DATA" },
    { NONE, "NONE" }
};

// ===============================================================================

const wxTrack::DATA_MODE_STR wxTrack::DataModeString[] =
{
    { AUDIO, "AUDIO" },
    { CDG, "CDG" },
    { MODE1_2048, "MODE1/2048" },
    { MODE1_2352, "MODE1/2352" },
    { MODE2_2336, "MODE2/2336" },
    { MODE2_2352, "MODE2/2352" },
    { CDI_2336, "CDI/2336" },
    { CDI_2352, "CDI/2352" }
};

// ===============================================================================

wxTrack::wxTrack(void):
    wxCueComponent(true), m_number(0)
{
}

wxTrack::wxTrack(const wxTrack& track):
    wxCueComponent(true), m_dataMode(AUDIO)
{
    copy(track);
}

wxTrack::wxTrack(unsigned long number):
    wxCueComponent(true), m_number(number), m_dataMode(AUDIO)
{
}

wxTrack& wxTrack::operator =(const wxTrack& track)
{
    copy(track);
    return *this;
}

void wxTrack::copy(const wxTrack& track)
{
    wxCueComponent::copy(track);

    ClearPreGap();
    ClearPostGap();

    m_number = track.m_number;
    m_dataMode = track.m_dataMode;
    m_indexes = track.m_indexes;
    m_flags = track.m_flags;

    if (track.HasPreGap()) SetPreGap(track.GetPreGap());

    if (track.HasPostGap()) SetPreGap(track.GetPostGap());
}

size_t wxTrack::GetNumber() const
{
    return m_number;
}

wxTrack& wxTrack::SetNumber(size_t nNumber)
{
    m_number = nNumber;
    return *this;
}

wxTrack::DataMode wxTrack::GetMode() const
{
    return m_dataMode;
}

wxString wxTrack::GetModeAsString() const
{
    return DataModeToString(m_dataMode);
}

wxTrack& wxTrack::SetMode(wxTrack::DataMode dataMode)
{
    m_dataMode = dataMode;
    return *this;
}

bool wxTrack::HasIndexes() const
{
    return !m_indexes.IsEmpty();
}

size_t wxTrack::GetIndexesCount() const
{
    return m_indexes.GetCount();
}

const wxArrayIndex& wxTrack::GetIndexes() const
{
    return m_indexes;
}

void wxTrack::AddIndex(const wxIndex& idx)
{
    m_indexes.Add(idx);
}

bool wxTrack::HasPreGap() const
{
    return m_pPreGap;
}

bool wxTrack::HasZeroIndex() const
{
    return (!m_indexes.IsEmpty() && (m_indexes[0].GetNumber() == 0u));
}

bool wxTrack::HasPostGap() const
{
    return m_pPostGap;
}

const wxIndex& wxTrack::GetPreGap() const
{
    if (HasPreGap())
    {
        return *m_pPreGap;
    }
    else
    {
        wxFAIL_MSG("Trying to access nonexistient pre-gap");
        return *m_pPreGap;	// to make compiler happy
    }
}

const wxIndex& wxTrack::GetPostGap() const
{
    if (HasPostGap())
    {
        return *m_pPostGap;
    }
    else
    {
        wxFAIL_MSG("Trying to access nonexistient post-gap");
        return *m_pPostGap;	// to make compiler happy
    }
}

void wxTrack::ClearPreGap()
{
    m_pPreGap.reset();
}

void wxTrack::ClearPostGap()
{
    m_pPostGap.reset();
}

void wxTrack::SetPreGap(const wxIndex& preGap)
{
    m_pPreGap.reset(new wxIndex(preGap));
}

void wxTrack::SetPostGap(const wxIndex& postGap)
{
    m_pPostGap.reset(new wxIndex(postGap));
}

bool wxTrack::IsRelatedToDataFileIdx(size_t nDataFileIdx, bool bPrePost) const
{
    wxASSERT(nDataFileIdx != wxIndex::UnknownDataFileIdx);

    if (bPrePost)
    {
        if (HasPreGap())
            if (m_pPreGap->HasDataFileIdx() && (m_pPreGap->GetDataFileIdx() == nDataFileIdx)) return true;

        if (HasPostGap())
            if (m_pPostGap->HasDataFileIdx() && (m_pPostGap->GetDataFileIdx() == nDataFileIdx)) return true;
    }

    for (size_t i = 0, nCount = m_indexes.GetCount(); i < nCount; i++)
    {
        if (!bPrePost && m_indexes[i].IsZero()) continue;

        if (m_indexes[i].HasDataFileIdx() && (m_indexes[i].GetDataFileIdx() == nDataFileIdx)) return true;
    }

    return false;
}

static void MaxDataFile(size_t& nDataFileIdx, const wxIndex& idx, bool bPrePost)
{
    if (!bPrePost && idx.IsZero()) return;

    if (idx.HasDataFileIdx())
    {
        if (nDataFileIdx == wxIndex::UnknownDataFileIdx) nDataFileIdx = idx.GetDataFileIdx();
        else if (idx.GetDataFileIdx() > nDataFileIdx) nDataFileIdx = idx.GetDataFileIdx();
    }
}

size_t wxTrack::GetMaxDataFileIdx(bool bPrePost) const
{
    size_t nDataFileIdx = wxIndex::UnknownDataFileIdx;

    if (bPrePost)
    {
        if (HasPreGap()) MaxDataFile(nDataFileIdx, *m_pPreGap, true);

        if (HasPostGap()) MaxDataFile(nDataFileIdx, *m_pPostGap, true);
    }

    for (size_t i = 0, nCount = m_indexes.GetCount(); i < nCount; i++)
    {
        MaxDataFile(nDataFileIdx, m_indexes[i], bPrePost);
    }

    return nDataFileIdx;
}

static void MinDataFile(size_t& nDataFileIdx, const wxIndex& idx, bool bPrePost)
{
    if (!bPrePost && idx.IsZero()) return;

    if (idx.HasDataFileIdx())
    {
        if (nDataFileIdx == wxIndex::UnknownDataFileIdx) nDataFileIdx = idx.GetDataFileIdx();
        else if (idx.GetDataFileIdx() < nDataFileIdx) nDataFileIdx = idx.GetDataFileIdx();
    }
}

size_t wxTrack::GetMinDataFileIdx(bool bPrePost) const
{
    size_t nDataFileIdx = wxIndex::UnknownDataFileIdx;

    if (bPrePost)
    {
        if (HasPreGap()) MinDataFile(nDataFileIdx, *m_pPreGap, true);

        if (HasPostGap()) MinDataFile(nDataFileIdx, *m_pPostGap, true);
    }

    for (size_t i = 0, nCount = m_indexes.GetCount(); i < nCount; ++i)
    {
        MinDataFile(nDataFileIdx, m_indexes[i], bPrePost);
    }

    return nDataFileIdx;
}

void wxTrack::ShiftDataFileIdx(size_t nOffset)
{
    if (HasPreGap()) m_pPreGap->ShiftDataFileIdx(nOffset);

    if (HasPostGap()) m_pPostGap->ShiftDataFileIdx(nOffset);

    for (size_t i = 0, nCount = m_indexes.GetCount(); i < nCount; i++)
    {
        m_indexes[i].ShiftDataFileIdx(nOffset);
    }
}

void wxTrack::SetDataFileIdx(size_t nDataFileIdx)
{
    if (HasPreGap()) m_pPreGap->ShiftDataFileIdx(nDataFileIdx);

    if (HasPostGap()) m_pPostGap->ShiftDataFileIdx(nDataFileIdx);

    for (size_t i = 0, nCount = m_indexes.GetCount(); i < nCount; i++)
    {
        m_indexes[i].ShiftDataFileIdx(nDataFileIdx);
    }
}

const wxTrack::wxArrayFlag& wxTrack::GetFlags() const
{
    return m_flags;
}

wxTrack& wxTrack::AddFlag(wxTrack::Flag flag)
{
    m_flags.Add(flag);
    return *this;
}

bool wxTrack::AddFlag(const wxString& sFlag)
{
    Flag flag;

    if (wxTrack::StringToFlag(sFlag, flag))
    {
        m_flags.Add(flag);
        return true;
    }
    else
    {
        return false;
    }
}

void wxTrack::ClearFlags()
{
    m_flags.Clear();
}

wxString wxTrack::GetFlagsAsString() const
{
    wxString s;

    for (size_t i = 0, nCount = m_flags.GetCount(); i < nCount; i++)
    {
        s << FlagToString(m_flags[i]) << ' ';
    }

    s = s.RemoveLast();
    return s;
}

bool wxTrack::HasFlags() const
{
    return (m_flags.GetCount() > 0u);
}

bool wxTrack::HasFlag(wxTrack::Flag f) const
{
    for (size_t i = 0, nCount = m_flags.GetCount(); i < nCount; i++)
    {
        if (m_flags[i] == f) return true;
    }

    return false;
}

wxString wxTrack::GetFlagRegExp()
{
    return get_texts_regexp(FlagString);
}

wxString wxTrack::FlagToString(wxTrack::Flag flag)
{
    return to_string(flag, FlagString);
}

bool wxTrack::StringToFlag(const wxString& s, wxTrack::Flag& flag)
{
    return from_string(s, flag, FlagString);
}

wxString wxTrack::GetDataModeRegExp()
{
    return get_texts_regexp(DataModeString);
}

wxString wxTrack::DataModeToString(wxTrack::DataMode mode)
{
    return to_string(mode, DataModeString);
}

bool wxTrack::StringToDataMode(const wxString& s, wxTrack::DataMode& mode)
{
    return from_string(s, mode, DataModeString);
}

bool wxTrack::SetMode(const wxString& sMode)
{
    DataMode mode;

    if (sMode.IsEmpty()) mode = AUDIO;
    else if (!StringToDataMode(sMode, mode)) return false;

    m_dataMode = mode;
    return true;
}

bool wxTrack::IsValid() const
{
    return (m_number < 100);
}

int wxTrack::CompareFn(wxTrack** t1, wxTrack** t2)
{
    if ((*t1)->GetNumber() < (*t2)->GetNumber()) return -1;
    else if ((*t1)->GetNumber() > (*t2)->GetNumber()) return 1;
    else return 0;
}

wxArrayIndex& wxTrack::SortIndicies()
{
    m_indexes.Sort(wxIndex::CompareFn);
    return m_indexes;
}

const wxIndex& wxTrack::GetZeroIndex() const
{
    wxASSERT(HasZeroIndex());
    return m_indexes[0];
}

const wxIndex& wxTrack::GetFirstIndex() const
{
    for (size_t i = 0, nCount = m_indexes.GetCount(); i < nCount; i++)
    {
        if (m_indexes[i].GetNumber() == 1) return m_indexes[i];
    }

    wxASSERT(false);
    return m_indexes[0];
}

void wxTrack::GetReplacements(wxCueTag::TagSources sources, wxHashString& replacements) const
{
    wxCueComponent::GetReplacements(sources, replacements);
    wxString sValue;

    sValue.Printf("%02" wxSizeTFmtSpec "d", m_number);
    replacements["tn"] = sValue;
}

#include <wx/arrimpl.cpp>	// this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(wxArrayTrack);

