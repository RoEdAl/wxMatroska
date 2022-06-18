/*
 * wxXmlCueSheetRenderer.h
 */

#ifndef _WX_XML_CUE_SHEET_RENDERER_H_
#define _WX_XML_CUE_SHEET_RENDERER_H_

#ifndef _WX_PRIMITIVE_RENDERER_H_
#include "wxPrimitiveRenderer.h"
#endif

#ifndef _WX_CUE_SHEET_RENDERER_H_
#include <wxCueFile/wxCueSheetRenderer.h>
#endif

#ifndef _WX_INPUT_FILE_H_
#include "wxInputFile.h"
#endif

#ifndef _WX_CONFIGURATION_H_
class wxConfiguration;
#endif

#ifndef _WX_TAG_SYNONIMS_H_
#include <wxCueFile/wxTagSynonims.h>
#endif

#ifndef _WX_SAMPLING_INFO_H_
#include <wxCueFile/wxSamplingInfo.h>
#endif

#ifndef _WX_STRING_PROCESSOR_H_
class wxStringProcessor;
#endif

#include "wxTemporaryFilesProvider.h"

class wxXmlCueSheetRenderer:
    public wxCueSheetRenderer, protected wxTagRenderer, public wxTemporaryFilesProvider
{
    protected:

    virtual bool OnPreRenderDisc(const wxCueSheet&);
    virtual bool OnPreRenderTrack(const wxCueSheet&, const wxTrack&);
    virtual bool OnRenderTrack(const wxCueSheet&, const wxTrack&);
    virtual bool OnPostRenderTrack(const wxCueSheet&, const wxTrack&);
    virtual bool OnPostRenderDisc(const wxCueSheet&);

    virtual bool OnRenderPreGap(const wxCueSheet&, const wxTrack&, const wxIndex&);
    virtual bool OnRenderPostGap(const wxCueSheet&, const wxTrack&, const wxIndex&);
    virtual bool OnRenderIndex(const wxCueSheet&, const wxTrack&, const wxIndex&);

    protected:

    wxScopedPtr< wxXmlDocument > m_pXmlChapters;
    wxXmlNode* m_pChapterAtom;
    wxXmlNode* m_pFirstChapterAtom;
    wxXmlNode* m_pPrevChapterAtom;
    wxXmlNode* m_pEditionEntry;

    wxScopedPtr< wxXmlDocument > m_pXmlTags;
    wxXmlNode* m_pTags;

    const wxConfiguration& m_cfg;
    const wxInputFile& m_inputFile;

    wxScopedPtr< wxStringProcessor > m_pStringProcessor;
    wxFileName m_chaptersFile;
    wxFileName m_tagsFile;

    wxTagSynonimsCollection m_discCdTextSynonims;
    wxTagSynonimsCollection m_discSynonims;
    wxTagSynonimsCollection m_trackCdTextSynonims;
    wxTagSynonimsCollection m_trackSynonims;

    wxArrayCueTag m_artistTags;
    wxArrayFileName m_temporaryFiles;

    public:

    struct Tag
    {
        static const char CATALOG_NUMBER[];
        static const char TOTAL_PARTS[];
        static const char PART_NUMBER[];
    };

    struct Xml
    {
        static const char BINARY[];
        static const char CHAPTER_UID[];
        static const char EDITION_ENTRY[];
        static const char EDITION_UID[];
        static const char CHAPTER_TIME_START[];
        static const char CHAPTER_TIME_END[];
        static const char CHAPTER_DISPLAY[];
        static const char CHAPTER_STRING[];
        static const char CHAPTER_LANGUAGE[];
        static const char CHAPTER_FLAG_HIDDEN[];
        static const char CHAPTER_ATOM[];
        static const char CHAPTERS[];
        static const char TARGETS[];
        static const char TAGS[];
        static const char TARGET_TYPE_VALUE[];
        static const char TARGET_TYPE[];
        static const char TAG[];
        static const char TAG_LANGUAGE[];
        static const char NAME[];
        static const char STRING[];
        static const char SIMPLE[];
    };

    struct XmlValue
    {
        static const char ALBUM[];
        static const char TRACK[];
    };

    protected:

    void AddTags(const wxCueComponent&,
                  const wxTagSynonimsCollection&,
                  const wxTagSynonimsCollection&,
                  wxXmlNode*);
    void AddCdTextInfo(const wxCueComponent&, wxXmlNode*);
    wxXmlNode* AddDiscTags(const wxCueSheet&, wxXmlNode*, const wxULongLong&, int = 50);
    wxXmlNode* AppendDiscTags(const wxCueSheet&, wxXmlNode*, long = 50);

    wxXmlNode* SetTotalParts(size_t, wxXmlNode*, long = 50);
    wxXmlNode* AddTrackTags(const wxTrack&, const wxULongLong&, wxXmlNode*, int = 30);

    wxXmlNode* AddChapterTimeStart(wxXmlNode*, const wxCueSheet&, const wxIndex&) const;

    wxXmlNode* AddChapterTimeEnd(wxXmlNode*, const wxCueSheet&, const wxTrack&) const;
    wxXmlNode* AddChapterTimeEnd(wxXmlNode*, const wxCueSheet&, const wxIndex&, const wxString&) const;
    wxXmlNode* AddChapterTimeEnd(wxXmlNode*, const wxDuration&, const wxString&) const;

    wxXmlNode* AddIdxChapterAtom(wxXmlNode*, const wxCueSheet&, const wxIndex&) const;

    void init_synonims();
    bool SaveXmlDoc(const wxScopedPtr< wxXmlDocument >&, const wxFileName&) const;

    protected:

    static const wxXmlNode* get_last_child(const wxXmlNode* const);
    static wxXmlNode* add_chapter_uid(wxXmlNode*, const wxULongLong& uid);
    static wxXmlNode* add_chapter_time_start(wxXmlNode*, const wxString&, const wxString&);
    static wxXmlNode* add_chapter_time_start(wxXmlNode*, const wxDuration&, const wxString&);
    static wxXmlNode* find_chapter_time_start(wxXmlNode*);
    static bool is_album_tag(const wxXmlNode* const, long);
    static wxXmlNode* find_disc_tag_node(const wxXmlNode* const, long);

    static bool set_total_parts(wxXmlNode*, size_t);
    static wxXmlNode* add_chapter_time_end(wxXmlNode*, const wxString&, const wxString&);
    static wxXmlNode* add_chapter_time_end(wxXmlNode*, const wxDuration&, const wxString&);
    static bool has_chapter_time_end(const wxXmlNode* const);
    static wxXmlNode* add_chapter_display(wxXmlNode*, const wxString&, const wxString&);
    static wxXmlNode* add_hidden_flag(wxXmlNode*, bool);

    static wxXmlNode* add_idx_chapter_atom(wxXmlNode*, const wxDuration&, size_t, const wxString&, const wxString&, bool);
    wxXmlNode* create_simple_tag(const wxCueTag&) const;
    wxXmlDocument* create_xml_document(const wxString&);
    static bool is_simple(const wxXmlNode* const, const wxCueTag&);
    static wxXmlNode* find_simple_tag(const wxXmlNode* const, const wxCueTag&);
    void add_simple_tag(wxXmlNode*, const wxString&, const wxString&) const;

    void add_simple_tag(wxXmlNode*, const wxString&, size_t) const;
    void add_simple_tag(wxXmlNode*, const wxCueTag&) const;
    void add_simple_tags(wxXmlNode*, const wxArrayCueTag&) const;
    static wxXmlNode* create_comment_node(const wxString&);
    static void add_comment_node(wxXmlNode*, const wxString&);
    static wxULongLong GenerateUID();

    public:

    wxXmlCueSheetRenderer(const wxConfiguration&, const wxInputFile&);
    wxXmlCueSheetRenderer(const wxConfiguration&, const wxInputFile&, const wxString&);
    virtual void GetTemporaryFiles(wxArrayFileName&) const;

    bool HasXmlChapters() const;
    bool HasXmlTags() const;

    wxXmlDocument& GetXmlChapters() const;
    wxXmlDocument& GetXmlTags() const;

    const wxFileName& GetChaptersFile() const;
    const wxFileName& GetTagsFile() const;

    bool Save();
};

#endif

