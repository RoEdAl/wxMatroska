/*
 * wxDashesCorrector.h
 */

#ifndef _WX_DASHES_CORRECTOR_H_
#define _WX_DASHES_CORRECTOR_H_

class wxDashesCorrector
{
	public:

        typedef wxUniChar::value_type uvalue_type;

		static const char REG_EX_EN[];
        static const char REG_EX_EM[];

        static wxString GetReplacement( uvalue_type, uvalue_type );

        wxDashesCorrector( void );
        wxDashesCorrector( uvalue_type, uvalue_type, uvalue_type );

		void Replace( wxString& ) const;
		void ReplaceEx( const wxString&, wxString& ) const;

		const wxRegEx& GetEnRegEx() const;
        const wxRegEx& GetEmRegEx( ) const;

	protected:

		wxRegEx	 m_reEn;
        wxRegEx	 m_reEm;

        wxString m_sEnReplacement;
        wxString m_sEmReplacement;
};

#endif

