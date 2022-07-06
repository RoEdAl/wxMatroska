/*
 * wxMultiLanguage.h
 */

#ifndef _WX_MULTI_LANGUAGE_H_
#define _WX_MULTI_LANGUAGE_H_

class wxMultiLanguage
{
    protected:

    IMultiLanguage2* m_pMLang;

    public:

    wxMultiLanguage(void);
    wxMultiLanguage(const wxMultiLanguage&);
    ~wxMultiLanguage(void);

    bool IsValid() const;
    void Close();

    IMultiLanguage2* operator ->() const
    {
        return m_pMLang;
    }

    public:

    HRESULT DetectInputCodepage(DWORD, DWORD, const wxCharBuffer&, DetectEncodingInfo*, INT*);
    HRESULT DetectCodepageInStream(DWORD, DWORD, const wxFileName&, DetectEncodingInfo*, INT*);
    HRESULT GetCodePageDescription(UINT, wxString&) const;

    public:

    static bool GetDefaultCodePage(wxUint32&);
};

#endif

