/*
	wxCueSheet.cpp
*/

#include "StdWx.h"
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheet.h>

wxIMPLEMENT_DYNAMIC_CLASS( wxCueSheet, wxCueComponent )

wxCueSheet::wxCueSheet(void)
{
}

wxCueSheet::wxCueSheet( const wxCueSheet& cs )
{
	copy( cs );
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

wxCueSheet& wxCueSheet::SetCatalog( const wxString& sCatalog )
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

wxCueSheet& wxCueSheet::AddTrack( const wxTrack& track )
{
	m_tracks.Add( track );
	return *this;
}

wxTrack& wxCueSheet::GetTrack(size_t idx)
{
	return m_tracks[idx];
}

bool wxCueSheet::HasTrack( unsigned long trackNo ) const
{
	size_t numTracks = m_tracks.Count();
	for( size_t i=0; i<numTracks; i++ )
	{
		if ( m_tracks[i].GetNumber() == trackNo )
		{
			return true;
		}
	}
	return false;
}

wxTrack& wxCueSheet::GetTrackByNumber( unsigned long trackNo )
{
	wxASSERT( HasTrack( trackNo ) );

	size_t numTracks = m_tracks.Count();
	for( size_t i=0; i<numTracks; i++ )
	{
		if ( m_tracks[i].GetNumber() == trackNo )
		{
			return m_tracks[i];
		}
	}

	wxASSERT( false );
	return m_tracks[0];
}

wxTrack& wxCueSheet::GetLastTrack()
{
	return m_tracks.Last();
}

const wxTrack& wxCueSheet::GetTrack( size_t idx ) const
{
	return m_tracks[idx];
}

const wxTrack& wxCueSheet::GetLastTrack() const
{
	return m_tracks.Last();
}

bool wxCueSheet::HasGarbage() const
{
	bool bRes = (m_garbage.Count() > 0 );
	size_t nTracks = m_tracks.Count();
	size_t i = 0;
	while( !bRes && (i < nTracks) )
	{
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

void wxCueSheet::copy( const wxCueSheet& cs )
{
	wxCueComponent::copy( cs );
	m_sCatalog = cs.m_sCatalog;
	m_sCdTextFile = cs.m_sCdTextFile;
	m_tracks = cs.m_tracks;
}

wxCueSheet& wxCueSheet::Append( const wxCueSheet& cs, wxULongLong offset, const wxSamplingInfo& si )
{
	wxCueComponent::Append( cs );

	if ( m_sCatalog.IsEmpty() )
	{
		m_sCatalog = cs.m_sCatalog;
	}

	if ( m_sCdTextFile.IsEmpty() )
	{
		m_sCdTextFile = cs.m_sCdTextFile;
	}

	wxTrack& lastTrack = GetLastTrack();
	size_t nNumberOffset = lastTrack.GetNumber() + 1;

	wxArrayTrack tracks( cs.m_tracks );
	for( size_t nCount = tracks.Count(), i = 0; i < nCount; i++ )
	{
		tracks[i].SetNumber( nNumberOffset + tracks[i].GetNumber() );
		tracks[i].Shift( offset, si );
	}

	WX_APPEND_ARRAY( m_tracks, tracks );
	return *this;
}

bool wxCueSheet::Append( const wxCueSheet& cs, const wxString& sAlternateExt )
{
	size_t nLastTrack;
	wxDataFile dataFile;
	if ( !cs.GetLastDataFile( nLastTrack, dataFile ) )
	{
		return false;
	}

	wxSamplingInfo si;
	wxULongLong frames;
	if ( dataFile.GetInfo( si, frames, sAlternateExt ) )
	{
		wxLogDebug( wxT("File: \u201C%s\u201D, frames: %s"), dataFile.GetFileName(), frames.ToString() );
		Append( cs, frames, si );
		return true;
	}
	else
	{
		return false;
	}
}

wxArrayTrack& wxCueSheet::SortTracks()
{
	for( size_t nCount = m_tracks.Count(), i=0; i < nCount; i++ )
	{
		m_tracks[i].SortIndicies();
	}
	m_tracks.Sort( wxTrack::CompareFn );
	return m_tracks;
}

bool wxCueSheet::GetLastDataFile( size_t& trackNo, wxDataFile& dataFile ) const
{
	size_t tracks = m_tracks.Count();
	if ( tracks == 0 ) return false;

	for( size_t i = tracks - 1; i > 0; i -= 1 )
	{
		if ( m_tracks[i].HasDataFile() )
		{
			trackNo = i;
			dataFile = m_tracks[i].GetDataFile();
			return true;
		}
	}

	if ( m_tracks[0].HasDataFile() )
	{
		trackNo = 0;
		dataFile = m_tracks[0].GetDataFile();
		return true;
	}
	else
	{
		return false;
	}
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
	const wxTrack& track = GetTrack( trackNo );
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

wxString wxCueSheet::Format( const wxString& sFmt ) const
{
	wxHashString replacements;
	GetReplacements( replacements );

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

wxCueSheet& wxCueSheet::SetDataFiles( const wxArrayDataFile& dataFile )
{
	size_t j=0;

	for( size_t nCount = m_tracks.Count(), i=0; ( i < nCount ) && ( j<dataFile.Count() ); i++ )
	{
		if ( m_tracks[i].HasDataFile() )
		{
			m_tracks[i].SetDataFile( dataFile[j++] );
		}
	}

	if ( j < dataFile.Count() )
	{
		wxLogWarning( _("Not all data files in cue sheet are replaced") );
		wxLogWarning( _("%d data file(s) left"), (dataFile.Count()-j) );
	}
	return *this;
}
