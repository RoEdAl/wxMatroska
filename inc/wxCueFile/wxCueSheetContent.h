/*
        wxCueSheetContent.h
 */

#ifndef _WX_CUE_SHEET_CONTENT_H_
#define _WX_CUE_SHEET_CONTENT_H_

class wxCueSheetContent:
	public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxCueSheetContent );

protected:

	wxString m_sValue;
	wxFileName m_source;
	bool m_bEmbedded;

public:

	wxCueSheetContent( void );
	wxCueSheetContent( const wxString& );
	wxCueSheetContent( const wxString&, const wxFileName&, bool );
	wxCueSheetContent( const wxCueSheetContent& );

	wxCueSheetContent& operator =( const wxCueSheetContent& );

	bool HasSource() const;
	const wxFileName& GetSource() const;
	bool IsEmbedded() const;
	const wxString&	  GetValue() const;

protected:

	void copy( const wxCueSheetContent& );
};

WX_DECLARE_OBJARRAY( wxCueSheetContent, wxArrayCueSheetContent );

#endif // _WX_CUE_SHEET_CONTENT_H_