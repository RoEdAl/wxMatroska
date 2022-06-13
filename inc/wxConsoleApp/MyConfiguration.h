/*
 *      MyConfiguration.h
 */
#ifndef _MY_CONFIGURATION_H_
#define _MY_CONFIGURATION_H_

class MyConfiguration:
    public wxObject
{
    public:

    MyConfiguration(void);

    static bool ReadNegatableSwitchValue(const wxCmdLineParser&, const wxString&, bool&);
    static bool ReadNegatableSwitchValue(const wxCmdLineParser&, const wxString&, std::optional<bool>&);
    static bool ReadNegatableSwitchValueAndNegate(const wxCmdLineParser&, const wxString&, bool&);
    static bool ReadNegatableSwitchValueAndNegate(const wxCmdLineParser&, const wxString&, std::optional<bool>&);

    protected:

    static wxString BoolToStr(bool);
    static wxString BoolToStr(const std::optional<bool>&);
    static wxString ToString(bool);
};

#endif

