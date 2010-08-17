/*
	wxUnquoter.h
*/

#ifndef _WX_UNQUOTER_H_
#define _WX_UNQUOTER_H_

class wxUnquoter :public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxUnquoter)

protected:

	wxRegEx m_reQuotes;
	wxRegEx m_reDoubleQuotes;

	wxString m_sSingleQuotes;
	wxString m_sDoubleQuotes;

public:

	static const wxChar ENGLISH_DOUBLE_QUOTES[];
	static const wxChar ENGLISH_SINGLE_QUOTES[];
	static const wxChar POLISH_DOUBLE_QUOTES[];
	static const wxChar POLISH_SINGLE_QUOTES[];
	static const wxChar GERMAN_DOUBLE_QUOTES[];
	static const wxChar GERMAN_SINGLE_QUOTES[];
	static const wxChar FRENCH_DOUBLE_QUOTES[];
	static const wxChar FRENCH_SINGLE_QUOTES[];

public:

	wxUnquoter(void);
	~wxUnquoter(void);

	void SetLang( const wxString& );

	wxString Unquote( const wxString& ) const;
	wxString UnquoteAndCorrect( const wxString& ) const;

	const wxRegEx& GetReSingleQuotes() const;
	const wxRegEx& GetReDoubleQuotes() const;



};

#endif