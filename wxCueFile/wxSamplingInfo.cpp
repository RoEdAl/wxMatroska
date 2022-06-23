/*
 * wxSamplingInfo.cpp
 */

#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>

 // ===============================================================================

const wxTimeSpan wxSamplingInfo::wxInvalidDuration = wxTimeSpan::Hours(-1);
const wxUint64   wxSamplingInfo::wxInvalidNumberOfFrames = wxUINT64_MAX;

// ===============================================================================

wxSamplingInfo::wxSamplingInfo(void):
    m_samplingRate(44100), m_numChannels(2), m_bitsPerSample(16)
{
}

wxSamplingInfo::wxSamplingInfo(const wxSamplingInfo& si)
{
    copy(si);
}

wxSamplingInfo::wxSamplingInfo(unsigned long samplingRate, unsigned short numChannels, unsigned short bitsPerSample):
    m_samplingRate(samplingRate), m_numChannels(numChannels),
    m_bitsPerSample(bitsPerSample)
{
}

wxSamplingInfo& wxSamplingInfo::Assign(unsigned long samplingRate, unsigned short numChannels, unsigned short bitsPerSample)
{
    m_samplingRate = samplingRate;
    m_numChannels = numChannels;
    m_bitsPerSample = bitsPerSample;
    return *this;
}

wxSamplingInfo& wxSamplingInfo::SetDefault()
{
    m_samplingRate = 44100;
    m_numChannels = 2;
    m_bitsPerSample = 16;
    return *this;
}

wxSamplingInfo& wxSamplingInfo::operator =(const wxSamplingInfo& si)
{
    copy(si);
    return *this;
}

void wxSamplingInfo::copy(const wxSamplingInfo& si)
{
    m_samplingRate = si.m_samplingRate;
    m_numChannels = si.m_numChannels;
    m_bitsPerSample = si.m_bitsPerSample;
}

unsigned long wxSamplingInfo::GetSamplingRate() const
{
    return m_samplingRate;
}

unsigned short wxSamplingInfo::GetNumberOfChannels() const
{
    return m_numChannels;
}

unsigned short wxSamplingInfo::GetBitsPerSample() const
{
    return m_bitsPerSample;
}

wxSamplingInfo& wxSamplingInfo::SetSamplingRate(unsigned long samplingRate)
{
    m_samplingRate = samplingRate;
    return *this;
}

wxSamplingInfo& wxSamplingInfo::SetNumberOfChannels(unsigned short numChannels)
{
    m_numChannels = numChannels;
    return *this;
}

wxSamplingInfo& wxSamplingInfo::SetBitsPerSample(unsigned short bitsPerSample)
{
    m_bitsPerSample = bitsPerSample;
    return *this;
}

bool wxSamplingInfo::IsOK(bool ignoreBitsPerSample) const
{
    return
        (m_samplingRate > 0) &&
        (m_numChannels > 0) &&
        (ignoreBitsPerSample ? true : (m_bitsPerSample > 0)) &&
        ((m_bitsPerSample % 8) == 0);
}

bool wxSamplingInfo::Equals(const wxSamplingInfo& si, bool ignoreBitsPerSample) const
{
    if (!(IsOK(ignoreBitsPerSample) || si.IsOK(ignoreBitsPerSample))) return true;
    else if (IsOK(ignoreBitsPerSample) && si.IsOK(ignoreBitsPerSample))
        return
        (m_samplingRate == si.m_samplingRate) &&
        (m_numChannels == si.m_numChannels) &&
        (ignoreBitsPerSample ? true : (m_bitsPerSample == si.m_bitsPerSample));
    else return false;
}

wxULongLong wxSamplingInfo::GetNumberOfFramesFromBytes(const wxULongLong& bytes) const
{
    wxASSERT(IsOK());
    wxULongLong bytesPerFrame(0, m_bitsPerSample * m_numChannels / 8);
    wxULongLong frames(bytes);

    frames /= bytesPerFrame;
    return frames;
}

wxTimeSpan wxSamplingInfo::GetDuration(wxULongLong frames) const
{
    wxASSERT(IsOK(true));

    if (frames.GetValue() == wxInvalidNumberOfFrames) return wxInvalidDuration;

    // samples -> duration
    // 441(00) = 10(00) ms
    wxULongLong duration(frames);

    duration *= wxULL(1000);
    wxULongLong longSamplesRate(0, m_samplingRate);

    duration /= longSamplesRate;
    return wxTimeSpan::Milliseconds(duration.GetValue());
}

void wxSamplingInfo::GetNumberOfCdFrames(wxULongLong frames, wxULongLong& cdFrames, wxUint32& rest) const
{
    cdFrames = frames;
    cdFrames *= wxULL(75);
    wxULongLong samplingRate(0, m_samplingRate);
    wxULongLong urest(cdFrames % samplingRate);

    cdFrames /= samplingRate;
    rest = urest.GetLo();
}

wxULongLong wxSamplingInfo::GetNumberOfCdFrames(wxULongLong frames) const
{
    wxULongLong cdFrames;
    wxUint32    rest;

    GetNumberOfCdFrames(frames, cdFrames, rest);
    return cdFrames;
}

wxDouble wxSamplingInfo::ToSeconds(wxULongLong frames) const
{
    const wxULongLong s(frames);
    const wxULongLong samplingRate(0, m_samplingRate);

    return s.ToDouble() / samplingRate.ToDouble();
}

wxULongLong wxSamplingInfo::ToNanoseconds(wxULongLong frames) const
{
    wxDouble res = ToSeconds(frames);
    res *= 1e9;
    return wx_truncate_cast(wxULongLong_t, res);
}

wxString wxSamplingInfo::GetSamplesStr(wxULongLong frames) const
{
    // 1.0 = sampling rate
    wxULongLong s(frames);
    wxULongLong samplingRate(0, m_samplingRate);
    wxULongLong sr(frames % samplingRate);
    double      rest = sr.ToDouble() / m_samplingRate;

    s -= sr;
    s /= samplingRate;

    // seconds
    wxULongLong ss(s % 60);

    s -= ss;
    s /= wxULL(60);
    rest += ss.ToDouble();

    // minutes
    wxULongLong mm(s % 60);

    s -= mm;
    s /= wxULL(60);

    // hours

    return wxIndex::GetTimeStr(s.GetLo(), mm.GetLo(), rest);
}

wxString wxSamplingInfo::GetCdFramesStr(wxULongLong frames) const
{
    wxULongLong cdFrames(GetNumberOfCdFrames(frames));

    wxULongLong nf(cdFrames % wxULL(75));

    cdFrames -= nf;
    cdFrames /= wxULL(75);
    wxULongLong ns = cdFrames % wxULL(60);

    cdFrames -= ns;
    cdFrames /= wxULL(60);

    unsigned long r_minutes = cdFrames.GetLo();
    unsigned long r_seconds = ns.GetLo();
    unsigned long r_frames = nf.GetLo();

    return wxString::Format("%d:%02d:%02d", r_minutes, r_seconds, r_frames);
}

wxULongLong wxSamplingInfo::GetFramesFromCdFrames(wxULongLong cdFrames) const
{
    wxULongLong samplingRate(0, m_samplingRate);
    wxULongLong res(cdFrames);

    res *= samplingRate;
    res /= wxULL(75);
    return res;
}

wxULongLong wxSamplingInfo::GetIndexOffset(const wxIndex& idx) const
{
    if (idx.HasCdFrames()) return GetFramesFromCdFrames(idx.GetOffset());
    else return idx.GetOffset();
}

wxIndex wxSamplingInfo::ConvertIndex(const wxIndex& idx) const
{
    wxIndex res;

    if (idx.HasCdFrames())
    {
        res.SetNumber(idx.GetNumber()).SetOffset(GetFramesFromCdFrames(idx.GetOffset()));
    }
    else
    {
        res = idx;
    }

    return res;
}

wxIndex wxSamplingInfo::ConvertIndex(const wxIndex& idx, wxULongLong offset, bool addOrSubtract) const
{
    wxIndex res;

    if (idx.HasCdFrames())
    {
        res.SetNumber(idx.GetNumber()).SetOffset(GetFramesFromCdFrames(idx.GetOffset())).SetDataFileIdx(idx.GetDataFileIdx());
    }
    else
    {
        res = idx;
    }

    if (addOrSubtract)
    {
        res += offset;
    }
    else
    {
        res -= offset;
    }

    return res;
}

wxString wxSamplingInfo::GetIndexOffsetStr(const wxIndex& idx) const
{
    return GetSamplesStr(GetIndexOffset(idx));
}

wxString wxSamplingInfo::GetIndexOffsetFramesStr(const wxIndex& idx) const
{
    return GetCdFramesStr(GetIndexOffset(idx));
}

