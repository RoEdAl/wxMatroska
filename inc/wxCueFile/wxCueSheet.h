/*
 * wxCueSheet.h
 */

#ifndef _WX_CUE_SHEET_H_
#define _WX_CUE_SHEET_H_

#ifndef _WX_DURATION_H_
#include "wxDuration.h"
#endif

#ifndef _WX_DATA_FILE_H_
#include "wxDataFile.h"
#endif

#ifndef _WX_TRACK_H_
#include "wxTrack.h"
#endif

#ifndef _WX_CUE_COMPONENT_H
#include "wxCueComponent.h"
#endif

#ifndef _WX_CUE_SHEET_CONTENT_H_
#include "wxCueSheetContent.h"
#endif

#ifndef _WX_COVER_FILE_H_
#include "wxCoverFile.h"
#endif

class wxCueSheet:
    public wxCueComponent, public wxAbstractDurationHolder
{
    wxDECLARE_DYNAMIC_CLASS(wxCueSheet);

    public:

    static wxString GetCdAliasesRegExp();

    static const char ALBUM_REG_EX1[];
    static const char ALBUM_REG_EX2[];

    protected:

    wxArrayCueSheetContent m_content;
    wxArrayFileName m_logs;
    wxArrayFileName m_accuripLogs;
    wxArrayCoverFile m_covers;
    wxArrayDataFile m_dataFiles;
    wxArrayCueTag m_catalogs;
    wxArrayFileName m_cdTextFiles;
    wxArrayTrack m_tracks;

    protected:

    void copy(const wxCueSheet&);
    void AddCdTextInfoTagToAllTracks(const wxCueTag&);
    void AddTagToAllTracks(const wxCueTag&);
    void PrepareToAppend();
    static void AppendFileNames(wxArrayFileName&, const wxArrayFileName&);

    public:

    wxCueSheet(void);
    wxCueSheet(const wxCueSheet&);
    wxCueSheet& operator =(const wxCueSheet&);
    wxCueSheet& Append(const wxCueSheet&);

    virtual bool HasGarbage() const;

    bool HasSingleDataFile() const;
    bool HasSingleDataFile(wxDataFile&) const;
    wxCueSheet& SetSingleDataFile(const wxDataFile&);
    wxCueSheet& SetDataFiles(const wxArrayDataFile&);

    size_t GetDataFileIdxIfLastForTrack(size_t) const;

    size_t GetContentsCount() const;
    const wxArrayCueSheetContent& GetContents() const;
    size_t GetLogsCount() const;
    const wxArrayFileName& GetLogs() const;
    size_t GetAccurateRipLogsCount() const;
    const wxArrayFileName& GetAccurateRipLogs() const;
    size_t GetCoversCount() const;
    const wxArrayCoverFile& GetCovers() const;
    size_t GetCatalogsCount() const;
    const wxArrayCueTag& GetCatalogs() const;
    size_t GetCdTextFilesCount() const;
    const wxArrayFileName& GetCdTextFiles() const;
    bool HasTracks() const;
    size_t GetTracksCount() const;
    const wxArrayTrack& GetTracks() const;
    bool HasDataFiles() const;
    size_t GetDataFilesCount() const;
    const wxArrayDataFile& GetDataFiles() const;
    size_t GetLastDataFileIdx() const;

    bool GetRelatedTracks(size_t, size_t&, size_t&) const;

    void SanitizeTags(const wxTagSynonimsCollection&, const wxTagSynonimsCollection&, bool, bool);

    void AddPreparerTag();
    wxCueSheet& AddCatalog(const wxString&);
    wxCueSheet& AddCdTextFile(const wxFileName&);
    wxCueSheet& AddContent(const wxCueSheetContent&);
    wxCueSheet& AddContent(const wxString&);
    wxCueSheet& AddLog(const wxFileName&);
    wxCueSheet& AddAccuripLog(const wxFileName&);
    void AddCover(const wxFileName&);
    void AddCover(const wxCoverFile&);
    void AddCovers(const wxArrayCoverFile&);
    wxCueSheet& AddDataFile(const wxDataFile&);

    virtual bool HasDuration() const;
    virtual wxDuration GetDuration() const;

    wxDuration GetDuration(size_t) const;
    bool CalculateDuration(const wxString & = wxEmptyString);

    bool AddTrack(const wxTrack&);

    wxTrack& GetTrack(size_t);
    wxTrack& GetLastTrack();
    wxTrack& GetBeforeLastTrack();

    bool HasTrack(size_t) const;
    wxTrack& GetTrackByNumber(size_t);
    size_t GetTrackIdxFromNumber(size_t) const;

    const wxTrack& GetTrack(size_t) const;
    const wxTrack& GetLastTrack() const;
    wxArrayTrack& SortTracks();

    void Clear(void);

    wxString Format(wxCueTag::TagSources, const wxString&) const;

    wxString FormatTrack(wxCueTag::TagSources, size_t, const wxString&) const;
};

#endif  // _WX_CUE_SHEET_H_

