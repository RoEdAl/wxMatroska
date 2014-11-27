/*
 * wxMkvmergeOptsRenderer.h
 */

#ifndef _WX_MKVMERGE_OPTS_RENDERER_H_
#define _WX_MKVMERGE_OPTS_RENDERER_H_

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
#include <wxCueFile/wxTextOutputStreamOnString.h>
#endif

#ifndef _WX_PRIMITIVE_RENDERER_H_
#include "wxPrimitiveRenderer.h"
#endif

class wxMkvmergeOptsRenderer:
	public wxPrimitiveRenderer
{
	protected:

		wxFileName m_matroskaOptsFile;

	protected:

		static wxString mkvmerge_escape( const wxString& );
		static wxString mkvmerge_escape( const wxFileName& );
		static wxString mkvmerge_escape( const wxDataFile& );

		static wxTextOutputStream& append_cover( wxTextOutputStream&, const wxCoverFile& );

		wxString GetEscapedFile( const wxFileName& );

		void write_cover_attachments( const wxInputFile&, const wxArrayCoverFile& ) const;
		void write_cdtextfiles_attachments( const wxArrayFileName& );
		void write_log_attachments( const wxArrayFileName& );
		void write_eac_attachments( const wxInputFile&, const wxCueSheet& );
		void write_source_eac_attachments( const wxInputFile&, const wxArrayCueSheetContent& );
		void write_decoded_eac_attachments( const wxInputFile&, const wxArrayCueSheetContent& );
		void write_rendered_eac_attachments( const wxInputFile&, const wxCueSheet& );
		bool save_cover( const wxInputFile&, wxCoverFile& ) const;
		bool save_cuesheet( const wxInputFile&, const wxString&, const wxString&, wxFileName& ) const;
		bool render_cuesheet( const wxInputFile&, const wxString&, const wxCueSheet&, wxFileName& );

		static wxString get_mapping_str( const wxCueSheet& );

	public:

		wxMkvmergeOptsRenderer( const wxConfiguration& );

		void RenderDisc( const wxInputFile&, const wxCueSheet& );
		bool Save();
		const wxFileName& GetMkvmergeOptsFile() const;
};
#endif

