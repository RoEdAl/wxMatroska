/*
	wxWavpackTagReader.cpp
*/

#include "StdWx.h"
#include <wxCueComponent.h>
#include "wxWavpackStream.h"
#include "wxWavpackTagReader.h"

wxWavpackTagReader::wxWavpackTagReader(void)
{
}

wxWavpackTagReader::~wxWavpackTagReader(void)
{
}

bool wxWavpackTagReader::ReadCueSheetTag( const wxString& sWavpackFile, wxString& sCueSheet )
{
	wxFileInputStream is( sWavpackFile );
	if ( !is.IsOk() )
	{
		return false;
	}

	wxNullOutputStream os;

	wxWavpackStream wavpackStream( is, os );
	wxWritableCharBuffer errBuffer;
	errBuffer.extend( 80 );
	WavpackContext* pWvCtx = WavpackOpenFileInputEx(
		wavpackStream.GetStream(),
		&wavpackStream, NULL,
		errBuffer,
		OPEN_TAGS,
		0
	);

	if ( pWvCtx == (WavpackContext*)NULL )
	{
		wxString sErr( wxString::FromUTF8( errBuffer ) );
		wxLogError( wxT("Fail to open Wavpack file %s. Error message: %s"), sWavpackFile, sErr );
		return false;
	}

	wxWritableCharBuffer valueBuffer;
	size_t nSize = WavpackGetTagItem( pWvCtx, "CUESHEET", NULL, 0 );
	size_t nSize2;
	
	if ( nSize > 0 )
	{
		if ( (nSize+1) > valueBuffer.length() )
		{
			valueBuffer.extend( nSize+1 );
		}

		nSize2 = WavpackGetTagItem( pWvCtx, "CUESHEET", valueBuffer, valueBuffer.length() );
		wxASSERT( nSize == nSize2 );
	}
	else
	{
		wxLogWarning( wxT("There's no CUESHEET tag in Wavpack file %s"), sWavpackFile );
	}

	WavpackFreeWrapper( pWvCtx );

	if ( nSize > 0 )
	{
		sCueSheet = wxString::FromUTF8( valueBuffer, nSize2 );
		return true;
	}
	else
	{
		return false;
	}
}

bool wxWavpackTagReader::ReadTags( const wxString& sWavpackFile, wxArrayCueTag& comments )
{
	wxFileInputStream is( sWavpackFile );
	if ( !is.IsOk() )
	{
		return false;
	}

	wxNullOutputStream os;

	wxWavpackStream wavpackStream( is, os );
	wxWritableCharBuffer errBuffer;
	errBuffer.extend( 80 );
	WavpackContext* pWvCtx = WavpackOpenFileInputEx(
		wavpackStream.GetStream(),
		&wavpackStream, NULL,
		errBuffer,
		OPEN_TAGS,
		0
	);

	if ( pWvCtx == (WavpackContext*)NULL )
	{
		wxString sErr( wxString::FromUTF8( errBuffer ) );
		wxLogError( wxT("Fail to open Wavpack file %s. Error message: %s"), sWavpackFile, sErr );
		return false;
	}

	int nItems = WavpackGetNumTagItems( pWvCtx );

	wxWritableCharBuffer itemBuffer;
	wxWritableCharBuffer valueBuffer;

	for( int i=0; i<nItems; i++ )
	{
		size_t nSize = WavpackGetTagItemIndexed( pWvCtx, i, NULL, 0 );
		wxASSERT( nSize > 0 );

		if ( (nSize+1) > itemBuffer.length() )
		{
			itemBuffer.extend( nSize+1 );
		}

		size_t nSize2 = WavpackGetTagItemIndexed( pWvCtx, i, itemBuffer, itemBuffer.length() );
		wxASSERT( nSize == nSize2 );

		wxString sItem( wxString::FromUTF8( itemBuffer, nSize2 ) );

		nSize = WavpackGetTagItem( pWvCtx, itemBuffer, NULL, 0 );
		wxASSERT( nSize > 0 );
		if ( (nSize+1) > valueBuffer.length() )
		{
			valueBuffer.extend( nSize+1 );
		}

		nSize2 = WavpackGetTagItem( pWvCtx, itemBuffer, valueBuffer, valueBuffer.length() );
		wxASSERT( nSize == nSize2 );

		wxStringTokenizer valueTokenizer( 
			wxString::FromUTF8( valueBuffer, nSize2 ),
			wxT('\0'),
			wxTOKEN_DEFAULT );

		while ( valueTokenizer.HasMoreTokens() )
		{
			wxCueTag comment( wxCueTag::TAG_MEDIA_METADATA, sItem, valueTokenizer.GetNextToken() );
			comments.Add( comment );
		}
	}

	WavpackFreeWrapper( pWvCtx );
	return true;
}
