/*
 * wxPrimitiveRenderer.cpp
 */

#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#include <wxEncodingDetection/wxTextInputStreamOnString.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>
#include "wxConfiguration.h"
#include "wxInputFile.h"
#include "wxPrimitiveRenderer.h"
#include "wxFileNameCorrector.h"

namespace
{
    class FileDesc
    {
        public:

        static size_t tc2padding(size_t count)
        {
            if (count > 1000) return 4;
            else if (count > 100) return 3;
            else if (count > 10) return 2;
            else return 1;
        }

        FileDesc():
            m_showFileNo(false), m_nPadding(-1)
        {
        }

        FileDesc(const char* filePrefix, size_t fileNo, const wxString& fileExt, size_t nTotalCount):
            m_filePrefix(filePrefix), m_fileNo(fileNo), m_fileExt(fileExt), m_nPadding(tc2padding(nTotalCount)), m_showFileNo(true)
        {
        }

        FileDesc(const char* filePrefix, size_t fileNo, const wxFileName& fn, size_t nTotalCount):
            m_filePrefix(filePrefix), m_fileNo(fileNo), m_fileExt(fn.GetExt().Lower()), m_nPadding(tc2padding(nTotalCount)), m_showFileNo(true)
        {
        }

        FileDesc(const char* filePrefix, const wxString& fileExt, size_t nTotalCount):
            m_filePrefix(filePrefix), m_showFileNo(false), m_fileExt(fileExt), m_nPadding(tc2padding(nTotalCount))
        {
        }

        FileDesc(const char* filePrefix, const wxFileName& fn, size_t nTotalCount):
            m_filePrefix(filePrefix), m_showFileNo(false), m_fileExt(fn.GetExt().Lower()), m_nPadding(tc2padding(nTotalCount))
        {
        }

        FileDesc(const FileDesc& fd):
            m_filePrefix(fd.m_filePrefix), m_fileNo(fd.m_fileNo), m_showFileNo(fd.m_showFileNo), m_fileExt(fd.m_fileExt), m_nPadding(fd.m_nPadding)
        {
        }

        protected:

        wxString GetPrefix() const
        {
            return m_filePrefix;
        }

        wxString GetFileNo() const
        {
            if (!m_showFileNo) return wxEmptyString;

            if (m_nPadding == 0)
            {
                wxString s;
                s.Printf("%" wxSizeTFmtSpec "u", m_fileNo);
                return s;
            }
            else
            {
                wxString s;
                s.Printf("%" wxSizeTFmtSpec "u", m_fileNo);

                size_t nLen = s.Length();

                if (nLen < m_nPadding) return s.Prepend(wxString(m_nPadding - nLen, '0'));
                else return s;
            }
        }

        wxString GetExt() const
        {
            return m_fileExt;
        }

        public:

        wxString ToString() const
        {
            return GetPrefix() + GetFileNo() + '.' + GetExt();
        }

        protected:

        const char* m_filePrefix;
        size_t m_fileNo;
        bool m_showFileNo;
        wxString m_fileExt;
        size_t m_nPadding;
    };
}

// ===============================================================================

wxMatroskaAttachment::wxMatroskaAttachment()
{
}

wxMatroskaAttachment::wxMatroskaAttachment(
    const wxFileName& fileName,
    const wxString& name,
    const wxString& mimeType,
    const wxString& description
)
    :m_fileName(fileName),
    m_name(name),
    m_mimeType(mimeType),
    m_description(description)
{
}

wxMatroskaAttachment::wxMatroskaAttachment(const wxMatroskaAttachment& attachment)
    :m_fileName(attachment.m_fileName),
    m_name(attachment.m_name),
    m_mimeType(attachment.m_mimeType),
    m_description(attachment.m_description)
{
}

bool wxMatroskaAttachment::IsOk() const
{
    return m_fileName.IsOk();
}

const wxFileName& wxMatroskaAttachment::GetFileName() const
{
    return m_fileName;
}

bool wxMatroskaAttachment::MakeRelative(const wxFileName& relDir, wxPathFormat pathFormat)
{
    wxASSERT(relDir.IsDir());
    return m_fileName.MakeRelativeTo(relDir.GetFullPath(), pathFormat);
}

const wxString wxMatroskaAttachment::GetName() const
{
    return m_name;
}

const wxString wxMatroskaAttachment::GetName(const wxString& ext) const
{
    wxFileName fn(m_name);
    fn.SetExt(ext);
    return fn.GetFullName();
}

bool wxMatroskaAttachment::HasMimeType() const
{
    return !m_mimeType.IsEmpty();
}

const wxString& wxMatroskaAttachment::GetMimeType() const
{
    return m_mimeType;
}

bool wxMatroskaAttachment::IsPdf() const
{
    wxASSERT(HasMimeType());
    return m_mimeType.CmpNoCase(wxCoverFile::PDF_MIME) == 0;
}

bool wxMatroskaAttachment::HasDescription() const
{
    return !m_description.IsEmpty();
}

const wxString& wxMatroskaAttachment::GetDescription() const
{
    return m_description;
}

#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(wxArrayMatroskaAttachment);

// ===============================================================================

const char wxTagRenderer::NON_ALPHA_REG_EX[] = "^\\P{L&}+$";

namespace
{
    const char* LANGUAGE_AGNOSTIC_TAGS[] = {
        wxCueTag::Name::DISCID,
        wxCueTag::Name::UPC_EAN,
        wxCueTag::Name::CATALOG,
        wxCueTag::Name::RELEASECOUNTRY,
        wxCueTag::Name::RIPPER,
        wxCueTag::Name::PREPARER,
    };

    template<size_t N>
    bool check_tag_name(const wxCueTag& tag, const char* (&tagNames)[N])
    {
        for (size_t i = 0; i < N; ++i)
        {
            if (tag == tagNames[i]) return true;
        }
        return false;
    }

    bool check_tag_name(const wxCueTag& tag)
    {
        return check_tag_name(tag, LANGUAGE_AGNOSTIC_TAGS);
    }

    bool is_binary(const wxCueTag& tag)
    {
        wxCueComponent::ENTRY_FORMAT entryFmt;
        return (wxCueComponent::GetCdTextEntryFormat(tag.GetName(), entryFmt) && (entryFmt == wxCueComponent::BINARY));
    }
}

wxTagRenderer::wxTagRenderer()
    :m_nonAlphaRegEx(NON_ALPHA_REG_EX, wxRE_NOSUB)
{
    wxASSERT(m_nonAlphaRegEx.IsValid());
}

bool wxTagRenderer::IsNonAlphaTag(const wxCueTag& tag) const
{
    return m_nonAlphaRegEx.Matches(tag.GetValue());
}

bool wxTagRenderer::IsLanguageAgnostic(const wxConfiguration& cfg, const wxCueTag& tag) const
{
    return
        cfg.IsLangUndefined() ||
        tag.IsReplayGain() ||
        is_binary(tag) ||
        check_tag_name(tag) ||
        IsNonAlphaTag(tag);
}

// ===============================================================================

const char wxPrimitiveRenderer::MIME::OCTET_STREAM[] = "application/octet-stream";
const char wxPrimitiveRenderer::MIME::TEXT_PLAIN[] = "text/plain";

const char wxPrimitiveRenderer::FPREFIX::COVER[] = "cover";
const char wxPrimitiveRenderer::FPREFIX::CDTEXT[] = "cdtext";
const char wxPrimitiveRenderer::FPREFIX::EAC[] = "eac";
const char wxPrimitiveRenderer::FPREFIX::ACCURIP[] = "accurip";
const char wxPrimitiveRenderer::FPREFIX::CUESHEET[] = "cuesheet";

// ===============================================================================

wxPrimitiveRenderer::wxPrimitiveRenderer(const wxConfiguration& cfg):
    m_cfg(cfg)
{
}

void wxPrimitiveRenderer::InitTagsSynonimsCollections(
    wxTagSynonimsCollection& discCdTextSynonims,
    wxTagSynonimsCollection& discSynonims,
    wxTagSynonimsCollection& trackCdTextSynonims,
    wxTagSynonimsCollection& trackSynonims
)
{
    wxArrayString as;

    // DISC: TITLE = (TITLE,ALBUM)
    as.Add(wxCueTag::Name::ALBUM);
    wxTagSynonims discSynonim1(wxCueTag::Name::TITLE, as);

    discCdTextSynonims.Add(discSynonim1);

    // DISC: TITLE = (TITLE,ALBUM)
    discSynonims.Add(discSynonim1);

    as.Clear();

    // DISC: ARTIST = (ARTIST,PERFORMER)
    as.Add(wxCueTag::Name::PERFORMER);
    wxTagSynonims discSynonim2(wxCueTag::Name::ARTIST, as);

    discCdTextSynonims.Add(discSynonim2);
    trackCdTextSynonims.Add(discSynonim2);

    as.Clear();

    // DISC: ARTIST = (ARTIST,ALBUM ARTIST,ALBUMARTIST)
    as.Add(wxCueTag::Name::ALBUM_ARTIST);
    as.Add(wxCueTag::Name::ALBUMARTIST);
    wxTagSynonims discSynonim3(wxCueTag::Name::ARTIST, as);

    discSynonims.Add(discSynonim3);
    trackSynonims.Add(discSynonim3);
}

void wxPrimitiveRenderer::InitTagsSynonimsCollections()
{
    InitTagsSynonimsCollections(
        m_discCdTextSynonims, m_discSynonims,
        m_trackCdTextSynonims, m_trackSynonims);
 }

wxFileName wxPrimitiveRenderer::GetRelativeFileName(const wxFileName& fn, const wxFileName& relDir) const
{
    if (relDir.IsOk())
    {
        wxFileName res(fn);
        if (res.MakeRelativeTo(relDir.GetFullPath()))
        {
            return res;
        }
    }
    return fn;
}

bool wxPrimitiveRenderer::SaveCover(const wxInputFile& inputFile, const wxString& tmpStem, wxCoverFile& cover)
{
    if (cover.HasFileName()) return true;

    wxASSERT(cover.HasData());
    const wxFileName fn = m_cfg.GetTemporaryFile(inputFile, tmpStem, wxConfiguration::TMP::IMG, cover.GetExt());
    const bool res = cover.Save(fn);
    if (res)
    {
        m_temporaryFiles.Add(fn);
    }
    return res;
}

bool wxPrimitiveRenderer::SaveCover(const wxInputFile& inputFile, const wxString& tmpStem, size_t coverNo, wxCoverFile& cover)
{
    if (cover.HasFileName()) return true;

    wxASSERT(cover.HasData());
    wxString imgFmt(wxConfiguration::TMP::IMG);
    imgFmt << "%02" wxSizeTFmtSpec "u";
    const wxString postFix = wxString::Format(imgFmt , coverNo);
    const wxFileName fn = m_cfg.GetTemporaryFile(inputFile, tmpStem, postFix, cover.GetExt());
    const bool res = cover.Save(fn);
    if (res)
    {
        m_temporaryFiles.Add(fn);
    }
    return res;
}

wxString wxPrimitiveRenderer::GetCoverDescription(const wxCoverFile& coverFile)
{
    wxString stype;
    wxCoverFile::GetStrFromType(coverFile.GetType(), stype);

    wxString res;
    if (coverFile.HasDescription())
    {
        res = coverFile.GetDescription();
        res << " [" << stype << ']';
    }
    else
    {
        res = stype;
    }

    return res;
}

void wxPrimitiveRenderer::AppendCoverAttachments(
    wxArrayMatroskaAttachment& attachments,
    const wxInputFile& inputFile,
    const wxString& tmpStem,
    const wxArrayCoverFile& coverFiles
)
{
    size_t nAttachments = coverFiles.GetCount();

    switch (nAttachments)
    {
        case 0:
        {
            break;
        }

        case 1:
        {
            wxCoverFile& cover = coverFiles[0];
            if (SaveCover(inputFile, tmpStem, cover))
            {
                const wxMatroskaAttachment coverAttachment(
                    cover.GetFileName(),
                    FileDesc(FPREFIX::COVER, cover.GetFileName(), nAttachments).ToString(),
                    cover.GetMimeType(),
                    GetCoverDescription(cover)
                );
                attachments.push_back(coverAttachment);
            }
            break;
        }

        default:
        {
            if (SaveCover(inputFile, tmpStem, coverFiles[0]))
            {
                const wxMatroskaAttachment coverAttachment(
                    coverFiles[0].GetFileName(),
                    FileDesc(FPREFIX::COVER, coverFiles[0].GetFileName(), nAttachments).ToString(),
                    coverFiles[0].GetMimeType(),
                    GetCoverDescription(coverFiles[0])

                );
                attachments.push_back(coverAttachment);
            }

            // rest
            for (size_t i = 1; i < nAttachments; ++i)
            {
                if (SaveCover(inputFile, tmpStem, i, coverFiles[i]))
                {
                    const wxMatroskaAttachment coverAttachment(
                        coverFiles[i].GetFileName(),
                        FileDesc(FPREFIX::COVER, i+1, coverFiles[i].GetFileName(), nAttachments).ToString(),
                        coverFiles[i].GetMimeType(),
                        GetCoverDescription(coverFiles[i])

                    );
                    attachments.push_back(coverAttachment);
                }
            }

            break;
        }
    }
}

void wxPrimitiveRenderer::AppendCdTextFilesAttachments(
    wxArrayMatroskaAttachment& attachments,
    const wxInputFile& inputFile,
    const wxArrayFileName& cdtFiles
) const
{
    size_t nAttachments = cdtFiles.GetCount();

    switch (nAttachments)
    {
        case 0:
        {
            break;
        }

        case 1:
        {
            const wxMatroskaAttachment a(
                cdtFiles[0],
                FileDesc(FPREFIX::CDTEXT, cdtFiles[0], nAttachments).ToString(),
                MIME::OCTET_STREAM,
                cdtFiles[0].GetFullName()
            );
            attachments.push_back(a);
            break;
        }

        default:
        {
            for (size_t i = 0; i < nAttachments; ++i)
            {
                const wxMatroskaAttachment a(
                    cdtFiles[i],
                    FileDesc(FPREFIX::CDTEXT, i+1, cdtFiles[i], nAttachments).ToString(),
                    MIME::OCTET_STREAM,
                    cdtFiles[i].GetFullName()
                );
                attachments.push_back(a);
            }

            break;
        }
    }
}

void wxPrimitiveRenderer::AppendLogFilesAttachments(
    wxArrayMatroskaAttachment& attachments,
    const wxInputFile& inputFile,
    const wxArrayFileName& logFiles
) const
{
    size_t nAttachments = logFiles.GetCount();

    switch (nAttachments)
    {
        case 0:
        {
            break;
        }

        case 1:
        {
            const wxMatroskaAttachment a(
                logFiles[0],
                FileDesc(FPREFIX::EAC, logFiles[0], nAttachments).ToString(),
                MIME::TEXT_PLAIN,
                logFiles[0].GetFullName()
            );
            attachments.push_back(a);
            break;
        }

        default:
        {
            for (size_t i = 0; i < nAttachments; ++i)
            {
                const wxMatroskaAttachment a(
                    logFiles[i],
                    FileDesc(FPREFIX::EAC, i+1, logFiles[i], nAttachments).ToString(),
                    MIME::TEXT_PLAIN,
                    logFiles[i].GetFullName()
                );
                attachments.push_back(a);
            }

            break;
        }
    }
}

void wxPrimitiveRenderer::AppendSourceEacFilesAttachments(
    wxArrayMatroskaAttachment& attachments,
    const wxInputFile& inputFile,
    const wxArrayCueSheetContent& contents) const
{
    size_t nContents = contents.GetCount();
    size_t nSourceContents = 0;

    for (size_t i = 0; i < nContents; ++i)
    {
        const wxCueSheetContent& cnt = contents[i];

        if (cnt.HasSource() && !cnt.IsEmbedded()) nSourceContents += 1;
    }

    switch (nSourceContents)
    {
        case 0:
        {
            break;
        }

        case 1:
        {
            for (size_t i = 0; i < nContents; ++i)
            {
                const wxCueSheetContent& cnt = contents[i];
                if (!(cnt.HasSource() && !cnt.IsEmbedded())) continue;

                const wxMatroskaAttachment a(
                    cnt.GetSource().GetRealFileName(),
                    FileDesc(FPREFIX::CUESHEET, cnt.GetSource().GetFileName(), nSourceContents).ToString(),
                    MIME::TEXT_PLAIN,
                    cnt.GetSource().GetRealFileName().GetFullName()
                );
                attachments.push_back(a);
            }

            break;
        }

        default:
        {
            for (size_t i = 0, nCounter = 1; i < nContents; ++i)
            {
                const wxCueSheetContent& cnt = contents[i];
                if (!(cnt.HasSource() && !cnt.IsEmbedded())) continue;

                const wxMatroskaAttachment a(
                    cnt.GetSource().GetRealFileName(),
                    FileDesc(FPREFIX::CUESHEET, nCounter, cnt.GetSource().GetFileName(), nSourceContents).ToString(),
                    MIME::TEXT_PLAIN,
                    cnt.GetSource().GetRealFileName().GetFullName()
                );
                attachments.push_back(a);
                nCounter += 1;
            }

            break;
        }
    }
}

void wxPrimitiveRenderer::AppendDecodedEacFilesAttachments(
    wxArrayMatroskaAttachment& attachments,
    const wxInputFile& inputFile,
    const wxString& tmpStem,
    const wxArrayCueSheetContent& contents)
{
    size_t     nContents = contents.GetCount();
    wxFileName cueSheetPath;

    switch (nContents)
    {
        case 0:
        {
            break;
        }

        case 1:
        {
            if (SaveCueSheet(inputFile, tmpStem, wxConfiguration::TMP::EMBEDDED, contents[0].GetValue(), cueSheetPath))
            {
                wxMatroskaAttachment a(
                    cueSheetPath,
                    FileDesc(FPREFIX::CUESHEET, cueSheetPath, nContents).ToString(),
                    MIME::TEXT_PLAIN,
                    wxEmptyString
                );
                attachments.push_back(a);
            }

            break;
        }

        default:
        {
            wxString embFmt(wxConfiguration::TMP::EMBEDDED);
            embFmt << "%02" wxSizeTFmtSpec "u";

            for (size_t i = 0; i < nContents; ++i)
            {
                if (SaveCueSheet(inputFile, tmpStem, wxString::Format(embFmt, i+1), contents[i].GetValue(), cueSheetPath))
                {
                    wxMatroskaAttachment a(
                        cueSheetPath,
                        FileDesc(FPREFIX::CUESHEET, i+1, cueSheetPath, nContents).ToString(),
                        MIME::TEXT_PLAIN,
                        wxEmptyString
                    );
                    attachments.push_back(a);
                }
            }

            break;
        }
    }
}

bool wxPrimitiveRenderer::RenderCueSheet(
    const wxInputFile& inputFile,
    const wxString& tmpStem,
    const wxString& postFix,
    const wxCueSheet& cueSheet,
    wxFileName& fn)
{
    wxTextOutputStreamOnString tos;
    if (!wxTextCueSheetRenderer::ToString(*tos, cueSheet))
    {
        return false;
    }
    return SaveCueSheet(inputFile, tmpStem,  postFix, tos.GetString(), fn);
}

void wxPrimitiveRenderer::AppendRenderedEacFilesAttachments(
    wxArrayMatroskaAttachment& attachments,
    const wxInputFile& inputFile,
    const wxString& tmpStem,
    const wxCueSheet& cueSheet)
{
    wxFileName cueSheetPath;

    if (RenderCueSheet(inputFile, tmpStem, wxConfiguration::TMP::RENDERED, cueSheet, cueSheetPath))
    {
        const wxMatroskaAttachment a(
            cueSheetPath,
            FileDesc(FPREFIX::CUESHEET, cueSheetPath, 1).ToString(),
            MIME::OCTET_STREAM,
            _("Rendered cue sheet")
        );
        attachments.push_back(a);
    }

}

void wxPrimitiveRenderer::AppendAccuripLogAttachments(
    wxArrayMatroskaAttachment& attachments,
    const wxArrayFileName& logFiles) const
{
    size_t         nAttachments = logFiles.GetCount();
    const wxString logExt("log");

    switch (nAttachments)
    {
        case 0:
        {
            break;
        }

        case 1:
        {
            const wxMatroskaAttachment a(
                logFiles[0],
                FileDesc(FPREFIX::ACCURIP, logExt, nAttachments).ToString(),
                MIME::TEXT_PLAIN,
                logFiles[0].GetFullName()
            );
            attachments.push_back(a);
            break;
        }

        default:
        {
            for (size_t i = 0; i < nAttachments; ++i)
            {
                const wxMatroskaAttachment a(
                    logFiles[i],
                    FileDesc(FPREFIX::ACCURIP, i + 1, logExt, nAttachments).ToString(),
                    MIME::TEXT_PLAIN,
                    logFiles[i].GetFullName()
                );
                attachments.push_back(a);
            }

            break;
        }
    }
}

void wxPrimitiveRenderer::AppendEacFilesAttachments(
    wxArrayMatroskaAttachment& attachments,
    const wxInputFile& inputFile,
    const wxString& tmpStem,
    const wxCueSheet& cueSheet)
{
    switch (m_cfg.GetCueSheetAttachMode())
    {
        case wxConfiguration::CUESHEET_ATTACH_SOURCE:
        {
            AppendSourceEacFilesAttachments(attachments, inputFile, cueSheet.GetContents());
            break;
        }

        case wxConfiguration::CUESHEET_ATTACH_DECODED:
        {
            AppendDecodedEacFilesAttachments(attachments, inputFile, tmpStem, cueSheet.GetContents());
            break;
        }

        case wxConfiguration::CUESHEET_ATTACH_RENDERED:
        {
            AppendRenderedEacFilesAttachments(attachments, inputFile, tmpStem, cueSheet);
            break;
        }
    }
}

void wxPrimitiveRenderer::MakeRelativePaths(
    wxArrayMatroskaAttachment& attachments,
    const wxFileName& relDir,
    wxPathFormat pathFormat) const
{
    if (!relDir.IsOk()) return;
    wxASSERT(relDir.IsAbsolute());

    for(size_t i=0, cnt = attachments.GetCount(); i<cnt; ++i)
    {
        wxMatroskaAttachment& a = attachments[i];
        if (!a.MakeRelative(relDir, pathFormat))
        {
            wxLogWarning(_wxS("Unable to relativize attachment path " ENQUOTED_STR_FMT), a.GetFileName().GetFullName());
        }
    }
}

void wxPrimitiveRenderer::MakeRelativePaths(
    wxArrayMatroskaAttachment& attachments,
    const wxInputFile& inputFile,
    wxPathFormat pathFormat) const
{
    if (m_cfg.UseFullPaths()) return;
    const wxFileName relDir = m_cfg.GetOutputDir(inputFile);
    MakeRelativePaths(attachments, relDir, pathFormat);
}

bool wxPrimitiveRenderer::SaveCueSheet(
    const wxInputFile& inputFile,
    const wxString& tmpStem,
    const wxString& postFix,
    const wxString& content,
    wxFileName& cueSheet)
{
    const wxFileName fn = m_cfg.GetTemporaryFile(inputFile, tmpStem, postFix, wxConfiguration::EXT::CUESHEET);
    wxFileOutputStream os(fn.GetFullPath());

    if (os.IsOk())
    {
        wxLogInfo(_wxS("Creating cue sheet file " ENQUOTED_STR_FMT), cueSheet.GetFullName());
        wxSharedPtr< wxTextOutputStream > stream(m_cfg.GetOutputTextStream(os));
        wxTextOutputStreamOnString::SaveTo(*stream, content);
        m_temporaryFiles.Add(fn);
        cueSheet = fn;
        return true;
    }
    else
    {
        wxLogError(_wxS("Fail to create cue sheet file " ENQUOTED_STR_FMT), cueSheet.GetFullName());
        return false;
    }
}

wxString wxPrimitiveRenderer::GetTrackName(const wxCueSheet& cueSheet) const
{
    const wxScopedPtr< wxStringProcessor > stringProcessor(m_cfg.CreateStringProcessor());
    wxString trackName = m_cfg.GetMatroskaNameFormat();
    (*stringProcessor)(trackName);
    return cueSheet.Format(m_cfg.GetTagSources(), trackName);
}

wxString wxPrimitiveRenderer::GetContainerFileName(const wxCueSheet& cueSheet) const
{
    const wxString res = GetTrackName(cueSheet);
    const wxFileNameCorrector fnCorrector;
    return fnCorrector(res);
}

bool wxPrimitiveRenderer::IsLanguageAgnostic(const wxCueTag& tag) const
{
    return wxTagRenderer::IsLanguageAgnostic(m_cfg, tag);
}

void wxPrimitiveRenderer::GetTemporaryFiles(wxArrayFileName& tmpFiles) const
{
    WX_APPEND_ARRAY(tmpFiles, m_temporaryFiles);
}

bool wxPrimitiveRenderer::ConvertCover(const wxCueSheet& cueSheet) const
{
    return (m_cfg.ConvertCoverFile() && cueSheet.HasCover()) || (m_cfg.CoverFromPdf() && cueSheet.HasPdfCover());
}