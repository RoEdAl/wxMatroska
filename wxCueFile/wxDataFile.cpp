/*
 * wxDataFile.cpp
 */

#include "StdWx.h"
#include <enum2str.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxDuration.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxMediaInfo.h>
#include <wxCueFile/wxCueTag.h>

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxDataFile, wxObject );

// ===============================================================================

const wxDataFile::FILE_TYPE_STR wxDataFile::FileTypeString[] =
{
	{ BINARY, wxT( "BINARY" ) },
	{ MOTOROLA, wxT( "MOTOROLA" ) },
	{ AIFF, wxT( "AIFF" ) },
	{ WAVE, wxT( "WAVE" ) },
	{ MP3, wxT( "MP3" ) }
};

// ===============================================================================

wxDataFile::wxDataFile( void ):
	m_ftype( BINARY ), m_mediaType( MEDIA_TYPE_UNKOWN )
{}

wxDataFile::wxDataFile( const wxDataFile& df )
{
	copy( df );
}

wxDataFile::wxDataFile( const wxString& sFilePath, wxDataFile::FileType ftype ):
	m_fileName( sFilePath ),
	m_ftype( ftype )
{}

wxDataFile::wxDataFile( const wxFileName& fileName, wxDataFile::FileType ftype ):
	m_fileName( fileName ),
	m_ftype( ftype )
{}

wxDataFile& wxDataFile::operator =( const wxDataFile& df )
{
	copy( df );
	return *this;
}

void wxDataFile::copy( const wxDataFile& df )
{
	m_fileName	   = df.m_fileName;
	m_realFileName = df.m_realFileName;
	m_ftype		   = df.m_ftype;
	m_sMIFormat	   = df.m_sMIFormat;
	m_sCueSheet	   = df.m_sCueSheet;

	if ( df.HasDuration() )
	{
		SetDuration( df.GetDuration() );
	}
	else
	{
		ClearDuration();
	}
}

wxString wxDataFile::GetFileTypeRegExp()
{
	return get_texts( FileTypeString ).Prepend( wxT( '(' ) ).Append( wxT( ')' ) );
}

wxString wxDataFile::ToString( wxDataFile::FileType ftype )
{
	return to_string( ftype, FileTypeString );
}

bool wxDataFile::FromString( const wxString& s, wxDataFile::FileType& ftype )
{
	return from_string( s, ftype, FileTypeString );
}

const wxFileName& wxDataFile::GetFileName() const
{
	return m_fileName;
}

bool wxDataFile::HasRealFileName() const
{
	return m_realFileName.IsOk();
}

const wxFileName& wxDataFile::GetRealFileName() const
{
	return m_realFileName;
}

wxDataFile::FileType wxDataFile::GetFileType() const
{
	return m_ftype;
}

const wxString& wxDataFile::GetMIFormat() const
{
	return m_sMIFormat;
}

wxDataFile::MediaType wxDataFile::GetMediaType( const wxString& sMIFormat )
{
	if ( sMIFormat.CmpNoCase( wxT( "FLAC" ) ) == 0 )
	{
		return MEDIA_TYPE_FLAC;
	}
	else if ( sMIFormat.CmpNoCase( wxT( "WAVPACK" ) ) == 0 )
	{
		return MEDIA_TYPE_WAVPACK;
	}
	else
	{
		return MEDIA_TYPE_UNKNOWN;
	}
}

wxDataFile::MediaType wxDataFile::GetMediaType() const
{
	return GetMediaType( m_sMIFormat );
}

bool wxDataFile::HasCueSheet() const
{
	return !m_sCueSheet.IsEmpty();
}

const wxString& wxDataFile::GetCueSheet() const
{
	return m_sCueSheet;
}

wxString wxDataFile::GetFileTypeAsString() const
{
	return ToString( m_ftype );
}

bool wxDataFile::IsEmpty() const
{
	return !m_fileName.IsOk();
}

bool wxDataFile::IsBinary() const
{
	return ( m_ftype == BINARY ) || ( m_ftype == MOTOROLA );
}

void wxDataFile::Clear()
{
	m_fileName.Clear();
	m_realFileName.Clear();
	m_ftype = BINARY;
	m_sMIFormat.Empty();
	m_sCueSheet.Empty();
}

wxDataFile& wxDataFile::Assign( const wxString& sFilePath, wxDataFile::FileType ftype )
{
	m_fileName = sFilePath;
	m_realFileName.Clear();
	m_ftype = ftype;
	m_sMIFormat.Empty();
	m_sCueSheet.Empty();

	return *this;
}

wxDataFile& wxDataFile::Assign( const wxFileName& fileName, wxDataFile::FileType ftype )
{
	m_fileName = fileName;
	m_realFileName.Clear();
	m_ftype = ftype;
	m_sMIFormat.Empty();
	m_sCueSheet.Empty();

	return *this;
}

bool wxDataFile::FindFile( wxFileName& fn, const wxString& sAlternateExt ) const
{
	if ( m_fileName.IsFileReadable() )	// file just exists
	{
		fn = m_fileName;
		return true;
	}

	wxString sTokens( sAlternateExt );

	if ( IsBinary() )
	{
		return false;
	}
	else if ( m_ftype == MP3 )
	{
		sTokens = wxT( "mp3" );
	}

	wxFileName		  fnTry( m_fileName );
	wxStringTokenizer tokenizer( sTokens, wxT( ',' ) );
	while ( tokenizer.HasMoreTokens() )
	{
		fnTry.SetExt( tokenizer.GetNextToken() );

		if ( fnTry.IsFileReadable() )
		{
			fn = fnTry;
			wxLogInfo( _( "%s -> %s" ), m_fileName.GetFullName(), fnTry.GetFullName() );
			return true;
		}
	}

	return false;
}

bool wxDataFile::FindFile( const wxString& sAlternateExt )
{
	return FindFile( m_realFileName, sAlternateExt );
}

wxULongLong wxDataFile::GetNumberOfFramesFromBinary( const wxFileName& fileName, const wxSamplingInfo& si )
{
	wxASSERT( fileName.IsFileReadable() );
	wxASSERT( si.IsOK() );

	wxULongLong size = fileName.GetSize();

	if ( size == wxInvalidSize )
	{
		return wxSamplingInfo::wxInvalidNumberOfFrames;
	}

	return si.GetNumberOfFramesFromBytes( size );
}

bool wxDataFile::GetMediaInfo(
	const wxFileName& fileName,
	wxULongLong& frames,
	wxSamplingInfo& si,
	wxDataFile::MediaType& eMediaType,
	wxString& sCueSheet )
{
	TagLib::FileRef fileRef( fileName.GetFullPath().t_str(), true, TagLib::AudioProperties::Fast );
	if ( fileRef.isNull() )
	{
		wxLogError( _("Fail to initialize TagLib library.") );
		return false;
	}

	TagLib::File const* pFile = fileRef.file();
	TagLib::AudioProperties const* pAprops = pFile->audioProperties();

	si.SetSamplingRate( pAprops->sampleRate() );
	si.SetNumberOfChannels( pAprops->channels() );

	eMediaType = MEDIA_TYPE_UNKNOWN;

	if ( dynamic_cast<TagLib::RIFF::WAV::Properties const*>( pAprops ) )
	{
		eMediaType = MEDIA_TYPE_WAV;
		TagLib::RIFF::WAV::Properties const* pProps = dynamic_cast<TagLib::RIFF::WAV::Properties const*>( pAprops );
		frames = pProps->sampleFrames();
		si.SetBitsPerSample( pProps->sampleWidth() );
	}
	else if ( dynamic_cast<TagLib::RIFF::AIFF::Properties const*>( pAprops ) )
	{
		eMediaType = MEDIA_TYPE_AIFF;
		TagLib::RIFF::AIFF::Properties const* pProps = dynamic_cast<TagLib::RIFF::AIFF::Properties const*>( pAprops );
		frames = pProps->sampleFrames();
		si.SetBitsPerSample( pProps->sampleWidth() );
	}
	else if ( dynamic_cast<TagLib::FLAC::Properties const*>( pAprops ) )
	{
		eMediaType = MEDIA_TYPE_FLAC;
		TagLib::FLAC::Properties const* pProps = dynamic_cast<TagLib::FLAC::Properties const*>( pAprops );
		frames = pProps->sampleFrames();
		si.SetBitsPerSample( pProps->sampleWidth() );
	}
	else if ( dynamic_cast<TagLib::WavPack::Properties const*>( pAprops ) )
	{
		eMediaType = MEDIA_TYPE_WAVPACK;
		TagLib::WavPack::Properties const* pProps = dynamic_cast<TagLib::WavPack::Properties const*>( pAprops );
		frames = pProps->sampleFrames();
		si.SetBitsPerSample( pProps->bitsPerSample() );
	}

	sCueSheet.Empty();
	TagLib::PropertyMap props( pFile->properties() );
	for( TagLib::PropertyMap::ConstIterator i = props.begin(), pend = props.end(); i != pend; ++i )
	{
		wxString propName( i->first.toWString() );
		if ( propName.CmpNoCase( wxCueTag::Name::CUESHEET ) == 0 )
		{
			sCueSheet = i->second[0].toWString();
			break;
		}
	}

	return (eMediaType != MEDIA_TYPE_UNKNOWN);
}

bool wxDataFile::GetFromMediaInfo( const wxFileName& fileName, wxULongLong& frames, wxSamplingInfo& si, wxString& sMIFormat, wxString& sCueSheet )
{
	MediaType eMedia = MEDIA_TYPE_UNKNOWN;
	GetMediaInfo( fileName, frames, si, eMedia, sCueSheet );

	// using MediaInfo to get basic information about media
	wxMediaInfo dll;

	if ( !dll.Load() )
	{
		wxLogError( _( "Fail to load MediaInfo library" ) );
		return false;
	}

	wxArrayString as1;
	wxArrayString as2;

	void*  handle = dll.MediaInfoNew();
	size_t res	  = dll.MediaInfoOpen( handle, fileName.GetFullPath() );

	if ( res == 0 )
	{
		wxLogError( _( "MediaInfo - fail to open file" ) );
		dll.MediaInfoDelete( handle );
		dll.Unload();
		return false;
	}

	for ( size_t i = 0; i < INFOS_SIZE; i++ )
	{
		wxString s1(
			dll.MediaInfoGet(
					handle,
					wxMediaInfo::MediaInfo_Stream_General,
					0,
					INFOS[ i ]
					)
			);

		wxString s2(
			dll.MediaInfoGet(
					handle,
					wxMediaInfo::MediaInfo_Stream_General,
					0,
					INFOS[ i ],
					wxMediaInfo::MediaInfo_Info_Measure
					)
			);

		as1.Add( s1 );
		as2.Add( s2 );
	}

	for ( size_t i = 0; i < AUDIO_INFOS_SIZE; i++ )
	{
		wxString s1(
			dll.MediaInfoGet(
					handle,
					wxMediaInfo::MediaInfo_Stream_Audio,
					0,
					AUDIO_INFOS[ i ]
					)
			);

		wxString s2(
			dll.MediaInfoGet(
					handle,
					wxMediaInfo::MediaInfo_Stream_Audio,
					0,
					AUDIO_INFOS[ i ],
					wxMediaInfo::MediaInfo_Info_Measure
					)
			);

		as1.Add( s1 );
		as2.Add( s2 );
	}

	dll.MediaInfoClose( handle );
	dll.MediaInfoDelete( handle );

	bool		  bCheck	= true;
	bool		  bCueSheet = false;
	unsigned long u;

	for ( size_t i = 0; i < ( INFOS_SIZE + AUDIO_INFOS_SIZE ); i++ )
	{
		switch ( i )
		{
			case 0:	// count of audio streams
			{
				if ( !as1[ i ].ToULong( &u ) || !( u > 0 ) )
				{
					wxLogWarning( _( "MediaInfo - cannot find audio stream" ) );
					bCheck = false;
				}

				break;
			}

			case 1:	// cue sheet #1
			{
				if ( !bCueSheet )
				{
					if ( !as1[ i ].IsEmpty() )
					{
						sCueSheet = as1[ i ];
						bCueSheet = true;
					}
				}

				break;
			}

			case 2:	// cue sheet #2
			{
				if ( !bCueSheet )
				{
					if ( !as1[ i ].IsEmpty() )
					{
						sCueSheet = as1[ i ];
						bCueSheet = true;
					}
				}

				break;
			}

			case 3:	// stream size
			{
				break;
			}

			case 4:	// duration
			{
				if ( !as1[ i ].ToULong( &u ) )
				{
					wxLogWarning( _( "MediaInfo - Invalid duration - %s" ), as1[ i ] );
					bCheck = false;
				}

				break;
			}

			case 5:	// format
			{
				sMIFormat = as1[ i ];
				break;
			}

			case 6:	// sampling rate
			{
				if ( !as1[ i ].ToULong( &u ) || ( u == 0u ) )
				{
					wxLogWarning( _( "MediaInfo - Invalid sample rate - %s" ), as1[ i ] );
					bCheck = false;
				}
				else
				{
					si.SetSamplingRate( u );
				}

				break;
			}

			case 7:	// bit depth
			{
				if ( !as1[ i ].IsEmpty() )
				{
					if ( !as1[ i ].ToULong( &u ) || ( u == 0u ) || ( u > 10000u ) )
					{
						wxLogWarning( _( "MediaInfo - Invalid bit depth - %s" ), as1[ i ] );
						bCheck = false;
					}
					else
					{
						si.SetBitsPerSample( (unsigned short)u );
					}
				}
				else
				{
					si.SetBitsPerSample( 0 );	// unknown MP3
				}

				break;
			}

			case 8:	// channels
			{
				if ( !as1[ i ].ToULong( &u ) || ( u == 0u ) || ( u > 128u ) )
				{
					wxLogWarning( _( "MediaInfo - Invalid number of channels - %s" ), as1[ i ] );
					bCheck = false;
				}
				else
				{
					si.SetNumberOfChannels( (unsigned short)u );
				}

				break;
			}

			case 9:	// SamplingCount
			{
				if ( !as1[ i ].IsEmpty() )
				{
					wxUint64 ul;

					if ( as1[ i ].ToULongLong( &ul ) )	// calculate duration
					{	// according to duration
						frames = ul;
					}
					else
					{
						wxLogWarning( _( "MediaInfo - Invalid samples count - %s" ), as1[ i ] );
						bCheck = false;
					}
				}

				break;
			}
		}
	}

	if ( !bCueSheet )
	{
		sCueSheet.Empty();
	}

	return bCheck;
}

bool wxDataFile::GetInfo( const wxString& sAlternateExt )
{
	if ( !HasRealFileName() )
	{
		if ( !FindFile( sAlternateExt ) )
		{
			return false;
		}
	}

	wxSamplingInfo si;
	wxULongLong	   frames;
	bool		   res;

	if ( IsBinary() )
	{
		si.SetDefault();
		m_sMIFormat = _T( "BINARY" );
		m_sCueSheet.Empty();
		frames = GetNumberOfFramesFromBinary( m_realFileName, si );
		res	   = ( frames != wxSamplingInfo::wxInvalidNumberOfFrames );
	}
	else
	{
		res = GetFromMediaInfo( m_realFileName, frames, si, m_sMIFormat, m_sCueSheet );
	}

	if ( res )
	{
		SetDuration( wxDuration( si, frames ) );
	}

	return res;
}

#include <wx/arrimpl.cpp>	// this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayDataFile );
WX_DEFINE_OBJARRAY( wxArrayFileName );

