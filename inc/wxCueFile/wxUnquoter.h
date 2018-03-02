/*
 * wxUnquoter.h
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

		wxString m_sGenericReplacement;

		wxRegEx m_reFullQuotes;
		wxRegEx m_reFullDoubleQuotes;

	public:

		static const char RE_SINGLE_QUOTES[];
		static const char RE_SINGLE_QUOTES_EX[];
		static const char RE_DOUBLE_QUOTES[];
		static const char RE_PSEUDO_DOUUBLE_QUOTES[];
		static const char RE_FULL_SINGLE_QUOTES[];
		static const char RE_FULL_DOUBLE_QUOTES[];

		static const wxChar POLISH_DOUBLE_QUOTES[];
		static const wxChar POLISH_SINGLE_QUOTES[];

		static bool correct_polish_qm( const wxString& );
		static bool correct_english_qm( const wxString& );
		static bool correct_german_qm( const wxString& );
		static bool correct_french_qm( const wxString& );

	protected:

		static const char OPENING_QOUTATION_MARK_REPLACEMENT;
		static const char CLOSING_QOUTATION_MARK_REPLACEMENT;
		static const char GENERIC_REPLACEMENT[];

		struct QUOTATION_MARKS
		{
			const wxChar* opening;
			const wxChar* closing;
		};

		static const QUOTATION_MARKS ASCII_QUOTES[];
		static const QUOTATION_MARKS ENGLISH_QUOTES[];
		static const QUOTATION_MARKS POLISH_QUOTES[];
		static const QUOTATION_MARKS GERMAN_QUOTES[];
		static const QUOTATION_MARKS FRENCH_QUOTES[];

		typedef wxScopedCharTypeBuffer< wxChar > wxScopedCharBuffer;
		typedef wxScopedCharBuffer ( wxUnquoter::* GET_REPLACEMENT_METHOD )( int, bool ) const;

		wxScopedCharBuffer get_standard_replacement( int, bool ) const;
		wxScopedCharBuffer get_english_replacement( int, bool ) const;
		wxScopedCharBuffer get_polish_replacement( int, bool ) const;
		wxScopedCharBuffer get_german_replacement( int, bool ) const;
		wxScopedCharBuffer get_french_replacement( int, bool ) const;

		template< size_t SIZE >
		static wxScopedCharBuffer get_replacement( const QUOTATION_MARKS(&quotation_marks_array)[ SIZE ], int nLevel, bool bOpening )
		{
			if ( nLevel < 0 )
			{
				return wxScopedCharBuffer();
			}

			if ( bOpening )
			{
				return wxScopedCharBuffer::CreateNonOwned( quotation_marks_array[ nLevel % SIZE ].opening );
			}
			else
			{
				return wxScopedCharBuffer::CreateNonOwned( quotation_marks_array[ nLevel % SIZE ].closing );
			}
		}

		void InternalCorrectQuotes( wxString& ) const;

	protected:

		GET_REPLACEMENT_METHOD m_replacement_method;

	public:

		wxUnquoter( void );

		void SetLang( const wxString& );

		wxString Unquote( const wxString& ) const;
		int CorrectQuotes( wxString& ) const;
		wxString UnquoteAndCorrect( const wxString& ) const;

		const wxRegEx& GetReSingleQuotes() const;
		const wxRegEx& GetReDoubleQuotes() const;

		const wxRegEx& GetReFullSingleQuotes() const;
		const wxRegEx& GetReFullDoubleQuotes() const;

		bool IsQuoted( const wxString& ) const;
};

#endif

