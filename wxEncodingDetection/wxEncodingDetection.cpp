/*
	wxEncodingDetection.cpp
*/

#include "StdWx.h"
#include "wxMultiLanguage.h"
#include <wxEncodingDetection.h>

class wxMBConv_MLang : public wxMBConv
{

public:

    wxMBConv_MLang()
		:m_minMBCharWidth(0)
    {
		wxASSERT( m_mlang.IsValid() );
		m_nCodePage = GetACP();
    }

	wxMBConv_MLang( wxUint32 nCodePage )
		:m_nCodePage( nCodePage ),m_minMBCharWidth(0)
    {
		wxASSERT( m_mlang.IsValid() );

		if ( nCodePage == CP_ACP )
		{
			m_nCodePage = GetACP();
		}
    }

	wxMBConv_MLang( const wxMultiLanguage& ml, wxUint32 nCodePage = CP_ACP )
		:m_mlang( ml ), m_nCodePage( nCodePage ), m_minMBCharWidth(0)
    {
		if ( nCodePage == CP_ACP )
		{
			m_nCodePage = GetACP();
		}
    }

    wxMBConv_MLang(const wxMBConv_MLang& conv)
		:wxMBConv(),
		 m_mlang( conv.m_mlang ),
		 m_nCodePage( conv.m_nCodePage ),
		 m_minMBCharWidth( conv.m_minMBCharWidth )
    {
    }

    virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const
    {
		wxASSERT( m_mlang.IsValid() );

		DWORD dwMode = 0;
		UINT nSrcSize = -1;
		UINT nDstSize = n;

		HRESULT hRes = m_mlang->ConvertStringToUnicodeEx(
			&dwMode,
			m_nCodePage, 
			const_cast<CHAR*>( psz ), &nSrcSize,
			buf, &nDstSize,
			0, NULL );

		if ( hRes == S_OK )
		{
			return nDstSize;
		}
		else
		{
			return wxCONV_FAILED;
		}
    }

    virtual size_t WC2MB(char *buf, const wchar_t *pwz, size_t n) const
    {
		wxASSERT( m_mlang.IsValid() );

		DWORD dwMode = 0;
		UINT nSrcSize = -1;
		UINT nDstSize = n;

		HRESULT hRes = m_mlang->ConvertStringFromUnicodeEx( 
			&dwMode,
			m_nCodePage,
			const_cast<WCHAR*>(pwz),
			&nSrcSize,
			buf,
			&nDstSize,
			0,
			NULL );

		if ( hRes == S_OK )
		{
			return nDstSize;
		}
		else
		{
			return wxCONV_FAILED;
		}
    }

    virtual size_t GetMBNulLen() const
    {
		wxASSERT( m_mlang.IsValid() );

		if ( m_minMBCharWidth == 0 )
		{
			DWORD dwMode = 0;
			UINT nSrcSize = 1;
			UINT nDstSize = 0;

			HRESULT hRes = m_mlang->ConvertStringFromUnicodeEx( 
				&dwMode,
				m_nCodePage,
				L"", &nSrcSize,
				NULL, &nDstSize,
				0,
				NULL );

			wxMBConv_MLang* const self = wxConstCast(this, wxMBConv_MLang);

			if ( hRes == S_OK )
			{
				switch ( nDstSize )
				{
					default:
						wxLogDebug(wxT("Unexpected NUL length %d"), nDstSize);
						self->m_minMBCharWidth = (size_t)-1;
						break;

					case 0:
						self->m_minMBCharWidth = (size_t)-1;
						break;

					case 1:
					case 2:
					case 4:
						self->m_minMBCharWidth = nDstSize;
						break;
				}
			}
			else
			{
				self->m_minMBCharWidth = (size_t)-1;
			}
		}

		return m_minMBCharWidth;
    }

    virtual wxMBConv *Clone() const { return new wxMBConv_MLang(*this); }

	bool IsOk() const { return m_mlang.IsValid() && (m_nCodePage != CP_ACP); }

protected:

	wxMultiLanguage m_mlang;

    // the code page we're working with
    wxUint32 m_nCodePage;

    // cached result of GetMBNulLen(), set to 0 initially meaning
    // "unknown"
    size_t m_minMBCharWidth;
};


wxMBConv* wxEncodingDetection::GetFileEncoding( const wxFileName& fn )
{
	wxMultiLanguage multiLanguage;

	if ( !multiLanguage.IsValid() )
	{
		wxLogError( wxT("Cannot create MultiLanguage COM object.") );
		return (wxMBConv*)NULL;
	}

	wxULongLong nFileSize = fn.GetSize();
	if ( nFileSize == wxInvalidSize )
	{
		wxLogError( wxT("Cannot determine size of file %s."), fn.GetName() );
		return (wxMBConv*)NULL;
	}

	if ( nFileSize == wxULL(0) )
	{
		wxLogError( wxT("Cannot determine encoding of empty file %s."), fn.GetName() );
		return (wxMBConv*)NULL;
	}

	DetectEncodingInfo dei[10];
	INT nSize = 10;

	if ( nFileSize < wxULL(256) )
	{
		wxCharBuffer buffer( nFileSize.GetLo() );
		wxCharBuffer newBuffer( 256 );
		size_t nLastRead;

		{
			wxFileInputStream fis( fn.GetFullPath() );
			if ( !fis.IsOk() )
			{
				wxLogError( _("Cannot open file %s."), fn.GetName() );
				return (wxMBConv*)NULL;
			}

			fis.Read( buffer.data(), buffer.length() );
			nLastRead = fis.LastRead();
			wxASSERT( nLastRead > 0 );
		}

		int steps = 256 / nLastRead;
		for( int i=0; i<steps; i++ )
		{
			wxTmemcpy( newBuffer.data() + (nLastRead * i), buffer.data(), nLastRead );
		}

		int rest = 256 % nLastRead;
		wxTmemcpy( newBuffer.data() + (nLastRead * steps), buffer.data(), rest );

		HRESULT hRes = multiLanguage.DetectInputCodepage( 0, 0, newBuffer, dei, &nSize );
		if ( hRes != S_OK )
		{
			wxLogError( _("Cannot determine encoding of file %s."), fn.GetName() );
			return (wxMBConv*)NULL;
		}
	}
	else if ( nFileSize > wxULL(102400) ) // > 100k
	{ // read only first 4k
		wxLogWarning( _("File %s is really big - trying first 4kb only."), fn.GetName() );
		wxCharBuffer buffer( 4 * 1024 );
		size_t nLastRead;

		{
			wxFileInputStream fis( fn.GetFullPath() );
			if ( !fis.IsOk() )
			{
				wxLogError( _("Cannot open file %s."), fn.GetName() );
				return (wxMBConv*)NULL;
			}

			fis.Read( buffer.data(), buffer.length() );
			nLastRead = fis.LastRead();
			wxASSERT( nLastRead > 0 );
			buffer.extend( nLastRead );
		}

		HRESULT hRes = multiLanguage.DetectInputCodepage( 0, 0, buffer, dei, &nSize );
		if ( hRes != S_OK )
		{
			wxLogError( _("Cannot determine encoding of file %s."), fn.GetName() );
			return (wxMBConv*)NULL;
		}
	}
	else
	{
		HRESULT hRes = multiLanguage.DetectCodepageInStream( MLDETECTCP_NONE, 0, fn, dei, &nSize );
		if ( hRes != S_OK )
		{
			wxLogError( _("Cannot determine encoding of file %s."), fn.GetName() );
			return (wxMBConv*)NULL;
		}
	}

	wxASSERT( nSize > 0 );
	wxLogInfo( _T("Detected encoding of file %s is %d (%d%%)."), fn.GetName(), dei[0].nCodePage, dei[0].nDocPercent );
	wxMBConv* pConv = new wxMBConv_MLang( multiLanguage, dei[0].nCodePage );
	return pConv;
}

void wxEncodingDetection::Test()
{
	wxString sPath( wxT("G:/Documents/Music/Daniel Mueller-Schott - Jonathan Gilad - Felix Mendelssohn Bartholdy.wv") );
	wxSharedPtr<wxMBConv> pConv( wxEncodingDetection::GetFileEncoding( sPath ) );
}
