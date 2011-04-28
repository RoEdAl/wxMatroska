/*
	wxMultiLanguage.cpp
*/

#include "StdWx.h"
#include "wxMultiLanguage.h"

wxMultiLanguage::wxMultiLanguage(void)
{
	m_pMLang = (IMultiLanguage2*)NULL;

	HRESULT hRes = CoCreateInstance( 
		CLSID_CMultiLanguage,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IMultiLanguage2,
		(LPVOID*)&m_pMLang
	);

	if ( hRes != S_OK )
	{
		m_pMLang = (IMultiLanguage2*)NULL;
	}
}

wxMultiLanguage::wxMultiLanguage( const wxMultiLanguage& ml  )
	:m_pMLang(ml.m_pMLang)
{
	if ( IsValid() )
	{
		m_pMLang->AddRef();
	}
}

wxMultiLanguage::~wxMultiLanguage(void)
{
	Close();
}

bool wxMultiLanguage::IsValid() const
{
	return ( m_pMLang != (IMultiLanguage2*)NULL );
}

void wxMultiLanguage::Close()
{
	if ( IsValid() )
	{
		m_pMLang->Release();
		m_pMLang = (IMultiLanguage2*)NULL;
	}
}

HRESULT wxMultiLanguage::DetectInputCodepage(
	DWORD dwFlag,
	DWORD dwPrefWinCodePage,
	const wxCharBuffer& srcStr,
	DetectEncodingInfo* lpEncoding, INT* pnScores )
{
	wxASSERT( IsValid() );

	INT nSize = (INT)srcStr.length();
	HRESULT hRes = m_pMLang->DetectInputCodepage( 
		dwFlag,
		dwPrefWinCodePage,
		const_cast<CHAR*>(srcStr.data()), &nSize,
		lpEncoding, pnScores );

	return hRes;
}

HRESULT wxMultiLanguage::DetectCodepageInStream(
	DWORD dwFlag,
	DWORD dwPrefWinCodePage,
	const wxFileName& fn,
	DetectEncodingInfo* lpEncoding, INT* pnScores )
{
	wxASSERT( IsValid() );

	IStream* pStream;
	HRESULT hRes = SHCreateStreamOnFile( fn.GetFullPath(), STGM_READ, &pStream );
	if ( hRes != S_OK )
	{
		return hRes;
	}

	hRes = m_pMLang->DetectCodepageInIStream( 
		dwFlag,
		dwPrefWinCodePage,
		pStream,
		lpEncoding, pnScores );

	pStream->Release();
	return hRes;
}

HRESULT wxMultiLanguage::GetCodePageDescription(UINT nCodePage, wxString& sDescription ) const
{
	wxASSERT( IsValid() );

	MIMECPINFO cpinfo;
	LANGID langid = LANG_USER_DEFAULT;
	HRESULT hRes = m_pMLang->GetCodePageInfo( nCodePage, langid, &cpinfo );
	if ( hRes == S_OK )
	{
		sDescription = cpinfo.wszDescription;
	}
	return hRes;
}