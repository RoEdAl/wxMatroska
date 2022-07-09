/*
 * wxCueComponent.cpp
 */

#include <wxCueFile/wxTagSynonims.h>
#include <wxCueFile/wxCueTag.h>
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxUnquoter.h>
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>

 // ===============================================================================

const wxCueComponent::CDTEXT_ENTRY wxCueComponent::CdTextFields[] =
{
    { wxCueTag::Name::ARRANGER, wxCueComponent::ANY, wxCueComponent::CHARACTER, 'a' },
    { wxCueTag::Name::COMPOSER, wxCueComponent::ANY, wxCueComponent::CHARACTER, 'c' },
    { wxCueTag::Name::DISC_ID, wxCueComponent::ANY, wxCueComponent::BINARY, '\000' },
    { wxCueTag::Name::GENRE, wxCueComponent::ANY, wxCueComponent::BINARY, '\000' },
    { wxCueTag::Name::ISRC, wxCueComponent::TRACK, wxCueComponent::BINARY, '\000' },
    { wxCueTag::Name::MESSAGE, wxCueComponent::ANY, wxCueComponent::CHARACTER, '\000' },
    { wxCueTag::Name::PERFORMER, wxCueComponent::ANY, wxCueComponent::CHARACTER, 'p' },
    { wxCueTag::Name::SONGWRITER, wxCueComponent::ANY, wxCueComponent::CHARACTER, 's' },
    { wxCueTag::Name::TITLE, wxCueComponent::ANY, wxCueComponent::CHARACTER, 't' },
    { wxCueTag::Name::UPC_EAN, wxCueComponent::DISC, wxCueComponent::CHARACTER, '\000' },
    { wxCueTag::Name::SIZE_INFO, wxCueComponent::ANY, wxCueComponent::CHARACTER, '\000' },
    { wxCueTag::Name::TOC_INFO, wxCueComponent::ANY, wxCueComponent::BINARY, '\000' },
    { wxCueTag::Name::TOC_INFO2, wxCueComponent::ANY, wxCueComponent::BINARY, '\000' }
};

// ===============================================================================

const wxCueComponent::KEYWORD_ENTRY wxCueComponent::Keywords[] =
{
    { wxCueComponent::ANY, "REM" },
    { wxCueComponent::TRACK, "INDEX" },
    { wxCueComponent::TRACK, "PREGAP" },
    { wxCueComponent::TRACK, "POSTGAP" },
    { wxCueComponent::ANY, "FILE" },
    { wxCueComponent::TRACK, "FLAGS" },
    { wxCueComponent::ANY, "TRACK" },
    { wxCueComponent::DISC, "CATALOG" },
    { wxCueComponent::DISC, "CDTEXTFILE" }
};

// ===============================================================================

const char wxCueComponent::REG_EXP_FMT[] = "^\\p{Xps}*%s\\p{Xps}+(\\P{Xps}.*\\P{Xps})\\p{Xps}*$";

// ===============================================================================

template< size_t SIZE >
wxString wxCueComponent::GetCdTextInfosRegExp(const wxCueComponent::CDTEXT_ENTRY(&cdTextFields)[SIZE])
{
    wxTextOutputStreamOnString tos;

    for (size_t i = 0; i < SIZE; ++i)
    {
        *tos << cdTextFields[i].keyword << '|';
    }

    (*tos).Flush();
    wxString s(tos.GetString());

    wxASSERT(!s.IsEmpty());
    return s.RemoveLast().Prepend('(').Append(')');
}

wxString wxCueComponent::GetCdTextInfoRegExp()
{
    return GetCdTextInfosRegExp(CdTextFields);
}

wxString wxCueComponent::GetKeywordsRegExp()
{
    return get_texts_regexp(Keywords);
}

template< size_t SIZE >
bool wxCueComponent::GetCdTextInfoFormat(const wxString& sKeyword, wxCueComponent::ENTRY_FORMAT& fmt, const wxCueComponent::CDTEXT_ENTRY(&cdTextFields)[SIZE])
{
    for (size_t i = 0; i < SIZE; ++i)
    {
        if (sKeyword.CmpNoCase(cdTextFields[i].keyword) == 0)
        {
            fmt = cdTextFields[i].format;
            return true;
        }
    }

    return false;
}

bool wxCueComponent::GetCdTextInfoFormat(const wxString& sKeyword, wxCueComponent::ENTRY_FORMAT& fmt)
{
    return GetCdTextInfoFormat(sKeyword, fmt, CdTextFields);
}

template< size_t SIZE >
bool wxCueComponent::GetCdTextInfoType(const wxString& sKeyword, wxCueComponent::ENTRY_TYPE& et, const CDTEXT_ENTRY(&cdTextFields)[SIZE])
{
    for (size_t i = 0; i < SIZE; ++i)
    {
        if (sKeyword.CmpNoCase(cdTextFields[i].keyword) == 0)
        {
            et = cdTextFields[i].type;
            return true;
        }
    }

    return false;
}

bool wxCueComponent::GetCdTextInfoType(const wxString& sKeyword, wxCueComponent::ENTRY_TYPE& et)
{
    return GetCdTextInfoType(sKeyword, et, CdTextFields);
}

bool wxCueComponent::GetEntryType(const wxString& sKeyword, wxCueComponent::ENTRY_TYPE& et)
{
    return from_string(sKeyword, et, Keywords);
}

template<size_t N>
bool wxCueComponent::GetCdTextEntryFormat(const wxString& tagName, const CDTEXT_ENTRY(&cdTextEntry)[N], ENTRY_FORMAT& entryFmt)
{
    for (size_t i = 0; i < N; ++i)
    {
        if (tagName.CmpNoCase(cdTextEntry[i].keyword) == 0)
        {
            entryFmt = cdTextEntry[i].format;
            return true;
        }
    }
    return false;
}

bool wxCueComponent::GetCdTextEntryFormat(const wxString& tagName, wxCueComponent::ENTRY_FORMAT& entryFmt)
{
    return GetCdTextEntryFormat(tagName, CdTextFields, entryFmt);
}

wxCueComponent::wxCueComponent(bool bTrack):
    m_isTrack(bTrack)
{
}

wxCueComponent::wxCueComponent(const wxCueComponent& component)
{
    copy(component);
}

wxCueComponent& wxCueComponent::operator =(const wxCueComponent& component)
{
    copy(component);
    return *this;
}

void wxCueComponent::copy(const wxCueComponent& component)
{
    m_isTrack = component.m_isTrack;
    m_comments = component.m_comments;
    m_garbage = component.m_garbage;
    m_cdTextTags = component.m_cdTextTags;
    m_tags = component.m_tags;
}

wxCueComponent& wxCueComponent::Append(const wxCueComponent& component)
{
    wxASSERT(m_isTrack == component.m_isTrack);

    WX_APPEND_ARRAY(m_comments, component.m_comments);
    WX_APPEND_ARRAY(m_garbage, component.m_garbage);
    AddCdTextInfoTags(component.m_cdTextTags);
    AddTags(component.m_tags);
    return *this;
}

bool wxCueComponent::IsTrack() const
{
    return m_isTrack;
}

wxCueComponent& wxCueComponent::operator +=(const wxCueComponent& component)
{
    return Append(component);
}

bool wxCueComponent::HasGarbage() const
{
    return !m_garbage.IsEmpty();
}

const wxArrayString& wxCueComponent::GetComments() const
{
    return m_comments;
}

const wxArrayString& wxCueComponent::GetGarbage() const
{
    return m_garbage;
}

void wxCueComponent::AddComment(const wxString& sComment)
{
    m_comments.Add(sComment);
}

void wxCueComponent::AddGarbage(const wxString& sLine)
{
    m_garbage.Add(sLine);
}

const wxArrayCueTag& wxCueComponent::GetCdTextTags() const
{
    return m_cdTextTags;
}

const wxArrayCueTag& wxCueComponent::GetTags() const
{
    return m_tags;
}

void wxCueComponent::AddTags(const wxArrayCueTag& newTags)
{
    wxCueTag::AddTags(m_tags, newTags);
}

size_t wxCueComponent::RemoveTags(const wxArrayCueTag& newTags)
{
    return wxCueTag::RemoveTags(m_tags, newTags);
}

size_t wxCueComponent::GetTags(const wxString& sTagName, wxArrayCueTag& tags) const
{
    return wxCueTag::GetTags(m_tags, sTagName, tags);
}

size_t wxCueComponent::MoveTags(const wxString& sTagName, wxArrayCueTag& tags)
{
    return wxCueTag::MoveTags(m_tags, sTagName, tags);
}

void wxCueComponent::ReplaceTag(const wxCueTag& tag)
{
    wxArrayCueTag tags;

    if (tag.Split(tags))
    {
        for (size_t i = 0, cnt = tags.GetCount(); i < cnt; ++i)
        {
            wxCueTag::RemoveTag(m_tags, tags[i].GetName());
            wxCueTag::AddTag(m_tags, tags[i]);
        }
    }
    else
    {
        wxCueTag::RemoveTag(m_tags, tag.GetName());
        wxCueTag::AddTag(m_tags, tag);
    }
}

void wxCueComponent::GetTags(
        wxCueTag::TagSources nTagSources,
        const wxTagSynonimsCollection& cdTagsSynonims,
        const wxTagSynonimsCollection& tagsSynonims,
        wxArrayCueTag& tags,
        wxArrayCueTag& rest) const
{
    tags.Clear();
    rest.Clear();

    wxHashArrayCueTag tagsHash;
    wxHashArrayCueTag restHash;

    wxCueTag cueTag;

    for (size_t i = 0, cnt = m_cdTextTags.GetCount(); i < cnt; ++i)
    {
        if (!m_cdTextTags[i].TestSource(nTagSources)) continue;

        if (cdTagsSynonims.GetName(m_cdTextTags[i], cueTag)) tagsHash[cueTag.GetName()].Add(cueTag);
        else restHash[cueTag.GetName()].Add(cueTag);
    }

    for (size_t i = 0, cnt = m_tags.GetCount(); i < cnt; ++i)
    {
        if (!m_tags[i].TestSource(nTagSources)) continue;

        if (tagsSynonims.GetName(m_tags[i], cueTag)) tagsHash[cueTag.GetName()].Add(cueTag);
        else restHash[cueTag.GetName()].Add(cueTag);
    }

    wxRegEx reEmptyValue("^[\\p{Xps}\\p{P}]*$", wxRE_ICASE | wxRE_NOSUB);

    wxASSERT(reEmptyValue.IsValid());

    remove_duplicates(reEmptyValue, tagsHash);
    remove_duplicates(reEmptyValue, restHash);

    wxCueTag::AddTags(tags, tagsHash);
    wxCueTag::AddTags(rest, restHash);
}

void wxCueComponent::remove_duplicates(const wxRegEx& reEmptyValue, wxHashArrayCueTag& tagsHash)
{
    for (wxHashArrayCueTag::iterator i = tagsHash.begin(), end = tagsHash.end(); i != end; ++i)
    {
        remove_duplicates(reEmptyValue, i->second);
    }
}

void wxCueComponent::remove_duplicates(const wxRegEx& reEmptyValue, wxArrayCueTag& tags)
{
    wxArrayString asLines;

    for (size_t i = 0, cnt = tags.GetCount(); i < cnt; ++i)
    {
        wxString sValue(tags[i].GetValue());
        bool     bRemove = false;
        for (size_t j = 0; j < cnt; ++j)
        {
            if (i == j) continue;

            asLines.Clear();
            size_t nReplCounter = 0;
            tags[j].GetValue(asLines);

            for (size_t k = 0, lcnt = asLines.GetCount(); k < lcnt; ++k)
            {
                if (sValue.Replace(asLines[k], wxEmptyString, false) > 0) nReplCounter += 1;
                else break;
            }

            if (nReplCounter == asLines.GetCount())
            {
                if (reEmptyValue.Matches(sValue))
                {
                    bRemove = true;
                    break;
                }
            }
        }

        if (bRemove)
        {
            wxLogInfo(_wxS("Removing tag \u201C%s\u201D - duplicated value \u201C%s\u201D"), tags[i].GetName(), tags[i].GetValue());
            tags.RemoveAt(i);
            i -= 1;
            cnt -= 1;
        }
    }
}

bool wxCueComponent::CheckEntryType(wxCueComponent::ENTRY_TYPE ctype) const
{
    return (ctype == ANY) || (m_isTrack ? (ctype == TRACK) : (ctype == DISC));
}

template< size_t SIZE >
bool wxCueComponent::AddCdTextInfoTag(const wxString& sKeyword, const wxString& sBody, const wxCueComponent::CDTEXT_ENTRY(&cdTextFields)[SIZE])
{
    for (size_t i = 0; i < SIZE; ++i)
    {
        if ((sKeyword.CmpNoCase(cdTextFields[i].keyword) == 0) && CheckEntryType(cdTextFields[i].type))
        {
            return wxCueTag::AddTag(m_cdTextTags, wxCueTag(wxCueTag::TAG_CD_TEXT, sKeyword, sBody));
        }
    }

    return false;
}

bool wxCueComponent::AddCdTextInfoTag(const wxString& sKeyword, const wxString& sBody)
{
    return AddCdTextInfoTag(sKeyword, sBody, CdTextFields);
}

template< size_t SIZE >
bool wxCueComponent::AddCdTextInfoTag(const wxCueTag& tag, const wxCueComponent::CDTEXT_ENTRY(&cdTextFields)[SIZE])
{
    for (size_t i = 0; i < SIZE; ++i)
    {
        if (tag == cdTextFields[i].keyword && CheckEntryType(cdTextFields[i].type))
        {
            return wxCueTag::AddTag(m_cdTextTags, tag);
        }
    }

    return false;
}

bool wxCueComponent::AddCdTextInfoTag(const wxCueTag& tag)
{
    return AddCdTextInfoTag(tag, CdTextFields);
}

void wxCueComponent::RemoveCdTextInfoTag(const wxCueTag& tag)
{
    wxCueTag::RemoveTag(m_cdTextTags, tag);
}

void wxCueComponent::RemoveCdTextInfoTag(const wxString& tagName)
{
    wxCueTag::RemoveTag(m_cdTextTags, tagName);
}

void wxCueComponent::AddCdTextInfoTags(const wxArrayCueTag& cueTags)
{
    for (size_t i = 0, cnt = cueTags.GetCount(); i < cnt; ++i)
    {
        AddCdTextInfoTag(cueTags[i]);
    }
}

void wxCueComponent::RemoveCdTextInfoTags(const wxArrayCueTag& cueTags)
{
    wxCueTag::RemoveTags(m_cdTextTags, cueTags);
}

void wxCueComponent::RemoveDiscInfoTags()
{
    RemoveTag(wxCueTag::Name::DISCNUMBER);
    RemoveTag(wxCueTag::Name::DISCTOTAL);
    RemoveTag(wxCueTag::Name::TOTALDISCS);
}

void wxCueComponent::AddTag(wxCueTag::TAG_SOURCE source, const wxString& keyword, const wxString& body)
{
    const wxCueTag tag(source, keyword, body);
    AddTag(tag);
}

bool wxCueComponent::AddTag(const wxCueTag& tag)
{
    wxArrayCueTag tags;

    if (tag.Split(tags))
    {
        AddTags(tags);
        return true;
    }
    else
    {
        return wxCueTag::AddTag(m_tags, tag);
    }
}

bool wxCueComponent::AddTagIf(const wxCueTag& tagToAdd, const wxCueTag& tagToCheck)
{
    return wxCueTag::AddTagIf(m_tags, tagToAdd, tagToCheck);
}

bool wxCueComponent::AddTagIfAndRemove(const wxCueTag& tagToAdd, const wxCueTag& tagToCheck)
{
    return wxCueTag::AddTagIfAndRemove(m_tags, tagToAdd, tagToCheck);
}

size_t wxCueComponent::RemoveTag(const wxCueTag& tag)
{
    return wxCueTag::RemoveTag(m_tags, tag);
}

size_t wxCueComponent::RemoveTag(const wxString& sTagName)
{
    return wxCueTag::RemoveTag(m_tags, sTagName);
}

template< size_t SIZE >
void wxCueComponent::GetSynonims(wxTagSynonimsCollection& synonimCollections, bool bTrack, const wxCueComponent::CDTEXT_ENTRY(&cdTextFields)[SIZE])
{
    synonimCollections.Clear();
    wxArrayString synonims;

    for (size_t i = 0; i < SIZE; ++i)
    {
        if ((bTrack ? (cdTextFields[i].type == wxCueComponent::TRACK) : (cdTextFields[i].type == wxCueComponent::DISC)) ||
             cdTextFields[i].type == wxCueComponent::ANY)
        {
            if (wxStricmp(cdTextFields[i].keyword, wxCueTag::Name::TITLE) == 0 && !bTrack)
            {
                // DISC: TITLE = ALBUM
                synonims.Clear();
                synonims.Add(wxCueTag::Name::ALBUM);
                synonimCollections.Add(wxTagSynonims(cdTextFields[i].keyword, synonims));
            }
            else if (wxStricmp(cdTextFields[i].keyword, wxCueTag::Name::PERFORMER) == 0 && bTrack)
            {
                // PERFORMER = ARTIST
                synonims.Clear();
                synonims.Add(wxCueTag::Name::ARTIST);
                synonimCollections.Add(wxTagSynonims(cdTextFields[i].keyword, synonims));
            }
            else if (wxStricmp(CdTextFields[i].keyword, wxCueTag::Name::PERFORMER) == 0 && !bTrack)
            {
                // PERFORMER = ARTIST, ALBUM_ARTIST, ALBUMARTIST
                synonims.Clear();
                synonims.Add(wxCueTag::Name::ARTIST);
                synonims.Add(wxCueTag::Name::ALBUMARTIST);
                synonims.Add(wxCueTag::Name::ALBUM_ARTIST);
                synonimCollections.Add(wxTagSynonims(cdTextFields[i].keyword, synonims));
            }
            else
            {
                synonimCollections.Add(wxTagSynonims(cdTextFields[i].keyword));
            }
        }
    }
}

void wxCueComponent::GetSynonims(wxTagSynonimsCollection& synonimCollections, bool bTrack)
{
    GetSynonims(synonimCollections, bTrack, CdTextFields);
}

size_t wxCueComponent::MoveCdTextInfoTags(const wxTagSynonimsCollection& synonimCollections)
{
    wxArrayCueTag cdTextTags;
    size_t        res = wxCueTag::MoveTags(m_tags, synonimCollections, cdTextTags);

    wxCueTag::AddTags(m_cdTextTags, cdTextTags);

    return res;
}

void wxCueComponent::Clear()
{
    m_comments.Clear();
    m_garbage.Clear();
    m_cdTextTags.Clear();
    m_tags.Clear();
}

wxString wxCueComponent::FormatCdTextData(const wxString& keyword, const wxString& value)
{
    ENTRY_FORMAT fmt;

    wxCHECK(GetCdTextInfoFormat(keyword, fmt), wxEmptyString);

    if (fmt == BINARY)
    {
        return value;
    }
    else
    {
        return wxCueTag::Quote(wxCueTag::Escape(value));
    }
}

static size_t find_keyword(const wxArrayCueTag& tags, const wxString& sKeyword)
{
    for (size_t i=0, cnt = tags.GetCount(); i < cnt; ++i)
    {
        if (tags[i] == sKeyword) return i;
    }

    return -1;
}

template< size_t SIZE >
void wxCueComponent::GetReplacements(wxCueComponent::wxHashString& replacements, wxCueTag::TagSources sources, const wxCueComponent::CDTEXT_ENTRY(&cdTextFields)[SIZE]) const
{
    for (size_t i = 0; i < SIZE; ++i)
    {
        if (cdTextFields[i].replacement == '\000') continue;

        wxArrayCueTag tags;
        wxCueTag::GetTags(m_cdTextTags, sources, cdTextFields[i].keyword, tags);

        const wxString value = wxCueTag::GetFlattenValues(tags, wxS(",\u2009"));	// thin space

        wxString s(cdTextFields[i].replacement);
        s.Prepend(m_isTrack ? 't' : 'd');

        bool bAdd = false;
        switch (cdTextFields[i].type)
        {
            case ANY:
            {
                bAdd = true;
                wxString s(cdTextFields[i].replacement);
                s.Prepend('a');
                replacements[s] = value;
                break;
            }

            case TRACK:
            {
                if (m_isTrack) bAdd = true;

                break;
            }

            case DISC:
            {
                if (!m_isTrack) bAdd = true;

                break;
            }
        }

        if (!bAdd) continue;
        replacements[s] = value;
    }
}

void wxCueComponent::GetReplacements(wxCueTag::TagSources sources, wxCueComponent::wxHashString& replacements) const
{
    return GetReplacements(replacements, sources, CdTextFields);
}

