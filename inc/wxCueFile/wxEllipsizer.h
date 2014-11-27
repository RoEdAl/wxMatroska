/*
 * wxEllipsizer.h
 */

#ifndef _WX_ELLIPSIZER_H_
#define _WX_ELLIPSIZER_H_

class wxEllipsizer:
	public wxObject
{
	public:

		wxEllipsizer( void );

		static const char	 REG_EX[];
		static const wxUChar ELLIPSIS;

		bool EllipsizeEx( const wxString&, wxString& ) const;
		wxString Ellipsize( const wxString& ) const;

	protected:

		wxRegEx m_reEllipsis;
};
#endif

