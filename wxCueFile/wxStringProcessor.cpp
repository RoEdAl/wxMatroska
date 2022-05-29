/*
 *      wxStringProcessor.cpp
 */

#include <wxCueFile/wxStringProcessor.h>

wxIMPLEMENT_ABSTRACT_CLASS( wxStringProcessor, wxObject )

wxStringProcessor::wxStringProcessor()
{}

bool wxStringProcessor::operator ()( wxString& s ) const
{
	wxString res;

	if ( Process( s, res ) )
	{
		s = res;
		return true;
	}
	return false;
}

wxString wxStringProcessor::operator ()( const wxString& s ) const
{
	wxString res;

	return Process( s, res ) ? res : s;
}

// #include <wx/arrimpl.cpp>
#define WX_DEFINE_OBJARRAY_EX( name )									   \
	name::value_type*													   \
	wxObjectArrayTraitsFor ## name::Clone( const name::value_type & item ) \
	{																	   \
		return item.Clone();											   \
	}																	   \
																		   \
	void wxObjectArrayTraitsFor ## name::Free( name::value_type * p )	   \
	{																	   \
		delete p;														   \
	}

WX_DEFINE_OBJARRAY_EX( wxArrayStringProcessor );

