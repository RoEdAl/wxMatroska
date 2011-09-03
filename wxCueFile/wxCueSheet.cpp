/*
   wxCueSheet.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxTextOutputStreamOnString.h>
#include <wxCueFile/wxDuration.h>
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheetContent.h>
#include <wxCueFile/wxCueSheet.h>

// ===============================================================================

const wxChar* const wxCueSheet::CD_ALIASES[] =
{
	wxT( "cd" ),
	wxT( "vol" ),
	wxT( "volume" ),
	wxT( "disc" ),
	wxT( "disk" ),
	wxT( "dysk" ),
	wxT( "disque" )
};

const size_t wxCueSheet::CD_ALIASES_SIZE = WXSIZEOF( wxCueSheet::CD_ALIASES );

// ===============================================================================

const wxChar wxCueSheet::ALBUM_REG_EX1[] =
	wxT( "\\A(.*[^[:space:]])[[:space:]]*([[:punct:]][[:space:]]*%s[[:space:][:punct:]]*([[:digit:]]{1,2})[[:space:]]*[[:punct:]])[[:space:]]*([^[:space:]].*){0,1}\\Z" );

const wxChar wxCueSheet::ALBUM_REG_EX2[] =
	wxT( "\\A(.*[^[:space:]])[[:space:]]*(%s[[:space:][:punct:]]*([[:digit:]]{1,2}))[[:space:]]*([^[:space:]].*){0,1}\\Z" );

// ===============================================================================

wxString wxCueSheet::GetCdAliasesRegExp()
{
	wxTextOutputStreamOnString tos;

	for ( size_t i = 0; i < CD_ALIASES_SIZE; i++ )
	{
		*tos << CD_ALIASES[ i ] << wxT( '|' );
	}

	const wxString& s = tos.GetString();
	wxASSERT( !s.IsEmpty() );
	return wxString::Format( wxT( "(%s)" ), s.Left( s.Length() - 1 ) );
}

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxCueSheet, wxCueComponent );

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

size_t wxCueSheet::GetCatalogsCount() const
{
	return m_catalog.GetCount();
}

const wxArrayCueTag& wxCueSheet::GetCatalogs() const
{
	return m_catalog;
}

wxCueSheet& wxCueSheet::AddCatalog( const wxString& sCatalog )
{
	m_catalog.Add( wxCueTag( wxCueTag::TAG_UNKNOWN, wxCueTag::Name::CATALOG, sCatalog ) );
	return *this;
}

size_t wxCueSheet::GetCdTextFilesCount() const
{
	return m_cdtextfile.GetCount();
}

const wxArrayCueTag& wxCueSheet::GetCdTextFiles() const
{
	return m_cdtextfile;
}

wxCueSheet& wxCueSheet::AddCdTextFile( const wxString& sCdTextFile )
{
	m_cdtextfile.Add( wxCueTag( wxCueTag::TAG_UNKNOWN, wxCueTag::Name::CDTEXTFILE, sCdTextFile ) );
	return *this;
}

size_t wxCueSheet::GetContentsCount() const
{
	return m_content.GetCount();
}

const wxArrayCueSheetContent& wxCueSheet::GetContents() const
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

size_t wxCueSheet::GetLogsCount() const
{
	return m_log.GetCount();
}

const wxArrayFileName& wxCueSheet::GetLogs() const
{
	return m_log;
}

size_t wxCueSheet::GetCoversCount() const
{
	return m_cover.GetCount();
}

const wxArrayFileName& wxCueSheet::GetCovers() const
{
	return m_cover;
}

wxCueSheet& wxCueSheet::AddLog( const wxFileName& logFile )
{
	m_log.Add( logFile );
	return *this;
}

bool wxCueSheet::AddCover( const wxFileName& cover )
{
	bool bAdd = true;

	for ( size_t i = 0, nCount = m_cover.GetCount(); i < nCount; i++ )
	{
		if ( m_cover[ i ] == cover )
		{
			wxLogDebug( wxT( "Adding cover %s second time." ), cover.GetFullName() );
			bAdd = false;
			break;
		}
	}

	if ( bAdd )
	{
		m_cover.Add( cover );
	}

	return bAdd;
}

size_t wxCueSheet::GetTracksCount() const
{
	return m_tracks.GetCount();
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
	for ( size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; i++ )
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

	for ( size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; i++ )
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
	bool   bRes	   = ( m_garbage.GetCount() > 0 );
	size_t nTracks = m_tracks.GetCount();
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
	m_cover.Clear();
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
	m_cover		 = cs.m_cover;
	m_catalog	 = cs.m_catalog;
	m_cdtextfile = cs.m_cdtextfile;
	m_tracks	 = cs.m_tracks;
}

void wxCueSheet::AddCdTextInfoTagToAllTracks( const wxCueTag& tag )
{
	for ( size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; i++ )
	{
		m_tracks[ i ].AddCdTextInfoTag( tag );
	}
}

void wxCueSheet::AddTagToAllTracks( const wxCueTag& tag )
{
	for ( size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; i++ )
	{
		m_tracks[ i ].AddTag( tag );
	}
}

void wxCueSheet::PrepareToAppend()
{
	for ( size_t i = 0, nCount = m_cdTextTags.GetCount(); i < nCount; i++ )
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

	for ( size_t i = 0, nCount = m_tags.GetCount(); i < nCount; i++ )
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

	for ( size_t i = 0, nCount = m_catalog.GetCount(); i < nCount; i++ )
	{
		AddTagToAllTracks( m_catalog[ i ] );
	}

	m_catalog.Clear();
	m_cdTextTags.Clear();
	m_tags.Clear();
}

void wxCueSheet::AppendFileNames( wxArrayFileName& dest, const wxArrayFileName& source )
{
	bool bAdd;

	for ( size_t i = 0, nSourceCount = source.GetCount(); i < nSourceCount; i++ )
	{
		bAdd = true;
		for ( size_t j = 0, nDestCount = dest.GetCount(); j < nDestCount; j++ )
		{
			if ( dest[ j ] == source[ i ] )
			{
				bAdd = false;
				break;
			}
		}

		if ( bAdd )
		{
			dest.Add( source[ i ] );
		}
	}
}

wxCueSheet& wxCueSheet::Append( const wxCueSheet& _cs, const wxDuration& offset )
{
	wxCueSheet cs( _cs );

	cs.PrepareToAppend();

	wxCueComponent::Append( cs );

	WX_APPEND_ARRAY( m_content, cs.m_content );
	AppendFileNames( m_log, cs.m_log );
	AppendFileNames( m_cover, cs.m_cover );
	WX_APPEND_ARRAY( m_catalog, cs.m_catalog );
	WX_APPEND_ARRAY( m_cdtextfile, cs.m_cdtextfile );

	size_t nNumberOffset = 0;
	if ( !m_tracks.IsEmpty() )
	{
		wxTrack& lastTrack = GetLastTrack();
		nNumberOffset = lastTrack.GetNumber();
	}

	wxArrayTrack tracks( cs.m_tracks );
	for ( size_t i = 0, nCount = tracks.GetCount(); i < nCount; i++ )
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
	for ( size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; i++ )
	{
		m_tracks[ i ].SortIndicies();
	}

	m_tracks.Sort( wxTrack::CompareFn );
	return m_tracks;
}

bool wxCueSheet::IsLastTrackForDataFile( size_t trackNo, wxDataFile& dataFile ) const
{
	bool   res	  = false;
	size_t tracks = m_tracks.GetCount();

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
		sFind.Prepend( wxT( '%' ) ).Append( wxT( '%' ) );
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
		sFind.Prepend( wxT( '%' ) ).Append( wxT( '%' ) );
		s.Replace( sFind, i->second, true );
	}

	return s;
}

bool wxCueSheet::HasSingleDataFile( wxDataFile& dataFile ) const
{
	size_t tracks	= m_tracks.GetCount();
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
	for ( size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; i++ )
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

	for ( size_t i = 0, nCount = m_tracks.GetCount(), nDataFileCount = dataFile.GetCount(); ( i < nCount ) && ( j < nDataFileCount ); i++ )
	{
		if ( m_tracks[ i ].HasDataFile() )
		{
			m_tracks[ i ].SetDataFile( dataFile[ j++ ] );
		}
	}

	if ( j < dataFile.GetCount() )
	{
		wxLogWarning( _( "Not all data files in cue sheet are replaced" ) );
		wxLogWarning( _( "%d data file(s) left" ), ( dataFile.GetCount() - j ) );
	}

	return *this;
}

bool wxCueSheet::HasDuration() const
{
	bool bRes = true;

	for ( size_t i = 0, nCount = m_tracks.GetCount(); ( i < nCount ) && bRes; i++ )
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

	for ( size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; i++ )
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

	for ( size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; i++ )
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

static size_t only_suitable_tags( wxArrayCueTag& tags )
{
	wxCueComponent::ENTRY_TYPE eEntryType;
	size_t					   nCounter = 0;

	for ( size_t i = 0, nCount = tags.GetCount(); i < nCount; i++ )
	{
		if ( wxCueComponent::GetCdTextInfoType( tags[ i ].GetName(), eEntryType ) &&
			 eEntryType == wxCueComponent::TRACK )
		{
			tags.RemoveAt( i );
			nCounter += 1;
			nCount	 -= 1;
			i		 -= 1;
		}
	}

	return nCounter;
}

static wxString concatenate( const wxString& s1, const wxString& s2 )
{
	if ( s1.IsEmpty() && s2.IsEmpty() )
	{
		return wxEmptyString;
	}
	else if ( s1.IsEmpty() && !s2.IsEmpty() )
	{
		return s2;
	}
	else if ( !s1.IsEmpty() && s2.IsEmpty() )
	{
		return s1;
	}
	else
	{
		return s1 + wxT( ' ' ) + s2;
	}
}

void wxCueSheet::FindCommonTags( const wxTagSynonimsCollection& discSynonims, const wxTagSynonimsCollection& trackSynonims, bool bMerge )
{
	size_t nTracks = m_tracks.GetCount();

	MoveCdTextInfoTags( discSynonims );
	for ( size_t i = 0; i < nTracks; i++ )
	{
		m_tracks[ i ].MoveCdTextInfoTags( trackSynonims );
	}

	if ( nTracks <= 1 )
	{
		wxLogDebug( wxT( "wxCueSheet::FindCommonTags - too few tracks" ) );
		return;
	}

	{
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
	}

	// ALBUM -> TITLE
	{
		wxArrayCueTag albumTags;
		MoveTags( wxCueTag::Name::ALBUM, albumTags );
		for ( size_t i = 0, nCount = albumTags.GetCount(); i < nCount; i++ )
		{
			AddCdTextInfoTag( wxCueTag( albumTags[ i ].GetSource(), wxCueTag::Name::TITLE, albumTags[ i ].GetValue() ) );
		}
	}

	// Trying to find common part of album tag
	if ( bMerge )
	{
		AddTag( wxCueTag(
				wxCueTag::TAG_AUTO_GENERATED,
				wxCueTag::Name::TOTALDISCS,
				wxString::Format( wxT( "%d" ), m_content.GetCount() ) ) );

		wxArrayCueTag albumTags;
		size_t		  nElements = 1;

		for ( size_t i = 0; i < nTracks && nElements > 0; i++ )
		{
			nElements = m_tracks[ i ].GetTags( wxCueTag::Name::ALBUM, albumTags );
		}

		if ( nElements > 0 )
		{
			wxASSERT( albumTags.GetCount() >= 1 );
			WX_DECLARE_STRING_HASH_MAP( unsigned long, wxHashMapStringToULong );
			wxHashMapStringToULong albumNumbers;

			bool	 bFirst	   = true;
			bool	 bIsCommon = true;
			wxString sCommonAlbum;
			wxRegEx	 reDisc1( wxString::Format( ALBUM_REG_EX1, GetCdAliasesRegExp() ), wxRE_ADVANCED | wxRE_ICASE );
			wxASSERT( reDisc1.IsValid() );
			wxRegEx reDisc2( wxString::Format( ALBUM_REG_EX2, GetCdAliasesRegExp() ), wxRE_ADVANCED | wxRE_ICASE );
			wxASSERT( reDisc2.IsValid() );
			for ( size_t j = 0, nCount = albumTags.GetCount(); j < nCount && bIsCommon; j++ )
			{
				wxString sAlbum( albumTags[ j ].GetValue() );
				if ( reDisc1.Matches( sAlbum ) )
				{
					wxASSERT( reDisc1.GetMatchCount() > 5 );
					wxString sLocalAlbum1( reDisc1.GetMatch( sAlbum, 1 ) );
					wxString sDiscNumber( reDisc1.GetMatch( sAlbum, 4 ) );
					wxString sLocalAlbum2( reDisc1.GetMatch( sAlbum, 5 ) );

					unsigned long u;
					if ( sDiscNumber.ToULong( &u ) )
					{
						albumNumbers[ albumTags[ j ].GetValue() ] = u;
					}

					sAlbum = concatenate( sLocalAlbum1, sLocalAlbum2 );
				}
				else if ( reDisc2.Matches( sAlbum ) )
				{
					wxASSERT( reDisc1.GetMatchCount() > 5 );
					wxString sLocalAlbum1( reDisc2.GetMatch( sAlbum, 1 ) );
					wxString sDiscNumber( reDisc2.GetMatch( sAlbum, 4 ) );
					wxString sLocalAlbum2( reDisc2.GetMatch( sAlbum, 5 ) );

					unsigned long u;
					if ( sDiscNumber.ToULong( &u ) )
					{
						albumNumbers[ albumTags[ j ].GetValue() ] = u;
					}

					sAlbum = concatenate( sLocalAlbum1, sLocalAlbum2 );
				}

				if ( bFirst )
				{
					sCommonAlbum = sAlbum;
					bFirst		 = false;
				}
				else if ( sCommonAlbum.Cmp( sAlbum ) != 0 )
				{
					bIsCommon = false;
				}
			}

			if ( bIsCommon )
			{
				wxLogInfo( _( "Album name: \u201C%s\u201D" ), sCommonAlbum );
				AddCdTextInfoTag( wxCueTag( wxCueTag::TAG_AUTO_GENERATED, wxCueTag::Name::TITLE, sCommonAlbum ) );

				for ( wxHashMapStringToULong::const_iterator i = albumNumbers.begin(), iend = albumNumbers.end(); i != iend; i++ )
				{
					wxCueTag discNumberTag( wxCueTag::TAG_AUTO_GENERATED, wxCueTag::Name::DISCNUMBER, wxString::Format( wxT( "%u" ), i->second ) );
					wxCueTag albumTag( wxCueTag::TAG_AUTO_GENERATED, wxCueTag::Name::ALBUM, i->first );

					for ( size_t j = 0; j < nTracks; j++ )
					{
						m_tracks[ j ].AddTagIfAndRemove( discNumberTag, albumTag );
					}
				}
			}
		}
	}
}

