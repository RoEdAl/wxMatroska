/*
 * wxTrack.h
 */

#ifndef _WX_TRACK_H_
#define _WX_TRACK_H_

#ifndef _WX_DURATION_H_
class wxDuration;
#endif

#ifndef _WX_INDEX_H_
#include "wxIndex.h"
#endif

#ifndef _WX_DATA_FILE_H_
#include "wxDataFile.h"
#endif

#ifndef _WX_CUE_COMPONENT_H
#include "wxCueComponent.h"
#endif

#ifndef _WX_DATA_FILE_H_
#include "wxDataFile.h"
#endif

#ifndef _WX_SAMPLING_INFO_H_
class wxSamplingInfo;
#endif

#include <enum2str.h>

class wxTrack:
    public wxCueComponent
{
    wxDECLARE_DYNAMIC_CLASS(wxTrack);

    public:

    enum Flag
    {
        DCP, CH4, PRE, SCMS, DATA, NONE
    };

    WX_DEFINE_ARRAY_INT(Flag, wxArrayFlag);

    enum DataMode
    {
        AUDIO, CDG, MODE1_2048, MODE1_2352, MODE2_2336, MODE2_2352, CDI_2336,
        CDI_2352
    };

    protected:

    size_t m_number;
    DataMode m_dataMode;
    wxArrayIndex m_indexes;
    wxScopedPtr< wxIndex > m_pPreGap;
    wxScopedPtr< wxIndex > m_pPostGap;
    wxArrayFlag m_flags;

    protected:

    typedef VALUE_NAME_PAIR< Flag > FLAG_STR;
    typedef VALUE_NAME_PAIR< DataMode > DATA_MODE_STR;

    static const FLAG_STR FlagString[];
    static const DATA_MODE_STR DataModeString[];

    protected:

    void copy(const wxTrack&);

    public:

    static int CompareFn(wxTrack**, wxTrack**);

    size_t GetNumber() const;

    wxTrack& SetNumber(size_t);

    DataMode GetMode() const;
    wxString GetModeAsString() const;

    wxTrack& SetMode(DataMode);
    bool SetMode(const wxString&);

    bool HasIndexes() const;
    size_t GetIndexesCount() const;
    const wxArrayIndex& GetIndexes() const;
    const wxIndex& GetZeroIndex() const;
    const wxIndex& GetFirstIndex() const;
    void AddIndex(const wxIndex&);

    bool HasPreGap() const;
    bool HasZeroIndex() const;
    bool HasPostGap() const;

    const wxIndex& GetPreGap() const;
    const wxIndex& GetPostGap() const;

    wxArrayIndex& SortIndicies();

    void ClearPreGap();
    void ClearPostGap();

    void SetPreGap(const wxIndex&);
    void SetPostGap(const wxIndex&);

    const wxArrayFlag& GetFlags() const;
    wxString GetFlagsAsString() const;

    wxTrack& AddFlag(Flag);
    bool AddFlag(const wxString&);
    void ClearFlags();
    bool HasFlags() const;

    bool HasFlag(Flag) const;

    bool IsRelatedToDataFileIdx(size_t, bool = true) const;
    size_t GetMaxDataFileIdx(bool = true) const;
    size_t GetMinDataFileIdx(bool = true) const;

    void SetDataFileIdx(size_t);
    void ShiftDataFileIdx(size_t);

    virtual void GetReplacements(wxCueTag::TagSources, wxHashString&) const;

    public:

    wxTrack(void);
    wxTrack(unsigned long);
    wxTrack(const wxTrack&);
    wxTrack& operator =(const wxTrack&);

    bool IsValid() const;

    static wxString FlagToString(Flag);
    static bool StringToFlag(const wxString&, Flag&);
    static wxString GetFlagRegExp();

    static wxString DataModeToString(DataMode);
    static bool StringToDataMode(const wxString&, DataMode&);
    static wxString GetDataModeRegExp();
};

WX_DECLARE_OBJARRAY(wxTrack, wxArrayTrack);

#endif

