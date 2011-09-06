/*
   wxMkvmergeOptsRenderer.h
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

class wxMkvmergeOptsRenderer:
	public wxObject
{
protected:

	const wxConfiguration&	   m_cfg;
	wxTextOutputStreamOnString m_os;
	wxFileName				   m_matroskaOptsFile;

protected:

	static wxString mkvmerge_escape( const wxString& );
	static wxString mkvmerge_escape( const wxFileName& );
	static wxString mkvmerge_escape( const wxDataFile& );

	wxString GetEscapedFile( const wxFileName& );

	void write_cover_attachments( const wxArrayFileName& );
	void write_log_attachments( const wxArrayFileName& );
	void write_eac_attachments( const wxInputFile&, const wxCueSheet& );
	void write_source_eac_attachments( const wxInputFile&, const wxArrayCueSheetContent& );
	void write_decoded_eac_attachments( const wxInputFile&, const wxArrayCueSheetContent& );
	void write_rendered_eac_attachments( const wxInputFile&, const wxCueSheet& );
	bool save_cuesheet( const wxInputFile&, const wxString&, const wxString&, wxFileName& );
	bool render_cuesheet( const wxInputFile&, const wxString&, const wxCueSheet&, wxFileName& );
	static void save_string_to_stream( wxTextOutputStream&, const wxString& );

	static wxString get_mapping_str( const wxCueSheet& );

public:

	wxMkvmergeOptsRenderer( const wxConfiguration& );

	void RenderDisc( const wxInputFile&, const wxCueSheet& );
	bool Save();
	const wxFileName& GetMkvmergeOptsFile() const;
};

#endif

