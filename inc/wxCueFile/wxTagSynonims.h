/*
 * wxTagSynonims.h
 */

#ifndef _WX_TAG_SYNONIMS_H_
#define _WX_TAG_SYNONIMS_H_

#ifndef _WX_CUE_TAG_H_
class wxCueTag;
#endif

class wxTagSynonims
{
    protected:

    wxString m_name;
    wxArrayString m_synonims;

    protected:

    void copy(const wxTagSynonims&);

    public:

    wxTagSynonims(void);
    wxTagSynonims(const wxString&);
    wxTagSynonims(const wxString&, const wxArrayString&);
    wxTagSynonims(const wxTagSynonims&);
    wxTagSynonims& operator =(const wxTagSynonims&);

    const wxString& GetName() const;
    bool GetName(const wxString&, wxString&) const;
    bool GetName(const wxCueTag&, wxCueTag&) const;
    const wxArrayString& GetSynonims() const;

    wxTagSynonims& SetName(const wxString&);
    wxTagSynonims& SetSynonims(const wxArrayString&);
};

WX_DECLARE_OBJARRAY(wxTagSynonims, _wxArrayTagSynonims);

class wxTagSynonimsCollection:
    public _wxArrayTagSynonims
{
    public:

    wxTagSynonimsCollection(void);
    wxTagSynonimsCollection(const wxTagSynonimsCollection&);
    wxTagSynonimsCollection& operator =(const wxTagSynonimsCollection&);

    public:

    bool GetName(const wxString&, wxString&) const;
    bool GetName(const wxCueTag&, wxCueTag&) const;
};

#endif

