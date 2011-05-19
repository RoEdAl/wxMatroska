/*
   wxCueSheet.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxDuration.h>
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheetContent.h>
#include <wxCueFile/wxCueSheet.h>

// ===============================================================================

const wxChar* const wxCueSheet::CD_ALIASES[] =
{
	wxT( "cd" ),
	wxT( "disc" ),
	wxT( "disk" ),
	wxT( "dysk" ),
	wxT( "disque" )
};

const size_t wxCueSheet::CD_ALIASES_SIZE = WXSIZEOF( wxCueSheet::CD_ALIASES );

const wxChar wxCueSheet::ALBUM_REG_EX[] =
	wxT( "\\A(.+[^[:space:][:punct:]\\(\\[])[[:space:][:punct:]]*[\\(\\[]{0,1}[[:space:]]*%s[[:space:][:punct:]]*\\Z" );

wxString wxCueSheet::GetCdAliasesRegExp()
{
	wxString s;

	for ( size_t i = 0; i < CD_ALIASES_SIZE; i++ )
	{
		s += CD_ALIASES[ i ];
		s += wxT( '|' );
	}

	s = s.RemoveLast();
	return wxString::Format( wxT( "(%s)" ), s );
}

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxCueSheet, wxCueComponent )
// ===============================================================================

wxCueSheet::wxCueSheet( void )
{}

wxCueSheet::wxCueSheet( const wxCueSheet& cs )
{
	copy( cs );
}

wxCueSheet& wxCueSheet::operator =( const wxCueSheet& cs )
{
	copy( cs );
	return *this;
}

const wxArrayCueTag& wxCueSheet::GetCatalog() const
{
	return m_catalog;
}

wxCueSheet& wxCueSheet::AddCatalog( const wxString& sCatalog )
{
	m_catalog.Add( wxCueTag( wxCueTag::TAG_UNKNOWN, wxCueTag::Name::CATALOG, sCatalog ) );
	return *this;
}

const wxArrayCueTag& wxCueSheet::GetCdTextFile() const
{
	return m_cdtextfile;
}

wxCueSheet& wxCueSheet::AddCdTextFile( const wxString& sCdTextFile )
{
	m_cdtextfile.Add( wxCueTag( wxCueTag::TAG_UNKNOWN, wxCueTag::Name::CDTEXTFILE, sCdTextFile ) );
	return *this;
}

const wxArrayCueSheetContent& wxCueSheet::GetContent() const
{
	return m_content;
}

wxCueSheet& wxCueSheet::AddContent( const wxCueSheetContent& content )
{
	m_content.Add( content );
	return *this;
}

wxCueSheet& wxCueSheet::AddContent( const wxString& sContent )
{
	m_content.Add( wxCueSheetContent( sContent ) );
	return *this;
}

const wxArrayFileName& wxCueSheet::GetLog() const
{
	return m_log;
}

wxCueSheet& wxCueSheet::AddLog( const wxFileName& logFile )
{
	m_log.Add( logFile );
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

wxTrack& wxCueSheet::GetTrack( size_t idx )
{
	return m_tracks[ idx ];
}

bool wxCueSheet::HasTrack( unsigned long trackNo ) const
{
	size_t numTracks = m_tracks.Count();

	for ( size_t i = 0; i < numTracks; i++ )
	{
		if ( m_tracks[ i ].GetNumber() == trackNo )
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
	for ( size_t i = 0; i < numTracks; i++ )
	{
		if ( m_tracks[ i ].GetNumber() == trackNo )
		{
			return m_tracks[ i ];
		}
	}

	wxASSERT( false );
	return m_tracks[ 0 ];
}

wxTrack& wxCueSheet::GetLastTrack()
{
	return m_tracks.Last();
}

const wxTrack& wxCueSheet::GetTrack( size_t idx ) const
{
	return m_tracks[ idx ];
}

const wxTrack& wxCueSheet::GetLastTrack() const
{
	return m_tracks.Last();
}

bool wxCueSheet::HasGarbage() const
{
	bool   bRes	   = ( m_garbage.Count() > 0 );
	size_t nTracks = m_tracks.Count();
	size_t i	   = 0;

	while ( !bRes && ( i < nTracks ) )
	{
		bRes = bRes && m_tracks[ i ].HasGarbage();
		i++;
	}

	return bRes;
}

void wxCueSheet::Clear( void )
{
	m_content.Clear();
	m_log.Clear();
	wxCueComponent::Clear();
	m_catalog.Clear();
	m_cdtextfile.Clear();
	m_tracks.Clear();
}

void wxCueSheet::copy( const wxCueSheet& cs )
{
	wxCueComponent::copy( cs );
	m_content	 = cs.m_content;
	m_log		 = cs.m_log;
	m_catalog	 = cs.m_catalog;
	m_cdtextfile = cs.m_cdtextfile;
	m_tracks	 = cs.m_tracks;
}

void wxCueSheet::AddCdTextInfoTagToAllTracks( const wxCueTag& tag )
{
	for ( size_t i = 0, nCount = m_tracks.Count(); i < nCount; i++ )
	{
		m_tracks[ i ].AddCdTextInfoTag( tag );
	}
}

void wxCueSheet::AddTagToAllTracks( const wxCueTag& tag )
{
	for ( size_t i = 0, nCount = m_tracks.Count(); i < nCount; i++ )
	{
		m_tracks[ i ].AddTag( tag );
	}
}

void wxCueSheet::PrepareToAppend()
{
	for ( size_t i = 0, nCount = m_cdTextTags.Count(); i < nCount; i++ )
	{
		if ( m_cdTextTags[ i ] == wxCueTag::Name::TITLE )
		{
			wxCueTag albumTag( m_cdTextTags[ i ].GetSource(), wxCueTag::Name::ALBUM, m_cdTextTags[ i ].GetValue() );
			AddTagToAllTracks( albumTag );
		}
		else
		{
			AddCdTextInfoTagToAllTracks( m_cdTextTags[ i ] );
		}
	}

	for ( size_t i = 0, nCount = m_tags.Count(); i < nCount; i++ )
	{
		if ( m_tags[ i ] == wxCueTag::Name::ARTIST )
		{
			wxCueTag albumArtistTag( m_tags[ i ].GetSource(), wxCueTag::Name::ALBUM_ARTIST, m_tags[ i ].GetValue() );
			AddTagToAllTracks( albumArtistTag );
		}
		else
		{
			AddTagToAllTracks( m_tags[ i ] );
		}
	}

	for ( size_t i = 0, nCount = m_catalog.Count(); i < nCount; i++ )
	{
		AddTagToAllTracks( m_catalog[ i ] );
	}

	m_catalog.Clear();
	m_cdTextTags.Clear();
	m_tags.Clear();
}

wxCueSheet& wxCueSheet::Append( const wxCueSheet& _cs, const wxDuration& offset )
{
	wxCueSheet cs( _cs );

	cs.PrepareToAppend();

	wxCueComponent::Append( cs );

	WX_APPEND_ARRAY( m_content, cs.m_content );
	WX_APPEND_ARRAY( m_log, cs.m_log );
	WX_APPEND_ARRAY( m_catalog, cs.m_catalog );
	WX_APPEND_ARRAY( m_cdtextfile, cs.m_cdtextfile );

	size_t nNumberOffset = 0;
	if ( !m_tracks.IsEmpty() )
	{
		wxTrack& lastTrack = GetLastTrack();
		nNumberOffset = lastTrack.GetNumber();
	}

	wxArrayTrack tracks( cs.m_tracks );
	for ( size_t nCount = tracks.Count(), i = 0; i < nCount; i++ )
	{
		tracks[ i ].SetNumber( nNumberOffset + tracks[ i ].GetNumber() );
		tracks[ i ].Shift( offset );
	}

	WX_APPEND_ARRAY( m_tracks, tracks );
	return *this;
}

bool wxCueSheet::Append( const wxCueSheet& cs )
{
	if ( !HasDuration() )
	{
		return false;
	}

	Append( cs, GetDuration() );
	return true;
}

wxArrayTrack& wxCueSheet::SortTracks()
{
	for ( size_t nCount = m_tracks.Count(), i = 0; i < nCount; i++ )
	{
		m_tracks[ i ].SortIndicies();
	}

	m_tracks.Sort( wxTrack::CompareFn );
	return m_tracks;
}

bool wxCueSheet::IsLastTrackForDataFile( size_t trackNo, wxDataFile& dataFile ) const
{
	bool   res	  = false;
	size_t tracks = m_tracks.Count();

	if ( ( trackNo + 1 ) < tracks )
	{
		if ( m_tracks[ trackNo + 1 ].HasDataFile() )
		{
			res = true;
		}
	}
	else if ( ( trackNo + 1 ) == tracks )
	{
		res = true;
	}

	if ( res )
	{
		bool found = false;
		while ( trackNo > 0 )
		{
			if ( m_tracks[ trackNo ].HasDataFile() )
			{
				dataFile = m_tracks[ trackNo ].GetDataFile();
				found	 = true;
				break;
			}

			trackNo--;
		}

		if ( !found ) // zero
		{
			if ( m_tracks[ 0 ].HasDataFile() )
			{
				dataFile = m_tracks[ 0 ].GetDataFile();
				found	 = true;
			}
		}

		if ( !found )
		{
			res = false;
		}
	}

	return res;
}

wxString wxCueSheet::FormatTrack( size_t trackNo, const wxString& sFmt ) const
{
	wxHashString replacements;

	GetReplacements( replacements );
	const wxTrack& track = GetTrack( trackNo );
	track.GetReplacements( replacements );

	wxString s( sFmt );
	for ( wxHashString::const_iterator i = replacements.begin(); i != replacements.end(); i++ )
	{
		wxString sFind( i->first );
		sFind.Prepend( wxT( "%" ) ).Append( wxT( "%" ) );
		s.Replace( sFind, i->second, true );
	}

	return s;
}

wxString wxCueSheet::Format( const wxString& sFmt ) const
{
	wxHashString replacements;

	GetReplacements( replacements );

	wxString s( sFmt );
	for ( wxHashString::const_iterator i = replacements.begin(); i != replacements.end(); i++ )
	{
		wxString sFind( i->first );
		sFind.Prepend( wxT( "%" ) ).Append( wxT( "%" ) );
		s.Replace( sFind, i->second, true );
	}

	return s;
}

bool wxCueSheet::HasSingleDataFile( wxDataFile& dataFile ) const
{
	size_t tracks	= m_tracks.Count();
	size_t nCounter = 0;
	bool   bFirst	= false;

	for ( size_t i = 0; ( i < tracks ) && ( nCounter < 2 ); i++ )
	{
		if ( ( m_tracks[ i ].GetNumber() == 1 ) && m_tracks[ i ].HasDataFile() )
		{
			dataFile = m_tracks[ i ].GetDataFile();
			bFirst	 = true;
		}

		if ( m_tracks[ i ].HasDataFile() )
		{
			nCounter += 1;
		}
	}

	return bFirst && ( nCounter == 1 );
}

bool wxCueSheet::HasSingleDataFile() const
{
	wxDataFile dummy;

	return HasSingleDataFile( dummy );
}

wxCueSheet& wxCueSheet::SetSingleDataFile( const wxDataFile& dataFile )
{
	size_t tracks = m_tracks.Count();

	for ( size_t i = 0; i < tracks; i++ )
	{
		if ( m_tracks[ i ].GetNumber() == 1 )
		{
			m_tracks[ i ].SetDataFile( dataFile );
		}
		else
		{
			m_tracks[ i ].ClearDataFile();
		}
	}

	return *this;
}

wxCueSheet& wxCueSheet::SetDataFiles( const wxArrayDataFile& dataFile )
{
	size_t j = 0;

	for ( size_t nCount = m_tracks.Count(), i = 0; ( i < nCount ) && ( j < dataFile.Count() ); i++ )
	{
		if ( m_tracks[ i ].HasDataFile() )
		{
			m_tracks[ i ].SetDataFile( dataFile[ j++ ] );
		}
	}

	if ( j < dataFile.Count() )
	{
		wxLogWarning( _( "Not all data files in cue sheet are replaced" ) );
		wxLogWarning( _( "%d data file(s) left" ), ( dataFile.Count() - j ) );
	}

	return *this;
}

bool wxCueSheet::HasDuration() const
{
	bool bRes = true;

	for ( size_t nCount = m_tracks.Count(), i = 0; ( i < nCount ) && bRes; i++ )
	{
		if ( m_tracks[ i ].HasDataFile() )
		{
			bRes = bRes && m_tracks[ i ].GetDataFile().HasDuration();
		}
	}

	return bRes;
}

wxDuration wxCueSheet::GetDuration() const
{
	wxASSERT( HasDuration() );

	wxDuration duration;
	bool	   bFirst = true;
	bool	   bStop  = false;

	for ( size_t nCount = m_tracks.Count(), i = 0; i < nCount; i++ )
	{
		if ( m_tracks[ i ].HasDataFile() )
		{
			const wxDuration& trackDuration = m_tracks[ i ].GetDataFile().GetDuration();
			if ( bFirst )
			{
				duration = trackDuration;
				bFirst	 = false;
			}
			else if ( !duration.Add( trackDuration ) )
			{
				bStop = true;
			}
		}
	}

	if ( bStop )
	{
		wxLogDebug( wxT( "Fail to calculate duration of cue sheet" ) );
		duration.Invalidate();
	}

	return duration;
}

bool wxCueSheet::CalculateDuration( const wxString& sAlternateExt )
{
	bool bRes = true;

	for ( size_t nCount = m_tracks.Count(), i = 0; i < nCount; i++ )
	{
		if ( m_tracks[ i ].HasDataFile() )
		{
			if ( !m_tracks[ i ].CalculateDuration( sAlternateExt ) )
			{
				wxLogDebug( wxT( "Fail to calculate duration for track %d" ), i );
				bRes = false;
			}
		}
	}

	return bRes;
}

static void only_suitable_tags( wxArrayCueTag& tags )
{
	wxArrayCueTag			   newTags;
	wxCueComponent::ENTRY_TYPE eEntryType;

	for ( size_t i = 0, nCount = tags.Count(); i < nCount; i++ )
	{
		if ( wxCueComponent::GetCdTextInfoType( tags[ i ].GetName(), eEntryType ) &&
			 eEntryType != wxCueComponent::TRACK )
		{
			newTags.Add( tags[ i ] );
		}
	}

	tags = newTags;
}

void wxCueSheet::FindCommonTags( bool bMerge )
{
	size_t nTracks = m_tracks.Count();

	if ( nTracks <= 1 )
	{
		wxLogDebug( wxT( "wxCueSheet::FindCommonTags - too few tracks" ) );
		return;
	}

	wxArrayCueTag commonTags;
	wxArrayCueTag group;

	// CD-TEXT
	group = m_tracks[ 0 ].GetCdTextTags();
	for ( size_t i = 1; i < nTracks; i++ )
	{
		wxCueTag::CommonTags( commonTags, group, m_tracks[ i ].GetCdTextTags() );
		group = commonTags;
	}

	only_suitable_tags( commonTags );

	AddCdTextInfoTags( commonTags );
	for ( size_t i = 0; i < nTracks; i++ )
	{
		m_tracks[ i ].RemoveCdTextInfoTags( commonTags );
	}

	// TAGS
	commonTags.Clear();
	group = m_tracks[ 0 ].GetTags();

	for ( size_t i = 1; i < nTracks; i++ )
	{
		wxCueTag::CommonTags( commonTags, group, m_tracks[ i ].GetTags() );
		group = commonTags;
	}

	AddTags( commonTags );
	for ( size_t i = 0; i < nTracks; i++ )
	{
		m_tracks[ i ].RemoveTags( commonTags );
	}

	// Trying to find common part of album tag
	if ( bMerge )
	{
		wxArrayCueTag albumTags;
		size_t		  nElements = 1;
		for ( size_t i = 0; i < nTracks && nElements > 0; i++ )
		{
			nElements = m_tracks[ i ].GetTags( wxCueTag::Name::ALBUM, albumTags );
		}

		if ( nElements > 0 )
		{
			wxASSERT( albumTags.Count() >= 1 );

			bool	 bFirst	   = true;
			bool	 bIsCommon = true;
			wxString sCommonAlbum;
			wxRegEx	 reDisc( wxString::Format( ALBUM_REG_EX, GetCdAliasesRegExp() ), wxRE_ADVANCED | wxRE_ICASE );
			wxASSERT( reDisc.IsValid() );
			for ( size_t j = 0, nCount = albumTags.Count(); j < nCount && bIsCommon; j++ )
			{
				wxString sAlbum( albumTags[ j ].GetValue() );
				if ( reDisc.Matches( sAlbum ) )
				{
					wxASSERT( reDisc.GetMatchCount() > 0 );
					sAlbum = reDisc.GetMatch( sAlbum, 1 );
				}

				if ( bFirst )
				{
					sCommonAlbum = sAlbum;
					bFirst		 = false;
				}
				else    if ( sCommonAlbum.Cmp( sAlbum ) != 0 )
				{
					bIsCommon = false;
				}
			}

			if ( bIsCommon )
			{
				wxLogInfo( wxT( "Album name: \u201C%s\u201D" ), sCommonAlbum );
				wxCueTag commonAlbumTag( wxCueTag::TAG_AUTO_GENERATED, wxCueTag::Name::TITLE, sCommonAlbum );
				AddCdTextInfoTag( commonAlbumTag );
				for ( size_t i = 0; i < nTracks; i++ )
				{
					m_tracks[ i ].RemoveTag( wxCueTag::Name::ALBUM );
				}
			}
		}
	}
}

