/*
 * wxCueSheetReader.h
 */

#ifndef _WX_CUE_SHEET_READER_H_
#define _WX_CUE_SHEET_READER_H_

#ifndef _WX_CUE_SHEET_H_
#include "wxCueSheet.h"
#endif

#ifndef _WX_INDEX_H_
class wxIndex;
#endif

#ifndef _WX_DATA_FILE_H_
#include "wxDataFile.h"
#endif

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

#ifndef _WX_UNQUOTER_H_
#include <wxCueFile/wxUnquoter.h>
#endif

#ifndef _WX_CUE_SHEET_CONTENT_H_
class wxCueSheetContent;
#endif

#ifndef NDEBUG

class TagLibDebugListener: public TagLib::DebugListener
{
    public:

    virtual void printMessage(const TagLib::String&);
};

#endif

class wxCueSheetReader
{
    wxDECLARE_NO_COPY_CLASS(wxCueSheetReader);

    public:

    struct eac_log
    {
        static const char EXT[];
        static const char MASK[];
    };

    struct accurip_log
    {
        static const char EXT[];
        static const char MASK[];
    };

    struct tags_file
    {
        static const char EXT[];
        static const char MASK[];
    };

    struct pdf_file
    {
        static const char EXT[];
        static const char MASK[];
    };

    typedef wxUint32 ReadFlags;

    enum
    {
        EC_PARSE_COMMENTS = 1,
        EC_ELLIPSIZE_TAGS = 2,
        EC_REMOVE_EXTRA_SPACES = 4,
        EC_MEDIA_READ_TAGS = 8,
        EC_SINGLE_MEDIA_FILE = 16,
        EC_FIND_COVER = 32,
        EC_FIND_LOG = 64,
        EC_CONVERT_UPPER_ROMAN_NUMERALS = 128,
        EC_CONVERT_LOWER_ROMAN_NUMERALS = 256,
        UNUSED_EC_CONVERT_COVER_TO_JPEG = 512,
        EC_CORRECT_DASHES = 1024,
        EC_FIND_ACCURIP_LOG = 2048,
        EC_SMALL_EM_DASH = 4096,
        EC_NUMBER_FULL_STOP = 8192,
        EC_SMALL_LETTER_PARENTHESIZED = 16384,
        EC_ASCII_TO_UNICODE = 32768,
        EC_APPLY_TAGS_FROM_FILE = 65536,
        EC_FIND_PDF = 131072,
        EC_ATTACHMENTS_IN_PARENT_DIR = 262144,
        EC_STRONG_ROMAN_NUMERALS_PARSER = 524288
    };

    static const ReadFlags DEF_READ_FLAGS;

    static bool TestReadFlags(ReadFlags, ReadFlags);
    bool TestReadFlags(ReadFlags) const;

    static wxStringProcessor* const CreateStringProcessor(ReadFlags, bool, const wxString&);

    protected:

    // regex
    wxRegEx m_reKeywords;
    wxRegEx m_reCdTextInfo;
    wxRegEx m_reEmpty;
    wxRegEx m_reIndex;
    wxRegEx m_reMsf;
    wxUnquoter m_unquoter;
    wxQuoteCorrector m_genericQuoteCorrector;
    wxQuoteCorrector m_quoteCorrector;
    wxRegEx m_reFlags;
    wxRegEx m_reDataMode;
    wxRegEx m_reDataFile;
    wxRegEx m_reCatalog;
    wxRegEx m_reIsrc;
    wxRegEx m_reTrackComment;
    wxRegEx m_reCommentMeta;
    wxScopedPtr< wxStringProcessor > m_stringProcessor;
    wxString m_oneTrackCue;

    // settings
    bool m_errorsAsWarnings;
    wxString m_lang;
    wxString m_alternateExt;
    ReadFlags m_readFlags;

    // work
    wxCueSheetContent m_cueSheetContent;
    wxArrayString m_errors;
    wxCueSheet m_cueSheet;

    // TagLib debug listener
#ifndef NDEBUG
    TagLibDebugListener m_debugListener;
#endif

    protected:

    static wxString GetOneTrackCue();

    template< typename T >
    static bool GetLogFile(const wxFileName&, bool, wxFileName&);

    template< typename T >
    static bool GetLogFile(const wxFileName&, bool, wxFileName&, bool);

    void AddError0(const wxString&);

    void AddError(const wxString, ...);

    void DumpErrors(size_t) const;

    bool CheckEntryType(wxCueComponent::ENTRY_TYPE) const;

    protected:

    typedef void (wxCueSheetReader::* PARSE_METHOD)(const wxString&, const wxString&);
    struct PARSE_STRUCT
    {
        const char* const token;
        PARSE_METHOD method;
    };

    static const PARSE_STRUCT parseArray[];

    protected:

    wxString internalReadCueSheet(wxInputStream& stream, wxMBConv& conv);

    bool ParseCue(const wxCueSheetContent&);

    bool ParseCueLine(const wxString&, size_t);

    template< size_t SIZE >
    bool ParseLine(const wxString&, const wxString&, const PARSE_STRUCT(&)[SIZE]);

    void ParseLine(size_t, const wxString&, const wxString&);
    void ParseCdTextInfo(size_t, const wxString&, const wxString&);

    void ParseGarbage(const wxString&);
    void ParseComment(const wxString&, const wxString&);
    void ParseComment(wxCueComponent&, const wxString&);
    bool ParseMsf(const wxString&, wxIndex&, bool = false);

    wxString Unquote(const wxString&) const;
    wxString UnquoteFs(const wxString&) const;

    void ParsePreGap(const wxString&, const wxString&);
    void ParsePostGap(const wxString&, const wxString&);
    void ParseIndex(const wxString&, const wxString&);
    void ParseFile(const wxString&, const wxString&);
    void ParseFlags(const wxString&, const wxString&);
    void ParseTrack(const wxString&, const wxString&);
    void ParseCatalog(const wxString&, const wxString&);
    void ParseCdTextFile(const wxString&, const wxString&);

    void CorrectTag(wxCueTag&) const;
    void CorrectString(wxString&) const;

    bool AddCdTextInfo(const wxString&, const wxString&);
    void AppendTags(const wxArrayCueTag&, bool);

    void AppendTags(const wxArrayCueTag&, size_t, size_t);

    protected:

    bool BuildFromSingleMediaFile(const wxDataFile&);
    bool ReadTagsFromRelatedFiles();

    bool ReadTagsFromMediaFile(const wxDataFile&, size_t, size_t);

    bool FindLog(const wxCueSheetContent&);
    bool FindAccurateRipLog(const wxCueSheetContent&);

    bool FindCoversInRelatedFiles();
    bool FindCover(const wxCueSheetContent&);
    bool FindPdfCover(const wxCueSheetContent&);
    size_t ExtractCoversFromDataFile(const wxDataFile&, wxArrayCoverFile&) const;

    bool ApplyTagsFromFile(const wxCueSheetContent&);

    public:

    wxCueSheetReader(void);

#ifndef NDEBUG
    virtual ~wxCueSheetReader(void);
#endif

    static wxString GetTagLibVersion();

    // settings
    bool ErrorsAsWarnings() const;
    wxCueSheetReader& SetErrorsAsWarnings(bool = true);
    wxCueSheetReader& CorrectQuotationMarks(bool, const wxString&);
    const wxString& GetAlternateExt() const;
    wxCueSheetReader& SetAlternateExt(const wxString&);
    ReadFlags GetReadFlags() const;

    wxCueSheetReader& SetReadFlags(ReadFlags);

    // parsing
    bool ReadCueSheet(const wxString&, bool = true);
    bool ReadCueSheet(const wxString&, wxMBConv&);
    bool ReadCueSheet(wxInputStream&);
    bool ReadCueSheet(wxInputStream&, wxMBConv&);
    bool ReadCueSheetEx(const wxString&, bool);

    // reading
    const wxCueSheet& GetCueSheet() const;
};

#endif

