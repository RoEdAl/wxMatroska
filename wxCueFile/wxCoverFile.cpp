/*
 *  wxCoverFile.cpp
 */

#include <wxCueFile/wxCoverFile.h>
#include <wxWEBPHandler/imagwebp.h>
#include "wxMd5.h"

 // ===============================================================================

const char* const wxCoverFile::CoverNames[] =
{
    "cover",
    "front",
    "folder",
    "picture"
};

// ===============================================================================

const wxBitmapType wxCoverFile::CoverFileTypes[] =
{
    wxBITMAP_TYPE_WEBP,
    wxBITMAP_TYPE_JPEG,
    wxBITMAP_TYPE_PNG,
    wxBITMAP_TYPE_GIF,
    wxBITMAP_TYPE_BMP
};

// ===============================================================================

const wxULongLong wxCoverFile::MAX_FILE_SIZE = wxUINT32_MAX;
const char wxCoverFile::PDF_EXT[] = "pdf";
const char wxCoverFile::PDF_MIME[] = "application/pdf";

// ===============================================================================

const wxCoverFile::TypeName wxCoverFile::TypeNames[] =
{
    { wxCoverFile::FrontCover, "FrontCover" },
    { wxCoverFile::BackCover, "BackCover" },
    { wxCoverFile::FileIcon, "FileIcon" },
    { wxCoverFile::LeafletPage, "LeafletPage" },
    { wxCoverFile::Media, "Media" },
    { wxCoverFile::LeadArtist, "LeadArtist" },
    { wxCoverFile::Artist, "Artist" },
    { wxCoverFile::Conductor, "Conductor" },
    { wxCoverFile::Band, "Band" },
    { wxCoverFile::Composer, "Composer" },
    { wxCoverFile::Lyricist, "Lyricist" },
    { wxCoverFile::RecordingLocation, "RecordingLocation" },
    { wxCoverFile::DuringRecording, "DuringRecording" },
    { wxCoverFile::DuringPerformance, "DuringPerformance" },
    { wxCoverFile::MovieScreenCapture, "MovieScreenCapture" },
    { wxCoverFile::ColouredFish, "ColouredFish" },
    { wxCoverFile::Illustration, "Illustration" },
    { wxCoverFile::BandLogo, "BandLogo" },
    { wxCoverFile::PublisherLogo, "PublisherLogo" },
    { wxCoverFile::OtherFileIcon, "OtherFileIcon" }
};

// ===============================================================================

namespace
{
    // how to avoid memory copying
    wxMemoryBuffer memory_stream_to_buffer(const wxMemoryOutputStream& os)
    {
        size_t         nSize = os.GetLength();
        wxMemoryBuffer data(nSize);
        void* pData = data.GetWriteBuf(nSize);

        os.CopyTo(pData, nSize);
        data.UngetWriteBuf(nSize);
        return data;
    }

    int size_cmp(size_t s1, size_t s2)
    {
        if (s1 < s2) return -1;
        else if (s1 > s2) return 1;
        else return 0;
    }
}

wxCoverFile::wxCoverFile(const wxCoverFile& cf):
    m_fileName(cf.m_fileName), m_data(cf.m_data), m_mimeType(cf.m_mimeType), m_description(cf.m_description), m_checksum(cf.m_checksum), m_type(cf.m_type), m_fileSize(cf.m_fileSize)
{
}

wxCoverFile::wxCoverFile(const wxFileName& fn, wxCoverFile::Type type):
    m_fileName(fn), m_type(type), m_mimeType(GetMimeFromExt(fn)), m_checksum(wxMD5::Get(fn)),m_fileSize(fn.GetSize())
{
}

wxCoverFile::wxCoverFile(const wxMemoryBuffer& data, wxCoverFile::Type type, const wxString& mimeType, const wxString& description):
    m_data(data), m_mimeType(mimeType), m_description(description), m_checksum(wxMD5::Get(data)), m_type(type)
{
}

wxCoverFile::wxCoverFile(const wxMemoryBuffer& data, const wxMemoryBuffer& checksum, wxCoverFile::Type type, const wxString& mimeType, const wxString& description):
    m_data(data), m_mimeType(mimeType), m_description(description), m_checksum(checksum), m_type(type)
{
}

const wxFileName& wxCoverFile::GetFileName() const
{
    return m_fileName;
}

const wxMemoryBuffer& wxCoverFile::GetData() const
{
    return m_data;
}

const wxString& wxCoverFile::GetMimeType() const
{
    return m_mimeType;
}

const wxString& wxCoverFile::GetDescription() const
{
    return m_description;
}

const wxMemoryBuffer& wxCoverFile::GetChecksum() const
{
    return m_checksum;
}

wxString wxCoverFile::GetExt() const
{
    if (HasFileName())
    {
        return m_fileName.GetExt();
    }
    else if (HasMimeType())
    {
        wxString ext;
        if (GetExtFromMime(m_mimeType, ext))
        {
            return ext;
        }
    }
    return wxEmptyString;
}

wxCoverFile::Type wxCoverFile::GetType() const
{
    return m_type;
}

size_t wxCoverFile::GetSize() const
{
    if (HasFileName())
    {
        wxASSERT(m_fileSize < MAX_FILE_SIZE);
        return m_fileSize.GetLo();
    }
    else
    {
        return m_data.GetDataLen();
    }
}

bool wxCoverFile::IsPdf() const
{
    if (HasMimeType())
    {
        return m_mimeType.CmpNoCase(PDF_MIME) == 0;
    }
    else if (HasFileName())
    {
        return m_fileName.GetExt().CmpNoCase(PDF_EXT) == 0;
    }
    else
    {
        return false;
    }
}

bool wxCoverFile::GetMimeFromExt(const wxFileName& fn, wxString& mimeType)
{
    return GetMimeFromExt(fn.GetExt(), mimeType);
}

bool wxCoverFile::GetMimeFromExt(const wxString& ext, wxString& mimeType)
{
    if (ext.CmpNoCase(PDF_EXT) == 0)
    {
        mimeType = PDF_MIME;
        return true;
    }

    wxList& imgHandlers = wxImage::GetHandlers();
    for (wxList::iterator i = imgHandlers.begin(), end = imgHandlers.end(); i != end; ++i)
    {
        const wxImageHandler* const imgHandler = wxStaticCast(*i, wxImageHandler);
        if (imgHandler->GetExtension().CmpNoCase(ext) == 0)
        {
            mimeType = imgHandler->GetMimeType();
            return true;
        }
    }
    return false;
}

wxString wxCoverFile::GetMimeFromExt(const wxFileName& fn)
{
    wxString mimeType;
    if (GetMimeFromExt(fn, mimeType))
    {
        return mimeType;
    }
    return wxEmptyString;
}

bool wxCoverFile::GuessMimeTypeFromData()
{
    wxASSERT(HasData());
    wxASSERT(!HasMimeType());

    wxMemoryInputStream dataStream(m_data.GetData(), m_data.GetDataLen());

    wxList& imgHandlers = wxImage::GetHandlers();
    for (wxList::iterator i = imgHandlers.begin(), end = imgHandlers.end(); i != end; ++i)
    {
        wxImageHandler* const imgHandler = wxStaticCast(*i, wxImageHandler);
        if (imgHandler->CanRead(dataStream))
        {
            m_mimeType = imgHandler->GetMimeType();
            return true;
        }
    }
    return false;
}

bool wxCoverFile::GetExtFromMime(const wxString& mimeType, wxString& ext)
{
    if (mimeType.CmpNoCase(PDF_MIME) == 0)
    {
        ext = PDF_EXT;
        return true;
    }

    wxList& imgHandlers = wxImage::GetHandlers();
    for (wxList::iterator i = imgHandlers.begin(), end = imgHandlers.end(); i != end; ++i)
    {
        const wxImageHandler* const imgHandler = wxStaticCast(*i, wxImageHandler);
        if (imgHandler->GetMimeType().CmpNoCase(mimeType) == 0)
        {
            ext = imgHandler->GetExtension();
            return true;
        }
    }
    return false;
}

wxCoverFile wxCoverFile::Load() const
{
    if (HasData() || !HasFileName())
    {
        return wxCoverFile(*this);
    }

    if (GetSize() > 0)
    {
        wxFileInputStream fs(m_fileName.GetFullPath());

        if (fs.IsOk())
        {
            wxMemoryOutputStream ms;

            fs.Read(ms);
            return wxCoverFile(memory_stream_to_buffer(ms), m_checksum, m_type, m_mimeType, m_description);
        }
    }

    return wxCoverFile(*this);
}

bool wxCoverFile::Save(const wxFileName& fn)
{
    wxASSERT(HasData());
    wxASSERT(!HasFileName());

    wxFileOutputStream fos(fn.GetFullPath());

    if (fos.IsOk())
    {
        wxLogInfo(_wxS("Creating image file " ENQUOTED_STR_FMT), fn.GetFullName());
        fos.Write(m_data.GetData(), m_data.GetDataLen());
        fos.Close();
        m_fileName = fn;
        m_fileSize = m_data.GetDataLen();
        return true;
    }
    else
    {
        wxLogError(_wxS("Fail to save image to " ENQUOTED_STR_FMT), fn.GetFullName());
        return false;
    }
}

bool wxCoverFile::Append(wxArrayCoverFile& ar, const wxCoverFile& cover)
{
    for (size_t i = 0, cnt = ar.GetCount(); i < cnt; ++i)
    {
        if (wxMD5::AreEqual(cover.GetChecksum(), ar[i].GetChecksum()))
        {
            return false;
        }
    }

    ar.Add(cover);
    return true;
}

size_t wxCoverFile::Append(wxArrayCoverFile& ar, const wxArrayCoverFile& covers)
{
    size_t res = 0;
    for (size_t i = 0, cnt = covers.GetCount(); i < cnt; ++i)
    {
        if (Append(ar, covers[i])) res += 1;
    }

    return res;
}

template< size_t SIZE >
bool wxCoverFile::IsCoverFile(const wxFileName& fileName, const wxBitmapType(&coverTypes)[SIZE])
{
    wxString ext;

    for (size_t i = 0; i < SIZE && ext.IsEmpty(); ++i)
    {
        const wxImageHandler* const handler = wxImage::FindHandler(coverTypes[i]);
        wxASSERT(handler != nullptr);

        if (handler->GetExtension().CmpNoCase(fileName.GetExt()) == 0)
        {
            ext = handler->GetExtension();
            continue;
        }

        const wxArrayString& altExt = handler->GetAltExtensions();
        for (size_t i = 0, cnt = altExt.GetCount(); i < cnt; ++i)
        {
            if (altExt[i].CmpNoCase(fileName.GetExt()) == 0)
            {
                ext = altExt[i];
                break;
            }
        }
    }

    if (ext.IsEmpty())
    {
        return false;
    }
    else
    {
        wxULongLong fsize = fileName.GetSize();
        return (fsize > 100 && fsize < MAX_FILE_SIZE);
    }
}

bool wxCoverFile::IsCoverFile(const wxFileName& fileName)
{
    return IsCoverFile(fileName, CoverFileTypes);
}

bool wxCoverFile::Find(const wxDir& sourceDir, const wxString& fileNameBase, wxFileName& coverFile)
{
    wxASSERT(sourceDir.IsOpened());

    const wxString fileSpec = wxString::Format("%s.*", fileNameBase);
    wxString fileName;

    if (sourceDir.GetFirst(&fileName, fileSpec, wxDIR_FILES))
    {
        while (true)
        {
            const wxFileName fn(sourceDir.GetName(), fileName);

            if (IsCoverFile(fn))
            {
                coverFile = fn;
                return true;
            }

            if (!sourceDir.GetNext(&fileName)) break;
        }
    }

    return false;
}

template< size_t SIZE >
bool wxCoverFile::Find(const wxFileName& inputFile, wxFileName& coverFile, const char* const (&coverNames)[SIZE])
{
    wxDir sourceDir(inputFile.GetPath());

    if (!sourceDir.IsOpened())
    {
        wxLogError(_wxS("Fail to open directory " ENQUOTED_STR_FMT), inputFile.GetPath());
        return false;
    }

    for (size_t i = 0; i < SIZE; ++i)
    {
        if (Find(sourceDir, coverNames[i], coverFile)) return true;
    }

    return false;
}

namespace
{
    wxFileName get_parent_dir(const wxFileName& fn)
    {
        wxFileName res(fn);
        res.SetName(wxEmptyString);
        res.ClearExt();
        wxASSERT(res.IsDir());
        if (res.GetDirCount() >= 2)
        {
            res.RemoveLastDir();
        }
        else
        {
            res.Clear();
        }
        return res;
    }
}

bool wxCoverFile::Find(const wxFileName& inputFile, wxFileName& coverFile, bool parentDir)
{
    if (Find(inputFile, coverFile, CoverNames))
    {
        return true;
    }

    if (parentDir)
    {
        const wxFileName parent = get_parent_dir(inputFile);
        if (parent.IsOk() && parent.IsDirReadable())
        {
            return Find(parent, coverFile, CoverNames);
        }
    }

    return false;
}

namespace
{
    size_t extract_flac_pictures(TagLib::FLAC::File& flac, wxArrayCoverFile& covers)
    {
        size_t res = 0;
        const TagLib::List<TagLib::FLAC::Picture*>& pictures = flac.pictureList();

        for(auto i = pictures.begin(); i != pictures.end(); ++i)
        {
            const TagLib::FLAC::Picture& picture = **i;
            const TagLib::ByteVector& pictureData = picture.data();

            wxMemoryBuffer buffer(pictureData.size());
            buffer.AppendData(pictureData.data(), pictureData.size());

            wxCoverFile::Type pictureType = (wxCoverFile::Type)picture.type();

            wxCoverFile cover(buffer, pictureType, picture.mimeType().toWString(), picture.description().toWString());
            if (!cover.HasMimeType())
            {
                if (!cover.GuessMimeTypeFromData())
                {
                    wxLogWarning(_wxS("Cover file: unknown cover type - skipping"));
                    continue;
                }
            }

            covers.Add(cover);
            res += 1;
        }
        return res;
    }

    size_t extract_ape_pictures(const TagLib::APE::Tag& tag, wxArrayCoverFile& covers)
    {
        size_t res = 0;
        for (auto i = tag.itemListMap().begin(), end = tag.itemListMap().end(); i != end; ++i)
        {
            if (i->second.type() != TagLib::APE::Item::Binary) continue;
            if (!i->first.startsWith("COVER ART")) continue;

            wxString           sName(i->first.toWString());
            TagLib::ByteVector binary = i->second.binaryData();

            int p = binary.find(0);

            if (p <= 0) continue;

            wxString s(binary.data(), wxConvUTF8, p);
            wxString mimeType;

            if (!wxCoverFile::GetMimeFromExt(s, mimeType)) continue;

            size_t         nPicLen = binary.size() - p - 1;
            wxMemoryBuffer buffer(nPicLen);
            buffer.AppendData(binary.data() + p + 1, nPicLen);

            wxString sDesc(i->first.substr(9).toCWString());
            sDesc.Trim(true).Trim(false);

            if (sDesc.StartsWith('(')) sDesc.Remove(0, 1);

            if (sDesc.EndsWith(')')) sDesc.RemoveLast();
            sDesc.Trim(true).Trim(false);

            wxCoverFile::Type pictureType = wxCoverFile::GetTypeFromStr(sDesc);

            if (pictureType != wxCoverFile::Other) sDesc.Empty();

            covers.Add(wxCoverFile(buffer, pictureType, mimeType, sDesc));
            res += 1;
        }
        return res;
    }
}

size_t wxCoverFile::Extract(const wxFileName& fileName, wxArrayCoverFile& covers)
{
    TagLib::FileRef fileRef(fileName.GetFullPath().t_str(), true, TagLib::AudioProperties::Accurate);

    if (fileRef.isNull())
    {
        wxLogWarning(_("Fail to extract cover file - TagLib library not initialized."));
        return 0;
    }

    TagLib::File* pFile = fileRef.file();

    if (dynamic_cast<TagLib::FLAC::File*>(pFile) != nullptr)
    {
        TagLib::FLAC::File* pFlac = dynamic_cast<TagLib::FLAC::File*>(pFile);
        return extract_flac_pictures(*pFlac, covers);
    }
    else if (dynamic_cast<TagLib::WavPack::File*>(pFile) != nullptr)
    {
        TagLib::WavPack::File* pWv = dynamic_cast<TagLib::WavPack::File*>(pFile);

        if (pWv->hasAPETag())
        {
            return extract_ape_pictures(*(pWv->APETag()), covers);
        }
    }
    return 0;
}

template< size_t SIZE >
wxCoverFile::Type wxCoverFile::GetTypeFromStr(const wxString& stype, const wxCoverFile::TypeName(&typeNames)[SIZE])
{
    for (size_t i = 0; i < SIZE; ++i)
    {
        if (stype.CmpNoCase(typeNames[i].name) == 0) return typeNames[i].type;
    }

    if (stype.CmpNoCase("Front") == 0) return FrontCover;
    else if (stype.CmpNoCase("Back") == 0) return BackCover;
    else return Other;
}

wxCoverFile::Type wxCoverFile::GetTypeFromStr(const wxString& stype)
{
    return GetTypeFromStr(stype, TypeNames);
}

template< size_t SIZE >
bool wxCoverFile::GetStrFromType(wxCoverFile::Type type, wxString& name, const wxCoverFile::TypeName(&typeNames)[SIZE])
{
    for (size_t i = 0; i < SIZE; ++i)
    {
        if (type == typeNames[i].type)
        {
            name = typeNames[i].name;
            return true;
        }
    }

    return false;
}

bool wxCoverFile::GetStrFromType(wxCoverFile::Type type, wxString& name)
{
    return GetStrFromType(type, name, TypeNames);
}

template< size_t SIZE >
size_t wxCoverFile::GetSortOrder(wxCoverFile::Type type, const wxCoverFile::TypeName(&typeNames)[SIZE])
{
    for (size_t i = 0; i < SIZE; ++i)
    {
        if (type == typeNames[i].type) return i;
    }

    return 100;
}

size_t wxCoverFile::GetSortOrder(wxCoverFile::Type type)
{
    return GetSortOrder(type, TypeNames);
}

int wxCoverFile::Cmp(wxCoverFile** f1, wxCoverFile** f2)
{
    const size_t so1 = GetSortOrder((*f1)->GetType());
    const size_t so2 = GetSortOrder((*f2)->GetType());

    if (so1 < so2) return -1;
    else if (so1 > so2) return 1;
    else return size_cmp((*f1)->GetSize(), (*f2)->GetSize());
}

void wxCoverFile::Sort(wxArrayCoverFile& covers)
{
    covers.Sort(wxCoverFile::Cmp);
}

// ===============================================================================

#include <wx/arrimpl.cpp>	// this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(wxArrayCoverFile);

