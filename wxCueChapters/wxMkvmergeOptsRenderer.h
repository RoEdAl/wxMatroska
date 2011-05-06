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

class wxMkvmergeOptsRenderer :public wxObject
{
protected:

	const wxConfiguration& m_cfg;

	wxArrayString m_asMmcPre;
	wxArrayString m_asMmcPost;
	wxArrayString m_asMmcInputFiles;
	wxArrayFileName m_logFiles;

	wxString m_sMatroskaOptsFile;

protected:

	static wxString mkvmerge_escape( const wxString& );
	static void write_as( wxTextOutputStream&, const wxArrayString& );

	static bool GetLogFile( const wxFileName&, wxFileName& );

	void write_attachments( wxTextOutputStream& );

public:

	static wxMkvmergeOptsRenderer* const Null;
	static const wxChar LOG_EXT[];

	wxMkvmergeOptsRenderer(const wxConfiguration&);

	void RenderDisc( const wxInputFile&, const wxCueSheet& );
	bool Save();
	const wxString& GetMkvmergeOptsFile() const;

};

#endif
