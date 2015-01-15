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

const char* const wxCoverFile::CoverExts[] =
{
	"jpg",
	"jpeg",
	"png"
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

wxCoverFile::wxCoverFile()
{}

wxCoverFile::wxCoverFile( const wxCoverFile& cf ):
	m_fileName( cf.m_fileName ), m_data( cf.m_data ), m_mimeType( cf.m_mimeType ), m_description( cf.m_description ), m_checksum( cf.m_checksum ), m_type( cf.m_type )
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
bool wxCoverFile::IsCoverFile( const wxFileName& fileName, const char* const (&coverExts)[SIZE] )
{
	for ( size_t i = 0; i < SIZE; ++i )
	{
		if ( fileName.GetExt().CmpNoCase( coverExts[ i ] ) == 0 )
		{
			wxULongLong fsize = fileName.GetSize();

			if ( fsize < MAX_FILE_SIZE )
			{
				return true;
			}
		}
	}

	return false;
}

bool wxCoverFile::IsCoverFile( const wxFileName& fileName )
{
    return IsCoverFile( fileName, CoverExts );
}

bool wxCoverFile::GetCoverFile( const wxDir& sourceDir, const wxString& sFileNameBase, wxFileName& coverFile )
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
bool wxCoverFile::GetCoverFile( const wxFileName& inputFile, wxFileName& coverFile, const char* const (&coverNames)[SIZE] )
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
		if ( GetCoverFile( sourceDir, coverNames[ i ], coverFile ) )
		{
			return true;
		}
	}

	return false;
}

bool wxCoverFile::GetCoverFile( const wxFileName& inputFile, wxFileName& coverFile )
{
    return GetCoverFile( inputFile, coverFile, CoverNames );
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

// ===============================================================================

#include <wx/arrimpl.cpp>	// this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayCoverFile );

