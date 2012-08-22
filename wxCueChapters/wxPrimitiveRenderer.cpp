/*
 * wxPrimitiveRenderer.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include <wxCueFile/wxTextOutputStreamOnString.h>
#include <wxCueFile/wxTextInputStreamOnString.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>
#include "wxConfiguration.h"
#include "wxInputFile.h"
#include "wxPrimitiveRenderer.h"
#include "wxApp.h"

// ===============================================================================

wxPrimitiveRenderer::wxPrimitiveRenderer( const wxConfiguration& cfg ):
	m_cfg( cfg )
{}

void wxPrimitiveRenderer::save_string_to_stream( wxTextOutputStream& stream, const wxString& s )
{
	wxTextInputStreamOnString tis( s );
	wxString				  sLine;

	while ( !tis.Eof() )
	{
		sLine = ( *tis ).ReadLine();

		if ( sLine.IsEmpty() )
		{
			stream << endl;
		}
		else
		{
			stream << sLine << endl;
		}
	}

	stream.Flush();
}

