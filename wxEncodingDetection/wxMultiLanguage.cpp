/*
 * wxMultiLanguage.cpp
 */

#include "wxMultiLanguage.h"

 // ===============================================================================

wxMultiLanguage::wxMultiLanguage(void)
{
    m_pMLang = nullptr;

    HRESULT hRes = CoCreateInstance(
            CLSID_CMultiLanguage,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IMultiLanguage2,
            (LPVOID*)&m_pMLang
    );

    if (hRes != S_OK)
    {
        wxLogError(_("Fail to get CMultiLanguage object; error 0x%08x"), hRes);
        m_pMLang = nullptr;
    }
}

wxMultiLanguage::wxMultiLanguage(const wxMultiLanguage& ml):
    m_pMLang(ml.m_pMLang)
{
    if (IsValid()) m_pMLang->AddRef();
}

wxMultiLanguage::~wxMultiLanguage(void)
{
    Close();
}

bool wxMultiLanguage::IsValid() const
{
    return (m_pMLang != nullptr);
}

void wxMultiLanguage::Close()
{
    if (IsValid())
    {
        m_pMLang->Release();
        m_pMLang = nullptr;
    }
}

HRESULT wxMultiLanguage::DetectInputCodepage(
        DWORD dwFlag,
        DWORD dwPrefWinCodePage,
        const wxCharBuffer& srcStr,
        DetectEncodingInfo* lpEncoding, INT* pnScores)
{
    wxASSERT(IsValid());

    INT     nSize = (INT)srcStr.length();
    HRESULT hRes = m_pMLang->DetectInputCodepage(
            dwFlag,
            dwPrefWinCodePage,
            const_cast<CHAR*>(srcStr.data()), &nSize,
            lpEncoding, pnScores);

    return hRes;
}

HRESULT wxMultiLanguage::DetectCodepageInStream(
        DWORD dwFlag,
        DWORD dwPrefWinCodePage,
        const wxFileName& fn,
        DetectEncodingInfo* lpEncoding, INT* pnScores)
{
    wxASSERT(IsValid());

    IStream* pStream;
    HRESULT  hRes = SHCreateStreamOnFile(fn.GetFullPath().t_str(), STGM_READ, &pStream);

    if (hRes != S_OK) return hRes;

    hRes = m_pMLang->DetectCodepageInIStream(
            dwFlag,
            dwPrefWinCodePage,
            pStream,
            lpEncoding, pnScores);

    pStream->Release();
    return hRes;
}

HRESULT wxMultiLanguage::GetCodePageDescription(UINT codePage, wxString& description) const
{
    wxASSERT(IsValid());

    MIMECPINFO cpinfo;
    LANGID     langid = LANG_USER_DEFAULT;
    HRESULT    hRes = m_pMLang->GetCodePageInfo(codePage, langid, &cpinfo);

    if (hRes == S_OK) description = cpinfo.wszDescription;

    return hRes;
}

bool wxMultiLanguage::GetDefaultCodePage(wxUint32& codePage)
{
    CPINFOEX cpInfo;

    if (GetCPInfoEx(CP_THREAD_ACP, 0, &cpInfo))
    {
        codePage = cpInfo.CodePage;
        return true;
    }
    else
    {
        return false;
    }
}
