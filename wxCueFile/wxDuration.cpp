/*
 * wxDuration.cpp
 */

#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxDuration.h>
 
// ===============================================================================

wxDuration::wxDuration(void)
{
}

wxDuration::wxDuration(const wxDuration& duration)
{
    copy(duration);
}

wxDuration::wxDuration(const wxSamplingInfo& si, wxULongLong numberOfSamples):
    m_si(si), m_numberOfSamples(numberOfSamples)
{
}

wxDuration& wxDuration::operator =(const wxDuration& duration)
{
    copy(duration);
    return *this;
}

void wxDuration::copy(const wxDuration& duration)
{
    m_si = duration.m_si;
    m_numberOfSamples = duration.m_numberOfSamples;
}

wxDuration& wxDuration::Assign(const wxSamplingInfo& si, wxULongLong numberOfSamples)
{
    m_si = si;
    m_numberOfSamples = numberOfSamples;
    return *this;
}

const wxSamplingInfo& wxDuration::GetSamplingInfo() const
{
    return m_si;
}

const wxULongLong& wxDuration::GetNumberOfSamples() const
{
    return m_numberOfSamples;
}

void wxDuration::Clear()
{
    m_si.SetDefault();
    m_numberOfSamples = wxULL(0);
}

void wxDuration::Invalidate()
{
    m_si.SetDefault();
    m_numberOfSamples = wxSamplingInfo::wxInvalidNumberOfFrames;
}

bool wxDuration::IsValid() const
{
    return m_si.IsOK() && (m_numberOfSamples.GetValue() != wxSamplingInfo::wxInvalidNumberOfFrames);
}

bool wxDuration::Add(const wxDuration& duration)
{
    if (m_si.Equals(duration.GetSamplingInfo()))
    {
        m_numberOfSamples += duration.m_numberOfSamples;
        return true;
    }
    else
    {
        return false;
    }
}

wxDuration& wxDuration::Add(wxULongLong numberOfSamples)
{
    m_numberOfSamples += numberOfSamples;
    return *this;
}

wxIndex wxDuration::ConvertIndex(const wxIndex& idx, bool bAdd) const
{
    return m_si.ConvertIndex(idx, m_numberOfSamples, bAdd);
}

wxString wxDuration::GetSamplesStr() const
{
    return m_si.GetSamplesStr(m_numberOfSamples);
}

wxString wxDuration::GetCdFramesStr() const
{
    return m_si.GetCdFramesStr(m_numberOfSamples);
}

wxDurationHolder::wxDurationHolder()
{
}

wxDurationHolder::wxDurationHolder(const wxDurationHolder& durationHolder)
    :m_duration(durationHolder.m_duration)
{}

bool wxDurationHolder::HasDuration() const
{
    return m_duration.has_value();
}

wxDuration wxDurationHolder::GetDuration() const
{
    wxASSERT(HasDuration());
    return *m_duration;
}

void wxDurationHolder::SetDuration(const wxDuration& duration)
{
    m_duration = duration;
}

void wxDurationHolder::ClearDuration()
{
    m_duration.reset();
}

void wxDurationHolder::Copy(const wxAbstractDurationHolder& durationHolder)
{
    if (durationHolder.HasDuration())
    {
        SetDuration(durationHolder.GetDuration());
    }
    else
    {
        ClearDuration();
    }
}


