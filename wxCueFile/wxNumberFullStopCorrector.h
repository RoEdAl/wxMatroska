/*
 * wxNumberFullStopCorrector.h
 */

#ifndef _WX_NUMBER_FULL_STOP_CORRECTOR_H_
#define _WX_NUMBER_FULL_STOP_CORRECTOR_H_

#ifndef _WX_STRING_PROCESSOR_H_
#include <wxCueFile/wxStringProcessor.h>
#endif


class wxNumberFullStopCorrector :public wxStringProcessor
{
	wxDECLARE_DYNAMIC_CLASS(wxNumberFullStopCorrector);

	public:

		static const char REG_EX[];

		wxNumberFullStopCorrector( void );

		virtual wxStringProcessor* const Clone() const;
		virtual bool Process( const wxString&, wxString& ) const;

	protected:

		wxRegEx m_re;
};

#endif

