/*
	wxDataFile.cpp
*/

#include "StdWx.h"
#include "wxIndex.h"
#include "wxDataFile.h"
#include "wxMediaInfo.h"

IMPLEMENT_DYNAMIC_CLASS( wxDataFile, wxObject )

static const wxChar* INFOS[] = {
	wxT("StreamSize"),
	wxT("Duration"),
	wxT("Format"),
	wxT("SamplingRate"),
	wxT("BitDepth"),
	wxT("Channel(s)"),
	wxT("SamplingCount")
};

static const size_t INFOS_SIZE = sizeof(INFOS)/sizeof(const wxChar*);

const wxTimeSpan wxDataFile::wxInvalidDuration = wxTimeSpan::Hours(-1);
const wxULongLong wxDataFile::wxInvalidNumberOfSamples = wxULongLong( 0xFFFFFFFF, 0xFFFFFFFF );

wxDataFile::wxDataFile(void)
	:m_ftype(BINARY)
{
}

wxDataFile::wxDataFile(const wxDataFile& df)
{
	copy( df );
}

wxDataFile::wxDataFile(const wxString& sFilePath, wxDataFile::FileType ftype )
	:m_fileName(sFilePath),m_ftype(ftype)
{
}

wxDataFile::wxDataFile(const wxFileName& fileName, wxDataFile::FileType ftype )
	:m_fileName(fileName),m_ftype(ftype)
{
}

wxDataFile& wxDataFile::operator =(const wxDataFile& df)
{
	copy( df );
	return *this;
}

wxDataFile::~wxDataFile(void)
{
}

void wxDataFile::copy(const wxDataFile& df)
{
	m_fileName = df.m_fileName;
	m_ftype = df.m_ftype;
}

wxDataFile::FILE_TYPE_STR wxDataFile::FileTypeString[] = {
	{ BINARY, wxT("BINARY") },
	{ MOTOROLA, wxT("MOTOROLA") },
	{ AIFF, wxT("AIFF") },
	{ WAVE, wxT("WAVE") },
	{ MP3, wxT("MP3") }
};

size_t wxDataFile::FileTypeStringSize = sizeof(wxDataFile::FileTypeString) / sizeof(wxDataFile::FILE_TYPE_STR);

wxString wxDataFile::GetFileTypeRegExp()
{
	wxString s;
	for( size_t i=0; i<FileTypeStringSize; i++ )
	{
		s += FileTypeString[i].szName;
		s += wxT('|');
	}
	s = s.RemoveLast();

	wxString sResult;
	sResult.Printf( wxT("(%s)"), s.GetData() );
	return sResult;
}

wxString wxDataFile::FileTypeToString(wxDataFile::FileType ftype)
{
	wxString s;
	for( size_t i=0; i<FileTypeStringSize; i++ )
	{
		if ( FileTypeString[i].ftype == ftype )
		{
			s = FileTypeString[i].szName;
			break;
		}
	}
	return s;
}

bool wxDataFile::StringToFileType( const wxString& s, wxDataFile::FileType& ftype )
{
	for( size_t i=0; i<FileTypeStringSize; i++ )
	{
		if ( s.CmpNoCase( FileTypeString[i].szName ) == 0 )
		{
			ftype = FileTypeString[i].ftype;
			return true;
		}
	}
	return false;
}

wxString wxDataFile::GetFullPath() const
{
	return m_fileName.GetFullPath();
}

wxString wxDataFile::GetFileName() const
{
	return m_fileName.GetFullName();
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

wxDataFile& wxDataFile::Assign(const wxString& sFilePath, wxDataFile::FileType ftype)
{
	m_fileName = sFilePath;
	m_ftype = ftype;
	return *this;
}

wxDataFile& wxDataFile::Assign(const wxFileName& fileName, wxDataFile::FileType ftype)
{
	m_fileName = fileName;
	m_ftype = ftype;
	return *this;
}

bool wxDataFile::FindFile( wxFileName& fn, const wxString& sAlternateExt ) const
{
	if ( m_fileName.FileExists() )
	{ // file just exists
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
		sTokens = wxT("mp3");
	}

	wxFileName fnTry( m_fileName );
	wxStringTokenizer tokenizer( sTokens, wxT(",") );
	while( tokenizer.HasMoreTokens() )
	{
		fnTry.SetExt( tokenizer.GetNextToken() );
		if ( fnTry.FileExists() )
		{
			fn = fnTry;
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

wxULongLong wxDataFile::GetNumberOfSamplesFromBinary( const wxFileName& fileName )
{
	wxASSERT( fileName.FileExists() );

	wxULongLong size = fileName.GetSize();

	if ( size == wxInvalidSize )
	{
		return wxInvalidNumberOfSamples;
	}

	// size -> samples
	// 1 sample = 2 * 2 bytes;
	size /= 4;
	return size;
}

wxULongLong wxDataFile::GetNumberOfSamplesFromMediaInfo( const wxFileName& fileName )
{
	// using MediaInfo to get basic information about media
	wxMediaInfo dll;
	if ( !dll.Load() )
	{
		wxLogError( _("Fail to load MediaInfo library") );
		return wxInvalidNumberOfSamples;
	}

	wxArrayString as1;
	wxArrayString as2;

	void* handle = dll.MediaInfoNew();
	size_t res = dll.MediaInfoOpen( handle, fileName.GetFullPath() );
	if ( res == 0 )
	{
		wxLogError( _("MediaInfo - fail to open file") );
		dll.MediaInfoDelete( handle );
		dll.Unload();
		return wxInvalidNumberOfSamples;
	}

	for( size_t i=0; i<INFOS_SIZE; i++ )
	{
		wxString s1( 
			dll.MediaInfoGet( 
				handle,
				wxMediaInfo::MediaInfo_Stream_Audio,
				0,
				INFOS[i]
			)
		);

		wxString s2( 
			dll.MediaInfoGet( 
				handle,
				wxMediaInfo::MediaInfo_Stream_Audio,
				0,
				INFOS[i],
				wxMediaInfo::MediaInfo_Info_Measure
			)
		);

		as1.Add( s1 );
		as2.Add( s2 );
	}

	dll.MediaInfoClose( handle );
	dll.MediaInfoDelete( handle );

	bool check = true;
	unsigned long u;
	wxULongLong samples( wxInvalidNumberOfSamples );

	for( size_t i=0; i<INFOS_SIZE; i++ )
	{
		switch( i )
		{
			case 0: // stream size
			break;

			case 1: // duration
			if ( as1[i].ToULong( &u ) )
			{
				// duration to samples
				// 10(00)ms = 441(00) samples
				u *= wxULL(441);
				u /= wxULL(10);
				samples = u;
			}
			else
			{
				wxLogWarning( wxT("MediaInfo - Invalid duration - %s"), as1[i].GetData() );
				check = false;
			}
			break;

			case 2: // format
			break;

			case 3: // sampling rate
			if ( !as1[i].ToULong( &u ) || (u != 44100u) )
			{
				wxLogWarning( wxT("MediaInfo - Invalid sample rate - %s"), as1[i].GetData() );
				check = false;
			}
			break;

			case 4: // bit depth
			if ( !as1[i].IsEmpty() )
			{
				if ( !as1[i].ToULong( &u ) || (u != 16u) )
				{
					wxLogWarning( wxT("MediaInfo - Invalid bit depth - %s"), as1[i].GetData() );
					check = false;
				}
			}
			break;

			case 5: // channels
			if ( !as1[i].ToULong( &u ) || (u != 2) )
			{
				wxLogWarning( wxT("MediaInfo - Invalid number of channels - %s"), as1[i].GetData() );
				check = false;
			}
			break;

			case 6: // SamplingCount
			if ( !as1[i].IsEmpty() )
			{
				wxULongLong_t ul;
				if ( as1[i].ToULongLong( &ul ) )
				{ // calculate duration according to duration
					samples = ul;
				}
				else
				{
					wxLogWarning( wxT("MediaInfo - Invalid samples count - %s"), as1[i].GetData() );
					check = false;
				}
			}
			break;
		}
	}

	if ( !check ) samples = wxInvalidNumberOfSamples;
	return samples;
}

wxTimeSpan wxDataFile::GetDuration( wxULongLong samples )
{
	if ( samples == wxInvalidNumberOfSamples )
	{
		return wxInvalidDuration;
	}

	// samples -> duration
	// 441(00) = 10(00) ms
	wxULongLong duration( samples );
	duration *= wxULL(10);
	duration /= wxULL(441);

	return wxTimeSpan::Milliseconds( duration.GetValue() );
}

wxTimeSpan wxDataFile::GetDuration( const wxString& sAlternateExt ) const
{
	return GetDuration( GetNumberOfSamples( sAlternateExt ) );
}

void wxDataFile::GetNumberOfFrames( wxULongLong samples, wxULongLong& frames, wxUint32& rest )
{
	// 44100 samples = 75 frames
	// 588 samples = 1 frame
	frames = samples;
	wxULongLong urest( frames % wxULL(588) );
	frames /= wxULL(588);
	rest = urest.GetLo();
}

wxULongLong wxDataFile::GetNumberOfFrames( wxULongLong samples )
{
	wxULongLong frames;
	wxUint32 rest;
	GetNumberOfFrames( samples, frames, rest );
	frames += (rest>294)? 1 : 0;
	return frames;
}

wxULongLong wxDataFile::GetNumberOfSamples( const wxString& sAlternateExt, bool bRoundDown ) const
{
	wxFileName fn;
	if ( !FindFile( fn, sAlternateExt) )
	{
		return wxInvalidNumberOfSamples;
	}

	wxULongLong res;
	if ( m_ftype == BINARY )
	{
		res = GetNumberOfSamplesFromBinary(fn);
	}
	else
	{
		res = GetNumberOfSamplesFromMediaInfo(fn);
	}

	if ( (res != wxInvalidNumberOfSamples) && bRoundDown )
	{
		wxULongLong frames( GetNumberOfFrames( res ) );
		res = frames;
		res *= wxULL(588);
	}

	return res;
}

wxString wxDataFile::GetSamplesStr(wxULongLong samples)
{
	// 1.0 = 44100
	wxULongLong s( samples );
	wxULongLong sr = samples % wxULL(44100);
	double rest = sr.ToDouble() / 44100;

	s -= sr;
	s /= wxULL(44100);

	// seconds
	wxULongLong ss = s % 60;
	s -= ss;
	s /= wxULL(60);
	rest += ss.ToDouble();

	// minutes
	wxULongLong mm = s % 60;
	s -= mm;
	s /= wxULL(60);

	// hours

	return wxIndex::GetTimeStr( s.GetLo(), mm.GetLo(), rest );
}
