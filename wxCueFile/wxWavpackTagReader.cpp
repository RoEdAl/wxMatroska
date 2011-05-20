/*
   wxWavpackTagReader.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxCueComponent.h>
#include "wxWavpackStream.h"
#include "wxWavpackTagReader.h"

// ==================================================

WavpackContext* const wxWavpackTagReader::wxWavpackContext::Null = (WavpackContext* const)NULL;

int wxWavpackTagReader::wxWavpackContext::GetNumTagItems()
{
	wxASSERT( this->operator bool() );
	return WavpackGetNumTagItems( m_pContext );
}

size_t wxWavpackTagReader::wxWavpackContext::GetTagItemIndexed(
	int nItem,
	wxWritableCharBuffer& itemBuffer,
	wxWritableCharBuffer& valueBuffer,
	wxString& sItem,
	wxString& sValue
	)
{
	wxASSERT( this->operator bool() );

	size_t nSize = WavpackGetTagItemIndexed( m_pContext, nItem, NULL, 0 );
	if ( nSize == 0 )
	{
		return nSize;
	}

	if ( ( nSize + 1 ) > itemBuffer.length() )
	{
		itemBuffer.extend( nSize + 1 );
	}

	size_t nSize2 = WavpackGetTagItemIndexed( m_pContext, nItem, itemBuffer, itemBuffer.length() );
	wxASSERT( nSize == nSize2 );

	sItem = wxString::FromUTF8( itemBuffer, nSize2 );

	nSize = WavpackGetTagItem( m_pContext, itemBuffer, NULL, 0 );
	wxASSERT( nSize > 0 );
	if ( ( nSize + 1 ) > valueBuffer.length() )
	{
		valueBuffer.extend( nSize + 1 );
	}

	nSize2 = WavpackGetTagItem( m_pContext, itemBuffer, valueBuffer, valueBuffer.length() );
	wxASSERT( nSize == nSize2 );

	sValue = wxString::FromUTF8( valueBuffer, nSize2 );

	return nSize2;
}

size_t wxWavpackTagReader::wxWavpackContext::GetTagItem( const wxString& sItem, wxWritableCharBuffer& valueBuffer )
{
	wxASSERT( this->operator bool() );

	size_t nSize = WavpackGetTagItem( m_pContext, sItem.ToAscii(), NULL, 0 );
	size_t nSize2;

	if ( nSize > 0 )
	{
		if ( ( nSize + 1 ) > valueBuffer.length() )
		{
			valueBuffer.extend( nSize + 1 );
		}

		nSize2 = WavpackGetTagItem( m_pContext, sItem.ToAscii(), valueBuffer, valueBuffer.length() );
		wxASSERT( nSize == nSize2 );

		return nSize2;
	}
	else
	{
		return nSize;
	}
}

// ==================================================

wxWavpackTagReader::wxWavpackTagReader( void )
{}

bool wxWavpackTagReader::ReadCueSheetTag( const wxString& sWavpackFile, wxString& sCueSheet )
{
	wxWritableCharBuffer valueBuffer;
	size_t				 nSize = 0;

	{
		wxFileInputStream is( sWavpackFile );
		if ( !is.IsOk() )
		{
			wxLogError( _( "Fail to open Wavpack file \u201C%s\u201D." ), sWavpackFile );
			return false;
		}

		wxNullOutputStream os;

		wxWavpackStream		 wavpackStream( is, os );
		wxWritableCharBuffer errBuffer;
		errBuffer.extend( 80 );
		wxWavpackContext ctx(
			WavpackOpenFileInputEx(
				wavpackStream.GetStream(),
				&wavpackStream, NULL,
				errBuffer,
				OPEN_TAGS,
				0
				)
			);

		if ( !ctx )
		{
			wxString sErr( wxString::FromUTF8( errBuffer ) );
			wxLogError( _( "Fail to open (using WavpackOpenFileInputEx) Wavpack file \u201C%s\u201D. Error message: %s" ), sWavpackFile, sErr );
			return false;
		}

		nSize = ctx.GetTagItem( wxCueTag::Name::CUESHEET, valueBuffer );
	} // Wavpack file closed here

	if ( nSize > 0 )
	{
		wxStringTokenizer valueTokenizer(
			wxString::FromUTF8( valueBuffer, nSize ),
			wxT( '\0' ),
			wxTOKEN_DEFAULT );
		if ( valueTokenizer.HasMoreTokens() )
		{
			sCueSheet = valueTokenizer.NextToken();
			return true;
		}
		else
		{
			wxLogWarning( _( "Invalid CUESHEET tag in Wavpack file \u201C%s\u201D" ), sWavpackFile );
			return false;
		}
	}
	else
	{
		wxLogWarning( _( "There's no CUESHEET tag in Wavpack file \u201C%s\u201D" ), sWavpackFile );
		return false;
	}
}

bool wxWavpackTagReader::ReadTags( const wxString& sWavpackFile, wxArrayCueTag& comments )
{
	wxFileInputStream is( sWavpackFile );

	if ( !is.IsOk() )
	{
		wxLogError( _( "Fail to open Wavpack file \u201C%s\u201D." ), sWavpackFile );
		return false;
	}

	wxNullOutputStream os;

	wxWavpackStream		 wavpackStream( is, os );
	wxWritableCharBuffer errBuffer;
	errBuffer.extend( 80 );
	wxWavpackContext ctx( WavpackOpenFileInputEx(
							  wavpackStream.GetStream(),
							  &wavpackStream, NULL,
							  errBuffer,
							  OPEN_TAGS,
							  0
							  )
						  );

	if ( !ctx )
	{
		wxString sErr( wxString::FromUTF8( errBuffer ) );
		wxLogError( _( "Fail to open (using WavpackOpenFileInputEx) Wavpack file \u201C%s\u201D. Error message: \u201C%s\u201D" ),
			sWavpackFile, sErr );
		return false;
	}

	int nItems = ctx.GetNumTagItems();

	wxWritableCharBuffer itemBuffer;
	wxWritableCharBuffer valueBuffer;
	wxString			 sItem;
	wxString			 sValue;

	for ( int i = 0; i < nItems; i++ )
	{
		size_t nSize = ctx.GetTagItemIndexed( i, itemBuffer, valueBuffer, sItem, sValue );
		wxASSERT( nSize > 0 );

		wxStringTokenizer valueTokenizer( sValue, wxT( '\0' ), wxTOKEN_DEFAULT );

		while ( valueTokenizer.HasMoreTokens() )
		{
			wxCueTag comment( wxCueTag::TAG_MEDIA_METADATA, sItem, valueTokenizer.GetNextToken() );
			comments.Add( comment );
		}
	}

	return true;
}

