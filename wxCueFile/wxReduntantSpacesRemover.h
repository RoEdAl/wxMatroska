/*
 * wxReduntantSpacesRemover.h
 */

#ifndef _WX_REDUNTANT_SPACES_REMOVER_H_
#define _WX_REDUNTANT_SPACES_REMOVER_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif

class wxReduntantSpacesRemover:
	public wxStringProcessor
{
	wxDECLARE_DYNAMIC_CLASS( wxReduntantSpacesRemover );

	public:

		static const char NORMAL_REG_EX[];
		static const char EN_REG_EX[];
		static const char EM_REG_EX[];

		wxReduntantSpacesRemover( void );

		virtual wxStringProcessor* const Clone() const;
		virtual bool Process( const wxString&, wxString& ) const;

	protected:

		wxRegEx m_re;
		wxRegEx m_reEn;
		wxRegEx m_reEm;

		wxString m_replacementNormal;
		wxString m_replacementEn;
		wxString m_replacementEm;
};

#endif

