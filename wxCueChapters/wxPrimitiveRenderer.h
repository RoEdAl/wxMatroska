/*
 * wxPrimitiveRenderer.h
 */

#ifndef _WX_PRIMITIVE_RENDERER_H_
#define _WX_PRIMITIVE_RENDERER_H_

#ifndef _WX_CONFIGURATION_H_
class wxConfiguration;
#endif

#ifndef _WX_CUE_SHEET_H_
class wxCueSheet;
#endif

#ifndef _WX_INPUT_FILE_H_
class wxInputFile;
#endif

#ifndef _WX_DATA_FILE_H_
class wxArrayFileName;
#endif

#ifndef _WX_TEXT_OUTPUT_STREAM_ON_STRING_H_
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#endif

class wxPrimitiveRenderer:
	public wxObject
{
	protected:

		const wxConfiguration&	   m_cfg;
		wxTextOutputStreamOnString m_os;

	protected:

		wxPrimitiveRenderer( const wxConfiguration& );
};
#endif

