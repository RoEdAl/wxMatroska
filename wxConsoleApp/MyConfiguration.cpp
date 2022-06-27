/*
 *      MyConfiguration.cpp
 */
#include <wxConsoleApp/MyConfiguration.h>

wxString MyConfiguration::BoolToStr(bool b)
{
    return b ? "yes" : "no";
}

wxString MyConfiguration::BoolToStr(const std::optional<bool>& b)
{
    if (b.has_value())
    {
        return BoolToStr(b.value());
    }
    else
    {
        return "<none>";
    }
}

wxString MyConfiguration::ToString(bool b)
{
    return b ? _("on") : _("off");
}

bool MyConfiguration::ReadNegatableSwitchValue(const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal)
{
    wxCmdLineSwitchState state = cmdLine.FoundSwitch(name);
    bool                 res = true;

    switch (state)
    {
        case wxCMD_SWITCH_ON:
        {
            switchVal = true;
            break;
        }

        case wxCMD_SWITCH_OFF:
        {
            switchVal = false;
            break;
        }

        default:
        {
            res = false;
            break;
        }
    }

    return res;
}

bool MyConfiguration::ReadNegatableSwitchValue(const wxCmdLineParser& cmdLine, const wxString& name, std::optional<bool>& switchVal)
{
    wxCmdLineSwitchState state = cmdLine.FoundSwitch(name);
    bool                 res = true;

    switch (state)
    {
        case wxCMD_SWITCH_ON:
        {
            switchVal = true;
            break;
        }

        case wxCMD_SWITCH_OFF:
        {
            switchVal = false;
            break;
        }

        default:
        {
            res = false;
            break;
        }
    }

    return res;
}

bool MyConfiguration::ReadNegatableSwitchValueAndNegate(const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal)
{
    if (ReadNegatableSwitchValue(cmdLine, name, switchVal))
    {
        switchVal = !switchVal;
        return true;
    }
    else
    {
        return false;
    }
}

bool MyConfiguration::ReadNegatableSwitchValueAndNegate(const wxCmdLineParser& cmdLine, const wxString& name, std::optional<bool>& switchVal)
{
    if (ReadNegatableSwitchValue(cmdLine, name, switchVal))
    {
        switchVal = !switchVal.value();
        return true;
    }
    else
    {
        return false;
    }
}
