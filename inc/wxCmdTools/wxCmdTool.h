/*
     wxCmdTool.h
 */
 
#ifndef _WX_CMD_TOOL_H_
#define _WX_CMD_TOOL_H_

class wxCmdTool
{
	public:

	enum TOOL
	{
		TOOL_UNKNOWN,
		TOOL_MKVMERGE,
		TOOL_FFMPEG,
		TOOL_CMAKE,
		TOOL_FFPROBE
	};
	
	static bool FindExecutable(const wxString&, const wxString&, wxFileName&);

	static bool FindTool(TOOL, wxFileName&);

	protected:

	static bool FindExecutableEnv(const wxString&, const wxString&, const wxString&, wxFileName&);
	static bool FindExecutable(const wxFileName&, const wxString&, const wxString&, wxFileName&);

	private:

	static wxString get_exe_ext();
	static wxFileName get_exe_sub_dir();
	static wxFileName get_exe_local_sub_dir();
};

#endif