/*
   wxUnquoter.h
 */

#ifndef _WX_UNQUOTER_H_
#define _WX_UNQUOTER_H_

class wxUnquoter:
	public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxUnquoter );

protected:

	wxRegEx m_reSingleQuotes;
	wxRegEx m_reDoubleQuotes;
	wxRegEx m_rePseudoDoubleQuotes;

	wxRegEx m_reFullQuotes;
	wxRegEx m_reFullDoubleQuotes;

	wxString m_sSingleQuotes;
	wxString m_sDoubleQuotes;

public:

	static const wxChar RE_SINGLE_QUOTES[];
	static const wxChar RE_DOUBLE_QUOTES[];
	static const wxChar RE_PSEUDO_DOUUBLE_QUOTES[];
	static const wxChar RE_FULL_SINGLE_QUOTES[];
	static const wxChar RE_FULL_DOUBLE_QUOTES[];

	static bool correct_polish_qm( const wxString& );
	static bool correct_english_qm( const wxString& );
	static bool correct_german_qm( const wxString& );
	static bool correct_french_qm( const wxString& );

	static const wxChar ENGLISH_DOUBLE_QUOTES[];
	static const wxChar ENGLISH_SINGLE_QUOTES[];
	static const wxChar POLISH_DOUBLE_QUOTES[];
	static const wxChar POLISH_SINGLE_QUOTES[];
	static const wxChar GERMAN_DOUBLE_QUOTES[];
	static const wxChar GERMAN_SINGLE_QUOTES[];
	static const wxChar FRENCH_DOUBLE_QUOTES[];
	static const wxChar FRENCH_SINGLE_QUOTES[];

public:

	wxUnquoter( void );

	void SetLang( const wxString& );

	wxString Unquote( const wxString& ) const;
	wxString UnquoteAndCorrect( const wxString& ) const;

	const wxRegEx& GetReSingleQuotes() const;
	const wxRegEx& GetReDoubleQuotes() const;

	const wxRegEx& GetReFullSingleQuotes() const;
	const wxRegEx& GetReFullDoubleQuotes() const;

	bool IsQuoted( const wxString& ) const;
};

#endif

