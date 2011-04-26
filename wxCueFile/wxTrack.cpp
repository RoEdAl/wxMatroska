/*
	wxTrack.cpp
*/

#include "StdWx.h"
#include "wxDataFile.h"
#include "wxIndex.h"
#include "wxTrack.h"

wxIMPLEMENT_DYNAMIC_CLASS( wxTrack, wxCueComponent )

wxTrack::wxTrack(void)
	:wxCueComponent(true),m_pPreGap((wxIndex*)NULL),m_pPostGap((wxIndex*)NULL),m_number(0)
{
}

wxTrack::wxTrack(const wxTrack& track)
	:wxCueComponent(true),m_pPreGap((wxIndex*)NULL),m_pPostGap((wxIndex*)NULL),m_dataMode(AUDIO)
{
	copy(track);
}

wxTrack::wxTrack(unsigned long number)
	:wxCueComponent(true),m_pPreGap((wxIndex*)NULL),m_pPostGap((wxIndex*)NULL),m_number(number),m_dataMode(AUDIO)
{
}

wxTrack::~wxTrack(void)
{
	if ( m_pPreGap != (wxIndex*)NULL )
	{
		delete m_pPreGap;
	}

	if ( m_pPostGap != (wxIndex*)NULL )
	{
		delete m_pPostGap;
	}
}

wxTrack& wxTrack::operator =(const wxTrack& track)
{
	copy( track );
	return *this;
}

void wxTrack::copy(const wxTrack& track)
{
	wxCueComponent::copy( track );

	ClearPreGap();
	ClearPostGap();

	m_number = track.m_number;
	m_dataMode = track.m_dataMode;
	m_indexes = track.m_indexes;
	m_flags = track.m_flags;
	m_df = track.m_df;

	if ( track.HasPreGap() )
	{
		SetPreGap( track.GetPreGap() );
	}

	if ( track.HasPostGap() )
	{
		SetPreGap( track.GetPostGap() );
	}
}

unsigned long wxTrack::GetNumber() const
{
	return m_number;
}

wxTrack::DataMode wxTrack::GetMode() const
{
	return m_dataMode;
}

wxString wxTrack::GetModeAsString() const
{
	return DataModeToString( m_dataMode );
}

wxTrack& wxTrack::SetMode(wxTrack::DataMode dataMode)
{
	m_dataMode = dataMode;
	return *this;
}

const wxArrayIndex& wxTrack::GetIndexes() const
{
	return m_indexes;
}

void wxTrack::AddIndex(const wxIndex& idx)
{
	m_indexes.Add( idx );
}

bool wxTrack::HasPreGap() const
{
	return (m_pPreGap != (const wxIndex*)NULL);
}

bool wxTrack::HasPostGap() const
{
	return (m_pPostGap != (const wxIndex*)NULL);
}

const wxIndex& wxTrack::GetPreGap() const
{
	if (m_pPreGap != (const wxIndex*)NULL)
	{
		return *m_pPreGap;
	}
	else
	{
		wxFAIL_MSG( wxT("Trying to access nonexistient pre-gap") );
		return *m_pPreGap; // to make compiler happy
	}
}

const wxIndex& wxTrack::GetPostGap() const
{
	if (m_pPostGap != (const wxIndex*)NULL)
	{
		return *m_pPostGap;
	}
	else
	{
		wxFAIL_MSG( wxT("Trying to access nonexistient post-gap") );
		return *m_pPostGap; // to make compiler happy
	}
}

void wxTrack::ClearPreGap()
{
	if ( m_pPreGap != (wxIndex*)NULL )
	{
		delete m_pPreGap;
		m_pPreGap = (wxIndex*)NULL;
	}
}

void wxTrack::ClearPostGap()
{
	if ( m_pPostGap != (wxIndex*)NULL )
	{
		delete m_pPostGap;
		m_pPostGap = (wxIndex*)NULL;
	}
}

void wxTrack::SetPreGap(const wxIndex& preGap)
{
	ClearPreGap();
	m_pPreGap = new wxIndex( preGap );
}

void wxTrack::SetPostGap(const wxIndex& postGap)
{
	ClearPostGap();
	m_pPostGap = new wxIndex( postGap );
}

const wxDataFile& wxTrack::GetDataFile() const
{
	return m_df;
}

bool wxTrack::HasDataFile() const
{
	return !m_df.IsEmpty();
}

const wxTrack::wxArrayFlag& wxTrack::GetFlags() const
{
	return m_flags;
}

wxTrack& wxTrack::AddFlag(wxTrack::Flag flag )
{
	m_flags.Add( flag );
	return *this;
}

bool wxTrack::AddFlag( const wxString& sFlag )
{
	Flag flag;
	if ( wxTrack::StringToFlag( sFlag, flag ) )
	{
		m_flags.Add( flag );
		return true;
	}
	else
	{
		return false;
	}
}

void wxTrack::ClearFlags()
{
	m_flags.Clear();
}

wxString wxTrack::GetFlagsAsString() const
{
	wxString s;
	size_t flags = m_flags.Count();
	for( size_t i=0; i<flags; i++ )
	{
		s += FlagToString( m_flags[i] );
		s += wxT(' ');
	}
	s = s.RemoveLast();
	return s;
}

bool wxTrack::HasFlags() const
{
	return (m_flags.Count() > 0u);
}

bool wxTrack::HasFlag( wxTrack::Flag f ) const
{
	size_t flags = m_flags.Count();
	for( size_t i=0; i<flags; i++ )
	{
		if ( m_flags[i] == f ) return true;
	}

	return false;
}

wxTrack::FLAG_STR wxTrack::FlagString[] = {
	{ DCP, wxT("DCP") },
	{ CH4, wxT("CH4") },
	{ PRE, wxT("PRE") },
	{ SCMS, wxT("SCMS") },
	{ DATA, wxT("DATA") },
	{ NONE, wxT("NONE") }
};

size_t wxTrack::FlagStringSize = sizeof(wxTrack::FlagString) / sizeof(wxTrack::FLAG_STR);


wxString wxTrack::GetFlagRegExp()
{
	wxString s;
	for( size_t i=0; i<FlagStringSize; i++ )
	{
		s += FlagString[i].szName;
		s += wxT('|');
	}
	s = s.RemoveLast();

	wxString sResult;
	sResult.Printf( wxT("(%s)"), s.GetData() );
	return sResult;
}

wxString wxTrack::FlagToString(wxTrack::Flag flag)
{
	wxString s;
	for( size_t i=0; i<FlagStringSize; i++ )
	{
		if ( FlagString[i].flag == flag )
		{
			s = FlagString[i].szName;
			break;
		}
	}
	return s;
}

bool wxTrack::StringToFlag( const wxString& s, wxTrack::Flag& flag )
{
	for( size_t i=0; i<FlagStringSize; i++ )
	{
		if ( s.CmpNoCase( FlagString[i].szName ) == 0 )
		{
			flag = FlagString[i].flag;
			return true;
		}
	}
	return false;
}

wxTrack::DATA_MODE_STR wxTrack::DataModeString[] = {
	{ AUDIO, wxT("AUDIO") },
	{ CDG, wxT("CDG") },
	{ MODE1_2048, wxT("MODE1/2048") },
	{ MODE1_2352, wxT("MODE1/2352") },
	{ MODE2_2336, wxT("MODE2/2336") },
	{ MODE2_2352, wxT("MODE2/2352") },
	{ CDI_2336, wxT("CDI/2336") },
	{ CDI_2352, wxT("CDI/2352") }
};

size_t wxTrack::DataModeStringSize = sizeof(wxTrack::DataModeString) / sizeof(wxTrack::DATA_MODE_STR);

wxString wxTrack::GetDataModeRegExp()
{
	wxString s;
	for( size_t i=0; i<DataModeStringSize; i++ )
	{
		s += DataModeString[i].szName;
		s += wxT('|');
	}
	s = s.RemoveLast();

	wxString sResult;
	sResult.Printf( wxT("(%s)"), s.GetData() );
	return sResult;
}

wxString wxTrack::DataModeToString(wxTrack::DataMode mode)
{
	wxString s;
	for( size_t i=0; i<DataModeStringSize; i++ )
	{
		if ( DataModeString[i].mode == mode )
		{
			s = DataModeString[i].szName;
			break;
		}
	}
	return s;
}

bool wxTrack::StringToDataMode( const wxString& s, wxTrack::DataMode& mode )
{
	for( size_t i=0; i<DataModeStringSize; i++ )
	{
		if ( s.CmpNoCase( DataModeString[i].szName ) == 0 )
		{
			mode = DataModeString[i].mode;
			return true;
		}
	}
	return false;
}

bool wxTrack::SetMode(const wxString& sMode)
{
	DataMode mode;
	if ( sMode.IsEmpty() )
	{
		mode = AUDIO;
	}
	else
	{
		if( !StringToDataMode( sMode, mode ) )
		{
			return false;
		}
	}
	
	m_dataMode = mode;
	return true;
}

void wxTrack::SetDataFile(const wxDataFile& df)
{
	m_df = df;
}

void wxTrack::ClearDataFile()
{
	m_df.Clear();
}

bool wxTrack::IsValid() const
{
	return (m_number < 100);
}

int wxTrack::CompareFn( wxTrack** t1, wxTrack** t2 )
{
	if ( (*t1)->GetNumber() < (*t2)->GetNumber() )
	{
		return -1;
	}
	else if ( (*t1)->GetNumber() > (*t2)->GetNumber() )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

wxArrayIndex& wxTrack::SortIndicies()
{
	m_indexes.Sort( wxIndex::CompareFn );
	return m_indexes;
}

const wxIndex& wxTrack::GetFirstIndex() const
{
	size_t idxs = m_indexes.Count();
	for( size_t i=0; i<idxs; i++ )
	{
		if ( m_indexes[i].GetNumber() == 1 )
		{
			return m_indexes[i];
		}
	}

	wxASSERT( false );
	return m_indexes[0];
}

void wxTrack::GetReplacements( wxHashString& replacements ) const
{
	wxCueComponent::GetReplacements( replacements );
	wxString sValue;
	sValue.Printf( wxT("%02d"), m_number );
	replacements[ wxT("tn") ] = sValue;
}

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayTrack );

