/*
 * wxApp.h
 */

#ifndef _WX_MY_APP_H_
#define _WX_MY_APP_H_

class wxMyApp:
    public wxApp
{
    protected:

    virtual void OnInitCmdLine(wxCmdLineParser& parser) wxOVERRIDE;
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) wxOVERRIDE;
    virtual bool OnInit() wxOVERRIDE;

    public:

    static const char APP_NAME[];
    static const char APP_VERSION[];
    static const char APP_VENDOR_NAME[];
    static const char APP_AUTHOR[];
    static const char LICENSE_FILE_NAME[];

    const wxFileName& GetCue2MkcPath() const;
    const wxFileName& GetMkvmergePath() const;
    const wxFileName& GetMkvPropEditPath() const;
    const wxFileName& GetFfmpegPath() const;
    const wxFileName& GetFfprobePath() const;

    bool MaterialDesignIconsFound() const;
    const wxFileName& GetMaterialDesignIconsPath() const;
    bool LoadMaterialDesignIcon(const wxWindow* const, const wxString&, wxBitmapBundle&) const;
    bool LoadMaterialDesignIcon(const wxString&, const wxWindowVariant, wxBitmapBundle&) const;

    void ShowToolPaths() const;

    bool ShowLogTimestamps(bool showTimestamps = true);

    protected:

    wxFileName m_cue2MkcPath;
    wxFileName m_mkvmergePath;
    wxFileName m_mkvPropEditPath;
    wxFileName m_ffmpegPath;
    wxFileName m_ffprobePath;
    wxFileName m_materialDesignIconsPath;

    public:

    wxMyApp(void);
};

wxDECLARE_APP(wxMyApp);
#endif

