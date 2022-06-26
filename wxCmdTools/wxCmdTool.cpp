/*
	wxCmdTool.cpp
 */
 
#include <wxCmdTools/wxCmdTool.h>

wxString wxCmdTool::get_exe_ext()
{
    const wxFileName fn(wxStandardPaths::Get().GetExecutablePath());
    return fn.GetExt();
}

wxFileName wxCmdTool::get_exe_sub_dir()
{
    wxFileName fn(wxStandardPaths::Get().GetExecutablePath());
    fn.SetFullName(wxEmptyString);
    fn.AppendDir("tools");

    return fn;
}

wxFileName wxCmdTool::get_exe_local_sub_dir()
{
    wxFileName fnExe(wxStandardPaths::Get().GetExecutablePath());

    wxString envVal;
    if (!wxGetEnv("LocalAppData", &envVal))
    {
        return wxFileName();
    }

    wxFileName fn = wxFileName::DirName(envVal);
    fn.AppendDir(fnExe.GetName());
    return fn;
}

bool wxCmdTool::FindExecutableEnv(const wxString& env, const wxString& name, const wxString& postFix, wxFileName& exe)
{
    wxString envVal;
    if (!wxGetEnv(env, &envVal))
    {
        return false;
    }
    return FindExecutable(wxFileName::DirName(envVal), name, postFix, exe);
}

bool wxCmdTool::FindExecutable(const wxFileName& dir, const wxString& name, const wxString& postFix, wxFileName& exe)
{
    if (!dir.IsOk()) return false;

    wxFileName fn(dir);

    {
        wxStringTokenizer tokenizer(postFix, "/");
        while (tokenizer.HasMoreTokens())
        {
            fn.AppendDir(tokenizer.GetNextToken());
        }
    }

    fn.SetName(name);
    fn.SetExt(get_exe_ext());

    if (fn.IsFileExecutable())
    {
        exe = fn;
        return true;
    }
    else
    {
        wxFileName res;
        res.SetName(name);
        res.SetExt(get_exe_ext());

        exe = res;
        return false;
    }
}

bool wxCmdTool::CheckExecutable(const wxString& name, wxFileName& exe)
{
    wxString envVal;
    if (!wxGetEnv(name.Upper(), &envVal))
    {
        return false;
    }

    const wxFileName fn = wxFileName::FileName(envVal);
    if (fn.IsFileExecutable())
    {
        exe = fn;
        return true;
    }

    return false;
}

bool wxCmdTool::FindExecutable(const wxString& name, const wxString& postFix, wxFileName& exe)
{
    return
        CheckExecutable(name, exe) ||
        FindExecutable(get_exe_sub_dir(), name, postFix, exe) ||
        FindExecutable(get_exe_local_sub_dir(), name, postFix, exe) ||
        FindExecutableEnv("LocalAppData", name, postFix, exe) ||
        FindExecutableEnv("ProgramW6432", name, postFix, exe) ||
        FindExecutableEnv("ProgramFiles", name, postFix, exe) ||
        FindExecutableEnv("ProgramFiles(x86)", name, postFix, exe);
}

bool wxCmdTool::FindTool(wxCmdTool::TOOL tool, wxFileName& exe)
{
    switch (tool)
    {
        case TOOL_FFMPEG:
        return FindExecutable("ffmpeg", "ffmpeg/bin", exe);

        case TOOL_FFPROBE:
        return FindExecutable("ffprobe", "ffmpeg/bin", exe);

        case TOOL_MKVMERGE:
        return FindExecutable("mkvmerge", "MKVToolNix", exe);

        case TOOL_CMAKE:
        return FindExecutable("cmake", "CMake/bin", exe);

        case TOOL_IMAGE_MAGICK:
        return FindExecutable("magick", "ImageMagick", exe);

        case TOOL_MUTOOL:
        return FindExecutable("mutool", "mupdf", exe);

        default:
        return false;
    }
}


