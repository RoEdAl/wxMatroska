/*
 * wxXmlCueSheetRenderer.cpp
 */

#include <wxCueFile/wxTagSynonims.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxDuration.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxCueFile/wxStringProcessor.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include "wxConfiguration.h"
#include "wxInputFile.h"
#include "wxXmlCueSheetRenderer.h"
#include "wxPrimitiveRenderer.h"
#include "wxApp.h"

// ===============================================================================

const char wxXmlCueSheetRenderer::Tag::CATALOG_NUMBER[] = "CATALOG_NUMBER";
const char wxXmlCueSheetRenderer::Tag::TOTAL_PARTS[] = "TOTAL_PARTS";
const char wxXmlCueSheetRenderer::Tag::PART_NUMBER[] = "PART_NUMBER";

// ===============================================================================

const char wxXmlCueSheetRenderer::Xml::BINARY[] = "Binary";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_UID[] = "ChapterUID";
const char wxXmlCueSheetRenderer::Xml::EDITION_ENTRY[] = "EditionEntry";
const char wxXmlCueSheetRenderer::Xml::EDITION_UID[] = "EditionUID";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_TIME_START[] = "ChapterTimeStart";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_TIME_END[] = "ChapterTimeEnd";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_DISPLAY[] = "ChapterDisplay";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_STRING[] = "ChapterString";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_LANGUAGE[] = "ChapterLanguage";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_FLAG_HIDDEN[] = "ChapterFlagHidden";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_ATOM[] = "ChapterAtom";
const char wxXmlCueSheetRenderer::Xml::CHAPTERS[] = "Chapters";
const char wxXmlCueSheetRenderer::Xml::TARGETS[] = "Targets";
const char wxXmlCueSheetRenderer::Xml::TAGS[] = "Tags";
const char wxXmlCueSheetRenderer::Xml::TARGET_TYPE_VALUE[] = "TargetTypeValue";
const char wxXmlCueSheetRenderer::Xml::TARGET_TYPE[] = "TargetType";
const char wxXmlCueSheetRenderer::Xml::TAG[] = "Tag";
const char wxXmlCueSheetRenderer::Xml::TAG_LANGUAGE[] = "TagLanguage";
const char wxXmlCueSheetRenderer::Xml::NAME[] = "Name";
const char wxXmlCueSheetRenderer::Xml::STRING[] = "String";
const char wxXmlCueSheetRenderer::Xml::SIMPLE[] = "Simple";

// ===============================================================================

const char wxXmlCueSheetRenderer::XmlValue::ALBUM[] = "ALBUM";
const char wxXmlCueSheetRenderer::XmlValue::TRACK[] = "TRACK";

// ===============================================================================

wxXmlNode* wxXmlCueSheetRenderer::create_comment_node(const wxString& sComment)
{
    wxXmlNode* const pComment = new wxXmlNode(nullptr, wxXML_COMMENT_NODE, wxEmptyString, sComment);

    return pComment;
}

void wxXmlCueSheetRenderer::add_comment_node(wxXmlNode* pNode, const wxString& sComment)
{
    wxASSERT(pNode != nullptr);
    pNode->AddChild(create_comment_node(sComment));
}

const wxXmlNode* wxXmlCueSheetRenderer::get_last_child(const wxXmlNode* const pNode)
{
    const wxXmlNode* pChild = pNode->GetChildren();
    const wxXmlNode* pPrev = nullptr;

    while (pChild != nullptr)
    {
        pPrev = pChild;
        pChild = pChild->GetNext();
    }

    return pPrev;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_uid(wxXmlNode* pChapterAtom, const wxULongLong& uid)
{
    wxXmlNode* const pChapterUID = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_UID);
    wxXmlNode* const pChapterUIDText = new wxXmlNode(pChapterUID, wxXML_TEXT_NODE, wxEmptyString, uid.ToString());

    pChapterAtom->AddChild(pChapterUID);
    return pChapterUID;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_start(wxXmlNode* pChapterAtom, const wxString& s, const wxString& sComment)
{
    wxXmlNode* const pChapterTimeStart = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_TIME_START);
    wxXmlNode* const pChapterTimeStartText = new wxXmlNode(pChapterTimeStart, wxXML_TEXT_NODE, wxEmptyString, s);
    wxXmlNode* const pCommentNode = create_comment_node(sComment);

    pChapterAtom->AddChild(pCommentNode);
    pChapterAtom->AddChild(pChapterTimeStart);

    return pChapterTimeStart;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_start(wxXmlNode* pChapterAtom, const wxDuration& duration, const wxString& sComment)
{
    return add_chapter_time_start(pChapterAtom, duration.GetSamplesStr(), sComment);
}

wxXmlNode* wxXmlCueSheetRenderer::find_chapter_time_start(wxXmlNode* pChapterAtom)
{
    wxXmlNode* pChild = pChapterAtom->GetChildren();
    wxXmlNode* pPrev = nullptr;

    while (pChild != nullptr)
    {
        if (pChild->GetName().CmpNoCase(Xml::CHAPTER_TIME_START) == 0) return pChild;

        pPrev = pChild;
        pChild = pChild->GetNext();
    }

    wxASSERT(pPrev != nullptr);
    return pPrev;
}

bool wxXmlCueSheetRenderer::is_album_tag(const wxXmlNode* const pTag, long nTargetTypeValue)
{
    const wxXmlNode* pChild = pTag->GetChildren();

    while (pChild != nullptr)
    {
        if (pChild->GetType() == wxXML_ELEMENT_NODE) break;

        pChild = pChild->GetNext();
    }

    if (pChild == nullptr) return false;

    if (pChild->GetName().CmpNoCase(Xml::TARGETS) != 0) return false;

    wxXmlNode* const pTargetTypeValue = pChild->GetChildren();

    if (pTargetTypeValue == nullptr) return false;

    wxXmlNode* const pText = pTargetTypeValue->GetChildren();

    if (pText == nullptr) return false;

    long nParsedTargetTypeValue;

    if (!pText->GetContent().ToCLong(&nParsedTargetTypeValue)) return false;

    return (nParsedTargetTypeValue == nTargetTypeValue);
}

wxXmlNode* wxXmlCueSheetRenderer::find_disc_tag_node(const wxXmlNode* const pTags, long nTargetTypeValue)
{
    wxXmlNode* pChild = pTags->GetChildren();

    while (pChild != nullptr)
    {
        if ((pChild->GetType() == wxXML_ELEMENT_NODE) && (pChild->GetName().CmpNoCase(Xml::TAG) == 0))
            if (is_album_tag(pChild, nTargetTypeValue)) return pChild;

        pChild = pChild->GetNext();
    }

    return nullptr;
}

bool wxXmlCueSheetRenderer::set_total_parts(wxXmlNode* pSimple, size_t nTotalParts)
{
    wxXmlNode* pChild = pSimple->GetChildren();

    while (pChild != nullptr)
    {
        if ((pChild->GetType() == wxXML_ELEMENT_NODE) && (pChild->GetName().CmpNoCase(Xml::STRING) == 0))
        {
            wxXmlNode* pText = pChild->GetChildren();
            wxASSERT(pText != nullptr);
            pText->SetContent(wxString::Format("%" wxSizeTFmtSpec "u", nTotalParts));
            return true;
        }

        pChild = pChild->GetNext();
    }

    return false;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_end(wxXmlNode* pChapterAtom, const wxString& text, const wxString& sComment)
{
    wxXmlNode* const pChapterTimeStart = find_chapter_time_start(pChapterAtom);
    wxXmlNode* const pChapterTimeEnd = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_TIME_END);
    wxXmlNode* const pChapterTimeEndText = new wxXmlNode(pChapterTimeEnd, wxXML_TEXT_NODE, wxEmptyString, text);
    wxXmlNode* const pCommentNode = create_comment_node(sComment);

    pChapterAtom->InsertChildAfter(pCommentNode, pChapterTimeStart);
    pChapterAtom->InsertChildAfter(pChapterTimeEnd, pCommentNode);

    return pChapterTimeEnd;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_end(wxXmlNode* pChapterAtom, const wxDuration& duration, const wxString& sComment)
{
    return add_chapter_time_end(pChapterAtom, duration.GetSamplesStr(), sComment);
}

bool wxXmlCueSheetRenderer::has_chapter_time_end(const wxXmlNode* const pChapterAtom)
{
    const wxXmlNode* pChild = pChapterAtom->GetChildren();

    while (pChild != nullptr)
    {
        if (pChild->GetName().CmpNoCase(Xml::CHAPTER_TIME_END) == 0) return true;

        pChild = pChild->GetNext();
    }

    return false;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_display(wxXmlNode* pChapterAtom, const wxString& sChapterString, const wxString& sLang)
{
    wxXmlNode* const pChapterDisplay = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_DISPLAY);
    wxXmlNode* const pChapterString = new wxXmlNode(pChapterDisplay, wxXML_ELEMENT_NODE, Xml::CHAPTER_STRING);
    wxXmlNode* const pChapterStringText = new wxXmlNode(pChapterString, wxXML_TEXT_NODE, wxEmptyString, sChapterString);
    wxXmlNode* const pChapterLanguage = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_LANGUAGE);
    wxXmlNode* const pChapterLanguageText = new wxXmlNode(pChapterLanguage, wxXML_TEXT_NODE, wxEmptyString, sLang);

    pChapterDisplay->AddChild(pChapterLanguage);
    pChapterAtom->AddChild(pChapterDisplay);
    return pChapterDisplay;
}

wxXmlNode* wxXmlCueSheetRenderer::add_hidden_flag(wxXmlNode* pChapterAtom, bool bHiddenFlag)
{
    wxXmlNode* const pChapterHidden = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_FLAG_HIDDEN);
    wxXmlNode* const pChapterHiddenText = new wxXmlNode(pChapterHidden, wxXML_TEXT_NODE, wxEmptyString, bHiddenFlag ? "1" : "0");

    pChapterAtom->AddChild(pChapterHidden);
    return pChapterHidden;
}

wxXmlNode* wxXmlCueSheetRenderer::add_idx_chapter_atom(
        wxXmlNode* pChapterAtom,
        const wxDuration& duration,
        size_t nIndexNumber,
        const wxString& sComment,
        const wxString& sLang,
        bool bHiddenIndexes
)
{
    wxASSERT(nIndexNumber > 1u);

    wxXmlNode* const pIdxChapterAtom = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_ATOM);
    wxXmlNode* const pChapterTimeStart = add_chapter_time_start(pIdxChapterAtom, duration, wxString::Format("INDEX %" wxSizeTFmtSpec "u %s", nIndexNumber, sComment));

    add_chapter_display(pIdxChapterAtom, wxString::Format("INDEX %02" wxSizeTFmtSpec "d", nIndexNumber - 1u), sLang);
    add_hidden_flag(pIdxChapterAtom, bHiddenIndexes);

    pChapterAtom->AddChild(pIdxChapterAtom);
    return pIdxChapterAtom;
}

// ===============================================================================

wxULongLong wxXmlCueSheetRenderer::GenerateUID()
{
    return wxULongLong((wxUint32)rand(), (wxUint32)rand());
}

wxXmlCueSheetRenderer::wxXmlCueSheetRenderer(const wxConfiguration& cfg, const wxInputFile& inputFile)
    :m_cfg(cfg),
    m_inputFile(inputFile),
    m_pStringProcessor(cfg.CreateStringProcessor()),
    m_chaptersFile(cfg.GetOutputFile(inputFile, wxConfiguration::EXT::MATROSKA_CHAPTERS)),
    m_tagsFile(cfg.GetOutputFile(inputFile, wxConfiguration::EXT::MATROSKA_TAGS)),
    m_pChapterAtom(nullptr), m_pFirstChapterAtom(nullptr),
    m_pPrevChapterAtom(nullptr), m_pEditionEntry(nullptr)
{
    init_synonims();
}

wxXmlCueSheetRenderer::wxXmlCueSheetRenderer(const wxConfiguration& cfg, const wxInputFile& inputFile, const wxString& tmpStem)
    :m_cfg(cfg),
    m_inputFile(inputFile),
    m_pStringProcessor(cfg.CreateStringProcessor()),
    m_chaptersFile(cfg.GetTemporaryFile(inputFile, tmpStem, wxConfiguration::TMP::MKC, wxConfiguration::EXT::XML)),
    m_tagsFile(cfg.GetTemporaryFile(inputFile, tmpStem, wxConfiguration::TMP::MKT, wxConfiguration::EXT::XML)),
    m_pChapterAtom(nullptr), m_pFirstChapterAtom(nullptr),
    m_pPrevChapterAtom(nullptr), m_pEditionEntry(nullptr)
{
    init_synonims();
}

void wxXmlCueSheetRenderer::init_synonims()
{
    wxPrimitiveRenderer::InitTagsSynonimsCollections(
        m_discCdTextSynonims, m_discSynonims,
        m_trackCdTextSynonims, m_trackSynonims
    );
}

bool wxXmlCueSheetRenderer::HasXmlChapters() const
{
    return m_pXmlChapters;
}

wxXmlDocument& wxXmlCueSheetRenderer::GetXmlChapters() const
{
    wxASSERT(HasXmlChapters());
    return *m_pXmlChapters;
}

bool wxXmlCueSheetRenderer::HasXmlTags() const
{
    return m_pXmlTags;
}

wxXmlDocument& wxXmlCueSheetRenderer::GetXmlTags() const
{
    wxASSERT(HasXmlTags());
    return *m_pXmlTags;
}

const wxFileName& wxXmlCueSheetRenderer::GetChaptersFile() const
{
    return m_chaptersFile;
}

const wxFileName& wxXmlCueSheetRenderer::GetTagsFile() const
{
    return m_tagsFile;
}

bool wxXmlCueSheetRenderer::SaveXmlDoc(const wxScopedPtr< wxXmlDocument >& pXmlDoc, const wxFileName& fileName) const
{
    wxSharedPtr< wxMBConv > pConv(m_cfg.GetXmlEncoding());
    wxStringOutputStream    outputStream(NULL, *pConv);

    if (!pXmlDoc->Save(outputStream))
    {
        wxLogError(_("Fail to stringify XML document"));
        return false;
    }

    wxFileOutputStream fos(fileName.GetFullPath());

    if (fos.IsOk())
    {
        wxSharedPtr< wxTextOutputStream > pStream(m_cfg.GetOutputTextStream(fos));
        pStream->WriteString(outputStream.GetString());
        return true;
    }
    else
    {
        wxLogDebug(wxS("Fail to save XML document to file " ENQUOTED_STR_FMT), fileName.GetFullName());
        return false;
    }
}

bool wxXmlCueSheetRenderer::Save()
{
    wxASSERT(HasXmlChapters());
    wxASSERT(HasXmlTags());

    if (!SaveXmlDoc(m_pXmlChapters, m_chaptersFile))
    {
        wxLogError(_wxS("Fail to save chapters to " ENQUOTED_STR_FMT), m_chaptersFile.GetFullName());
        return false;
    }
    m_temporaryFiles.Add(m_chaptersFile);

    if (m_cfg.GenerateTags())
    {
        wxASSERT(m_tagsFile.IsOk());

        if (!SaveXmlDoc(m_pXmlTags, m_tagsFile))
        {
            wxLogError(_wxS("Fail to save tags to " ENQUOTED_STR_FMT), m_tagsFile.GetFullName());
            return false;
        }
    }
    m_temporaryFiles.Add(m_tagsFile);
    return true;
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeStart(wxXmlNode* pChapterAtom, const wxCueSheet& cueSheet, const wxIndex& idx) const
{
    wxASSERT(idx.HasDataFileIdx());

    wxDuration duration(cueSheet.GetDuration(idx.GetDataFileIdx()));

    if (duration.IsValid())
    {
        wxULongLong indexOffset(duration.GetSamplingInfo().GetIndexOffset(idx));
        duration.Add(indexOffset);
        return add_chapter_time_start(pChapterAtom, duration, duration.GetCdFramesStr());
    }
    else
    {
        const wxDataFile& dataFile = cueSheet.GetDataFiles().Item(idx.GetDataFileIdx());
        wxLogError(_wxS("Cannot calulate duration of data file " ENQUOTED_STR_FMT " (index %d)"), dataFile.GetRealFileName().GetFullName(), idx.GetDataFileIdx());
        return nullptr;
    }
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeEnd(wxXmlNode* pChapterAtom, const wxDuration& duration, const wxString& sComment) const
{
    return add_chapter_time_end(pChapterAtom, duration, sComment);
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeEnd(wxXmlNode* pChapterAtom, const wxCueSheet& cueSheet, const wxIndex& idx, const wxString& sDescription) const
{
    wxASSERT(idx.HasDataFileIdx());

    wxDuration duration(cueSheet.GetDuration(idx.GetDataFileIdx()));

    if (duration.IsValid())
    {
        const wxULongLong indexOffset(duration.GetSamplingInfo().GetIndexOffset(idx));
        wxString          sComment;
        duration.Add(indexOffset);
        sComment.Printf("%s : %s", duration.GetCdFramesStr(), sDescription);
        return AddChapterTimeEnd(pChapterAtom, duration, sComment);
    }
    else
    {
        const wxDataFile& dataFile = cueSheet.GetDataFiles().Item(idx.GetDataFileIdx());
        wxLogError(_wxS("Cannot calulate duration of data file " ENQUOTED_STR_FMT " (index %" wxSizeTFmtSpec "u)"), dataFile.GetRealFileName().GetFullName(), idx.GetDataFileIdx());
        return nullptr;
    }
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeEnd(wxXmlNode* pChapterAtom, const wxCueSheet& cueSheet, const wxTrack& track) const
{
    const wxIndex& idx = track.GetFirstIndex();

    wxASSERT(idx.HasDataFileIdx());

    const wxSamplingInfo& si = cueSheet.GetDataFiles().Item(idx.GetDataFileIdx()).GetDuration().GetSamplingInfo();
    const wxIndex         fidx(si.ConvertIndex(idx, si.GetFramesFromCdFrames(m_cfg.GetChapterOffset()), false));

    wxASSERT(fidx.HasDataFileIdx());

    return AddChapterTimeEnd(pChapterAtom, cueSheet, fidx, wxString::Format("from next track %" wxSizeTFmtSpec "u", track.GetNumber()));
}

wxXmlNode* wxXmlCueSheetRenderer::AddIdxChapterAtom(wxXmlNode* pChapterAtom, const wxCueSheet& cueSheet, const wxIndex& idx) const
{
    wxDuration duration(cueSheet.GetDuration(idx.GetDataFileIdx()));

    if (duration.IsValid())
    {
        const wxULongLong indexOffset(duration.GetSamplingInfo().GetIndexOffset(idx));
        duration.Add(indexOffset);
        return add_idx_chapter_atom(pChapterAtom, duration, idx.GetNumber(), duration.GetCdFramesStr(), m_cfg.GetLang(), m_cfg.HiddenIndexes());
    }
    else
    {
        const wxDataFile& dataFile = cueSheet.GetDataFiles().Item(idx.GetDataFileIdx());
        wxLogError(_wxS("Cannot calulate duration of data file " ENQUOTED_STR_FMT " (index %" wxSizeTFmtSpec "u)"), dataFile.GetRealFileName().GetFullName(), idx.GetDataFileIdx());
        return nullptr;
    }
}

wxXmlDocument* wxXmlCueSheetRenderer::create_xml_document(const wxString& sRootNode)
{
    wxXmlDocument* const pXmlDoc = new wxXmlDocument();

    pXmlDoc->SetVersion("1.0");
    pXmlDoc->SetFileEncoding(m_cfg.GetXmlFileEncoding());
    wxXmlNode* const pRoot = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, sRootNode);

    pXmlDoc->SetRoot(pRoot);
    return pXmlDoc;
}

wxXmlNode* wxXmlCueSheetRenderer::create_simple_tag(const wxCueTag& tag) const
{
    if (!m_cfg.RenderReplayGainTags())
    {
        if (tag.IsReplayGain())
        {
            wxLogInfo("XML Renderer: skipping RG tag %s", tag.GetName());
            return nullptr;
        }
    }

    if (!m_cfg.RenderMultilineTags())
    {
        if (tag.IsMultiline())
        {
            wxLogInfo("XML renderer: skipping multiline tag %s", tag.GetName());
            return nullptr;
        }
    }

    wxXmlNode* const pSimple = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::SIMPLE);
    wxXmlNode* const pName = new wxXmlNode(pSimple, wxXML_ELEMENT_NODE, Xml::NAME);
    wxXmlNode* const pNameText = new wxXmlNode(pName, wxXML_TEXT_NODE, wxEmptyString, tag.GetName());

    add_comment_node(pSimple, wxString::Format("Source: %s", tag.GetSourceAsString()));

    wxXmlNode* pValue = nullptr;

    if (tag.IsMultiline())
    {
        wxString      sSeparator('=', 65);
        wxArrayString as;
        tag.GetValue(as);

        add_comment_node(pSimple, sSeparator);
        for (size_t i = 0, n = as.GetCount(); i < n; ++i)
        {
            add_comment_node(pSimple, as[i]);
        }

        add_comment_node(pSimple, sSeparator);

        wxString s;
        tag.GetValueBase64(76, as);
        wxASSERT(!as.IsEmpty());

        size_t nCount = as.GetCount();

        if (nCount > 1)
        {
            for (size_t i = 0, n = nCount - 1; i < n; ++i)
            {
                s << as[i] << "\r\n";
            }
        }
        s << as[nCount - 1];

        pValue = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::BINARY);
        wxXmlNode* pValueText = new wxXmlNode(pValue, wxXML_TEXT_NODE, wxEmptyString, s);
    }
    else
    {
        pValue = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::STRING);
        wxXmlNode* pValueText = new wxXmlNode(pValue, wxXML_TEXT_NODE, wxEmptyString, tag.GetValue());
    }

    wxASSERT(pValue != nullptr);
    pSimple->AddChild(pValue);

    wxXmlNode* const pLang = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::TAG_LANGUAGE);
    const bool       languageAgnostic = IsLanguageAgnostic(m_cfg, tag);
    wxXmlNode* const pLangText = new wxXmlNode(pLang, wxXML_TEXT_NODE, wxEmptyString, languageAgnostic ? wxConfiguration::LANG::UND : m_cfg.GetLang());

    pSimple->AddChild(pLang);

    return pSimple;
}

bool wxXmlCueSheetRenderer::is_simple(const wxXmlNode* const pNode, const wxCueTag& tag)
{
    wxASSERT(pNode != nullptr);

    wxXmlNode* pChild = pNode->GetChildren();
    bool       bName = false;
    bool       bValue = false;

    while (pChild != nullptr)
    {
        if (pChild->GetType() == wxXML_ELEMENT_NODE)
        {
            if (pChild->GetName().CmpNoCase(Xml::NAME) == 0)
            {
                wxXmlNode* pText = pChild->GetChildren();

                if (pText == nullptr) return false;

                if (pText->GetType() != wxXML_TEXT_NODE) return false;

                bName = (pText->GetContent().CmpNoCase(tag.GetName()) == 0);
            }

            if (pChild->GetName().CmpNoCase(Xml::STRING) == 0)
            {
                wxXmlNode* pText = pChild->GetChildren();

                if (pText == nullptr) return false;

                if (!((pText->GetType() == wxXML_TEXT_NODE) ||
                    (pText->GetType() == wxXML_CDATA_SECTION_NODE))) return false;

                bValue = (pText->GetContent().Cmp(tag.GetValue()) == 0);
            }
        }

        pChild = pChild->GetNext();
    }

    return (bName && bValue);
}

wxXmlNode* wxXmlCueSheetRenderer::find_simple_tag(const wxXmlNode* const pNode, const wxCueTag& tag)
{
    wxASSERT(pNode != nullptr);

    wxXmlNode* pChild = pNode->GetChildren();

    while (pChild != nullptr)
    {
        if (is_simple(pChild, tag)) return pChild;

        pChild = pChild->GetNext();
    }

    return nullptr;
}

void wxXmlCueSheetRenderer::add_simple_tag(wxXmlNode* pNode,
        const wxString& sName, const wxString& sValue) const
{
    wxASSERT(pNode != nullptr);

    const wxCueTag   tag(wxCueTag::TAG_AUTO_GENERATED, sName, sValue);
    wxXmlNode* const pTagNode = create_simple_tag(tag);

    if (pTagNode != nullptr) pNode->AddChild(pTagNode);
}

void wxXmlCueSheetRenderer::add_simple_tag(wxXmlNode* pNode,
        const wxString& sName, size_t nValue) const
{
    add_simple_tag(pNode, sName, wxString::Format("%" wxSizeTFmtSpec "u", nValue));
}

void wxXmlCueSheetRenderer::add_simple_tag(wxXmlNode* pNode, const wxCueTag& tag) const
{
    wxASSERT(pNode != nullptr);

    wxXmlNode* const pTagNode = create_simple_tag(tag);

    if (pTagNode != nullptr) pNode->AddChild(pTagNode);
}

void wxXmlCueSheetRenderer::add_simple_tags(wxXmlNode* pNode, const wxArrayCueTag& tags) const
{
    wxASSERT(pNode != nullptr);

    for (size_t i = 0, nCount = tags.GetCount(); i < nCount; ++i)
    {
        add_simple_tag(pNode, tags[i]);
    }
}

void wxXmlCueSheetRenderer::AddTags(
        const wxCueComponent& component,
        const wxTagSynonimsCollection& cdTextSynonims,
        const wxTagSynonimsCollection& synonims,
        wxXmlNode* pTag)
{
    wxASSERT(pTag != nullptr);

    wxArrayCueTag mappedTags;
    wxArrayCueTag rest;

    component.GetTags(m_cfg.GetTagSources(), cdTextSynonims, synonims, mappedTags, rest);

    if (m_cfg.RenderArtistForTrack())
    {
        // wxCueTag artistTag( wxCueTag::TAG_UNKNOWN, wxCueTag::Name::ARTIST, wxEmptyString );

        if (component.IsTrack())
        {
            wxArrayCueTag artists;
            const size_t  res = wxCueTag::GetTags(mappedTags, wxCueTag::Name::ARTIST, artists);

            if (res == 0 && !m_artistTags.IsEmpty()) wxCueTag::AddTags(mappedTags, m_artistTags);
        }
        else
        {
            wxCueTag::GetTags(mappedTags, wxCueTag::Name::ARTIST, m_artistTags);
        }
    }

    add_simple_tags(pTag, mappedTags);
    add_simple_tags(pTag, rest);
}

void wxXmlCueSheetRenderer::AddCdTextInfo(const wxCueComponent& component, wxXmlNode* pTag)
{
    wxASSERT(pTag != nullptr);

    if (component.IsTrack()) AddTags(component, m_trackCdTextSynonims, m_trackSynonims, pTag);

    // wxCueTag::FindTag(  )
    else AddTags(component, m_discCdTextSynonims, m_discSynonims, pTag);
}

wxXmlNode* wxXmlCueSheetRenderer::AddDiscTags(
        const wxCueSheet& cueSheet,
        wxXmlNode* pTags,
        const wxULongLong& editionUID,
        int nTargetTypeValue)
{
    wxASSERT(pTags != nullptr);

    wxXmlNode* const pTag = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::TAG);

    pTags->AddChild(pTag);

    add_comment_node(pTag, "Disc tag");

    wxXmlNode* const pTargets = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::TARGETS);

    pTag->AddChild(pTargets);

    wxXmlNode* const pTargetTypeValue = new wxXmlNode(pTargets, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE_VALUE);
    wxXmlNode* const pTargetTypeValueText = new wxXmlNode(pTargetTypeValue, wxXML_TEXT_NODE, wxEmptyString, wxString::Format("%d", nTargetTypeValue));

    wxXmlNode* const pTargetType = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE);
    wxXmlNode* const pTargetTypeText = new wxXmlNode(pTargetType, wxXML_TEXT_NODE, wxEmptyString, XmlValue::ALBUM);

    pTargets->AddChild(pTargetType);

    if (m_cfg.GenerateEditionUID())
    {
        wxXmlNode* const pEditionUID = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::EDITION_UID);
        wxXmlNode* const pEditionUIDText = new wxXmlNode(pEditionUID, wxXML_TEXT_NODE, wxEmptyString, editionUID.ToString());
        pTargets->AddChild(pEditionUID);
    }

    AppendDiscTags(cueSheet, pTags, nTargetTypeValue);
    return pTag;
}

wxXmlNode* wxXmlCueSheetRenderer::AppendDiscTags(
        const wxCueSheet& cueSheet,
        wxXmlNode* pTags,
        long nTargetTypeValue)
{
    wxASSERT(pTags != nullptr);

    wxXmlNode* const pTag = find_disc_tag_node(pTags, nTargetTypeValue);

    wxASSERT(pTag != nullptr);

    add_comment_node(pTag, wxString::Format(wxS("CUE file: " ENQUOTED_STR_FMT), m_inputFile.ToString(false)));

    AddCdTextInfo(cueSheet, pTag);

    const wxArrayCueTag& catalogs = cueSheet.GetCatalogs();

    for (size_t i = 0, nCount = catalogs.GetCount(); i < nCount; ++i)
    {
        const wxCueTag catalogNumberTag(catalogs[i].GetSource(), Tag::CATALOG_NUMBER, catalogs[i].GetValue());
        add_simple_tag(pTag, catalogNumberTag);
    }

    return pTag;
}

wxXmlNode* wxXmlCueSheetRenderer::SetTotalParts(
        size_t nTotalParts,
        wxXmlNode* pTags,
        long nTargetTypeValue)
{
    wxASSERT(pTags != nullptr);

    wxXmlNode* const pTag = find_disc_tag_node(pTags, nTargetTypeValue);

    wxASSERT(pTag != nullptr);

    add_comment_node(pTag, "Total number of tracks");
    add_simple_tag(pTag, Tag::TOTAL_PARTS, nTotalParts);

    return pTag;
}

wxXmlNode* wxXmlCueSheetRenderer::AddTrackTags(
        const wxTrack& track,
        const wxULongLong& chapterUID,
        wxXmlNode* pTags,
        int nTargetTypeValue)
{
    wxASSERT(pTags != nullptr);

    wxXmlNode* const pTag = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::TAG);

    pTags->AddChild(pTag);

    add_comment_node(pTag, wxString::Format("Track %02" wxSizeTFmtSpec "d", track.GetNumber()));

    wxXmlNode* const pTargets = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::TARGETS);

    pTag->AddChild(pTargets);

    wxXmlNode* const pTargetTypeValue = new wxXmlNode(pTargets, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE_VALUE);
    wxXmlNode* const pTargetTypeValueText = new wxXmlNode(pTargetTypeValue, wxXML_TEXT_NODE, wxEmptyString, wxString::Format("%d", nTargetTypeValue));

    wxXmlNode* const pTargetType = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE);
    wxXmlNode* const pTargetTypeText = new wxXmlNode(pTargetType, wxXML_TEXT_NODE, wxEmptyString, XmlValue::TRACK);

    pTargets->AddChild(pTargetType);

    wxXmlNode* const pChapterUID = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_UID);
    wxXmlNode* const pChapterUIDText = new wxXmlNode(pChapterUID, wxXML_TEXT_NODE, wxEmptyString, chapterUID.ToString());

    pTargets->AddChild(pChapterUID);

    add_simple_tag(pTag, Tag::PART_NUMBER, track.GetNumber());

    AddCdTextInfo(track, pTag);

    return pTag;
}

bool wxXmlCueSheetRenderer::OnPreRenderDisc(const wxCueSheet& cueSheet)
{
    wxASSERT(!HasXmlChapters());
    wxASSERT(!HasXmlTags());

    m_pFirstChapterAtom = nullptr;
    const wxULongLong editionUID(GenerateUID());

    wxLogDebug(_("Creating XML document"));
    m_pXmlChapters.reset(create_xml_document(Xml::CHAPTERS));
    wxXmlNode* const pChapters = m_pXmlChapters->GetRoot();

    m_cfg.BuildXmlComments(m_chaptersFile, pChapters);
    m_pEditionEntry = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::EDITION_ENTRY);
    pChapters->AddChild(m_pEditionEntry);

    if (m_cfg.GenerateEditionUID())
    {
        wxXmlNode* const pEditionUID = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::EDITION_UID);
        wxXmlNode* const pEditionUIDText = new wxXmlNode(pEditionUID, wxXML_TEXT_NODE, wxEmptyString, editionUID.ToString());
        m_pEditionEntry->AddChild(pEditionUID);
    }

    m_pChapterAtom = nullptr;
    m_pPrevChapterAtom = nullptr;

    wxASSERT(m_pEditionEntry != nullptr);
    add_comment_node(m_pEditionEntry, wxString::Format(wxS("CUE file: " ENQUOTED_STR_FMT), m_inputFile.ToString(false)));

    m_pXmlTags.reset(create_xml_document(Xml::TAGS));
    m_pTags = m_pXmlTags->GetRoot();
    m_cfg.BuildXmlComments(m_tagsFile, m_pTags);
    m_artistTags.Empty();
    AddDiscTags(cueSheet, m_pTags, editionUID);

    add_comment_node(m_pTags, wxString::Format(wxS("CUE file: " ENQUOTED_STR_FMT), m_inputFile.ToString(false)));
    add_comment_node(m_pTags, wxString::Format("Number of tracks: %" wxSizeTFmtSpec "u", cueSheet.GetTracksCount()));

    return wxCueSheetRenderer::OnPreRenderDisc(cueSheet);
}

bool wxXmlCueSheetRenderer::OnPreRenderTrack(const wxCueSheet& cueSheet, const wxTrack& track)
{
    wxASSERT(m_pEditionEntry != nullptr);
    wxLogDebug(_("Converting track %" wxSizeTFmtSpec "u"), track.GetNumber());

    m_pChapterAtom = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_ATOM);

    if (m_pFirstChapterAtom == nullptr) m_pFirstChapterAtom = m_pChapterAtom;

    add_comment_node(m_pChapterAtom, wxString::Format("Track %02" wxSizeTFmtSpec "u", track.GetNumber()));
    return wxCueSheetRenderer::OnPreRenderTrack(cueSheet, track);
}

bool wxXmlCueSheetRenderer::OnRenderTrack(const wxCueSheet& cueSheet, const wxTrack& track)
{
    const wxULongLong chapterUID(GenerateUID());

    add_chapter_uid(m_pChapterAtom, chapterUID);
    AddTrackTags(track, chapterUID, m_pTags);
    return wxCueSheetRenderer::OnRenderTrack(cueSheet, track);
}

bool wxXmlCueSheetRenderer::OnPostRenderTrack(const wxCueSheet& cueSheet, const wxTrack& track)
{
    wxASSERT(m_pEditionEntry != nullptr);

    const size_t nTrackIdx = cueSheet.GetTrackIdxFromNumber(track.GetNumber());

    wxString trackNameFmt = m_cfg.GetTrackNameFormat();
    (*m_pStringProcessor)(trackNameFmt);

    add_chapter_display(m_pChapterAtom, cueSheet.FormatTrack(m_cfg.GetTagSources(), nTrackIdx, trackNameFmt), m_cfg.GetLang());
    m_pEditionEntry->AddChild(m_pChapterAtom);
    m_pPrevChapterAtom = m_pChapterAtom;
    return wxCueSheetRenderer::OnPostRenderTrack(cueSheet, track);
}

bool wxXmlCueSheetRenderer::OnRenderPreGap(const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& preGap)
{
    wxLogDebug(_("Converting pre-gap of track %" wxSizeTFmtSpec "u"), track.GetNumber());

    if (track.GetNumber() == 1u)
    {
        if (!m_cfg.TrackOneIndexOne())
            if (AddChapterTimeStart(m_pChapterAtom, cueSheet, preGap) == nullptr) return false;
    }
    else if (m_cfg.GetChapterTimeEnd())
    {
        if (m_pPrevChapterAtom != nullptr)
            if (AddChapterTimeEnd(m_pPrevChapterAtom, cueSheet, preGap, wxString::Format("from pre-gap of track %" wxSizeTFmtSpec "u", track.GetNumber())) == nullptr) return false;
    }

    return wxCueSheetRenderer::OnRenderPreGap(cueSheet, track, preGap);
}

bool wxXmlCueSheetRenderer::OnRenderPostGap(const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& postGap)
{
    wxLogDebug(_("Converting post-gap of track %" wxSizeTFmtSpec "u"), track.GetNumber());

    if (m_cfg.GetChapterTimeEnd())
    {
        if (AddChapterTimeEnd(m_pChapterAtom, cueSheet, postGap, wxString::Format("from post-gap of track %" wxSizeTFmtSpec "d", track.GetNumber())) == nullptr) return false;
    }

    return wxCueSheetRenderer::OnRenderPostGap(cueSheet, track, postGap);
}

bool wxXmlCueSheetRenderer::OnRenderIndex(const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& idx)
{
    wxLogDebug(_("Converting index %" wxSizeTFmtSpec "d of track %" wxSizeTFmtSpec "d"), idx.GetNumber(), track.GetNumber());

    switch (idx.GetNumber())
    {
        case 0:	// pre-gap
        {
            if (track.GetNumber() == 1u)
            {
                if (!m_cfg.TrackOneIndexOne())
                    if (AddChapterTimeStart(m_pChapterAtom, cueSheet, idx) == nullptr) return false;
            }
            else if (m_cfg.GetChapterTimeEnd())
            {
                if (m_pPrevChapterAtom != nullptr)
                    if (AddChapterTimeEnd(m_pPrevChapterAtom, cueSheet, idx, wxString::Format("from index 0 of track %" wxSizeTFmtSpec "d", track.GetNumber())) == nullptr) return false;
            }

            break;
        }

        case 1:	// start
        {
            if (track.GetNumber() == 1u)
            {
                if (m_cfg.TrackOneIndexOne())
                    if (AddChapterTimeStart(m_pChapterAtom, cueSheet, idx) == nullptr) return false;
            }
            else if (AddChapterTimeStart(m_pChapterAtom, cueSheet, idx) == nullptr)
            {
                return false;
            }

            break;
        }

        default:
        {
            if (AddIdxChapterAtom(m_pChapterAtom, cueSheet, idx) == nullptr) return false;

            break;
        }
    }

    return wxCueSheetRenderer::OnRenderIndex(cueSheet, track, idx);
}

bool wxXmlCueSheetRenderer::OnPostRenderDisc(const wxCueSheet& cueSheet)
{
    wxLogDebug(_("Calculating chapter names and end time from data file(s)"));

    const size_t nTracksCount = cueSheet.GetTracksCount();

    SetTotalParts(nTracksCount, m_pTags);

    if (m_cfg.GetChapterTimeEnd())
    {
        wxASSERT(m_pFirstChapterAtom != nullptr);
        wxXmlNode* pChapterAtom = m_pFirstChapterAtom;

        for (size_t i = 0; i < nTracksCount; ++i)
        {
            if (!has_chapter_time_end(pChapterAtom))
            {
                if (m_cfg.UseDataFiles())
                {
                    const size_t nDataFileIdx = cueSheet.GetDataFileIdxIfLastForTrack(i);

                    if (nDataFileIdx != wxIndex::UnknownDataFileIdx)
                    {
                        const wxDataFile& dataFile = cueSheet.GetDataFiles().Item(nDataFileIdx);
                        const wxDuration  duration(cueSheet.GetDuration(nDataFileIdx + 1u));

                        if (!duration.IsValid())
                        {
                            wxString s;
                            s << _wxS("Cannot calulate duration for data file \u201C") <<
                                dataFile.GetRealFileName().GetFullName() <<
                                _wxS("\u201D (index ") << nDataFileIdx << ')';
                            wxLogError(s);
                            return false;
                        }

                        wxString sComment;
                        sComment << duration.GetCdFramesStr() << " : from media file(s) (" <<
                            (nDataFileIdx + 1u) << wxS(" \u201C") << dataFile.GetRealFileName().GetFullName() << wxS("\u201D)");

                        if (AddChapterTimeEnd(pChapterAtom, duration, sComment) == nullptr) return false;
                    }
                }
                else if (m_cfg.GetChapterEndTimeFromNextChapter())
                {
                    if ((i + 1u) < cueSheet.GetTracksCount())
                    {
                        const wxTrack& nextTrack = cueSheet.GetTrack(i + 1u);

                        if (AddChapterTimeEnd(pChapterAtom, cueSheet, nextTrack) == nullptr) return false;
                    }
                }
            }

            pChapterAtom = pChapterAtom->GetNext();
        }
    }

    wxLogDebug(_("XML document created"));
    return wxCueSheetRenderer::OnPostRenderDisc(cueSheet);
}

void wxXmlCueSheetRenderer::GetTemporaryFiles(wxArrayFileName& tmpFiles) const
{
    WX_APPEND_ARRAY(tmpFiles, m_temporaryFiles);
}
