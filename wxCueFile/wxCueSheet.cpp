/*
	wxCueSheet.cpp
*/

#include "StdWx.h"
#include "wxTrack.h"
#include "wxCueSheet.h"

IMPLEMENT_DYNAMIC_CLASS( wxCueSheet, wxCueComponent )

wxCueSheet::wxCueSheet(void)
{
}

wxCueSheet::wxCueSheet(const wxCueSheet& cs)
{
	copy( cs );
}


wxCueSheet::~wxCueSheet(void)
{
}

wxCueSheet& wxCueSheet::operator =(const wxCueSheet& cs)
{
	copy( cs );
	return *this;
}

const wxString& wxCueSheet::GetCatalog() const
{
	return m_sCatalog;
}

wxCueSheet& wxCueSheet::SetCatalog(const wxString& sCatalog)
{
	m_sCatalog = sCatalog;
	return *this;
}

const wxString& wxCueSheet::GetCdTextFile() const
{
	return m_sCdTextFile;
}

wxCueSheet& wxCueSheet::SetCdTextFile(const wxString& sCdTextFile)
{
	m_sCdTextFile = sCdTextFile;
	return *this;
}

const wxArrayTrack& wxCueSheet::GetTracks() const
{
	return m_tracks;
}

wxCueSheet& wxCueSheet::AddTrack(const wxTrack& track)
{
	m_tracks.Add( track );
	return *this;
}

wxTrack& wxCueSheet::GetTrack(size_t idx)
{
	return m_tracks[idx];
}

wxTrack& wxCueSheet::GetLastTrack()
{
	return m_tracks.Last();
}

const wxTrack& wxCueSheet::GetTrackConst(size_t idx) const
{
	return m_tracks[idx];
}

const wxTrack& wxCueSheet::GetLastTrackConst() const
{
	return m_tracks.Last();
}

bool wxCueSheet::HasGarbage() const
{
	bool bRes = (m_garbage.Count() > 0 );
	size_t nTracks = m_tracks.Count();
	size_t i = 0;
	while( !bRes && (i < nTracks) ) {
		bRes = bRes && m_tracks[i].HasGarbage();
		i++;
	}
	return bRes;
}

void wxCueSheet::Clear(void)
{
	wxCueComponent::Clear();
	m_sCatalog.Empty();
	m_sCdTextFile.Empty();
	m_tracks.Clear();
}

void wxCueSheet::copy(const wxCueSheet& cs)
{
	wxCueComponent::copy( cs );
	m_sCatalog = cs.m_sCatalog;
	m_sCdTextFile = cs.m_sCdTextFile;
	m_tracks = cs.m_tracks;
}

void wxCueSheet::ToStream(wxTextOutputStream& stream, int nDumpFlags ) const
{
	wxCueComponent::ToStream( stream, nDumpFlags );
	DumpString( stream, wxT("CATALOG"), m_sCatalog );

	if ( !m_sCdTextFile.IsEmpty() )
	{
		wxString sQuoted( m_sCdTextFile );
		sQuoted.Prepend( wxT('"') ).Append( wxT('"') );
		DumpString( stream, wxT("CDTEXTFILE"), sQuoted );
	}

	size_t tracks = m_tracks.Count();
	for( size_t i=0; i<tracks; i++ )
	{
		m_tracks[i].ToStream( stream, nDumpFlags );
	}
}

wxArrayTrack& wxCueSheet::SortTracks()
{
	size_t tracks = m_tracks.Count();
	for( size_t i=0; i<tracks; i++ )
	{
		m_tracks[i].SortIndicies();
	}
	m_tracks.Sort( wxTrack::CompareFn );
	return m_tracks;
}

bool wxCueSheet::IsLastTrackForDataFile( size_t trackNo, wxDataFile& dataFile ) const
{
	bool res = false;
	size_t tracks = m_tracks.Count();
	if ( (trackNo+1) < tracks )
	{
		if ( !m_tracks[trackNo+1].GetDataFile().IsEmpty() )
		{
			res = true;
		}
	}
	else if ( (trackNo+1) == tracks )
	{
		res = true;
	}

	if ( res )
	{
		bool found = false;
		while( trackNo > 0 )
		{
			if ( !m_tracks[trackNo].GetDataFile().IsEmpty() )
			{
				dataFile = m_tracks[trackNo].GetDataFile();
				found = true;
				break;
			}
			trackNo--;
		}

		if ( !found ) // zero
		{
			if ( !m_tracks[0].GetDataFile().IsEmpty() )
			{
				dataFile = m_tracks[0].GetDataFile();
				found = true;
			}
		}

		if ( !found ) res = false;
	}
	return res;
}

wxString wxCueSheet::FormatTrack(size_t trackNo, const wxString& sFmt ) const
{
	wxHashString replacements;
	GetReplacements( replacements );
	const wxTrack& track = GetTrackConst( trackNo );
	track.GetReplacements( replacements );

	wxString s( sFmt );
	for( wxHashString::const_iterator i = replacements.begin(); i != replacements.end(); i++ )
	{
		wxString sFind( i->first );
		sFind.Prepend( wxT("%") ).Append( wxT("%") );
		s.Replace( sFind, i->second, true );
	}

	return s;
}

bool wxCueSheet::HasSingleDataFile( wxDataFile& dataFile ) const
{
	size_t tracks = m_tracks.Count();
	size_t nCounter = 0;
	bool bFirst = false;
	for( size_t i=0; (i<tracks) && (nCounter<2); i++ )
	{
		if ( ( m_tracks[i].GetNumber() == 1 ) && m_tracks[i].HasDataFile() ) 
		{
			dataFile = m_tracks[i].GetDataFile();
			bFirst = true;
		}
		if ( m_tracks[i].HasDataFile() ) nCounter += 1;
	}

	return bFirst && (nCounter==1);
}

bool wxCueSheet::HasSingleDataFile() const
{
	wxDataFile dummy;
	return HasSingleDataFile( dummy );
}

wxCueSheet& wxCueSheet::SetSingleDataFile( const wxDataFile& dataFile )
{
	size_t tracks = m_tracks.Count();
	for( size_t i=0; i<tracks; i++ )
	{
		if ( m_tracks[i].GetNumber() == 1 )
		{
			m_tracks[i].SetDataFile( dataFile );
		}
		else
		{
			m_tracks[i].ClearDataFile();
		}
	}
	return *this;
}
