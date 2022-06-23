/*
 * wxCMakeScriptRenderer.cpp
 */

#include "wxCMakeScriptRenderer.h"
#include "wxApp.h"

// ===============================================================================

wxCMakeScriptRenderer::wxCMakeScriptRenderer(const wxConfiguration& cfg):
    wxPrimitiveRenderer(cfg)
{
}

wxString wxCMakeScriptRenderer::GetCMakePath(const wxFileName& path)
{
	if (path.HasVolume())
	{
		wxString res = path.GetVolume();
		res << wxFileName::GetVolumeSeparator() << path.GetFullPath(wxPATH_UNIX);
		return res;
	}
	else
	{
		return path.GetFullPath(wxPATH_UNIX);
	}
}

void wxCMakeScriptRenderer::RenderHeader(const wxInputFile& inputFile) const
{
	const wxFileName scriptFn = m_cfg.GetOutputFile(inputFile, wxConfiguration::EXT::CMAKE_SCRIPT);

	*m_os << '#' << endl;
    *m_os << _("# This file was created by ") << wxGetApp().GetAppDisplayName() << endl;
	*m_os << _("# You can invoke this script manually by typing:") << endl;
	*m_os << _("#    cmake -P ") << scriptFn.GetFullName() << endl;
	*m_os << '#' << endl;

    {
        wxArrayString cfgDesc;
        m_cfg.ToArray(cfgDesc);

        for (wxArrayString::const_iterator i = cfgDesc.begin(), end = cfgDesc.end(); i != end; ++i)
        {
            *m_os << "# CFG " << *i << endl;
        }

        *m_os << endl;
    }
}

void wxCMakeScriptRenderer::RenderMinimumVersion() const
{
    *m_os << "CMAKE_MINIMUM_REQUIRED(VERSION 3.21)" << endl << endl;
}

void wxCMakeScriptRenderer::RenderToolFinder(const wxString& toolName, const wxString& pathSuffix) const
{
	const wxString toolNameUpper = toolName.Upper();

	*m_os << "# " << "Find " << toolName << " executable" << endl;
	*m_os << "IF (NOT " << toolNameUpper << ')' << endl;
	*m_os << "    FIND_PROGRAM(CMAKE_" << toolNameUpper << " " << toolName << endl;
	*m_os << "        HINTS" << endl;
	*m_os << "            ENV LocalAppData" << endl;
	*m_os << "            ENV ProgramW6432" << endl;
	*m_os << "            ENV ProgramFiles" << endl;
	*m_os << "            ENV \"ProgramFiles(x86)\"" << endl;
	*m_os << "            \"$ENV{SystemDrive}/Program Files\"" << endl;
	*m_os << "            \"$ENV{SystemDrive}/Program Files (x86)\"" << endl;
	*m_os << "        PATH_SUFFIXES " << pathSuffix << endl;
	*m_os << "        NO_CACHE" << endl;
	*m_os << "        REQUIRED" << endl;
	*m_os << "        NO_DEFAULT_PATH" << endl;
	*m_os << "        NO_PACKAGE_ROOT_PATH" << endl;
	*m_os << "        NO_CMAKE_PATH" << endl;
	*m_os << "        NO_CMAKE_ENVIRONMENT_PATH" << endl;
	*m_os << "        NO_CMAKE_SYSTEM_PATH" << endl;
	*m_os << "        NO_CMAKE_FIND_ROOT_PATH" << endl;
	*m_os << "    )" << endl;
	*m_os << "    CMAKE_PATH(NATIVE_PATH CMAKE_" << toolNameUpper << ' ' << toolNameUpper << ')' << endl;
	*m_os << "ELSEIF(NOT EXISTS ${" << toolNameUpper << "})" << endl;
	*m_os << "    MESSAGE(FATAL_ERROR \"" << toolName << " not found - ${" << toolNameUpper << "}\")" << endl;
	*m_os << "ENDIF()" << endl << endl;
}

void wxCMakeScriptRenderer::RenderFfmpegFinder() const
{
	RenderToolFinder("ffmpeg", "ffmpeg/bin");
}

void wxCMakeScriptRenderer::RenderToolEnvCheck(const wxString& toolName) const
{
	const wxString toolNameUpper = toolName.Upper();

	*m_os << "IF(DEFINED " << toolNameUpper << ')' << endl;
	*m_os << "    MESSAGE(DEBUG \"" << toolNameUpper << " set to ${" << toolNameUpper << "}\")" << endl;
	*m_os << "ELSEIF(DEFINED ENV{" << toolNameUpper << "})" << endl;
	*m_os << "    CMAKE_PATH(SET " << toolNameUpper << " $ENV{" << toolNameUpper << "})" << endl;
	*m_os << "    MESSAGE(DEBUG \"" << toolNameUpper << " set to ${" << toolNameUpper << "} via environment variable\")" << endl;
	*m_os << "ELSE()" << endl;
	*m_os << "    MESSAGE(FATAL_ERROR \"Required variable " << toolNameUpper << " is not defined\")" << endl;
	*m_os << "ENDIF()" << endl;
}
