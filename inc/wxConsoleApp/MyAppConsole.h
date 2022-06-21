/*
 *      MyAppConsole.h
 */

#ifndef _MY_APP_CONSOLE_H
#define _MY_APP_CONSOLE_H

class MyAppConsole:
    public wxAppConsole
{
    public:

    MyAppConsole(void);
#if defined( __WXMSW__ ) && defined( __VISUALC__ ) && defined( UNICODE )
    int GetTranslationMode() const;
#endif

    static const char APP_VENDOR_NAME[];
    static const char APP_AUTHOR[];
    static const char LICENSE_FILE_NAME[];

    protected:

    static wxDateTime GetNow();
    static bool CheckLicense();
    static void ShowLicense(wxMessageOutput&);

    protected:

    virtual bool OnInit();
    virtual wxAppTraits* CreateTraits();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
#if defined( __WXMSW__ ) && defined( __VISUALC__ ) && defined( UNICODE )
    virtual bool OnCmdLineHelp(wxCmdLineParser& parser);
#endif
    virtual bool OnCmdLineParsed(wxCmdLineParser&);

    virtual void OnAssertFailure(const wxChar*, int, const wxChar*, const wxChar*, const wxChar*);
    virtual int OnExit();

    void AddSeparator(wxCmdLineParser&);

    private:

#if defined( __WXMSW__ ) && defined( __VISUALC__ ) && defined( UNICODE )
    bool ProcessOutputCharset(wxCmdLineParser&);
#endif

    protected:

    wxString m_sSeparator;
#if defined( __WXMSW__ ) && defined( __VISUALC__ ) && defined( UNICODE )
    int m_translationMode;
#endif

};

#endif

