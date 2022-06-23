/*
 * wxTagSynonims.cpp
 */

#include <wxCueFile/wxCueTag.h>
#include <wxCueFile/wxTagSynonims.h>
#include <wxCueFile/wxCueComponent.h>
 
// ===============================================================================

wxTagSynonims::wxTagSynonims(void)
{
}

wxTagSynonims::wxTagSynonims(const wxString& name):
    m_name(name)
{
    wxASSERT(!name.IsEmpty());
}

wxTagSynonims::wxTagSynonims(const wxString& name, const wxArrayString& synonims):
    m_name(name), m_synonims(synonims)
{
    wxASSERT(!name.IsEmpty());
}

wxTagSynonims::wxTagSynonims(const wxTagSynonims& synonims)
{
    copy(synonims);
}

wxTagSynonims& wxTagSynonims::operator =(const wxTagSynonims& synonims)
{
    copy(synonims);
    return *this;
}

void wxTagSynonims::copy(const wxTagSynonims& synonims)
{
    m_name = synonims.m_name;
    m_synonims = synonims.m_synonims;
}

wxTagSynonims& wxTagSynonims::SetName(const wxString& name)
{
    wxASSERT(!name.IsEmpty());
    m_name = name;
    return *this;
}

wxTagSynonims& wxTagSynonims::SetSynonims(const wxArrayString& synonims)
{
    m_synonims = synonims;
    return *this;
}

const wxString& wxTagSynonims::GetName() const
{
    return m_name;
}

const wxArrayString& wxTagSynonims::GetSynonims() const
{
    return m_synonims;
}

bool wxTagSynonims::GetName(const wxString& synonim, wxString& name) const
{
    if (m_name.CmpNoCase(synonim) == 0)
    {
        name = m_name;
        return true;
    }

    for (size_t i = 0, cnt = m_synonims.GetCount(); i < cnt; ++i)
    {
        if (m_synonims[i].CmpNoCase(synonim) == 0)
        {
            name = m_synonims[i];
            return true;
        }
    }

    name = synonim;
    return false;
}

bool wxTagSynonims::GetName(const wxCueTag& synonim, wxCueTag& cueTag) const
{
    if (m_name.CmpNoCase(synonim.GetName()) == 0)
    {
        cueTag.SetSource(synonim.GetSource());
        cueTag.SetName(m_name);
        cueTag.SetValue(synonim.GetValue());
        return true;
    }

    for (size_t i = 0, cnt = m_synonims.GetCount(); i < cnt; ++i)
    {
        if (m_synonims[i].CmpNoCase(synonim.GetName()) == 0)
        {
            cueTag.SetSource(synonim.GetSource());
            cueTag.SetName(m_name);
            cueTag.SetValue(synonim.GetValue());
            return true;
        }
    }

    cueTag = synonim;
    return false;
}

// =================================================================================

wxTagSynonimsCollection::wxTagSynonimsCollection(void)
{
}

wxTagSynonimsCollection::wxTagSynonimsCollection(const wxTagSynonimsCollection& src):
    _wxArrayTagSynonims(src)
{
}

wxTagSynonimsCollection& wxTagSynonimsCollection::operator =(const wxTagSynonimsCollection& src)
{
    _wxArrayTagSynonims::operator =(src);

    return *this;
}

bool wxTagSynonimsCollection::GetName(const wxString& synonim, wxString& name) const
{
    for (size_t i = 0, cnt = GetCount(); i < cnt; ++i)
    {
        if (Item(i).GetName(synonim, name)) return true;
    }

    name = synonim;
    return false;
}

bool wxTagSynonimsCollection::GetName(const wxCueTag& synonim, wxCueTag& cueTag) const
{
    for (size_t i = 0, cnt = GetCount(); i < cnt; ++i)
    {
        if (Item(i).GetName(synonim, cueTag)) return true;
    }

    cueTag = synonim;
    return false;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(_wxArrayTagSynonims);

