/*
 *  wxCoverFile.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxCoverFile.h>
#include "wxMd5.h"

// ===============================================================================

const char* const wxCoverFile::CoverNames[] =
{
	"cover",
	"front",
	"folder",
	"picture"
};

// ===============================================================================

const wxBitmapType wxCoverFile::CoverFileTypes[] =
{
    wxBITMAP_TYPE_JPEG,
    wxBITMAP_TYPE_PNG,
    wxBITMAP_TYPE_GIF,
    wxBITMAP_TYPE_BMP
};

// ===============================================================================

const wxULongLong wxCoverFile::MAX_FILE_SIZE = wxUINT32_MAX;

// ===============================================================================

const wxCoverFile::TypeName wxCoverFile::TypeNames[] =
{
	{ wxCoverFile::FrontCover, "FrontCover" },
	{ wxCoverFile::BackCover, "BackCover" },
	{ wxCoverFile::FileIcon, "FileIcon" },
	{ wxCoverFile::LeafletPage, "LeafletPage" },
	{ wxCoverFile::Media, "Media" },
	{ wxCoverFile::LeadArtist, "LeadArtist" },
	{ wxCoverFile::Artist, "Artist" },
	{ wxCoverFile::Conductor, "Conductor" },
	{ wxCoverFile::Band, "Band" },
	{ wxCoverFile::Composer, "Composer" },
	{ wxCoverFile::Lyricist, "Lyricist" },
	{ wxCoverFile::RecordingLocation, "RecordingLocation" },
	{ wxCoverFile::DuringRecording, "DuringRecording" },
	{ wxCoverFile::DuringPerformance, "DuringPerformance" },
	{ wxCoverFile::MovieScreenCapture, "MovieScreenCapture" },
	{ wxCoverFile::ColouredFish, "ColouredFish" },
	{ wxCoverFile::Illustration, "Illustration" },
	{ wxCoverFile::BandLogo, "BandLogo" },
	{ wxCoverFile::PublisherLogo, "PublisherLogo" },
	{ wxCoverFile::OtherFileIcon, "OtherFileIcon" }
};

// ===============================================================================

namespace
{
    // how to avoid memory copying
    wxMemoryBuffer memory_stream_to_buffer( const wxMemoryOutputStream& os )
    {
        size_t nSize = os.GetLength( );
        wxMemoryBuffer data( nSize );
        void* pData = data.GetWriteBuf( nSize );
        os.CopyTo( pData, nSize );
        data.UngetWriteBuf(nSize);
        return data;
    }
}

wxCoverFile::wxCoverFile()
{}

wxCoverFile::wxCoverFile( const wxCoverFile& cf ):
m_fileName( cf.m_fileName ), m_data( cf.m_data ), m_mimeType( cf.m_mimeType ), m_description( cf.m_description ), m_checksum( cf.m_checksum ), m_type( cf.m_type ), m_fileSize( cf.m_fileSize )
{}

bool wxCoverFile::GetMimeFromExt( const wxFileName& fn, wxString& mimeType )
{
	wxScopedPtr< wxFileType > pFileType( wxTheMimeTypesManager->GetFileTypeFromExtension( fn.GetExt() ) );

	if ( pFileType.get() != nullptr )
	{
		wxString s;

		if ( pFileType->GetMimeType( &s ) )
		{
			mimeType = s;
			return true;
		}
		else
		{
			wxLogWarning( _( "Could not find mime type for extension %s" ), fn.GetExt() );
			return false;
		}
	}
	return false;
}

wxCoverFile::wxCoverFile( const wxFileName& fn, wxCoverFile::Type type ):
	m_fileName( fn ), m_type( type )
{
	m_checksum = wxMD5::Get( fn );
	GetMimeFromExt( fn, m_mimeType );
	m_fileSize = fn.GetSize();
}

wxCoverFile::wxCoverFile( const wxMemoryBuffer& data, wxCoverFile::Type type, const wxString& mimeType, const wxString& description ):
	m_data( data ), m_mimeType( mimeType ), m_description( description ), m_checksum( wxMD5::Get( data ) ), m_type( type )
{}

wxCoverFile::wxCoverFile( const wxMemoryBuffer& data, const wxMemoryBuffer& checksum, wxCoverFile::Type type, const wxString& mimeType, const wxString& description ) :
m_data( data ), m_mimeType( mimeType ), m_description( description ), m_checksum( checksum ), m_type( type )
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
	if ( HasFileName() )
	{
		return m_fileName.GetExt();
	}
	else if ( HasMimeType() )
	{
		wxScopedPtr< wxFileType > pFileType( wxTheMimeTypesManager->GetFileTypeFromMimeType( m_mimeType ) );

		if ( pFileType.get() != nullptr )
		{
			wxArrayString exts;

			if ( pFileType->GetExtensions( exts ) && !exts.IsEmpty() )
			{
				wxString ext;

				if ( exts[ 0 ].StartsWith( '.', &ext ) )
				{
					return ext;
				}
				else
				{
					return exts[ 0 ];
				}
			}
		}
	}

	return wxEmptyString;
}

wxCoverFile::Type wxCoverFile::GetType() const
{
	return m_type;
}

size_t wxCoverFile::GetSize() const
{
	if ( HasFileName() )
	{
		wxASSERT( m_fileSize < MAX_FILE_SIZE );
		return m_fileSize.GetLo();
	}
	else
	{
		return m_data.GetDataLen();
	}
}

bool wxCoverFile::IsTypeOf( const wxImageHandler* const handler) const
{
    return handler->GetMimeType( ).Cmp( m_mimeType ) == 0;
}

wxCoverFile wxCoverFile::Load() const
{
    if (HasData() || !HasFileName()) return  wxCoverFile( *this );

    if (GetSize() > 0)
    {
        wxFileInputStream fs( m_fileName.GetFullPath( ) );
        if (fs.IsOk( ))
        {
            wxMemoryOutputStream ms;

            fs.Read( ms );
            return wxCoverFile( memory_stream_to_buffer( ms ), m_checksum, m_type, m_mimeType, m_description );
        }
    }

    return wxCoverFile( *this );
}

bool wxCoverFile::Save( const wxFileName& fn )
{
	wxASSERT( HasData() );
	wxASSERT( !HasFileName() );

	wxFileOutputStream fos( fn.GetFullPath() );

	if ( fos.IsOk() )
	{
		wxLogInfo( _( "Creating image file \u201C%s\u201D" ), fn.GetFullName() );
		fos.Write( m_data.GetData(), m_data.GetDataLen() );
		fos.Close();
		m_fileName = fn;
		m_fileSize = m_data.GetDataLen();
		return true;
	}
	else
	{
		wxLogError( _( "Fail to save image to \u201C%s\u201D" ), fn.GetFullName() );
		return false;
	}
}

void wxCoverFile::Append( wxArrayCoverFile& ar, const wxCoverFile& cover )
{
	bool bAdd = true;

	for ( size_t i = 0, nSize = ar.GetCount(); i < nSize; ++i )
	{
		if ( wxMD5::AreEqual( cover.GetChecksum(), ar[ i ].GetChecksum() ) )
		{
			bAdd = false;
			break;
		}
	}

	if ( bAdd )
	{
		ar.Add( cover );
	}
}

void wxCoverFile::Append( wxArrayCoverFile& ar, const wxArrayCoverFile& covers )
{
	for ( size_t i = 0, nSize = covers.GetCount(); i < nSize; ++i )
	{
		Append( ar, covers[ i ] );
	}
}

template<size_t SIZE>
bool wxCoverFile::IsCoverFile( const wxFileName& fileName, const wxBitmapType (&coverTypes)[SIZE] )
{
    wxString sExt;

	for ( size_t i = 0; i < SIZE && sExt.IsEmpty(); ++i )
	{
        wxImageHandler* handler = wxImage::FindHandler( coverTypes[i] );
        wxASSERT( handler != nullptr );

        if (handler->GetExtension( ).CmpNoCase( fileName.GetExt( ) ) == 0)
        {
            sExt = handler->GetExtension();
            continue;
        }

        const wxArrayString& altExt = handler->GetAltExtensions( );
        for (size_t i = 0, nSize = altExt.GetCount( ); i < nSize; ++i)
        {
            if (altExt[i].CmpNoCase( fileName.GetExt( ) ) == 0)
            {
                sExt = altExt[i];
                break;
            }
        }
	}

    if (sExt.IsEmpty())
    {
        return false;
    }
    else
    {
        wxULongLong fsize = fileName.GetSize( );
        return ( fsize > 100 && fsize < MAX_FILE_SIZE);
    }
}

bool wxCoverFile::IsCoverFile( const wxFileName& fileName )
{
    return IsCoverFile( fileName, CoverFileTypes );
}

bool wxCoverFile::Find( const wxDir& sourceDir, const wxString& sFileNameBase, wxFileName& coverFile )
{
	wxASSERT( sourceDir.IsOpened() );

	wxString sFileName;
	size_t	 nCounter = 0;
	wxString sFileSpec( wxString::Format( "%s.*", sFileNameBase ) );

	if ( sourceDir.GetFirst( &sFileName, sFileSpec, wxDIR_FILES ) )
	{
		while ( true )
		{
			wxFileName fileName( sourceDir.GetName(), sFileName );

			if ( IsCoverFile( fileName ) )
			{
				coverFile = fileName;
				return true;
			}

			if ( !sourceDir.GetNext( &sFileName ) )
			{
				break;
			}
		}
	}

	return false;
}

template<size_t SIZE>
bool wxCoverFile::Find( const wxFileName& inputFile, wxFileName& coverFile, const char* const (&coverNames)[SIZE] )
{
	wxFileName sourceDirFn( inputFile );

	sourceDirFn.SetName( wxEmptyString );
	sourceDirFn.ClearExt();
	wxASSERT( sourceDirFn.IsDirReadable() );

	wxDir sourceDir( sourceDirFn.GetPath() );

	if ( !sourceDir.IsOpened() )
	{
		wxLogError( _( "Fail to open directory \u201C%s\u201D" ), sourceDirFn.GetPath() );
		return false;
	}

	for ( size_t i = 0; i < SIZE; ++i )
	{
		if ( Find( sourceDir, coverNames[ i ], coverFile ) )
		{
			return true;
		}
	}

	return false;
}

bool wxCoverFile::Find( const wxFileName& inputFile, wxFileName& coverFile )
{
    return Find( inputFile, coverFile, CoverNames );
}

namespace
{
	void extract_flac_pictures( TagLib::FLAC::File& flac, wxArrayCoverFile& covers )
	{
		const TagLib::List< TagLib::FLAC::Picture* >& pictures = flac.pictureList();

		for ( auto i = pictures.begin(); i != pictures.end(); ++i )
		{
			const TagLib::FLAC::Picture& picture	 = **i;
			const TagLib::ByteVector&	 pictureData = picture.data();

			wxMemoryBuffer buffer( pictureData.size() );
			buffer.AppendData( pictureData.data(), pictureData.size() );

			wxCoverFile::Type pictureType = (wxCoverFile::Type)picture.type();

			covers.Add( wxCoverFile( buffer, pictureType, picture.mimeType().toWString(), picture.description().toWString() ) );
		}
	}

	void extract_ape_pictures( const TagLib::APE::Tag& tag, wxArrayCoverFile& covers )
	{
		for ( auto i = tag.itemListMap().begin(), end = tag.itemListMap().end(); i != end; ++i )
		{
			if ( i->second.type() != TagLib::APE::Item::Binary ) { continue; }

			if ( !i->first.startsWith( "COVER ART" ) ) { continue; }

			wxString		   sName( i->first.toWString() );
			TagLib::ByteVector binary = i->second.binaryData();

			int p = binary.find( 0 );

			if ( p <= 0 ) { continue; }

			wxString s( binary.data(), wxConvUTF8, p );
			wxString mimeType;

			if ( !wxCoverFile::GetMimeFromExt( s, mimeType ) ) { continue; }

			size_t		   nPicLen = binary.size() - p - 1;
			wxMemoryBuffer buffer( nPicLen );
			buffer.AppendData( binary.data() + p + 1, nPicLen );

			wxString sDesc( i->first.substr( 9 ).toCWString() );
			sDesc.Trim( true ).Trim( false );

			if ( sDesc.StartsWith( '(' ) ) { sDesc.Remove( 0, 1 ); }

			if ( sDesc.EndsWith( ')' ) ) { sDesc.RemoveLast(); }
			sDesc.Trim( true ).Trim( false );

			wxCoverFile::Type pictureType = wxCoverFile::GetTypeFromStr( sDesc );

			if ( pictureType != wxCoverFile::Other ) { sDesc.Empty(); }

			covers.Add( wxCoverFile( buffer, pictureType, mimeType, sDesc ) );
		}
	}
}

void wxCoverFile::Extract( const wxFileName& fileName, wxArrayCoverFile& covers )
{
	TagLib::FileRef fileRef( fileName.GetFullPath().t_str(), true, TagLib::AudioProperties::Accurate );

	if ( fileRef.isNull() )
	{
		wxLogError( _( "Fail to initialize TagLib library." ) );
		return;
	}

	TagLib::File* pFile = fileRef.file();

	if ( dynamic_cast< TagLib::FLAC::File* >( pFile ) != nullptr )
	{
		TagLib::FLAC::File* pFlac = dynamic_cast< TagLib::FLAC::File* >( pFile );
		extract_flac_pictures( *pFlac, covers );
	}
	else if ( dynamic_cast< TagLib::WavPack::File* >( pFile ) != nullptr )
	{
		TagLib::WavPack::File* pWv = dynamic_cast< TagLib::WavPack::File* >( pFile );

		if ( pWv->hasAPETag() )
		{
			extract_ape_pictures( *( pWv->APETag() ), covers );
		}
	}
}

template<size_t SIZE>
wxCoverFile::Type wxCoverFile::GetTypeFromStr( const wxString& stype, const wxCoverFile::TypeName (&typeNames)[SIZE] )
{
	for ( size_t i = 0; i < SIZE; ++i )
	{
		if ( stype.CmpNoCase( typeNames[ i ].name ) == 0 ) { return typeNames[ i ].type; }
	}

	if ( stype.CmpNoCase( "Front" ) == 0 )
	{
		return FrontCover;
	}
	else if ( stype.CmpNoCase( "Back" ) == 0 )
	{
		return BackCover;
	}
	else
	{
		return Other;
	}
}

wxCoverFile::Type wxCoverFile::GetTypeFromStr( const wxString& stype )
{
    return GetTypeFromStr( stype, TypeNames );
}

template<size_t SIZE>
bool wxCoverFile::GetStrFromType( wxCoverFile::Type type, wxString& name, const wxCoverFile::TypeName( &typeNames )[SIZE] )
{
	for ( size_t i = 0; i < SIZE; ++i )
	{
		if ( type == typeNames[ i ].type )
		{
			name = typeNames[ i ].name;
			return true;
		}
	}

	return false;
}

bool wxCoverFile::GetStrFromType( wxCoverFile::Type type, wxString& name )
{
    return GetStrFromType( type, name, TypeNames );
}

template<size_t SIZE>
size_t wxCoverFile::GetSortOrder( wxCoverFile::Type type, const wxCoverFile::TypeName( &typeNames )[SIZE] )
{
	for ( size_t i = 0; i < SIZE; ++i )
	{
		if ( type == typeNames[ i ].type ) { return i; }
	}

	return 100;
}

size_t wxCoverFile::GetSortOrder( wxCoverFile::Type type )
{
    return GetSortOrder( type, TypeNames );
}

namespace
{
	inline int size_cmp( size_t s1, size_t s2 )
	{
		if ( s1 < s2 )
		{
			return -1;
		}
		else if ( s1 > s2 )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

int wxCoverFile::Cmp( wxCoverFile** f1, wxCoverFile** f2 )
{
	size_t so1 = GetSortOrder( ( *f1 )->GetType() );
	size_t so2 = GetSortOrder( ( *f2 )->GetType() );

	if ( so1 < so2 )
	{
		return -1;
	}
	else if ( so1 > so2 )
	{
		return 1;
	}
	else
	{
		return size_cmp( ( *f1 )->GetSize(), ( *f2 )->GetSize() );
	}
}

void wxCoverFile::Sort( wxArrayCoverFile& covers )
{
	covers.Sort( wxCoverFile::Cmp );
}

wxImage wxCoverFile::ToImageFromData() const
{
    wxASSERT( HasData() );

    wxMemoryInputStream is( m_data.GetData( ), m_data.GetDataLen( ) );
    return wxImage( is, m_mimeType );
}

wxImage wxCoverFile::ToImage() const
{
    if (HasData())
    {
        return ToImageFromData();
    }
    else
    {
        wxCoverFile inMem( Load() );
        if (inMem.HasData())
        {
            return inMem.ToImageFromData();
        }
    }

    return wxImage();
}

wxCoverFile wxCoverFile::Convert( wxImageHandler* const handler, int nQuality) const
{
    if (IsTypeOf(handler)) return wxCoverFile( *this );

    wxImage img( ToImage() );
    if (img.IsOk())
    {
        img.SetOption( wxIMAGE_OPTION_QUALITY, nQuality );
        wxMemoryOutputStream os;
        if (handler->SaveFile( &img, os ))
        {
            return wxCoverFile( memory_stream_to_buffer(os), m_type, handler->GetMimeType(), m_description );
        }
    }

   return wxCoverFile();
}

size_t wxCoverFile::Convert( const wxArrayCoverFile& covers, wxArrayCoverFile& ncovers, wxImageHandler* const handler, int nQuality )
{
    size_t nCounter = 0;

    for (size_t i = 0, nSize = covers.GetCount( ); i < nSize; ++i)
    {
        if (covers[i].IsTypeOf( handler ) )
        { // do not convert
            ncovers.Add( covers[i] );
            continue;
        }

        wxCoverFile jpeg( covers[i].Convert( handler, nQuality ) );
        if (jpeg.HasData())
        {
            ncovers.Add( jpeg );
        }
        else
        {
            wxLogDebug( "Cannot convert cover to JPEG - using original one" );
            nCounter += 1;
            ncovers.Add( covers[i] );
        }
    }

    return nCounter;
}

// ===============================================================================

#include <wx/arrimpl.cpp>	// this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayCoverFile );

