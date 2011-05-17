/*
   wxDataFile.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxDuration.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxMediaInfo.h>

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxDataFile, wxObject )

// ===============================================================================

static const wxChar * INFOS[] =
{
	wxT( "StreamSize" ),
	wxT( "Duration" ),
	wxT( "Format" ),
	wxT( "SamplingRate" ),
	wxT( "BitDepth" ),
	wxT( "Channel(s)" ),
	wxT( "SamplingCount" )
};

static const size_t INFOS_SIZE = WXSIZEOF( INFOS );

// ===============================================================================

wxDataFile::wxDataFile( void ):
	m_ftype( BINARY )
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
	m_fileName = df.m_fileName;
	m_ftype	   = df.m_ftype;
	if ( df.HasDuration() )
	{
		SetDuration( df.GetDuration() );
	}
	else
	{
		ClearDuration();
	}
}

wxDataFile::FILE_TYPE_STR wxDataFile::FileTypeString[] =
{
	{ BINARY, wxT( "BINARY" ) },
	{ MOTOROLA, wxT( "MOTOROLA" ) },
	{ AIFF, wxT( "AIFF" ) },
	{ WAVE, wxT( "WAVE" ) },
	{ MP3, wxT( "MP3" ) }
};

size_t wxDataFile::FileTypeStringSize = WXSIZEOF( wxDataFile::FileTypeString );

wxString wxDataFile::GetFileTypeRegExp()
{
	wxString s;

	for ( size_t i = 0; i < FileTypeStringSize; i++ )
	{
		s += FileTypeString[ i ].szName;
		s += wxT( '|' );
	}

	s = s.RemoveLast();

	wxString sResult;
	sResult.Printf( wxT( "(%s)" ), s );
	return sResult;
}

wxString wxDataFile::FileTypeToString( wxDataFile::FileType ftype )
{
	wxString s;

	for ( size_t i = 0; i < FileTypeStringSize; i++ )
	{
		if ( FileTypeString[ i ].ftype == ftype )
		{
			s = FileTypeString[ i ].szName;
			break;
		}
	}

	return s;
}

bool wxDataFile::StringToFileType( const wxString& s, wxDataFile::FileType& ftype )
{
	for ( size_t i = 0; i < FileTypeStringSize; i++ )
	{
		if ( s.CmpNoCase( FileTypeString[ i ].szName ) == 0 )
		{
			ftype = FileTypeString[ i ].ftype;
			return true;
		}
	}

	return false;
}

const wxFileName& wxDataFile::GetFileName() const
{
	return m_fileName;
}

wxDataFile::FileType wxDataFile::GetFileType() const
{
	return m_ftype;
}

wxString wxDataFile::GetFileTypeAsString() const
{
	return FileTypeToString( m_ftype );
}

bool wxDataFile::IsEmpty() const
{
	return !m_fileName.IsOk();
}

void wxDataFile::Clear()
{
	m_fileName.Clear();
	m_ftype = BINARY;
}

wxDataFile& wxDataFile::Assign( const wxString& sFilePath, wxDataFile::FileType ftype )
{
	m_fileName = sFilePath;
	m_ftype	   = ftype;
	return *this;
}

wxDataFile& wxDataFile::Assign( const wxFileName& fileName, wxDataFile::FileType ftype )
{
	m_fileName = fileName;
	m_ftype	   = ftype;
	return *this;
}

bool wxDataFile::FindFile( wxFileName& fn, const wxString& sAlternateExt ) const
{
	if ( m_fileName.FileExists() ) // file just exists
	{
		fn = m_fileName;
		return true;
	}

	wxString sTokens( sAlternateExt );
	if ( m_ftype == BINARY )
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
		if ( fnTry.FileExists() )
		{
			fn = fnTry;
			wxLogInfo( _( "%s -> %s" ), m_fileName.GetFullName(), fnTry.GetFullName() );
			return true;
		}
	}

	return false;
}

bool wxDataFile::FileExists( const wxString& sAlternateExt ) const
{
	wxFileName fn;

	return FindFile( fn, sAlternateExt );
}

wxULongLong wxDataFile::GetNumberOfFramesFromBinary( const wxFileName& fileName, const wxSamplingInfo& si )
{
	wxASSERT( fileName.FileExists() );
	wxASSERT( si.IsOK() );

	wxULongLong size = fileName.GetSize();

	if ( size == wxInvalidSize )
	{
		return wxSamplingInfo::wxInvalidNumberOfFrames;
	}

	return si.GetNumberOfFramesFromBytes( size );
}

bool wxDataFile::GetFromMediaInfo( const wxFileName& fileName, wxULongLong& frames, wxSamplingInfo& si )
{
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
				wxMediaInfo::MediaInfo_Stream_Audio,
				0,
				INFOS[ i ]
				)
			);

		wxString s2(
			dll.MediaInfoGet(
				handle,
				wxMediaInfo::MediaInfo_Stream_Audio,
				0,
				INFOS[ i ],
				wxMediaInfo::MediaInfo_Info_Measure
				)
			);

		as1.Add( s1 );
		as2.Add( s2 );
	}

	dll.MediaInfoClose( handle );
	dll.MediaInfoDelete( handle );

	bool		  check = true;
	unsigned long u;

	for ( size_t i = 0; i < INFOS_SIZE; i++ )
	{
		switch ( i )
		{
			case 0: // stream size
			break;

			case 1: // duration
			if ( !as1[ i ].ToULong( &u ) )
			{
				wxLogWarning( _( "MediaInfo - Invalid duration - %s" ), as1[ i ] );
				check = false;
			}

			break;

			case 2: // format
			break;

			case 3: // sampling rate
			if ( !as1[ i ].ToULong( &u ) || ( u == 0u ) )
			{
				wxLogWarning( _( "MediaInfo - Invalid sample rate - %s" ), as1[ i ] );
				check = false;
			}
			else
			{
				si.SetSamplingRate( u );
			}

			break;

			case 4: // bit depth
			if ( !as1[ i ].IsEmpty() )
			{
				if ( !as1[ i ].ToULong( &u ) || ( u == 0u ) || ( u > 10000u ) )
				{
					wxLogWarning( _( "MediaInfo - Invalid bit depth - %s" ), as1[ i ] );
					check = false;
				}
				else
				{
					si.SetBitsPerSample( (unsigned short)u );
				}
			}
			else
			{
				si.SetBitsPerSample( 0 ); // unknown MP3
			}

			break;

			case 5: // channels
			if ( !as1[ i ].ToULong( &u ) || ( u == 0u ) || ( u > 128u ) )
			{
				wxLogWarning( _( "MediaInfo - Invalid number of channels - %s" ), as1[ i ] );
				check = false;
			}
			else
			{
				si.SetNumberOfChannels( (unsigned short)u );
			}

			break;

			case 6: // SamplingCount
			if ( !as1[ i ].IsEmpty() )
			{
				wxUint64 ul;
				if ( as1[ i ].ToULongLong( &ul ) ) // calculate duration
				{ // according to duration
					frames = ul;
				}
				else
				{
					wxLogWarning( _( "MediaInfo - Invalid samples count - %s" ), as1[ i ] );
					check = false;
				}
			}

			break;
		}
	}

	return check;
}

bool wxDataFile::GetInfo( wxSamplingInfo& si, wxULongLong& frames, const wxString& sAlternateExt ) const
{
	wxFileName fn;

	if ( !FindFile( fn, sAlternateExt ) )
	{
		return false;
	}

	bool res;
	if ( m_ftype == BINARY )
	{
		si.SetDefault();
		frames = GetNumberOfFramesFromBinary( fn, si );
		res	   = ( frames != wxSamplingInfo::wxInvalidNumberOfFrames );
	}
	else
	{
		res = GetFromMediaInfo( fn, frames, si );
	}

	return res;
}

bool wxDataFile::GetInfo( wxDuration& duration, const wxString& sAlternateExt ) const
{
	wxSamplingInfo si;
	wxULongLong	   numberOfSamples;

	if ( GetInfo( si, numberOfSamples, sAlternateExt ) )
	{
		duration.Assign( si, numberOfSamples );
		return true;
	}
	else
	{
		return false;
	}
}

const wxAbstractDurationHolder& wxDataFile::CalculateDuration( const wxString& sAlternateExt )
{
	wxDuration duration;

	if ( GetInfo( duration, sAlternateExt ) )
	{
		SetDuration( duration );
	}

	return const_cast<const wxDataFile&>( *this );
}

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayDataFile );
WX_DEFINE_OBJARRAY( wxArrayFileName );

