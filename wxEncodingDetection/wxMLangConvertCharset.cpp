/*
 * wxMLangConvertCharset.cpp
 */

#include "StdWx.h"
#include "wxMultiLanguage.h"
#include "wxMLangConvertCharset.h"

// ===============================================================================

wxUint32 wxMLangConvertCharset::GetRealCodePage( wxUint32 nCodePage )
{
	switch ( nCodePage )
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
		return nCodePage;
	}
}

wxMLangConvertCharset::wxMLangConvertCharset( void ):
	m_pMLang( ( IMLangConvertCharset* )NULL )
{}

wxMLangConvertCharset::wxMLangConvertCharset( wxUint32 nCodepageFrom, wxUint32 nCodepageTo ):
	m_pMLang( ( IMLangConvertCharset* )NULL )
{
	Initialize( nCodepageFrom, nCodepageTo );
}

wxMLangConvertCharset::wxMLangConvertCharset( const wxMultiLanguage& mlang, wxUint32 nCodepageFrom, wxUint32 nCodepageTo ):
	m_pMLang( ( IMLangConvertCharset* )NULL )
{
	HRESULT hRes = mlang->CreateConvertCharset( GetRealCodePage( nCodepageFrom ), GetRealCodePage( nCodepageTo ), 0, &m_pMLang );

	if ( hRes != S_OK )
	{
		wxLogWarning( _( "Unable to create converter from codepage %u to codepage %u; error: 0x%08x" ), nCodepageFrom, nCodepageTo, hRes );
		m_pMLang = (IMLangConvertCharset*)NULL;
	}
}

wxMLangConvertCharset::wxMLangConvertCharset( const wxMLangConvertCharset& ml ):
	m_pMLang( ml.m_pMLang )
{
	if ( IsValid() )
	{
		m_pMLang->AddRef();
	}
}

wxMLangConvertCharset::~wxMLangConvertCharset( void )
{
	Close();
}

bool wxMLangConvertCharset::Initialize( wxUint32 nCodepageFrom, wxUint32 nCodepageTo )
{
	wxASSERT( !IsValid() );

	HRESULT hRes = CoCreateInstance(
			CLSID_CMLangConvertCharset,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IMLangConvertCharset,
			(LPVOID*)&m_pMLang
			);

	if ( hRes != S_OK )
	{
		m_pMLang = (IMLangConvertCharset*)NULL;
		wxLogError( _( "Fail to get CMLangConvertCharset object; error 0x%08x" ), hRes );
		return false;
	}

	hRes = m_pMLang->Initialize( GetRealCodePage( nCodepageFrom ), GetRealCodePage( nCodepageTo ), 0 );

	if ( hRes != S_OK )
	{
		wxLogError( _( "Fail to initialize CMLangConvertCharset object witch codepages %u and %u; error 0x%08x" ), nCodepageFrom, nCodepageTo, hRes );
		m_pMLang->Release();
		m_pMLang = (IMLangConvertCharset*)NULL;
		return false;
	}

	return true;
}

bool wxMLangConvertCharset::IsValid() const
{
	return ( m_pMLang != (IMLangConvertCharset*)NULL );
}

void wxMLangConvertCharset::Close()
{
	if ( IsValid() )
	{
		m_pMLang->Release();
		m_pMLang = (IMLangConvertCharset*)NULL;
	}
}

