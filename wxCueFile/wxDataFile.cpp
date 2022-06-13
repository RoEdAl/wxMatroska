/*
 * wxDataFile.cpp
 */

#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxDuration.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxCoverFile.h>

 // ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxDataFile, wxObject);

// ===============================================================================

const wxDataFile::FILE_TYPE_STR wxDataFile::FileTypeString[] =
{
    { BINARY, "BINARY" },
    { MOTOROLA, "MOTOROLA" },
    { AIFF, "AIFF" },
    { WAVE, "WAVE" },
    { MP3, "MP3" }
};

// ===============================================================================

wxDataFile::wxDataFile(void):
    m_ftype(wxDataFile::BINARY), m_mediaType(wxDataFile::MEDIA_TYPE_UNKNOWN)
{
}

wxDataFile::wxDataFile(const wxDataFile& df)
{
    copy(df);
}

wxDataFile::wxDataFile(const wxString& sFilePath, wxDataFile::FileType ftype):
    m_fileName(sFilePath),
    m_ftype(ftype)
{
}

wxDataFile::wxDataFile(const wxFileName& fileName, wxDataFile::FileType ftype):
    m_fileName(fileName),
    m_ftype(ftype)
{
}

wxDataFile& wxDataFile::operator =(const wxDataFile& df)
{
    copy(df);
    return *this;
}

void wxDataFile::copy(const wxDataFile& df)
{
    m_fileName = df.m_fileName;
    m_realFileName = df.m_realFileName;
    m_ftype = df.m_ftype;
    m_mediaType = df.m_mediaType;
    m_tags = df.m_tags;
    m_sCueSheet = df.m_sCueSheet;
    wxDurationHolder::Copy(df);
}

wxString wxDataFile::GetFileTypeRegExp()
{
    return get_texts_regexp(FileTypeString);
}

wxString wxDataFile::ToString(wxDataFile::FileType ftype)
{
    return to_string(ftype, FileTypeString);
}

bool wxDataFile::FromString(const wxString& s, wxDataFile::FileType& ftype)
{
    return from_string(s, ftype, FileTypeString);
}

const wxFileName& wxDataFile::GetFileName() const
{
    return m_fileName;
}

bool wxDataFile::HasRealFileName() const
{
    return m_realFileName.HasName();
}

const wxFileName& wxDataFile::GetRealFileName() const
{
    return m_realFileName;
}

wxDataFile::FileType wxDataFile::GetFileType() const
{
    return m_ftype;
}

wxDataFile::MediaType wxDataFile::GetMediaType() const
{
    return m_mediaType;
}

bool wxDataFile::HasTags() const
{
    return !m_tags.IsEmpty();
}

const wxArrayCueTag& wxDataFile::GetTags() const
{
    return m_tags;
}

bool wxDataFile::HasCueSheet() const
{
    return !m_sCueSheet.IsEmpty();
}

const wxString& wxDataFile::GetCueSheet() const
{
    return m_sCueSheet;
}

wxString wxDataFile::GetFileTypeAsString() const
{
    return ToString(m_ftype);
}

bool wxDataFile::IsEmpty() const
{
    return !m_fileName.IsOk();
}

bool wxDataFile::IsBinary() const
{
    return (m_ftype == BINARY) || (m_ftype == MOTOROLA);
}

void wxDataFile::Clear()
{
    m_fileName.Clear();
    m_realFileName.Clear();
    m_ftype = BINARY;
    m_mediaType = MEDIA_TYPE_UNKNOWN;
    m_tags.Clear();
    m_sCueSheet.Empty();
    ClearDuration();
}

wxDataFile& wxDataFile::Assign(const wxString& sFilePath, wxDataFile::FileType ftype)
{
    m_fileName = sFilePath;
    m_realFileName.Clear();
    m_ftype = ftype;
    m_mediaType = MEDIA_TYPE_UNKNOWN;
    m_tags.Clear();
    m_sCueSheet.Empty();
    ClearDuration();

    return *this;
}

wxDataFile& wxDataFile::Assign(const wxFileName& fileName, wxDataFile::FileType ftype)
{
    m_fileName = fileName;
    m_realFileName.Clear();
    m_ftype = ftype;
    m_mediaType = MEDIA_TYPE_UNKNOWN;
    m_tags.Clear();
    m_sCueSheet.Empty();
    ClearDuration();

    return *this;
}

bool wxDataFile::FindFile(wxFileName& fn, const wxString& sAlternateExt) const
{
    if (m_fileName.IsFileReadable())	// file just exists
    {
        fn = m_fileName;
        return true;
    }

    wxString sTokens(sAlternateExt);

    if (IsBinary()) return false;
    else if (m_ftype == MP3) sTokens = "mp3";

    wxFileName        fnTry(m_fileName);
    wxStringTokenizer tokenizer(sTokens, ',');

    while (tokenizer.HasMoreTokens())
    {
        fnTry.SetExt(tokenizer.GetNextToken());

        if (fnTry.IsFileReadable())
        {
            fn = fnTry;
            wxLogInfo(_("%s -> %s"), m_fileName.GetFullName(), fnTry.GetFullName());
            return true;
        }
    }

    return false;
}

bool wxDataFile::FindFile(const wxString& sAlternateExt)
{
    return FindFile(m_realFileName, sAlternateExt);
}

wxULongLong wxDataFile::GetNumberOfFramesFromBinary(const wxFileName& fileName, const wxSamplingInfo& si)
{
    wxASSERT(fileName.IsFileReadable());
    wxASSERT(si.IsOK());

    wxULongLong size = fileName.GetSize();

    if (size == wxInvalidSize) return wxSamplingInfo::wxInvalidNumberOfFrames;

    return si.GetNumberOfFramesFromBytes(size);
}

bool wxDataFile::GetMediaInfo(
        const wxFileName& fileName,
        wxULongLong& frames,
        wxSamplingInfo& si,
        wxDataFile::MediaType& eMediaType,
        wxArrayCueTag& tags,
        wxString& sCueSheet)
{
    TagLib::FileRef fileRef(fileName.GetFullPath().t_str(), true, TagLib::AudioProperties::Accurate);

    if (fileRef.isNull())
    {
        if (fileName.GetExt().CmpNoCase("cue") != 0) wxLogError(_("Fail to initialize TagLib library."));
        return false;
    }

    TagLib::File const* pFile = fileRef.file();
    TagLib::AudioProperties const* pAprops = pFile->audioProperties();

    si.SetSamplingRate(pAprops->sampleRate());
    si.SetNumberOfChannels(pAprops->channels());

    eMediaType = MEDIA_TYPE_UNKNOWN;

    if (dynamic_cast<TagLib::RIFF::WAV::Properties const*>(pAprops))
    {
        eMediaType = MEDIA_TYPE_WAV;
        TagLib::RIFF::WAV::Properties const* pProps = dynamic_cast<TagLib::RIFF::WAV::Properties const*>(pAprops);
        frames = pProps->sampleFrames();
        si.SetBitsPerSample(pProps->sampleWidth());
    }
    else if (dynamic_cast<TagLib::RIFF::AIFF::Properties const*>(pAprops))
    {
        eMediaType = MEDIA_TYPE_AIFF;
        TagLib::RIFF::AIFF::Properties const* pProps = dynamic_cast<TagLib::RIFF::AIFF::Properties const*>(pAprops);
        frames = pProps->sampleFrames();
        si.SetBitsPerSample(pProps->sampleWidth());
    }
    else if (dynamic_cast<TagLib::FLAC::Properties const*>(pAprops))
    {
        eMediaType = MEDIA_TYPE_FLAC;
        TagLib::FLAC::Properties const* pProps = dynamic_cast<TagLib::FLAC::Properties const*>(pAprops);
        frames = pProps->sampleFrames();
        si.SetBitsPerSample(pProps->sampleWidth());
    }
    else if (dynamic_cast<TagLib::WavPack::Properties const*>(pAprops))
    {
        eMediaType = MEDIA_TYPE_WAVPACK;
        TagLib::WavPack::Properties const* pProps = dynamic_cast<TagLib::WavPack::Properties const*>(pAprops);
        frames = pProps->sampleFrames();
        si.SetBitsPerSample(pProps->bitsPerSample());
    }

    if (eMediaType == MEDIA_TYPE_UNKNOWN) return false;

    sCueSheet.Empty();
    tags.Empty();
    TagLib::PropertyMap props(pFile->properties());

    for (TagLib::PropertyMap::ConstIterator i = props.begin(), pend = props.end(); i != pend; ++i)
    {
        wxString propName(i->first.toWString());

        if (propName.CmpNoCase(wxCueTag::Name::CUESHEET) == 0)
        {
            sCueSheet = i->second[0].toWString();
        }
        else
        {
            for (TagLib::StringList::ConstIterator j = i->second.begin(), jend = i->second.end(); j != jend; ++j)
            {
                tags.Add(wxCueTag(wxCueTag::TAG_MEDIA_METADATA, propName, j->toWString()));
            }
        }
    }

    return true;
}

bool wxDataFile::GetInfo(const wxString& sAlternateExt)
{
    if (!HasRealFileName())
        if (!FindFile(sAlternateExt)) return false;

    wxSamplingInfo si;
    wxULongLong    frames;
    bool           res;

    if (IsBinary())
    {
        si.SetDefault();
        m_mediaType = MEDIA_TYPE_UNKNOWN;
        m_tags.Clear();
        m_sCueSheet.Empty();
        frames = GetNumberOfFramesFromBinary(m_realFileName, si);
        res = (frames != wxSamplingInfo::wxInvalidNumberOfFrames);
    }
    else
    {
        res = GetMediaInfo(m_realFileName, frames, si, m_mediaType, m_tags, m_sCueSheet);
    }

    if (res) SetDuration(wxDuration(si, frames));

    return res;
}

#include <wx/arrimpl.cpp>	// this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(wxArrayDataFile);
WX_DEFINE_OBJARRAY(wxArrayFileName);

