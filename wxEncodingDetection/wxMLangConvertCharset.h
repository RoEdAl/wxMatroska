/*
 * wxMLangConvertCharset.h
 */

#ifndef _WX_MLANG_CONVERT_CHARSET_H_
#define _WX_MLANG_CONVERT_CHARSET_H_

#ifndef _WX_MULTI_LANGUAGE_H_
class wxMultiLanguage;
#endif

class wxMLangConvertCharset:
	public wxObject
{
	protected:

		IMLangConvertCharset* m_pMLang;

	public:

		wxMLangConvertCharset( void );
		wxMLangConvertCharset( wxUint32, wxUint32 );
		wxMLangConvertCharset( const wxMultiLanguage&, wxUint32, wxUint32 );
		wxMLangConvertCharset( const wxMLangConvertCharset& );
		~wxMLangConvertCharset( void );

		bool Initialize( wxUint32, wxUint32 );
		bool IsValid() const;
		void Close();

		IMLangConvertCharset* operator ->() const
		{
			return m_pMLang;
		}

		static wxUint32 GetRealCodePage( wxUint32 );
};
#endif

