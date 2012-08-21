/*
 *      wxCueSheetContent.h
 */

#ifndef _WX_CUE_SHEET_CONTENT_H_
#define _WX_CUE_SHEET_CONTENT_H_

#ifndef _WX_DATA_FILE_H_
#include "wxDataFile.h"
#endif

class wxCueSheetContent:
	public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxCueSheetContent );

	protected:

		wxString   m_sValue;
		wxDataFile m_source;

	public:

		wxCueSheetContent( void );
		wxCueSheetContent( const wxString& );
		wxCueSheetContent( const wxString&, const wxDataFile& );
		wxCueSheetContent( const wxCueSheetContent& );

		wxCueSheetContent& operator =( const wxCueSheetContent& );

		bool HasSource() const;
		const wxDataFile& GetSource() const;
		const wxString& GetValue() const;

	protected:

		void copy( const wxCueSheetContent& );
};

WX_DECLARE_OBJARRAY( wxCueSheetContent, wxArrayCueSheetContent );

#endif	// _WX_CUE_SHEET_CONTENT_H_

