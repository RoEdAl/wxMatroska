/*
 * wxTrailingSpacesRemover.h
 */

#ifndef _WX_TRAILING_SPACES_REMOVER_H_
#define _WX_TRAILING_SPACES_REMOVER_H_

class wxTrailingSpacesRemover:
	public wxObject
{
	public:

		static const char REG_EX[];

		wxTrailingSpacesRemover( void );

		wxString Remove( const wxString& ) const;
		bool RemoveEx( const wxString&, wxString& ) const;

		const wxRegEx& GetRegEx() const;

	protected:

		wxRegEx m_reTrailingSpaces;
};

#endif

