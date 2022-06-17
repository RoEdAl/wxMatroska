/*
 * wxDataFile.h
 */

#ifndef _WX_DATA_FILE_H_
#define _WX_DATA_FILE_H_

#ifndef _WX_DURATION_H_
#include "wxDuration.h"
#endif

#ifndef _WX_CUE_TAG_H_
#include "wxCueTag.h"
#endif

#include <enum2str.h>

class wxDataFile;
class wxArrayCoverFile;

WX_DECLARE_OBJARRAY(wxFileName, wxArrayFileName);
WX_DECLARE_OBJARRAY(wxDataFile, wxArrayDataFile);

class wxDataFile:
    public wxObject, public wxDurationHolder
{
    wxDECLARE_DYNAMIC_CLASS(wxDataFile);

    public:

    enum FileType
    {
        BINARY, MOTOROLA, AIFF, WAVE, MP3
    };

    enum MediaType
    {
        MEDIA_TYPE_UNKNOWN,
        MEDIA_TYPE_WAV,
        MEDIA_TYPE_AIFF,
        MEDIA_TYPE_FLAC,
        MEDIA_TYPE_WAVPACK
    };

    protected:

    typedef VALUE_NAME_PAIR< FileType > FILE_TYPE_STR;
    static const FILE_TYPE_STR FileTypeString[];

    protected:

    wxFileName m_fileName;
    wxFileName m_realFileName;
    FileType m_ftype;
    MediaType m_mediaType;
    wxArrayCueTag m_tags;
    wxString m_sCueSheet;

    protected:

    void copy(const wxDataFile&);

    static wxULongLong GetNumberOfFramesFromBinary(const wxFileName&,
                                                    const wxSamplingInfo& si);
    static bool GetMediaInfo(const wxFileName&, wxULongLong&,
                              wxSamplingInfo&, MediaType&, wxArrayCueTag&, wxString&);

    public:

    const wxFileName& GetFileName() const;
    bool HasRealFileName() const;
    const wxFileName& GetRealFileName() const;
    FileType GetFileType() const;
    wxString GetFileTypeAsString() const;
    MediaType GetMediaType() const;
    bool HasTags() const;
    const wxArrayCueTag& GetTags() const;
    bool HasCueSheet() const;
    const wxString& GetCueSheet() const;
    bool IsBinary() const;
    bool IsEmpty() const;

    wxDataFile& Assign(const wxString&, FileType = BINARY);
    wxDataFile& Assign(const wxFileName&, FileType = BINARY);

    public:

    wxDataFile(void);
    wxDataFile(const wxDataFile&);
    wxDataFile(const wxString&, FileType = BINARY);
    wxDataFile(const wxFileName&, FileType = BINARY);

    wxDataFile& operator =(const wxDataFile&);
    void Clear();

    bool FindFile(wxFileName&, const wxString & = wxEmptyString) const;
    bool FindFile(const wxString & = wxEmptyString);

    bool GetInfo(const wxString & = wxEmptyString);

    static wxString ToString(FileType);
    static bool FromString(const wxString&, FileType&);
    static wxString GetFileTypeRegExp();
};

#endif  // _WX_DATA_FILE_H_

