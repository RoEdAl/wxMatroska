/*
 * wxReduntantSpacesRemover.cpp
 */

#include "wxReduntantSpacesRemover.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxReduntantSpacesRemover, wxStringProcessor)

// ===============================================================================

const char wxReduntantSpacesRemover::NORMAL_REG_EX[] = "\\p{Z}{2}"; // two spaces -> simple space
const char wxReduntantSpacesRemover::EN_REG_EX[] = "\\p{Z}{3}"; // three spaces -> en space
const char wxReduntantSpacesRemover::EM_REG_EX[] = "\\p{Z}{4,}"; // four or more spaces -> em space

// ===============================================================================

wxReduntantSpacesRemover::wxReduntantSpacesRemover() :
	m_re(NORMAL_REG_EX),
	m_reEn(EN_REG_EX),
	m_reEm(EM_REG_EX),
	m_replacementNormal(wxUniChar(0x0020)),
	m_replacementEn(wxUniChar(0x2002)),
	m_replacementEm(wxUniChar(0x2001))
{
	wxASSERT( m_re.IsValid() );
	wxASSERT(m_reEn.IsValid());
	wxASSERT(m_reEm.IsValid());
}

wxStringProcessor* const wxReduntantSpacesRemover::Clone() const
{
	return new wxReduntantSpacesRemover();
}

bool wxReduntantSpacesRemover::Process( const wxString& sIn, wxString& sOut ) const
{
	wxString w(sIn);
	int res = 0;

	res += m_re.ReplaceAll( &w, m_replacementNormal );
	res += m_reEn.ReplaceAll(&w, m_replacementEn);
	res += m_reEm.ReplaceAll(&w, m_replacementEm);

	if (res > 0)
	{
		sOut = w;
		return true;
	}
	else
	{
		return false;
	}
}
