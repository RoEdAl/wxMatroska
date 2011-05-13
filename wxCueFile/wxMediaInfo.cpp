/*
   wxMediaInfo.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxMediaInfo.h>

wxIMPLEMENT_ABSTRACT_CLASS( wxMediaInfo, wxObject )

const wxChar wxMediaInfo::MEDIA_INFO_LIBRARY[] = wxT( "MediaInfo.dll" );

const wxChar* wxMediaInfo::SYMBOL_NAMES[] =
{
	wxT( "MediaInfo_New" ),
	wxT( "MediaInfo_Delete" ),
	wxT( "MediaInfo_Open" ),
	wxT( "MediaInfo_Close" ),
	wxT( "MediaInfo_Get" )
};

const size_t wxMediaInfo::SYMBOL_NAMES_SIZE = WXSIZEOF( wxMediaInfo::SYMBOL_NAMES );

bool wxMediaInfo::load_symbols( const wxDynamicLibrary& dll, wxArrayPtrVoid& pointers )
{
	pointers.Clear();
	for ( size_t i = 0 ; i < SYMBOL_NAMES_SIZE ; i++ )
	{
		if ( !dll.HasSymbol( SYMBOL_NAMES[ i ] ) )
		{
			wxLogError( _( "Cannot find symbol %s in dynamic library" ), SYMBOL_NAMES[ i ] );
			return false;
		}

		void* p = dll.GetSymbol( SYMBOL_NAMES[ i ] );
		pointers.Add( p );
	}

	return true;
}

wxMediaInfo::wxMediaInfo( void )
{}

bool wxMediaInfo::Load()
{
	if ( !m_dll.Load( MEDIA_INFO_LIBRARY ) )
	{
		return false;
	}

	wxArrayPtrVoid pointers;

	if ( !load_symbols( m_dll, pointers ) )
	{
		m_dll.Unload();
		return false;
	}

	m_fnNew	   = (MEDIAINFO_New)pointers[ 0 ];
	m_fnDelete = (MEDIAINFO_Delete)pointers[ 1 ];
	m_fnOpen   = (MEDIAINFO_Open)pointers[ 2 ];
	m_fnClose  = (MEDIAINFO_Close)pointers[ 3 ];
	m_fnGet	   = (MEDIAINFO_Get)pointers[ 4 ];
	return true;
}

bool wxMediaInfo::IsLoaded() const
{
	return m_dll.IsLoaded();
}

void wxMediaInfo::Unload()
{
	if ( IsLoaded() )
	{
		m_dll.Unload();
	}
}

void* wxMediaInfo::MediaInfoNew()
{
	wxASSERT( IsLoaded() );
	return m_fnNew();
}

void wxMediaInfo::MediaInfoDelete( void* handle )
{
	wxASSERT( IsLoaded() );
	return m_fnDelete( handle );
}

size_t wxMediaInfo::MediaInfoOpen( void* handle, const wxChar* media )
{
	wxASSERT( IsLoaded() );
	return m_fnOpen( handle, media );
}

void wxMediaInfo::MediaInfoClose( void* handle )
{
	wxASSERT( IsLoaded() );
	m_fnClose( handle );
}

const wxChar* wxMediaInfo::MediaInfoGet( void* handle, MediaInfo_stream_C StreamKind, size_t StreamNumber, const wxChar* Parameter, MediaInfo_info_C KindOfInfo, MediaInfo_info_C KindOfSearch )
{
	wxASSERT( IsLoaded() );
	return m_fnGet( handle, StreamKind, StreamNumber, Parameter, KindOfInfo, KindOfSearch );
}