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
    wxSamplingInfo(unsigned long, unsigned short, unsigned short);

    bool IsOK(bool = false) const;
    bool Equals(const wxSamplingInfo&, bool = false) const;

    wxSamplingInfo& operator =(const wxSamplingInfo&);

    unsigned long GetSamplingRate() const;
    unsigned short GetNumberOfChannels() const;
    unsigned short GetBitsPerSample() const;

    wxSamplingInfo& SetSamplingRate(unsigned long);
    wxSamplingInfo& SetNumberOfChannels(unsigned short);
    wxSamplingInfo& SetBitsPerSample(unsigned short);

    wxSamplingInfo& Assign(unsigned long, unsigned short, unsigned short);
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

    unsigned long m_samplingRate;
    unsigned short m_numChannels;
    unsigned short m_bitsPerSample;

    protected:

    void copy(const wxSamplingInfo&);
};

#endif

