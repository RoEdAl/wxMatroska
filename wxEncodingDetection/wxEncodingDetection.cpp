/*
 * wxEncodingDetection.cpp
 */

#include "wxMultiLanguage.h"
#include "wxMLangConvertCharset.h"
#include <wxEncodingDetection/wxNoConv.h>
#include <wxEncodingDetection/wxEncodingDetection.h>

 // ===========================================================

const wxByte wxEncodingDetection::BOM::UTF32_BE[4] = { 0x00, 0x00, 0xFE, 0xEF };
const wxByte wxEncodingDetection::BOM::UTF32_LE[4] = { 0xFF, 0xFE, 0x00, 0x00 };
const wxByte wxEncodingDetection::BOM::UTF16_BE[2] = { 0xFE, 0xFF };
const wxByte wxEncodingDetection::BOM::UTF16_LE[2] = { 0xFF, 0xFE };
const wxByte wxEncodingDetection::BOM::UTF8[3] = { 0xEF, 0xBB, 0xBF };

const wxUint32 wxEncodingDetection::CP::UTF32_BE = 12001;
const wxUint32 wxEncodingDetection::CP::UTF32_LE = 12000;
const wxUint32 wxEncodingDetection::CP::UTF16_BE = 1201;
const wxUint32 wxEncodingDetection::CP::UTF16_LE = 1200;
const wxUint32 wxEncodingDetection::CP::UTF8 = 65001;

// ===========================================================

namespace
{
    class wxMBConv_MLang:
        public wxMBConv
    {
        public:

    #if WORDS_BIGENDIAN
        static const wxUint32 UNICODE_CP = wxEncodingDetection::CP::UTF16_BE;
    #else
        static const wxUint32 UNICODE_CP = wxEncodingDetection::CP::UTF16_LE;
    #endif

        static wxMBConv* Create(wxUint32 nCodePage, wxString& sDescription)
        {
            if (nCodePage == UNICODE_CP)
            {
                if (GetDescription(UNICODE_CP, sDescription))
                {
                    wxNoConv* const pConv = new wxNoConv();
                    return pConv;
                }
                else
                {
                    return NULL;
                }
            }
            else
            {
                wxMBConv_MLang* pConvMLang = new wxMBConv_MLang(nCodePage);

                if (pConvMLang->GetDescription(sDescription))
                {
                    return pConvMLang;
                }
                else
                {
                    wxLogError(_("Unable to get encoding description: %s"), sDescription);
                    wxDELETE(pConvMLang);
                    return NULL;
                }
            }
        }

        protected:

        wxMBConv_MLang():
            m_nCodePage(0),
            m_minMBCharWidth(0)
        {
        }

        wxMBConv_MLang(wxUint32 nCodePage):
            m_nCodePage(nCodePage),
            m_minMBCharWidth(0)
        {
        }

        wxMBConv_MLang(const wxMBConv_MLang& conv):
            m_nCodePage(conv.m_nCodePage),
            m_mlangFromUnicode(conv.m_mlangFromUnicode),
            m_mlangToUnicode(conv.m_mlangToUnicode),
            m_minMBCharWidth(conv.m_minMBCharWidth)
        {
        }

        public:

        virtual size_t ToWChar(
            wchar_t* dst, size_t dstLen,
            const char* src, size_t srcLen = wxNO_LEN) const wxOVERRIDE
        {
            wxASSERT(!NoConversion());

            wxMBConv_MLang* const        self = wxConstCast(this, wxMBConv_MLang);
            const wxMLangConvertCharset& toUnicode = self->GetToUnicode();

            wxASSERT(toUnicode.IsValid());

            UINT nSrcSize = srcLen;
            UINT nDstSize = dstLen * sizeof(wchar_t);

            HRESULT hRes = toUnicode->DoConversion(
                    (BYTE*)src,
                    &nSrcSize,
                    (BYTE*)dst,
                    &nDstSize);

            if (hRes == S_OK)
            {
                if (nDstSize > 0)
                {
                    if (dst != NULL && *dst == wxS('\uFFFD') && srcLen <= 3)
                    {
                        wxLogDebug("Unicode replacement character - FFFE");
                        return wxCONV_FAILED;
                    }
                    else if (nDstSize < sizeof(wchar_t))
                    {
                        return wxCONV_FAILED;
                    }
                    else
                    {
                        return nDstSize / sizeof(wchar_t);
                    }
                }
                else
                {
                    return wxCONV_FAILED;
                }
            }
            else
            {
                wxLogDebug("wxMBConv_MLang::ToWChar: DoConvert error: 0x%08x", hRes);
                return wxCONV_FAILED;
            }
        }

        virtual size_t FromWChar(char* dst, size_t dstLen, const wchar_t* src,
            size_t srcLen = wxNO_LEN) const wxOVERRIDE
        {
            wxASSERT(!NoConversion());

            wxMBConv_MLang* const        self = wxConstCast(this, wxMBConv_MLang);
            const wxMLangConvertCharset& fromUnicode = self->GetFromUnicode();

            wxASSERT(fromUnicode.IsValid());

            UINT nSrcSize = srcLen * sizeof(wchar_t);
            UINT nDstSize = dstLen;

            HRESULT hRes = fromUnicode->DoConversion(
                    (BYTE*)src,
                    &nSrcSize,
                    (BYTE*)dst,
                    &nDstSize);

            if (hRes == S_OK)
            {
                if (nDstSize > 0) return nDstSize;
                else return wxCONV_FAILED;
            }
            else
            {
                wxLogDebug("wxMBConv_MLang::FromWChar: DoConvert error: 0x%08x", hRes);
                return wxCONV_FAILED;
            }
        }

        virtual size_t GetMBNulLen() const wxOVERRIDE
        {
            wxMBConv_MLang* const        self = wxConstCast(this, wxMBConv_MLang);
            const wxMLangConvertCharset& fromUnicode = self->GetFromUnicode();

            wxASSERT(fromUnicode.IsValid());

            if (m_minMBCharWidth == 0)
            {
                DWORD   dwMode = 0;
                UINT    nSrcSize = sizeof(wchar_t);
                UINT    nDstSize = 0;
                wchar_t wNull = L'\0';

                HRESULT hRes = fromUnicode->DoConversion(
                        (BYTE*)&wNull, &nSrcSize,
                        NULL, &nDstSize);

                if (hRes == S_OK)
                {
                    switch (nDstSize)
                    {
                        default:
                        {
                            wxLogDebug("Unexpected NUL length %u", nDstSize);
                            self->m_minMBCharWidth = (size_t)-1;
                            break;
                        }

                        case 0:
                        {
                            self->m_minMBCharWidth = (size_t)-1;
                            break;
                        }

                        case 1:
                        case 2:
                        case 4:
                        {
                            self->m_minMBCharWidth = nDstSize;
                            break;
                        }
                    }
                }
                else
                {
                    self->m_minMBCharWidth = (size_t)-1;
                }
            }

            return m_minMBCharWidth;
        }

        virtual wxMBConv* Clone() const wxOVERRIDE
        {
            return new wxMBConv_MLang(*this);
        }

        static bool GetDescription(wxUint32 nCodePage, wxString& sDescription)
        {
            wxString        sCPDescription;
            wxMultiLanguage mlang;

            if (!mlang.IsValid()) return false;

            HRESULT hRes = mlang.GetCodePageDescription(nCodePage, sCPDescription);

            if (hRes == S_OK)
            {
                sDescription.Printf(_("%s [CP:%u]"), sCPDescription, nCodePage);
                return true;
            }
            else
            {
                sDescription.Printf(_("<ERR:%08x> [CP:%u]"), hRes, nCodePage);
                return false;
            }
        }

        bool GetDescription(wxString& sDescription) const
        {
            return GetDescription(m_nCodePage, sDescription);
        }

        protected:

        bool NoConversion() const
        {
            return m_nCodePage == UNICODE_CP;
        }

        const wxMLangConvertCharset& GetFromUnicode()
        {
            if (!m_mlangFromUnicode.IsValid()) m_mlangFromUnicode.Initialize(UNICODE_CP, m_nCodePage);
            return m_mlangFromUnicode;
        }

        const wxMLangConvertCharset& GetToUnicode()
        {
            if (!m_mlangToUnicode.IsValid()) m_mlangToUnicode.Initialize(m_nCodePage, UNICODE_CP);
            return m_mlangToUnicode;
        }

        protected:

        wxUint32 m_nCodePage;
        wxMLangConvertCharset m_mlangToUnicode;
        wxMLangConvertCharset m_mlangFromUnicode;

        // cached result of GetMBNulLen(), set to 0 initially meaning "unknown"
        size_t m_minMBCharWidth;
    };

    class wxMBConv_BOM: public wxMBConv
    {
        typedef wxScopedPtr<wxMBConv> wxMBConvScopedPtr;
        typedef wxCharTypeBuffer<wxByte> wxByteBuffer;

        public:

        static wxMBConv_BOM* Create(const wxByte* bom, size_t nLen, wxUint32 nCodePage, bool bUseMLang, wxString& sDescription)
        {
            wxMBConv* const pConvStd = wxEncodingDetection::GetStandardMBConv(nCodePage, bUseMLang, sDescription);
            return new wxMBConv_BOM(bom, nLen, pConvStd);
        }

        static wxMBConv_BOM* Create(const wxByte* bom, size_t nLen, wxUint32 nCodePage, wxString& sDescription)
        {
            return Create(bom, nLen, nCodePage, true, sDescription);
        }

        protected:

        wxMBConv_BOM(const wxByte* const bom, size_t len, wxMBConv* const conv):
            m_bom(bom, len),
            m_conv(conv->Clone()),
            m_bomConsumed(false)
        {
            wxASSERT(m_conv);
            wxASSERT(len >= 2);
        }

        wxMBConv_BOM(const wxMBConv_BOM& convBom):
            m_bom(convBom.m_bom),
            m_conv(convBom.m_conv->Clone()),
            m_bomConsumed(false)
        {
        }

        public:

        virtual wxMBConv* Clone() const wxOVERRIDE { return new wxMBConv_BOM(*this); }

        virtual size_t ToWChar(wchar_t* dst, size_t dstLen, const char* src, size_t srcLen) const wxOVERRIDE
        {
            wxMBConv_BOM* self = const_cast<wxMBConv_BOM*>(this);

            if (!m_bomConsumed)
            {
                if (!SkipBOM(src, srcLen)) return wxCONV_FAILED;
                else if (srcLen == 0) return wxCONV_FAILED;
            }

            size_t rc = m_conv->ToWChar(dst, dstLen, src, srcLen);

            // don't skip the BOM again the next time if we really consumed it
            if (rc != wxCONV_FAILED && dst && !m_bomConsumed) self->m_bomConsumed = true;

            return rc;
        }

        virtual size_t FromWChar(char* dst, size_t dstLen, const wchar_t* src, size_t srcLen) const wxOVERRIDE
        {
            return m_conv->FromWChar(dst, dstLen, src, srcLen);
        }

        virtual size_t GetMBNulLen() const wxOVERRIDE
        {
            return m_conv->GetMBNulLen();
        }

        protected:

        bool SkipBOM(const char*& src, size_t& len) const
        {
            size_t realLen = (len != (size_t)-1) ? len : wxStrnlen(src, 16);

            if (realLen < m_bom.length())	// still waiting
                return false;

            if (!check_bom(src)) return false;

            src += m_bom.length();

            if (len != (size_t)-1) len -= m_bom.length();

            return true;
        }

        bool check_bom(const char* src) const
        {
            bool bRes = true;

            for (size_t i = 0; bRes && (i < m_bom.length()); i++)
            {
                if (m_bom[i] != (wxByte)(src[i])) bRes = false;
            }

            return bRes;
        }

        protected:

        wxMBConvScopedPtr m_conv;
        wxByteBuffer m_bom;
        bool m_bomConsumed;
    };
}

wxUint32 wxEncodingDetection::GetDefaultEncoding()
{
    return wxMLangConvertCharset::GetRealCodePage(CP_THREAD_ACP);
}

wxMBConv* wxEncodingDetection::GetDefaultEncoding(bool useMLang, wxString& description)
{
    if (useMLang)
    {
        return wxMBConv_MLang::Create(CP_THREAD_ACP, description);
    }
    else
    {
        description = _("Default (Native)");
        return wxConvLocal.Clone();
    }
}

wxMBConv* wxEncodingDetection::GetStandardMBConv(wxUint32 codePage, bool useMLang, wxString& description)
{
    wxMBConv* pConv = nullptr;

    if (!useMLang)
    {
        switch (codePage)
        {
            case CP::UTF32_BE:
            {
                pConv = new wxMBConvUTF32BE();
                description = _("UTF-32 BE (Native)");
                break;
            }

            case CP::UTF32_LE:
            {
                pConv = new wxMBConvUTF32LE();
                description = _("UTF-32 LE (Native)");
                break;
            }

            case CP::UTF16_BE:
            {
                pConv = new wxMBConvUTF16BE();
                description = _("UTF-16 BE (Native)");
                break;
            }

            case CP::UTF16_LE:
            {
                pConv = new wxMBConvUTF16LE();
                description = _("UTF-16 LE (Native)");
                break;
            }

            case CP::UTF8:
            {
                pConv = wxConvUTF8.Clone();
                description = _("UTF-8 (Native)");
                break;
            }

            default:
            {
                wxFAIL_MSG("Invalid nCodePage parameter");
                break;
            }
        }
    }
    else
    {
        pConv = wxMBConv_MLang::Create(codePage, description);
    }

    return pConv;
}

bool wxEncodingDetection::test_bom(const wxByteBuffer& buffer, const wxByte* bom, size_t len)
{
    wxASSERT(len >= 2);
    bool bRes = true;

    for (size_t i = 0; bRes && (i < len); ++i)
    {
        if (buffer[i] != bom[i]) bRes = false;
    }

    return bRes;
}

wxMBConv* wxEncodingDetection::GetFileEncodingFromBOM(const wxFileName& fn, bool useMLang, wxString& description)
{
    wxByteBuffer buffer(4);
    size_t lastRead = 0;
    wxMBConv* pRes = nullptr;

    {
        wxFileInputStream fis(fn.GetFullPath());

        if (!fis.IsOk())
        {
            wxLogError(_wxS("Cannot open file " ENQUOTED_STR_FMT), fn.GetName());
            return pRes;
        }

        lastRead = fis.Read(buffer.data(), buffer.length()).LastRead();
    }

    switch (lastRead)
    {
        default:// 4 and more
        {
            if (test_bom(buffer, BOM::UTF32_BE, 4))
            {
                pRes = wxMBConv_BOM::Create(BOM::UTF32_BE, 4, CP::UTF32_BE, useMLang, description);
                break;
            }
            else if (test_bom(buffer, BOM::UTF32_LE, 4))
            {
                pRes = wxMBConv_BOM::Create(BOM::UTF32_LE, 4, CP::UTF16_LE, useMLang, description);
                break;
            }
        }

        case 3:	// UTF8
        {
            if (test_bom(buffer, BOM::UTF8, 3))
            {
                pRes = wxMBConv_BOM::Create(BOM::UTF8, 3, CP::UTF8, useMLang, description);
                break;
            }
        }

        case 2:	// UTF16
        {
            if (test_bom(buffer, BOM::UTF16_BE, 2))
            {
                pRes = wxMBConv_BOM::Create(BOM::UTF16_BE, 2, CP::UTF16_BE, useMLang, description);
            }
            else if (test_bom(buffer, BOM::UTF16_LE, 2))
            {
                pRes = wxMBConv_BOM::Create(BOM::UTF16_LE, 2, CP::UTF16_LE, useMLang, description);
            }

            break;
        }

        case 0:
        case 1:
        {
            wxLogError(_wxS("Cannot read BOM - file " ENQUOTED_STR_FMT " is too small"), fn.GetName());
            break;
        }
    }

    return pRes;
}

wxMBConv* wxEncodingDetection::GetFileEncoding(const wxFileName& fn, bool useMLang, wxString& description)
{
    wxMBConv* pRes = nullptr;
    wxULongLong fileSize = fn.GetSize();

    if (fileSize == wxInvalidSize)
    {
        wxLogError(_wxS("Cannot determine size of file " ENQUOTED_STR_FMT), fn.GetName());
        return pRes;
    }

    if (fileSize == wxULL(0))
    {
        wxLogError(_wxS("Cannot determine encoding of empty file " ENQUOTED_STR_FMT), fn.GetName());
        return pRes;
    }

    pRes = GetFileEncodingFromBOM(fn, useMLang, description);

    if (pRes == nullptr)
    {
        return pRes;
    }

    if (useMLang)
    {
        wxMultiLanguage multiLanguage;
        wxUint32        defCodePage = 0;
        wxMultiLanguage::GetDefaultCodePage(defCodePage);

        if (!multiLanguage.IsValid())
        {
            return nullptr;
        }

        DetectEncodingInfo dei[10];
        INT                nSize = 10;

        if (fileSize < wxULL(256))
        {
            wxCharBuffer buffer(fileSize.GetLo());
            wxCharBuffer newBuffer(256);
            size_t lastRead;

            {
                wxFileInputStream fis(fn.GetFullPath());

                if (!fis.IsOk())
                {
                    wxLogError(_wxS("Cannot open file " ENQUOTED_STR_FMT), fn.GetName());
                    return nullptr;
                }

                lastRead = fis.Read(buffer.data(), buffer.length()).LastRead();
                wxASSERT(lastRead > 0);
            }

            int steps = 256 / lastRead;
            for (int i = 0; i < steps; i++)
            {
                wxTmemcpy(newBuffer.data() + (lastRead * i), buffer.data(), lastRead);
            }

            int rest = 256 % lastRead;
            wxTmemcpy(newBuffer.data() + (lastRead * steps), buffer.data(), rest);

            HRESULT hRes =
                multiLanguage.DetectInputCodepage(MLDETECTCP_NONE,
                        defCodePage,
                        newBuffer, dei,
                        &nSize);

            if (hRes != S_OK)
            {
                wxLogError(_wxS("Cannot determine encoding of file \u201C%s\u201D"), fn.GetName());
                return nullptr;
            }
        }
        else if (fileSize > wxULL(102400))	// > 100k
        {	// read only first 4k
            wxLogWarning(_wxS("File " ENQUOTED_STR_FMT " is really big - trying first 4kb only"), fn.GetName());
            wxCharBuffer buffer(4 * 1024);
            size_t       lastRead;

            {
                wxFileInputStream fis(fn.GetFullPath());

                if (!fis.IsOk())
                {
                    wxLogError(_wxS("Cannot open file " ENQUOTED_STR_FMT), fn.GetName());
                    return nullptr;
                }

                fis.Read(buffer.data(), buffer.length());
                lastRead = fis.LastRead();
                wxASSERT(lastRead > 0);
                buffer.extend(lastRead);
            }

            HRESULT hRes =
                multiLanguage.DetectInputCodepage(MLDETECTCP_NONE,
                        defCodePage,
                        buffer, dei,
                        &nSize);

            if (hRes != S_OK)
            {
                wxLogError(_wxS("Cannot determine encoding of file " ENQUOTED_STR_FMT), fn.GetName());
                return nullptr;
            }
        }
        else
        {
            HRESULT hRes = multiLanguage.DetectCodepageInStream(
                    MLDETECTCP_NONE, defCodePage, fn, dei, &nSize);

            if (hRes != S_OK)
            {
                wxLogError(_wxS("Cannot determine encoding of file " ENQUOTED_STR_FMT), fn.GetName());
                return nullptr;
            }
        }

        if (nSize > 0)
        {
            for (INT i = 0; i < nSize; i++)
            {
                wxLogDebug(wxS("Detected encoding of file " ENQUOTED_STR_FMT " is % d(% d%%)"), fn.GetName(), dei[i].nCodePage, dei[i].nDocPercent);
            }

            pRes = wxMBConv_MLang::Create(dei[0].nCodePage, description);
        }
        else
        {
            wxLogDebug("Cannot detect code page - using default encoding");
            pRes = wxMBConv_MLang::Create(defCodePage, description);
        }
    }
    else
    {
        pRes = GetDefaultEncoding(false, description);
    }

    return pRes;
}

bool wxEncodingDetection::GetBOM(wxUint32 codePage, wxByteBuffer& bom)
{
    bool res = true;

    switch (codePage)
    {
        case CP::UTF32_BE:
        {
            bom = wxByteBuffer(BOM::UTF32_BE, 4);
            break;
        }

        case CP::UTF32_LE:
        {
            bom = wxByteBuffer(BOM::UTF32_LE, 4);
            break;
        }

        case CP::UTF16_BE:
        {
            bom = wxByteBuffer(BOM::UTF16_BE, 2);
            break;
        }

        case CP::UTF16_LE:
        {
            bom = wxByteBuffer(BOM::UTF16_LE, 2);
            break;
        }

        case CP::UTF8:
        {
            bom = wxByteBuffer(BOM::UTF8, 3);
            break;
        }

        default:
        {
            res = false;
            break;
        }
    }

    return res;
}

