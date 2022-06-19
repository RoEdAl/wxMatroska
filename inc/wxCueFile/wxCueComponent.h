/*
 * wxCueComponent.h
 */

#ifndef _WX_CUE_COMPONENT_H
#define _WX_CUE_COMPONENT_H

#ifndef _WX_TAG_SYNONIMS_H_
class wxTagSynonimsCollection;
#endif

#ifndef _WX_CUE_TAG_H_
#include "wxCueTag.h"
#endif

#include <enum2str.h>

class wxCueComponent
{
    public:

    enum ENTRY_FORMAT
    {
        CHARACTER, BINARY
    };

    enum ENTRY_TYPE
    {
        ANY, TRACK, DISC
    };

    WX_DECLARE_STRING_HASH_MAP(wxString, wxHashString);

    public:

    struct CDTEXT_ENTRY
    {
        const char* const keyword;
        ENTRY_TYPE type;
        ENTRY_FORMAT format;
        char replacement;
    };

    static const CDTEXT_ENTRY CdTextFields[];

    typedef VALUE_NAME_PAIR< ENTRY_TYPE > KEYWORD_ENTRY;
    static const KEYWORD_ENTRY Keywords[];

    template<size_t N>
    static bool GetCdTextEntryFormat(const wxString&, const CDTEXT_ENTRY (&)[N], ENTRY_FORMAT&);
    static bool GetCdTextEntryFormat(const wxString&, ENTRY_FORMAT&);

    static void GetSynonims(wxTagSynonimsCollection&, bool);

    public:

    static const char REG_EXP_FMT[];
    static wxString GetCdTextInfoRegExp();
    static wxString GetKeywordsRegExp();

    protected:

    bool m_bTrack;
    wxArrayString m_comments;
    wxArrayString m_garbage;
    wxArrayCueTag m_cdTextTags;
    wxArrayCueTag m_tags;

    protected:

    void copy(const wxCueComponent&);
    static void remove_duplicates(const wxRegEx&, wxHashArrayCueTag&);
    static void remove_duplicates(const wxRegEx&, wxArrayCueTag&);

    public:

    const wxArrayString& GetComments() const;
    const wxArrayString& GetGarbage() const;
    virtual bool HasGarbage() const;
    const wxArrayCueTag& GetCdTextTags() const;
    const wxArrayCueTag& GetTags() const;
    size_t GetTags(const wxString&, wxArrayCueTag&) const;
    size_t MoveTags(const wxString&, wxArrayCueTag&);
    void GetTags(wxCueTag::TagSources, const wxTagSynonimsCollection&, const wxTagSynonimsCollection&, wxArrayCueTag&, wxArrayCueTag&) const;

    bool AddCdTextInfoTag(const wxString&, const wxString&);
    bool AddCdTextInfoTag(const wxCueTag&);
    void AddCdTextInfoTags(const wxArrayCueTag&);

    void RemoveCdTextInfoTag(const wxCueTag&);
    void RemoveCdTextInfoTag(const wxString&);
    void RemoveCdTextInfoTags(const wxArrayCueTag&);

    void AddTag(wxCueTag::TAG_SOURCE, const wxString&, const wxString&);
    bool AddTag(const wxCueTag&);
    bool AddTagIf(const wxCueTag&, const wxCueTag&);
    bool AddTagIfAndRemove(const wxCueTag&, const wxCueTag&);
    void AddTags(const wxArrayCueTag&);

    size_t MoveCdTextInfoTags(const wxTagSynonimsCollection&);

    size_t RemoveTag(const wxCueTag&);
    size_t RemoveTag(const wxString&);
    size_t RemoveTags(const wxArrayCueTag&);

    bool CheckEntryType(ENTRY_TYPE) const;

    void Clear(void);
    bool IsTrack() const;

    void AddComment(const wxString&);
    void AddGarbage(const wxString&);

    virtual void GetReplacements(wxCueTag::TagSources, wxHashString&) const;

    public:

    wxCueComponent(bool = false);
    wxCueComponent(const wxCueComponent&);
    wxCueComponent& operator =(const wxCueComponent&);

    wxCueComponent& operator +=(const wxCueComponent&);
    wxCueComponent& Append(const wxCueComponent&);

    static bool GetCdTextInfoFormat(const wxString&, ENTRY_FORMAT&);
    static bool GetCdTextInfoType(const wxString&, ENTRY_TYPE&);
    static bool GetEntryType(const wxString&, ENTRY_TYPE&);

    static wxString FormatCdTextData(const wxString&, const wxString&);

    private:

    template< size_t SIZE >
    static wxString GetCdTextInfosRegExp(const CDTEXT_ENTRY(&)[SIZE]);

    template< size_t SIZE >
    static bool GetCdTextInfoFormat(const wxString&, ENTRY_FORMAT&, const CDTEXT_ENTRY(&)[SIZE]);

    template< size_t SIZE >
    static bool GetCdTextInfoType(const wxString&, ENTRY_TYPE&, const CDTEXT_ENTRY(&)[SIZE]);

    template< size_t SIZE >
    bool AddCdTextInfoTag(const wxString&, const wxString&, const CDTEXT_ENTRY(&)[SIZE]);

    template< size_t SIZE >
    bool AddCdTextInfoTag(const wxCueTag&, const CDTEXT_ENTRY(&)[SIZE]);

    template< size_t SIZE >
    static void GetSynonims(wxTagSynonimsCollection&, bool, const CDTEXT_ENTRY(&)[SIZE]);

    template< size_t SIZE >
    void GetReplacements(wxHashString&, wxCueTag::TagSources, const CDTEXT_ENTRY(&)[SIZE]) const;
};

#endif

