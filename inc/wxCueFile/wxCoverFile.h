/*
    wxCoverFile.h
*/

#ifndef _WX_COVER_FILE_H_
#define _WX_COVER_FILE_H_

class wxCoverFile;

WX_DECLARE_OBJARRAY( wxCoverFile, wxArrayCoverFile );

class wxCoverFile
{
    public:

    static const wxChar* const CoverNames[];
    static const size_t		   CoverNamesSize;

    static const wxChar* const CoverExts[];
    static const size_t		   CoverExtsSize;

    wxCoverFile();
    wxCoverFile( const wxCoverFile& );
    wxCoverFile( const wxFileName& );
    wxCoverFile( const wxMemoryBuffer&, const wxString&, const wxString& );

    public:

    const wxFileName& GetFileName() const;
    const wxMemoryBuffer& GetData() const;
    const wxString& GetMimeType() const;
    const wxString& GetDescription() const;
    const wxMemoryBuffer& GetChecksum() const;
    wxString GetExt() const;

    static bool GetMimeFromExt( const wxFileName&, wxString& );

    bool HasFileName() const
    {
        return m_fileName.IsOk();
    }

    bool HasData() const
    {
        return !m_data.IsEmpty();
    }

    bool HasMimeType() const
    {
        return !m_mimeType.IsEmpty();
    }

    bool HasDescription() const
    {
        return !m_description.IsEmpty();
    }

    static void Append( wxArrayCoverFile&, const wxCoverFile& );
    static void Append( wxArrayCoverFile&, const wxArrayCoverFile& );

    static bool IsCoverFile( const wxFileName& );
    static bool GetCoverFile( const wxDir&, const wxString&, wxFileName& );
    static bool GetCoverFile( const wxFileName&, wxFileName& );

    bool Save( const wxFileName& );

    protected:

    wxFileName m_fileName;
    wxMemoryBuffer m_data;
    wxString m_mimeType;
    wxString m_description;
    wxMemoryBuffer m_checksum;
};

#endif