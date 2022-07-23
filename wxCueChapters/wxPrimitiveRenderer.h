/*
 * wxPrimitiveRenderer.h
 */

#ifndef _WX_PRIMITIVE_RENDERER_H_
#define _WX_PRIMITIVE_RENDERER_H_

#ifndef _WX_CONFIGURATION_H_
class wxConfiguration;
#endif

#ifndef _WX_CUE_SHEET_H_
class wxCueSheet;
class wxArrayCueSheet;
#endif

#ifndef _WX_CUE_SHEET_CONTENT_H_
class wxCueSheetContent;
class wxArrayCueSheetContent;
#endif

#ifndef _WX_INPUT_FILE_H_
class wxInputFile;
#endif

#ifndef _WX_COVER_FILE_H_
class wxCoverFile;
class wxArrayCoverFile;
#endif

#include "wxTemporaryFilesProvider.h"

#ifndef _WX_TEXT_OUTPUT_STREAM_ON_STRING_H_
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#endif

#ifndef _WX_TAG_SYNONIMS_H_
#include <wxCueFile/wxTagSynonims.h>
#endif

class wxMatroskaAttachment
{
    public:

    wxMatroskaAttachment();
    wxMatroskaAttachment(const wxFileName&, const wxString&, const wxString&, const wxString&);
    wxMatroskaAttachment(const wxMatroskaAttachment&);

    public:

    bool IsOk() const;
    const wxFileName& GetFileName() const;
    bool MakeRelative(const wxFileName&, wxPathFormat = wxPATH_NATIVE);
    const wxString GetName() const;
    const wxString GetName(const wxString&) const;
    bool HasMimeType() const;
    const wxString& GetMimeType() const;
    bool IsPdf() const;
    bool HasDescription() const;
    const wxString& GetDescription() const;

    protected:

    wxFileName m_fileName;
    wxString m_name;
    wxString m_mimeType;
    wxString m_description;
};

WX_DECLARE_OBJARRAY(wxMatroskaAttachment, wxArrayMatroskaAttachment);

class wxTagRenderer
{
    wxDECLARE_NO_ASSIGN_CLASS(wxTagRenderer);

    protected:

    wxTagRenderer();

    public:

    static const char NON_ALPHA_REG_EX[];

    bool IsNonAlphaTag(const wxCueTag&) const;
    bool IsLanguageAgnostic(const wxConfiguration&, const wxCueTag&) const;

    private:

    wxRegEx m_nonAlphaRegEx;
};

class wxPrimitiveRenderer :protected wxTagRenderer, public wxTemporaryFilesProvider
{
    wxDECLARE_NO_COPY_CLASS(wxPrimitiveRenderer);

    protected:

    struct MIME
    {
        static const char OCTET_STREAM[];
        static const char TEXT_PLAIN[];
    };

    struct FPREFIX
    {
        static const char COVER[];
        static const char CDTEXT[];
        static const char EAC[];
        static const char ACCURIP[];
        static const char CUESHEET[];
    };

    protected:

    const wxConfiguration& m_cfg;
    wxTextOutputStreamOnString m_os;
    wxArrayFileName m_temporaryFiles;

    protected:

    wxTagSynonimsCollection m_discCdTextSynonims;
    wxTagSynonimsCollection m_discSynonims;

    wxTagSynonimsCollection m_trackCdTextSynonims;
    wxTagSynonimsCollection m_trackSynonims;

    protected:

    wxPrimitiveRenderer(const wxConfiguration&);
    void InitTagsSynonimsCollections();
    wxFileName GetRelativeFileName(const wxFileName&, const wxFileName&) const;
    bool SaveCover(const wxInputFile&, const wxString&, wxCoverFile&);
    bool SaveCover(const wxInputFile&, const wxString&, size_t, wxCoverFile&);
    static wxString GetCoverDescription(const wxCoverFile&);
    void AppendCoverAttachments(wxArrayMatroskaAttachment&, const wxInputFile&, const wxString&, const wxArrayCoverFile&);
    void AppendCdTextFilesAttachments(wxArrayMatroskaAttachment&, const wxInputFile&, const wxArrayFileName&) const;
    void AppendLogFilesAttachments(wxArrayMatroskaAttachment&, const wxInputFile&, const wxArrayFileName&) const;
    void AppendSourceEacFilesAttachments(wxArrayMatroskaAttachment&, const wxInputFile&, const wxArrayCueSheetContent&) const;
    void AppendDecodedEacFilesAttachments(wxArrayMatroskaAttachment&, const wxInputFile&, const wxString&, const wxArrayCueSheetContent&);
    void AppendRenderedEacFilesAttachments(wxArrayMatroskaAttachment&, const wxInputFile&, const wxString&, const wxCueSheet&);
    void AppendEacFilesAttachments(wxArrayMatroskaAttachment&, const wxInputFile&, const wxString&, const wxCueSheet&);
    void AppendAccuripLogAttachments(wxArrayMatroskaAttachment&, const wxArrayFileName&) const;
    void MakeRelativePaths(wxArrayMatroskaAttachment&, const wxFileName&, wxPathFormat = wxPATH_NATIVE) const;
    void MakeRelativePaths(wxArrayMatroskaAttachment&, const wxInputFile&, wxPathFormat = wxPATH_NATIVE) const;
    bool SaveCueSheet(const wxInputFile&, const wxString&, const wxString&, const wxString&, wxFileName&);
    bool RenderCueSheet(const wxInputFile&, const wxString&, const wxString&, const wxCueSheet&, wxFileName&);
    wxString GetTrackName(const wxCueSheet&) const;
    wxString GetContainerFileName(const wxCueSheet&) const;
    bool IsLanguageAgnostic(const wxCueTag&) const;
    virtual void GetTemporaryFiles(wxArrayFileName&) const;
    wxSamplingInfo GetSamplingInfo(const wxCueSheet&) const;
    wxSamplingInfo GetDowngradedSamplingInfo(const wxCueSheet&) const;
    static bool NeedTemporaryLink(const wxFileName&, bool);

    public:

    bool ConvertCover(const wxCueSheet&) const;
    static void InitTagsSynonimsCollections(
        wxTagSynonimsCollection&, wxTagSynonimsCollection&,
        wxTagSynonimsCollection&, wxTagSynonimsCollection&);
};

#endif

