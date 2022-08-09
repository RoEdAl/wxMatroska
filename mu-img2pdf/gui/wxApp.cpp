/*
 * wxApp.cpp
 */

#include <app_config.h>
#include <wxCmdTools/wxCmdTool.h>
#include "wxApp.h"
#include "wxMainFrame.h"
#include "VariantExt.h"

 // ===============================================================================

const char wxMyApp::APP_NAME[] = "img2pdf-frontend";
const char wxMyApp::APP_VERSION[] = WXMATROSKA_VERSION_STR;
const char wxMyApp::APP_VENDOR_NAME[] = "Edmunt Pienkowsky";
const char wxMyApp::APP_AUTHOR[] = "Edmunt Pienkowsky - roed@onet.eu";
const char wxMyApp::LICENSE_FILE_NAME[] = "license.txt";

// ===============================================================================

namespace
{
#pragma pack(push)
#pragma pack(2)

    // icon entry in the icon directory resource
    typedef struct
    {
        BYTE   bWidth;               // Width, in pixels, of the image
        BYTE   bHeight;              // Height, in pixels, of the image
        BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
        BYTE   bReserved;            // Reserved
        WORD   wPlanes;              // Color Planes
        WORD   wBitCount;            // Bits per pixel
        DWORD  dwBytesInRes;         // how many bytes in this resource?
        WORD   nID;                  // the ID
    } GRPICONDIRENTRY, * LPGRPICONDIRENTRY;

    // icon directory resource
    typedef struct
    {
        WORD            idReserved;   // Reserved (must be 0)
        WORD            idType;       // Resource type (1 for icons)
        WORD            idCount;      // How many images?
        GRPICONDIRENTRY idEntries[1]; // The entries for each image
    } GRPICONDIR, * LPGRPICONDIR;

#pragma pack(pop)

    bool load_from_resource(wxVector<wxIcon>& icons, const wxString& name, WXHINSTANCE module)
    {
        const void* data = NULL;
        size_t outLen = 0;

        // load the icon directory resource
        if (!wxLoadUserResource(&data, &outLen, name, RT_GROUP_ICON, module))
        {
            wxLogError(_("Failed to load icons from resource '%s'."), name);
            return false;
        }

        // load the individual icons referred from the icon directory
        const GRPICONDIR* const grpIconDir = static_cast<const GRPICONDIR*>(data);

        for (WORD i = 0; i < grpIconDir->idCount; ++i)
        {
            const GRPICONDIRENTRY& iconDir = grpIconDir->idEntries[i];
            const WORD iconID = iconDir.nID;

            if (wxLoadUserResource(&data, &outLen, wxString::Format(wxS("#%u"), iconID), RT_ICON, module))
            {
                const WXHICON hIcon = CreateIconFromResourceEx(
                    static_cast<PBYTE>(const_cast<void*>(data)),
                    static_cast<DWORD>(outLen),
                    TRUE,
                    0x00030000,
                    0, 0,
                    LR_DEFAULTCOLOR);
                if (hIcon == NULL)
                {
                    wxLogDebug(wxS("Failed to load icon from resource with id %u."), iconID);
                    continue;
                }

                wxIcon icon;

                if (icon.CreateFromHICON(hIcon))
                {
                    const WORD iconDepth = iconDir.wBitCount * iconDir.wPlanes;
                    if (iconDepth > 0) icon.SetDepth(iconDepth);
                    icons.push_back(icon);
                }
                else
                {
                    DestroyIcon(hIcon);
                    wxLogDebug(wxS("Failed to create icon from resource with id %u."), iconID);
                }
            }
            else
            {
                wxLogDebug(wxS("Failed to load icon with id %u for group icon resource '%s'."), iconID, name);
            }
        }

        return true;
    }

    class IconBundleHolder
    {
        wxDECLARE_NO_COPY_CLASS(IconBundleHolder);

        public:

        IconBundleHolder(wxIconBundle& iconBundle, const int iconPos)
            :m_iconBundle(iconBundle), m_iconPos(iconPos), m_pos(0), m_iconsLoaded(false)
        {
        }

        void Load(WXHINSTANCE hModule)
        {
            if (m_iconPos < 0)
            {
                const WXWORD iconId = static_cast<WXWORD>(-m_iconPos);
                const wxString iconIdStr = wxString::Format(wxS("#%u"), iconId);
                if (!(m_iconsLoaded = load_from_resource(m_icons, iconIdStr, hModule)))
                {
                    return;
                }
            }
            else
            {
                EnumResourceNames(hModule, RT_GROUP_ICON, ResEnumProc, reinterpret_cast<LONG_PTR>(this));
                if (!m_iconsLoaded)
                {
                    return;
                }
            }

            std::sort(m_icons.begin(), m_icons.end(), icon_cmp);
            for (wxVector<wxIcon>::const_iterator i = m_icons.begin(), end = m_icons.end(); i != end; ++i)
            {
                m_iconBundle.AddIcon(*i);
            }
        }

        bool IconsLoaded() const
        {
            return m_iconsLoaded;
        }

        protected:

        bool Process(WXHINSTANCE hModule, LPCWSTR pszName)
        {
            if (m_iconPos == 0 || ++m_pos == m_iconPos)
            {
                if (IS_INTRESOURCE(pszName))
                {
                    const wxString resName = wxString::Format("#%" wxLongLongFmtSpec "u", reinterpret_cast<ULONG_PTR>(pszName));
                    if (!load_from_resource(m_icons, resName, hModule)) return false;
                }
                else
                {
                    if (!load_from_resource(m_icons, pszName, hModule)) return false;
                }

                m_iconsLoaded = true;
            }
            return m_iconPos > 0 && m_pos < m_iconPos;
        }

        static bool Process(WXHINSTANCE hModule, LPCWSTR pszName, LONG_PTR paramThis)
        {
            IconBundleHolder* const pThis = reinterpret_cast<IconBundleHolder*>(paramThis);
            return pThis->Process(hModule, pszName);
        }

        static BOOL CALLBACK ResEnumProc(HMODULE hModule, LPCWSTR pszType, LPWSTR pszName, LONG_PTR param)
        {
            return Process(hModule, pszName, param);
        }

        static bool icon_cmp(const wxIcon& i1, const wxIcon& i2)
        {
            const wxSize sz1 = i1.GetSize();
            const wxSize sz2 = i2.GetSize();

            const int px1 = sz1.GetWidth() * sz1.GetHeight();
            const int px2 = sz2.GetWidth() * sz2.GetHeight();

            if (px1 < px2)
            {
                return true;
            }
            else if (px1 > px2)
            {
                return false;
            }
            else
            {
                const int d1 = i1.GetDepth();
                const int d2 = i2.GetDepth();

                return d1 < d2;
            }
        }

        protected:

        const int m_iconPos;
        int m_pos;
        bool m_iconsLoaded;
        wxVector<wxIcon> m_icons;
        wxIconBundle& m_iconBundle;
    };

    class ResourceModuleLoader
    {
        wxDECLARE_NO_COPY_CLASS(ResourceModuleLoader);

        public:

        ResourceModuleLoader(const wxString& moduleName)
        {
            m_hModule = LoadLibraryEx(moduleName.wc_str(), NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);
        }

        ~ResourceModuleLoader()
        {
            if (m_hModule != NULL) FreeLibrary(m_hModule);
        }

        operator bool() const
        {
            return m_hModule != NULL;
        }

        operator WXHINSTANCE() const
        {
            return m_hModule;
        }

        protected:

        WXHINSTANCE m_hModule;
    };

    bool load_icons(const wxIconLocation& iconLocation, wxIconBundle& iconBundle)
    {
        ResourceModuleLoader resourceLoader(iconLocation.GetFileName());
        if (!resourceLoader) return false;

        IconBundleHolder bundleHolder(iconBundle, iconLocation.GetIndex());
        bundleHolder.Load(resourceLoader);

        return bundleHolder.IconsLoaded();
    }

    bool get_file_type_icons(const wxString& fext, wxIconBundle& iconBundle)
    {
        const wxScopedPtr<wxFileType> ft(wxTheMimeTypesManager->GetFileTypeFromExtension(fext));
        if (!ft) return false;

        wxIconLocation iconLocation;
        if (!ft->GetIcon(&iconLocation)) return false;

        return load_icons(iconLocation, iconBundle);
    }
}


void wxMyApp::fill_icon_map()
{
    {
        wxIconBundle icons;

        if (get_file_type_icons(".jpg", icons))
        {
            m_iconMap["jpg"] = icons;
        }
    }

    {
        wxIconBundle icons;
        if (get_file_type_icons(".png", icons))
        {
            m_iconMap["png"] = icons;
        }
    }

    {
        wxIconBundle icons;
        if (get_file_type_icons(".pdf", icons))
        {
            m_iconMap["pdf"] = icons;
        }
    }
}

namespace
{
    bool get_fn_ext(const wxFileName& fn, wxString& ext)
    {
        if (!fn.HasExt()) return false;
        ext = fn.GetExt();
        ext.LowerCase();
        return true;
    }
}

bool wxMyApp::GetFnColumn(const wxFileName& fn, wxVector<wxVariant>& column) const
{
    wxString ext;
    if (!get_fn_ext(fn, ext)) return false;

    try
    {
        const bool isPdf = ext.CmpNoCase("pdf") == 0;
        const wxSize elSize(isPdf ? 0 : -1, isPdf ? 0 : -1);

        column.push_back(wxVariant(wxBitmapBundle::FromIconBundle(m_iconMap.at(ext))));
        column.push_back(wxVariantDataFileName::Get(fn));
        column.push_back(wxVariantDataSize::Get(elSize));
        column.push_back(wxVariantDataSize::Get(elSize));
        return true;
    }
    catch (std::out_of_range WXUNUSED(oe))
    {
        return false;
    }
}

const wxIconBundle& wxMyApp::GetAppIcon() const
{
    try
    {
        return m_iconMap.at("pdf");
    }
    catch (std::out_of_range WXUNUSED(oe))
    {
        return m_appIcons;
    }
}

// ===============================================================================

wxIMPLEMENT_APP(wxMyApp);

wxMyApp::wxMyApp(void)
{
}

void wxMyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxApp::OnInitCmdLine(parser);

    parser.AddLongSwitch("log-timestamps", _("Show/hide log timestamps"), wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_SWITCH_NEGATABLE);
}

bool wxMyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    wxASSERT(parser.AreLongOptionsEnabled());

    const bool res = wxApp::OnCmdLineParsed(parser);

    if (res)
    {
        const wxCmdLineSwitchState state = parser.FoundSwitch("verbose");
        switch (state)
        {
            case wxCMD_SWITCH_ON:
            {
                // wxLog::SetLogLevel(wxLOG_Info);
                wxLog::SetVerbose(true);
                break;
            }

            default:
            {
                // wxLog::SetLogLevel(wxLOG_Message);
                wxLog::SetVerbose(false);
                break;
            }
        }

        if (parser.FoundSwitch("log-timestamps") == wxCMD_SWITCH_OFF) wxLog::DisableTimestamp();
    }

    return res;
}

bool wxMyApp::OnInit()
{
    SetAppName(APP_NAME);
    SetVendorName(APP_VENDOR_NAME);
    SetVendorDisplayName(APP_AUTHOR);

    if (!wxApp::OnInit()) return false;

    wxInitAllImageHandlers();
    fill_icon_map();

    wxLog::EnableLogging(false);
#ifdef NDEBUG
    wxDisableAsserts();
#endif

    m_scriptPath.Assign(wxStandardPaths::Get().GetExecutablePath());
    m_scriptPath.SetFullName("img2pdf.js");

    if (!m_scriptPath.IsFileReadable())
    {
    #ifdef NDEBUG
        wxLogDebug(_("Using img2pdf.js tool from current directory"));

        const wxString fullName(m_scriptPath.GetFullName());
        m_scriptPath.AssignCwd();
        m_scriptPath.SetFullName(fullName);
    #else
        const wxArrayString& dirs = m_scriptPath.GetDirs();
        m_scriptPath.RemoveLastDir();
        m_scriptPath.RemoveLastDir();
        m_scriptPath.AppendDir("js");
        m_scriptPath.AppendDir("public");
    #endif
    }

    if (!wxCmdTool::FindTool(wxCmdTool::TOOL_MUTOOL, m_muToolPath))
    {
        wxLogWarning(_("mutool tool not found."));
    }

    wxFrame* const pFrame = new wxMainFrame();
    pFrame->Show(true);

    return true;
}

const wxFileName& wxMyApp::GetScriptPath() const
{
    return m_scriptPath;
}

const wxFileName& wxMyApp::GetMuToolPath() const
{
    return m_muToolPath;
}

namespace
{
    void show_tool_path(const wxFileName& tool)
    {
        if (tool.IsOk() && tool.IsAbsolute())
        {
            wxLogMessage(_("[tool] %-15s: %s"), tool.GetName(), tool.GetFullPath());
        }
        else
        {
            wxLogMessage(_("[tool] %-15s: <not found>"), tool.GetName());
        }
    }
}

void wxMyApp::ShowToolPaths() const
{
    show_tool_path(m_muToolPath);
    show_tool_path(m_scriptPath);
}
