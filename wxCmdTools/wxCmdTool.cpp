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

bool wxCmdTool::FindExecutable(const wxFileName& dir, const wxString& name, const wxArrayString& subDirs, wxFileName& exe)
{
    wxASSERT(dir.IsOk());

    wxFileName fn(dir);
    for(wxArrayString::const_iterator i=subDirs.begin(), end = subDirs.end(); i != end; ++i)
    {
        fn.AppendDir(*i);
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

namespace
{
    void get_subdirs(const wxString& postFix, wxArrayString& subDirs)
    {
        wxStringTokenizer tokenizer(postFix, "/");
        while (tokenizer.HasMoreTokens())
        {
            subDirs.Add(tokenizer.GetNextToken());
        }
    }

    void enum_subdirs(const wxString& dir, const wxString& dirMask, wxArrayString& subDirs)
    {
        wxString dirName;

        wxDir dirEnum;
        if (!dirEnum.Open(dir))
        {
            wxLogError(_("Unable to enumerate directory: %s"), dir);
            return;
        }

        bool cont = dirEnum.GetFirst(&dirName, dirMask, wxDIR_DIRS | wxDIR_NO_FOLLOW);
        while (cont)
        {
            subDirs.Add(dirName);
            cont = dirEnum.GetNext(&dirName);
        }
    }
}

bool wxCmdTool::FindExecutable(const wxFileName& dir, const wxString& name, const wxString& postFix, wxFileName& exe)
{
    if (!dir.DirExists())
    {
        return false;
    }

    wxArrayString subDirs;
    get_subdirs(postFix, subDirs);
    if (subDirs.IsEmpty()) return false;

    wxString topSubDir;
    if (subDirs[0].EndsWith('*', &topSubDir))
    {
        subDirs[0] = topSubDir;
        if (FindExecutable(dir, name, subDirs, exe))
        {
            return true;
        }

        wxArrayString topSubDirs;
        enum_subdirs(dir.GetFullPath(), topSubDir.Append("-*"), topSubDirs);
        for (wxArrayString::const_iterator i = topSubDirs.begin(), end = topSubDirs.end(); i != end; ++i)
        {
            subDirs[0] = *i;
            if (FindExecutable(dir, name, subDirs, exe))
            {
                return true;
            }
        }

        return false;
    }
    else
    {
        return FindExecutable(dir, name, subDirs, exe);
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
        FindExecutable(get_exe_local_sub_dir(), name, postFix, exe) ||
        FindExecutable(get_exe_sub_dir(), name, postFix, exe) ||
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

        case TOOL_MKVPROPEDIT:
        return FindExecutable("mkvpropedit", "MKVToolNix", exe);

        case TOOL_CMAKE:
        return FindExecutable("cmake", "CMake/bin", exe);

        case TOOL_IMAGE_MAGICK:
        return FindExecutable("magick", "ImageMagick*", exe);

        case TOOL_MUTOOL:
        return FindExecutable("mutool", "mupdf*", exe);

        default:
        return false;
    }
}

namespace
{
    void add_path_from_env_var(const wxString& env, wxArrayString& dirs)
    {
        wxString envVal;
        if (!wxGetEnv(env, &envVal))
        {
            return;
        }
        const wxFileName fn = wxFileName::DirName(envVal);
        for (wxArrayString::const_iterator i = dirs.begin(), end = dirs.end(); i != end; ++i)
        {
            const wxFileName cfn = wxFileName::DirName(*i);
            if (cfn.SameAs(fn)) return;
        }
        dirs.Add(envVal);
    }
}

void wxCmdTool::GetSearchDirectories(wxArrayString& searchDirs)
{
    searchDirs.Add(get_exe_local_sub_dir().GetFullPath());
    searchDirs.Add(get_exe_sub_dir().GetFullPath());
    add_path_from_env_var("LocalAppData", searchDirs);
    add_path_from_env_var("ProgramW6432", searchDirs);
    add_path_from_env_var("ProgramFiles", searchDirs);
    add_path_from_env_var("ProgramFiles(x86)", searchDirs);
}


