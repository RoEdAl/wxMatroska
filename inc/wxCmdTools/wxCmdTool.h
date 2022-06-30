/*
     wxCmdTool.h
 */
 
#ifndef _WX_CMD_TOOL_H_
#define _WX_CMD_TOOL_H_

class wxCmdTool
{
	wxDECLARE_NO_COPY_CLASS(wxCmdTool);

	public:

	enum TOOL
	{
		TOOL_UNKNOWN,
		TOOL_MKVMERGE,
		TOOL_FFMPEG,
		TOOL_CMAKE,
		TOOL_FFPROBE,
		TOOL_IMAGE_MAGICK,
		TOOL_MUTOOL
	};
	
	static bool FindExecutable(const wxString&, const wxString&, wxFileName&);
	static bool FindTool(TOOL, wxFileName&);
	static void GetSearchDirectories(wxArrayString&);

	protected:

	static bool CheckExecutable(const wxString&, wxFileName&);
	static bool FindExecutableEnv(const wxString&, const wxString&, const wxString&, wxFileName&);
	static bool FindExecutable(const wxFileName&, const wxString&, const wxString&, wxFileName&);
	static bool FindExecutable(const wxFileName&, const wxString&, const wxArrayString&, wxFileName&);

	private:

	static wxString get_exe_ext();
	static wxFileName get_exe_sub_dir();
	static wxFileName get_exe_local_sub_dir();
};

#endif