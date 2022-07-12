/*
 * wxMLangConvertCharset.cpp
 */

#include "wxMultiLanguage.h"
#include "wxMLangConvertCharset.h"

 // ===============================================================================

wxUint32 wxMLangConvertCharset::GetRealCodePage(wxUint32 nCodePage)
{
    switch (nCodePage)
    {
        case CP_ACP:
        case CP_THREAD_ACP:
        {
            return GetACP();
        }

        case CP_OEMCP:
        {
            return GetOEMCP();
        }

        default:
        {
            return nCodePage;
        }
    }
}

wxMLangConvertCharset::wxMLangConvertCharset(void):
    m_pMLang(nullptr)
{
}

wxMLangConvertCharset::wxMLangConvertCharset(wxUint32 codepageFrom, wxUint32 codepageTo):
    m_pMLang(nullptr)
{
    Initialize(codepageFrom, codepageTo);
}

wxMLangConvertCharset::wxMLangConvertCharset(const wxMultiLanguage& mlang, wxUint32 codepageFrom, wxUint32 codepageTo):
    m_pMLang(nullptr)
{
    HRESULT hRes = mlang->CreateConvertCharset(GetRealCodePage(codepageFrom), GetRealCodePage(codepageTo), 0, &m_pMLang);

    if (hRes != S_OK)
    {
        wxLogWarning(_("Unable to create converter from codepage %u to codepage %u; error: 0x%08x"), codepageFrom, codepageTo, hRes);
        m_pMLang = nullptr;
    }
}

wxMLangConvertCharset::wxMLangConvertCharset(const wxMLangConvertCharset& ml):
    m_pMLang(ml.m_pMLang)
{
    if (IsValid()) m_pMLang->AddRef();
}

wxMLangConvertCharset::~wxMLangConvertCharset(void)
{
    Close();
}

bool wxMLangConvertCharset::Initialize(wxUint32 codepageFrom, wxUint32 codepageTo)
{
    wxASSERT(!IsValid());

    HRESULT hRes = CoCreateInstance(
            CLSID_CMLangConvertCharset,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IMLangConvertCharset,
            (LPVOID*)&m_pMLang
    );

    if (hRes != S_OK)
    {
        m_pMLang = nullptr;
        wxLogError(_("Fail to get CMLangConvertCharset object; error 0x%08x"), hRes);
        return false;
    }

    hRes = m_pMLang->Initialize(GetRealCodePage(codepageFrom), GetRealCodePage(codepageTo), 0);

    if (hRes != S_OK)
    {
        wxLogError(_("Fail to initialize CMLangConvertCharset object witch codepages %u and %u; error 0x%08x"), codepageFrom, codepageTo, hRes);
        m_pMLang->Release();
        m_pMLang = nullptr;
        return false;
    }

    return true;
}

bool wxMLangConvertCharset::IsValid() const
{
    return (m_pMLang != nullptr);
}

void wxMLangConvertCharset::Close()
{
    if (IsValid())
    {
        m_pMLang->Release();
        m_pMLang = nullptr;
    }
}

#ifdef __MINGW32__
EXTERN_C const CLSID CLSID_CMLangConvertCharset = {0xd66d6f99, 0xcdaa, 0x11d0, {0xb8, 0x22, 0x00, 0xc0, 0x4f, 0xc9, 0xb3, 0x1f}};
EXTERN_C const IID IID_IMLangConvertCharset =     {0xd66d6f98, 0xcdaa, 0x11d0, {0xb8, 0x22, 0x00, 0xc0, 0x4f, 0xc9, 0xb3, 0x1f}};
#endif

