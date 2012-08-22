/*
 *      enum2str.h
 */

#ifndef _ENUM_2_STR_H_
#define _ENUM_2_STR_H_

template< typename T, typename D, size_t SIZE >
static bool from_string( const wxString& s, T& e, const D( &desc )[ SIZE ] )
{
	for ( size_t i = 0; i < SIZE; i++ )
	{
		if ( s.CmpNoCase( desc[ i ].description ) == 0 )
		{
			e = desc[ i ].value;
			return true;
		}
	}

	return false;
}

template< typename T, typename D, size_t SIZE >
static wxString to_string( T e, const D( &desc )[ SIZE ] )
{
	for ( size_t i = 0; i < SIZE; i++ )
	{
		if ( desc[ i ].value == e )
		{
			return desc[ i ].description;
		}
	}

	return wxString::Format( "<%d>", static_cast< int >( e ) );
}

template< typename D, size_t SIZE >
static wxString get_texts( const D( &desc )[ SIZE ] )
{
	wxString s;

	for ( size_t i = 0; i < SIZE; i++ )
	{
		s << desc[ i ].description << "|";
	}

	return s.RemoveLast();
}

#endif

