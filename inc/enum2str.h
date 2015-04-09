/*
 *      enum2str.h
 */

#ifndef _ENUM_2_STR_H_
#define _ENUM_2_STR_H_

template<typename T>
struct VALUE_NAME_PAIR
{
    const T& value;
    const char* const description;
};

template< typename T, size_t SIZE >
static inline bool from_string( const wxString& s, T& e, const VALUE_NAME_PAIR<T>(&desc)[ SIZE ] )
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

template< typename T, size_t SIZE >
static inline wxString to_string( const T& e, const VALUE_NAME_PAIR<T>(&desc)[ SIZE ] )
{
	for ( size_t i = 0; i < SIZE; i++ )
	{
		if ( desc[ i ].value == e )
		{
			return desc[ i ].description;
		}
	}

    wxLogDebug( "to_string<%d> - unknnown value name", static_cast<int>(e) );
	return wxString::Format( "<%d>", static_cast< int >( e ) );
}

template< typename T, size_t SIZE >
static inline wxString get_texts( const VALUE_NAME_PAIR<T>(&desc)[ SIZE ] )
{
	wxString s;

	for ( size_t i = 0; i < SIZE; i++ )
	{
		s << desc[ i ].description << "|";
	}

	return s.RemoveLast();
}

template<typename T, size_t SIZE>
static wxString get_texts_regexp( const VALUE_NAME_PAIR<T>( &arr )[SIZE] )
{
    return get_texts<T, SIZE>( arr ).RemoveLast( ).Prepend( '(' ).Append( ')' );
}


#endif

