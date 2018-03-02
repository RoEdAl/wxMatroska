/*
 * wxReduntantSpacesRemover.h
 */

#ifndef _WX_REDUNTANT_SPACES_REMOVER_H_
#define _WX_REDUNTANT_SPACES_REMOVER_H_

class wxReduntantSpacesRemover
{
	public:

		static const char REG_EX[];

		wxReduntantSpacesRemover( void );

		int Remove( wxString& ) const;
		int RemoveEx( const wxString&, wxString& ) const;

		const wxRegEx& GetRegEx() const;

	protected:

		wxRegEx m_re;
		wxUniChar m_cReplacement;
};

#endif

