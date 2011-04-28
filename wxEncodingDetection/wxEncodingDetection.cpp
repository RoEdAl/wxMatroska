/*
	wxEncodingDetection.cpp
*/

#include "StdWx.h"
#include "wxMultiLanguage.h"
#include <wxEncodingDetection.h>

// ===========================================================

const wxByte wxEncodingDetection::BOM_UTF32_BE[4] = { 0x00, 0x00, 0xFE, 0xEF };
const wxByte wxEncodingDetection::BOM_UTF32_LE[4] = { 0xFF, 0xFE, 0x00, 0x00 };
const wxByte wxEncodingDetection::BOM_UTF16_BE[2] = { 0xFE, 0xFF };
const wxByte wxEncodingDetection::BOM_UTF16_LE[2] =	{ 0xFF, 0xFE };
const wxByte wxEncodingDetection::BOM_UTF8[3] =		{ 0xEF, 0xBB, 0xBF };

// ===========================================================

class wxMBConv_MLang : public wxMBConv
{

public:

	static wxMBConv_MLang* Create( const wxMultiLanguage& ml, wxUint32 nCodePage, wxString& sDescription )
	{
		wxMBConv_MLang* pConvMLang = new wxMBConv_MLang( ml, nCodePage );
		sDescription = pConvMLang->GetDescription();
		return pConvMLang;
	}

	static wxMBConv_MLang* Create( wxUint32 nCodePage, wxString& sDescription )
	{
		wxMBConv_MLang* pConvMLang = new wxMBConv_MLang( nCodePage );
		sDescription = pConvMLang->GetDescription();
		return pConvMLang;
	}

protected:

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
		:m_mlang( conv.m_mlang ),
		 m_nCodePage( conv.m_nCodePage ),
		 m_minMBCharWidth( conv.m_minMBCharWidth )
    {
    }

public:

	virtual size_t ToWChar(wchar_t *dst, size_t dstLen, const char *src, size_t srcLen = wxNO_LEN) const
	{
		wxASSERT( m_mlang.IsValid() );

		DWORD dwMode = 0;
		UINT nSrcSize = srcLen;
		UINT nDstSize = dstLen;

		HRESULT hRes = m_mlang->ConvertStringToUnicodeEx(
			&dwMode,
			m_nCodePage, 
			const_cast<CHAR*>( src ), &nSrcSize,
			dst, &nDstSize,
			0, NULL );

		if ( hRes == S_OK )
		{
			if ( nDstSize > 0 )
			{
				return nDstSize;
			}
			else
			{
				return wxCONV_FAILED;
			}
		}
		else
		{
			return wxCONV_FAILED;
		}
	}

    virtual size_t FromWChar(char *dst, size_t dstLen, const wchar_t *src, size_t srcLen = wxNO_LEN) const
	{
		wxASSERT( m_mlang.IsValid() );

		DWORD dwMode = 0;
		UINT nSrcSize = srcLen;
		UINT nDstSize = dstLen;

		HRESULT hRes = m_mlang->ConvertStringFromUnicodeEx( 
			&dwMode,
			m_nCodePage,
			const_cast<WCHAR*>(src),
			&nSrcSize,
			dst,
			&nDstSize,
			0,
			NULL );

		if ( hRes == S_OK )
		{
			if ( nDstSize > 0 )
			{
				return nDstSize;
			}
			else
			{
				return wxCONV_FAILED;
			}
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

	wxString GetDescription() const
	{
		wxString sDescription;
		wxString sCPDescription;
		HRESULT hRes = m_mlang.GetCodePageDescription( m_nCodePage, sCPDescription );
		if ( hRes == S_OK )
		{
			sDescription.Printf( wxT("%s [CP:%d]"), sCPDescription, m_nCodePage );
		}
		else
		{
			sDescription.Printf( wxT("<ERR:%08x> [CP:%d]"), hRes, m_nCodePage );
		}
		return sDescription;
	}

protected:

	wxMultiLanguage m_mlang;

    // the code page we're working with
    wxUint32 m_nCodePage;

    // cached result of GetMBNulLen(), set to 0 initially meaning
    // "unknown"
    size_t m_minMBCharWidth;
};

class wxMBConv_BOM : public wxMBConv
{
public:

	typedef wxCharTypeBuffer<wxByte> wxByteBuffer;

	static wxMBConv_BOM* Create( const wxByte* bom, size_t nLen, wxUint32 nCodePage, wxString& sDescription )
	{
		wxMBConv_MLang* pConvMLang = wxMBConv_MLang::Create( nCodePage, sDescription );
		return new wxMBConv_BOM( bom, nLen, pConvMLang, true );
	}

protected:

	wxMBConv_BOM( const wxByte* bom, size_t nLen, wxMBConv* pConv, bool bConvOwner = false )
		:m_pConv( pConv ), m_bConvOwner( bConvOwner )
	{
		wxASSERT( pConv != (wxMBConv*)NULL );
		wxASSERT( nLen >= 2 );

		wxByteBuffer bom_buf( bom, nLen );
		m_bom = bom_buf;

		m_bBOMConsumed = false;
	}

	wxMBConv_BOM( const wxMBConv_BOM& convBom )
		:m_bom( convBom.m_bom )
	{
		m_pConv = convBom.m_pConv->Clone();
		m_bConvOwner = true;

		m_bBOMConsumed = false;
	}

public:

	~wxMBConv_BOM()
	{
		if ( m_bConvOwner )
		{
			delete m_pConv;
		}
	}

	virtual wxMBConv *Clone() const { return new wxMBConv_BOM(*this); }

	virtual size_t ToWChar(wchar_t *dst, size_t dstLen, const char *src, size_t srcLen) const
	{
		wxMBConv_BOM *self = const_cast<wxMBConv_BOM*>(this);

		if ( !m_bBOMConsumed )
		{
			if ( !SkipBOM(src, srcLen) )
			{
				return wxCONV_FAILED;
			}
			else if ( srcLen == 0 )
			{
				return wxCONV_FAILED;
			}
		}

		size_t rc = m_pConv->ToWChar(dst, dstLen, src, srcLen);

		// don't skip the BOM again the next time if we really consumed it
		if ( rc != wxCONV_FAILED && dst && !m_bBOMConsumed )
		{
			self->m_bBOMConsumed = true;
		}

		return rc;
	}

	virtual size_t FromWChar(char *dst, size_t dstLen, const wchar_t *src, size_t srcLen) const
	{
		return m_pConv->FromWChar(dst, dstLen, src, srcLen);
	}

	virtual size_t GetMBNulLen() const
	{
		return m_pConv->GetMBNulLen();
	}

protected:

	bool SkipBOM( const char*& src, size_t& len ) const
	{
		size_t realLen = (len != (size_t)-1)? len : wxStrnlen( src, 16 );
		if ( realLen < m_bom.length() )
		{ // still waiting
			return false;
		}

		if ( !check_bom( src ) )
		{
			return false;
		}

		src += m_bom.length();
		if ( len != (size_t)-1 )
		{
			len -= m_bom.length();
		}
		return true;
	}

	bool check_bom( const char* src ) const
	{
		bool bRes = true;
		for( size_t i = 0; bRes && (i < m_bom.length()); i++ )
		{
			if ( m_bom[i] != (wxByte)( src[i] ) )
			{
				bRes = false;
			}
		}
		return bRes;
	}

protected:

	bool m_bConvOwner;
	wxMBConv* m_pConv;
	wxByteBuffer m_bom;

	bool m_bBOMConsumed;
};

wxMBConv* const wxEncodingDetection::wxNullMBConv = (wxMBConv* const)NULL;

bool wxEncodingDetection::test_bom( const wxByteBuffer& buffer, const wxByte* bom, size_t nLen )
{
	wxASSERT( nLen >= 2 );
	bool bRes = true;
	for( size_t i=0; bRes && (i < nLen); i++ )
	{
		if ( buffer[i] != bom[i] ) bRes = false;
	}
	return bRes;
}

wxMBConv* wxEncodingDetection::GetFileEncodingFromBOM( const wxFileName& fn, wxString& sDescription )
{
	wxByteBuffer buffer( 4 );
	size_t nLastRead = 0;

	{
		wxFileInputStream fis( fn.GetFullPath() );
		if ( !fis.IsOk() )
		{
			wxLogError( _("Cannot open file \u201C%s\u201D."), fn.GetName() );
			return wxNullMBConv;
		}
		nLastRead = fis.Read( buffer.data(), buffer.length() ).LastRead();
	}

	switch( nLastRead )
	{
		default: // 4 and more
		if ( test_bom( buffer, BOM_UTF32_BE, 4 ) )
		{
			return wxMBConv_BOM::Create( BOM_UTF32_BE, 4, 12001, sDescription );
		}
		else if ( test_bom( buffer, BOM_UTF32_LE, 4 ) )
		{
			return wxMBConv_BOM::Create( BOM_UTF32_LE, 4, 12000, sDescription );
		}

		case 3: // UTF8
		if ( test_bom( buffer, BOM_UTF8, 3 ) )
		{
			return wxMBConv_BOM::Create( BOM_UTF8, 3, 65001, sDescription );
		}

		case 2: // UTF16
		if ( test_bom( buffer, BOM_UTF16_BE, 2 ) )
		{
			return wxMBConv_BOM::Create( BOM_UTF16_BE, 2, 1201, sDescription );
		}
		else if ( test_bom( buffer, BOM_UTF16_LE, 2 ) )
		{
			return wxMBConv_BOM::Create( BOM_UTF16_LE, 2, 1200, sDescription );
		}

		case 0:
		case 1:
		wxLogError( _("Cannot read BOM - file %s is too small."), fn.GetName() );
		break;
	}

	return wxNullMBConv;
}

wxMBConv* wxEncodingDetection::GetFileEncoding( const wxFileName& fn, wxString& sDescription )
{
	wxULongLong nFileSize = fn.GetSize();
	if ( nFileSize == wxInvalidSize )
	{
		wxLogError( _("Cannot determine size of file \u201C%s\u201D."), fn.GetName() );
		return wxNullMBConv;
	}

	if ( nFileSize == wxULL(0) )
	{
		wxLogError( _("Cannot determine encoding of empty file \u201C%s\u201D."), fn.GetName() );
		return wxNullMBConv;
	}

	wxMBConv* pRes = GetFileEncodingFromBOM( fn, sDescription );
	if ( pRes != (wxMBConv*)NULL )
	{
		return pRes;
	}

	wxMultiLanguage multiLanguage;

	if ( !multiLanguage.IsValid() )
	{
		wxLogError( _("Cannot create MultiLanguage COM object.") );
		return wxNullMBConv;
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
				wxLogError( _("Cannot open file \u201C%s\u201D."), fn.GetName() );
				return wxNullMBConv;
			}

			nLastRead = fis.Read( buffer.data(), buffer.length() ).LastRead();
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
			wxLogError( _("Cannot determine encoding of file \u201C%s\u201D."), fn.GetName() );
			return wxNullMBConv;
		}
	}
	else if ( nFileSize > wxULL(102400) ) // > 100k
	{ // read only first 4k
		wxLogWarning( _("File \u201C%s\u201D is really big - trying first 4kb only."), fn.GetName() );
		wxCharBuffer buffer( 4 * 1024 );
		size_t nLastRead;

		{
			wxFileInputStream fis( fn.GetFullPath() );
			if ( !fis.IsOk() )
			{
				wxLogError( _("Cannot open file \u201C%s\u201D."), fn.GetName() );
				return wxNullMBConv;
			}

			fis.Read( buffer.data(), buffer.length() );
			nLastRead = fis.LastRead();
			wxASSERT( nLastRead > 0 );
			buffer.extend( nLastRead );
		}

		HRESULT hRes = multiLanguage.DetectInputCodepage( 0, 0, buffer, dei, &nSize );
		if ( hRes != S_OK )
		{
			wxLogError( _("Cannot determine encoding of file \u201C%s\u201D."), fn.GetName() );
			return wxNullMBConv;
		}
	}
	else
	{
		HRESULT hRes = multiLanguage.DetectCodepageInStream( MLDETECTCP_NONE, 0, fn, dei, &nSize );
		if ( hRes != S_OK )
		{
			wxLogError( _("Cannot determine encoding of file \u201C%s\u201D."), fn.GetName() );
			return wxNullMBConv;
		}
	}

	wxASSERT( nSize > 0 );
	for( INT i=0; i< nSize; i++ )
	{
		wxLogDebug( _("Detected encoding of file \u201C%s\u201D is %d (%d%%)."), fn.GetName(), dei[i].nCodePage, dei[i].nDocPercent );
	}

	pRes = wxMBConv_MLang::Create( multiLanguage, dei[0].nCodePage, sDescription );
	return pRes;
}

