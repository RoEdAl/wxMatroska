/*
 *      wxCueSheetContent.cpp
 */

#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxCueSheetContent.h>

 // ===============================================================================

wxCueSheetContent::wxCueSheetContent(void)
{
}

wxCueSheetContent::wxCueSheetContent(const wxString& sValue):
    m_sValue(sValue), m_embedded(false)
{
    wxASSERT(!sValue.IsEmpty());
}

wxCueSheetContent::wxCueSheetContent(const wxDataFile& source):
    m_sValue(source.GetCueSheet()), m_source(source), m_embedded(true)
{
    wxASSERT(source.GetFileName().IsOk() && !source.GetFileName().IsDir());
}

wxCueSheetContent::wxCueSheetContent(const wxString& sValue, const wxDataFile& source, bool bEmbedded):
    m_sValue(sValue), m_source(source), m_embedded(bEmbedded)
{
    wxASSERT(!sValue.IsEmpty());
    wxASSERT(source.GetFileName().IsOk() && !source.GetFileName().IsDir());
}

wxCueSheetContent::wxCueSheetContent(const wxCueSheetContent& csContent)
{
    copy(csContent);
}

wxCueSheetContent& wxCueSheetContent::operator =(const wxCueSheetContent& csContent)
{
    copy(csContent);
    return *this;
}

void wxCueSheetContent::copy(const wxCueSheetContent& csContent)
{
    m_source = csContent.m_source;
    m_sValue = csContent.m_sValue;
    m_embedded = csContent.m_embedded;
}

bool wxCueSheetContent::HasSource() const
{
    return m_source.GetFileName().IsOk();
}

const wxDataFile& wxCueSheetContent::GetSource() const
{
    wxASSERT(HasSource());
    return m_source;
}

const wxString& wxCueSheetContent::GetValue() const
{
    return m_sValue;
}

bool wxCueSheetContent::IsEmbedded() const
{
    return m_embedded;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayCueSheetContent);

