/*
    wxCoverFile.cpp
*/

#include "StdWx.h"
#include <wxCueFile/wxCoverFile.h>
#include "wxMd5.h"

// ===============================================================================

const wxChar* const wxCoverFile::CoverNames[] =
{
    wxS( "cover" ),
    wxS( "front" ),
    wxS( "folder" ),
    wxS( "picture" )
};

const size_t wxCoverFile::CoverNamesSize = WXSIZEOF( wxCoverFile::CoverNames );

// ===============================================================================

const wxChar* const wxCoverFile::CoverExts[] =
{
    wxS( "jpg" ),
    wxS( "jpeg" ),
    wxS( "png" )
};

const size_t wxCoverFile::CoverExtsSize = WXSIZEOF( wxCoverFile::CoverExts );


// ===============================================================================

wxCoverFile::wxCoverFile()
{}

wxCoverFile::wxCoverFile( const wxCoverFile& cf )
:m_fileName( cf.m_fileName ), m_data( cf.m_data ), m_mimeType( cf.m_mimeType ), m_description( cf.m_description ), m_checksum( cf.m_checksum )
{}

wxCoverFile::wxCoverFile( const wxFileName& fn )
: m_fileName( fn )
{
    m_checksum = wxMD5::Get( fn );
    wxScopedPtr<wxFileType> pFileType( wxTheMimeTypesManager->GetFileTypeFromExtension( fn.GetExt() ) );
    if (pFileType.get() != nullptr)
    {
        wxString mimeType;
        if (pFileType->GetMimeType( &mimeType ))
        {
            m_mimeType = mimeType;
        }
    }
}

wxCoverFile::wxCoverFile( const wxMemoryBuffer& data, const wxString& mimeType, const wxString& description )
: m_data( data ), m_mimeType( mimeType ), m_description( description ), m_checksum( wxMD5::Get( data ) )
{}

const wxFileName& wxCoverFile::GetFileName() const
{
    return m_fileName;
}

const wxMemoryBuffer& wxCoverFile::GetData() const
{
    return m_data;
}

const wxString& wxCoverFile::GetMimeType() const
{
    return m_mimeType;
}

const wxString& wxCoverFile::GetDescription() const
{
    return m_description;
}

const wxMemoryBuffer& wxCoverFile::GetChecksum() const
{
    return m_checksum;
}

wxString wxCoverFile::GetExt() const
{
    if (HasFileName())
    {
        return m_fileName.GetExt();
    }
    else if (HasMimeType())
    {
        wxScopedPtr<wxFileType> pFileType( wxTheMimeTypesManager->GetFileTypeFromMimeType( m_mimeType ) );
        if (pFileType.get() != nullptr)
        {
            wxArrayString exts;
            if (pFileType->GetExtensions( exts ) && !exts.IsEmpty() )
            {
                return exts[0];
            }
        }
    }

    return wxEmptyString;
}

bool wxCoverFile::Save( const wxFileName& fn )
{
    wxASSERT( HasData() );
    wxASSERT( !HasFileName() );

    wxFileOutputStream fos( fn.GetFullPath() );
    if (fos.IsOk())
    {
        wxLogInfo( _( "Creating image file \u201C%s\u201D" ), fn.GetFullName( ) );
        fos.Write( m_data.GetData(), m_data.GetDataLen() );
        fos.Close();
        m_fileName = fn;
        return true;
    }
    else
    {
        wxLogError( _( "Fail to save image to \u201C%s\u201D" ), fn.GetFullName( ) );
        return false;
    }
}

void wxCoverFile::Append( wxArrayCoverFile& ar, const wxCoverFile& cover )
{
    bool bAdd = true;

    for (size_t i = 0, nSize = ar.GetCount(); i < nSize; ++i)
    {
        if (wxMD5::AreEqual( cover.GetChecksum( ), ar[i].GetChecksum( ) ))
        {
            bAdd = false;
            break;
        }
    }

    if (bAdd)
    {
        ar.Add( cover );
    }
}

void wxCoverFile::Append( wxArrayCoverFile& ar, const wxArrayCoverFile& covers )
{
    for (size_t i = 0, nSize = covers.GetCount(); i < nSize; ++i) Append( ar, covers[i] );
}

bool wxCoverFile::IsCoverFile( const wxFileName& fileName )
{
    for (size_t i = 0; i < CoverExtsSize; i++)
    {
        if (fileName.GetExt( ).CmpNoCase( CoverExts[i] ) == 0)
        {
            return true;
        }
    }

    return false;
}

bool wxCoverFile::GetCoverFile( const wxDir& sourceDir, const wxString& sFileNameBase, wxFileName& coverFile )
{
    wxASSERT( sourceDir.IsOpened( ) );

    wxString sFileName;
    size_t	 nCounter = 0;
    wxString sFileSpec( wxString::Format( wxS( "%s.*" ), sFileNameBase ) );

    if (sourceDir.GetFirst( &sFileName, sFileSpec, wxDIR_FILES ))
    {
        while (true)
        {
            wxFileName fileName( sourceDir.GetName( ), sFileName );

            if (IsCoverFile( fileName ))
            {
                coverFile = fileName;
                return true;
            }

            if (!sourceDir.GetNext( &sFileName ))
            {
                break;
            }
        }
    }

    return false;
}

bool wxCoverFile::GetCoverFile( const wxFileName& inputFile, wxFileName& coverFile )
{
    wxFileName sourceDirFn( inputFile );

    sourceDirFn.SetName( wxEmptyString );
    sourceDirFn.ClearExt( );
    wxASSERT( sourceDirFn.IsDirReadable( ) );

    wxDir sourceDir( sourceDirFn.GetPath( ) );

    if (!sourceDir.IsOpened( ))
    {
        wxLogError( _( "Fail to open directory \u201C%s\u201D" ), sourceDirFn.GetPath( ) );
        return false;
    }

    for (size_t i = 0; i < CoverNamesSize; i++)
    {
        if (GetCoverFile( sourceDir, CoverNames[i], coverFile ))
        {
            return true;
        }
    }

    return false;
}

// ===============================================================================

#include <wx/arrimpl.cpp>	// this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayCoverFile );
