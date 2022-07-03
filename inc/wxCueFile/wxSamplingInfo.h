/*
 * wxSamplingInfo.h
 */

#ifndef _WX_SAMPLING_INFO_H_
#define _WX_SAMPLING_INFO_H_

#ifndef _WX_INDEX_H_
#include "wxIndex.h"
#endif

class wxSamplingInfo
{
    public:

    static const wxTimeSpan wxInvalidDuration;
    static const wxUint64 wxInvalidNumberOfFrames;

    wxSamplingInfo(void);
    wxSamplingInfo(const wxSamplingInfo&);
    wxSamplingInfo(wxUint32, wxUint16, wxUint16);

    bool IsOK(bool = false) const;
    bool Equals(const wxSamplingInfo&, bool = false) const;
    bool IsDefault() const;
    bool IsStandardAudioFormat() const;

    wxSamplingInfo& operator =(const wxSamplingInfo&);

    wxUint32 GetSamplingRate() const;
    wxUint16 GetNumberOfChannels() const;

    bool HasBitsPerSample() const;
    wxUint16 GetBitsPerSample() const;

    wxSamplingInfo& SetSamplingRate(wxUint32);
    wxSamplingInfo& SetNumberOfChannels(wxUint16);
    wxSamplingInfo& SetBitsPerSample(wxUint16);
    wxSamplingInfo& ClearBitsPerSample();

    wxSamplingInfo& Assign(wxUint32, wxUint16, wxUint16);
    wxSamplingInfo& Invalidate();
    wxSamplingInfo& SetDefault();

    wxULongLong GetNumberOfFramesFromBytes(const wxULongLong&) const;

    wxTimeSpan GetDuration(wxULongLong) const;

    void GetNumberOfCdFrames(wxULongLong, wxULongLong&, wxUint32&) const;
    wxULongLong GetNumberOfCdFrames(wxULongLong) const;

    wxString GetSamplesStr(wxULongLong) const;
    wxString GetCdFramesStr(wxULongLong) const;

    wxDouble ToSeconds(wxULongLong) const;
    wxULongLong ToNanoseconds(wxULongLong) const;

    wxULongLong GetFramesFromCdFrames(wxULongLong) const;
    wxULongLong GetIndexOffset(const wxIndex&) const;
    wxIndex ConvertIndex(const wxIndex&) const;

    wxIndex ConvertIndex(const wxIndex&, wxULongLong, bool) const;
    wxString GetIndexOffsetStr(const wxIndex&) const;
    wxString GetIndexOffsetFramesStr(const wxIndex&) const;

    wxString ToString() const;

    protected:

    wxUint32 m_samplingRate;
    wxUint16 m_numChannels;
    wxUint16 m_bitsPerSample;

    protected:

    void copy(const wxSamplingInfo&);
};

#endif

