/*
 * wxCueSheet.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxSamplingInfo.h>
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>
#include <wxCueFile/wxDuration.h>
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheetContent.h>
#include <wxCueFile/wxCueSheet.h>

// ===============================================================================

const char* const wxCueSheet::CD_ALIASES[] =
{
	"cd",
	"vol",
	"volume",
	"disc",
	"disk",
	"dysk",
	"disque"
};

const size_t wxCueSheet::CD_ALIASES_SIZE = WXSIZEOF( wxCueSheet::CD_ALIASES );

// ===============================================================================

const char wxCueSheet::ALBUM_REG_EX1[] =
	"\\A(.*[^[:space:]])[[:space:]]*([[:punct:]][[:space:]]*%s[[:space:][:punct:]]*([[:digit:]]{1,2})[[:space:]]*[[:punct:]])[[:space:]]*([^[:space:]].*){0,1}\\Z";

const char wxCueSheet::ALBUM_REG_EX2[] =
	"\\A(.*[^[:space:]])[[:space:]]*(%s[[:space:][:punct:]]*([[:digit:]]{1,2}))[[:space:]]*([^[:space:]].*){0,1}\\Z";

// ===============================================================================

wxString wxCueSheet::GetCdAliasesRegExp()
{
	wxTextOutputStreamOnString tos;

	for ( size_t i = 0; i < CD_ALIASES_SIZE; i++ )
	{
		*tos << CD_ALIASES[ i ] << '|';
	}

	const wxString& s = tos.GetString();
	wxASSERT( !s.IsEmpty() );
	return wxString::Format( "(%s)", s.Left( s.Length() - 1 ) );
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
	return m_catalogs.GetCount();
}

const wxArrayCueTag& wxCueSheet::GetCatalogs() const
{
	return m_catalogs;
}

wxCueSheet& wxCueSheet::AddCatalog( const wxString& sCatalog )
{
	m_catalogs.Add( wxCueTag( wxCueTag::TAG_UNKNOWN, wxCueTag::Name::CATALOG, sCatalog ) );
	return *this;
}

size_t wxCueSheet::GetCdTextFilesCount() const
{
	return m_cdtextfiles.GetCount();
}

const wxArrayFileName& wxCueSheet::GetCdTextFiles() const
{
	return m_cdtextfiles;
}

wxCueSheet& wxCueSheet::AddCdTextFile( const wxFileName& cdTextFile )
{
	m_cdtextfiles.Add( cdTextFile );
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
	return m_logs.GetCount();
}

const wxArrayFileName& wxCueSheet::GetLogs() const
{
	return m_logs;
}

size_t wxCueSheet::GetCoversCount() const
{
	return m_covers.GetCount();
}

const wxArrayCoverFile& wxCueSheet::GetCovers( ) const
{
	return m_covers;
}

void wxCueSheet::GetSortedCovers( wxArrayCoverFile& covers ) const
{
    covers.Clear();
    WX_APPEND_ARRAY( covers, m_covers );
    wxCoverFile::Sort( covers );
}

wxCueSheet& wxCueSheet::AddLog( const wxFileName& logFile )
{
	m_logs.Add( logFile );
	return *this;
}

void wxCueSheet::AddCover( const wxFileName& coverFn )
{
    AddCover( wxCoverFile( coverFn, wxCoverFile::FrontCover ) );
}

void wxCueSheet::AddCover( const wxCoverFile& cover )
{
    wxCoverFile::Append( m_covers, cover );
}

void wxCueSheet::AddCovers( const wxArrayCoverFile& covers )
{
    wxCoverFile::Append( m_covers, covers );
}

bool wxCueSheet::HasTracks() const
{
	return !m_tracks.IsEmpty();
}

size_t wxCueSheet::GetTracksCount() const
{
	return m_tracks.GetCount();
}

const wxArrayTrack& wxCueSheet::GetTracks() const
{
	return m_tracks;
}

bool wxCueSheet::AddTrack( const wxTrack& track )
{
	if ( m_dataFiles.IsEmpty() && track.HasIndexes() )
	{
		return false;
	}

	m_tracks.Add( track );
	return true;
}

wxTrack& wxCueSheet::GetTrack( size_t idx )
{
	return m_tracks[ idx ];
}

bool wxCueSheet::HasTrack( size_t nTrackNo ) const
{
	for ( size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; i++ )
	{
		if ( m_tracks[ i ].GetNumber() == nTrackNo )
		{
			return true;
		}
	}

	return false;
}

wxTrack& wxCueSheet::GetTrackByNumber( size_t nTrackNo )
{
	wxASSERT( HasTrack( nTrackNo ) );

	for ( size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; i++ )
	{
		if ( m_tracks[ i ].GetNumber() == nTrackNo )
		{
			return m_tracks[ i ];
		}
	}

	wxASSERT( false );
	return m_tracks[ 0 ];
}

size_t wxCueSheet::GetTrackIdxFromNumber( size_t nTrackNo ) const
{
	wxASSERT( HasTrack( nTrackNo ) );

	for ( size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; i++ )
	{
		if ( m_tracks[ i ].GetNumber() == nTrackNo )
		{
			return i;
		}
	}

	wxASSERT( false );
	return wxIndex::UnknownDataFileIdx;
}

wxTrack& wxCueSheet::GetLastTrack()
{
	wxASSERT( !m_tracks.IsEmpty() );
	return m_tracks.Last();
}

wxTrack& wxCueSheet::GetBeforeLastTrack()
{
	size_t nCount = m_tracks.GetCount();

	wxASSERT( nCount > 1u );
	return m_tracks[ nCount - 2 ];
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

bool wxCueSheet::HasDataFiles() const
{
	return !m_dataFiles.IsEmpty();
}

size_t wxCueSheet::GetDataFilesCount() const
{
	return m_dataFiles.GetCount();
}

size_t wxCueSheet::GetLastDataFileIdx() const
{
	if ( m_dataFiles.IsEmpty() )
	{
		return wxIndex::UnknownDataFileIdx;
	}
	else
	{
		return m_dataFiles.GetCount() - 1;
	}
}

bool wxCueSheet::GetRelatedTracks( size_t nDataFileIdx, size_t& nTrackFrom, size_t& nTrackTo ) const
{
	wxASSERT( nDataFileIdx != wxIndex::UnknownDataFileIdx );
	bool bTrackFrom = false, bTrackTo = false;
	for ( size_t i = 0, nCount = m_tracks.GetCount(); ( i < nCount ) && !( bTrackFrom && bTrackTo ); i++ )
	{
		if ( m_tracks[ i ].IsRelatedToDataFileIdx( nDataFileIdx, false ) )
		{
			if ( !bTrackFrom )
			{
				nTrackFrom = i;
				bTrackFrom = true;
			}
		}
		else if ( bTrackFrom )
		{
			wxASSERT( i > 0u );
			nTrackTo = i - 1;
			bTrackTo = true;
		}
	}

	if ( bTrackFrom && !bTrackTo )
	{
		wxASSERT( !m_tracks.IsEmpty() );
		nTrackTo = m_tracks.GetCount() - 1;
		bTrackTo = true;
	}

	return ( bTrackFrom && bTrackTo );
}

const wxArrayDataFile& wxCueSheet::GetDataFiles() const
{
	return m_dataFiles;
}

wxCueSheet& wxCueSheet::AddDataFile( const wxDataFile& dataFile )
{
	m_dataFiles.Add( dataFile );
	return *this;
}

void wxCueSheet::Clear( void )
{
	m_content.Clear();
	m_logs.Clear();
	m_covers.Clear();
	wxCueComponent::Clear();
	m_catalogs.Clear();
	m_cdtextfiles.Clear();
	m_tracks.Clear();
	m_dataFiles.Clear();
}

void wxCueSheet::copy( const wxCueSheet& cs )
{
	wxCueComponent::copy( cs );
	m_content	  = cs.m_content;
	m_logs		  = cs.m_logs;
	m_covers	  = cs.m_covers;
	m_catalogs	  = cs.m_catalogs;
	m_cdtextfiles = cs.m_cdtextfiles;
	m_tracks	  = cs.m_tracks;
	m_dataFiles	  = cs.m_dataFiles;
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

	for ( size_t i = 0, nCount = m_catalogs.GetCount(); i < nCount; i++ )
	{
		AddTagToAllTracks( m_catalogs[ i ] );
	}

	m_catalogs.Clear();
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

wxCueSheet& wxCueSheet::Append( const wxCueSheet& _cs )
{
	wxCueSheet cs( _cs );

	cs.PrepareToAppend();

	wxCueComponent::Append( cs );

	WX_APPEND_ARRAY( m_content, cs.m_content );
	AppendFileNames( m_logs, cs.m_logs );
    wxCoverFile::Append( m_covers, cs.m_covers );
	WX_APPEND_ARRAY( m_catalogs, cs.m_catalogs );
	WX_APPEND_ARRAY( m_cdtextfiles, cs.m_cdtextfiles );

	size_t nDataFilesCount = m_dataFiles.GetCount();
	WX_APPEND_ARRAY( m_dataFiles, cs.m_dataFiles );

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
		tracks[ i ].ShiftDataFileIdx( nDataFilesCount );
	}

	WX_APPEND_ARRAY( m_tracks, tracks );
	return *this;
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

size_t wxCueSheet::GetDataFileIdxIfLastForTrack( size_t nTrackNo ) const
{
	size_t nTracksCount = m_tracks.GetCount();

	wxASSERT( nTrackNo < nTracksCount );

	if ( ( nTrackNo + 1u ) == nTracksCount )
	{
		return GetTrack( nTrackNo ).GetMinDataFileIdx( false );
	}
	else
	{
		size_t nCurDataFileIdx	= GetTrack( nTrackNo ).GetMaxDataFileIdx( true );
		size_t nNextDataFileIdx = GetTrack( nTrackNo + 1u ).GetMinDataFileIdx( false );

		if ( nNextDataFileIdx > nCurDataFileIdx )
		{
			return GetTrack( nTrackNo ).GetMinDataFileIdx( false );
		}
		else
		{
			return wxIndex::UnknownDataFileIdx;
		}
	}
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
		sFind.Prepend( '%' ).Append( '%' );
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
		sFind.Prepend( '%' ).Append( '%' );
		s.Replace( sFind, i->second, true );
	}

	return s;
}

bool wxCueSheet::HasSingleDataFile( wxDataFile& dataFile ) const
{
	size_t nDataFiles = m_dataFiles.GetCount();

	if ( nDataFiles == 1u )
	{
		dataFile = m_dataFiles[ 0 ];
		return true;
	}
	else
	{
		return false;
	}
}

bool wxCueSheet::HasSingleDataFile() const
{
	return ( m_dataFiles.GetCount() == 1u );
}

wxCueSheet& wxCueSheet::SetSingleDataFile( const wxDataFile& dataFile )
{
	m_dataFiles.Clear();
	m_dataFiles.Add( dataFile );
	for ( size_t i = 0, nCount = m_tracks.GetCount(); i < nCount; i++ )
	{
		m_tracks[ i ].SetDataFileIdx( 0u );
	}

	return *this;
}

wxCueSheet& wxCueSheet::SetDataFiles( const wxArrayDataFile& dataFiles )
{
	size_t nDataFilesCount	  = m_dataFiles.GetCount();
	size_t nNewDataFilesCount = dataFiles.GetCount();

	if ( nDataFilesCount > nNewDataFilesCount )
	{
		wxLogWarning( _( "Not all data files in cue sheet are replaced" ) );
		wxLogWarning( _( "%d data file(s) left" ), ( nDataFilesCount - nNewDataFilesCount ) );
		for ( size_t i = 0; i < nNewDataFilesCount; i++ )
		{
			m_dataFiles[ i ] = dataFiles[ i ];
		}
	}
	else if ( nDataFilesCount == nNewDataFilesCount )
	{
		m_dataFiles.Clear();
		WX_APPEND_ARRAY( m_dataFiles, dataFiles );
	}
	else
	{
		wxLogWarning( _( "Too many data files" ) );
		wxLogWarning( _( "%d data file(s) too much" ), ( nNewDataFilesCount - nDataFilesCount ) );
		for ( size_t i = 0; i < nDataFilesCount; i++ )
		{
			m_dataFiles[ i ] = dataFiles[ i ];
		}
	}

	return *this;
}

bool wxCueSheet::HasDuration() const
{
	bool bRes = true;

	for ( size_t i = 0, nCount = m_dataFiles.GetCount(); ( i < nCount ) && bRes; i++ )
	{
		bRes = bRes && m_dataFiles[ i ].HasDuration();
	}

	return bRes;
}

wxDuration wxCueSheet::GetDuration( size_t nDataFileIdx ) const
{
	wxDuration duration;

	if ( nDataFileIdx == wxIndex::UnknownDataFileIdx || nDataFileIdx == 0u )
	{
		return duration;
	}

	wxASSERT( HasDuration() );
	wxASSERT( nDataFileIdx <= m_dataFiles.GetCount() );

	bool   bFirst = true;
	bool   bStop  = false;
	size_t nFirstTrack, nLastTrack;

	for ( size_t i = 0; i < nDataFileIdx; i++ )
	{
		if ( !GetRelatedTracks( i, nFirstTrack, nLastTrack ) )
		{
            wxLogDebug( "wxCueSheet::GetDuration - skipping unused data file %" wxSizeTFmtSpec "d", i );
			continue;
		}

		const wxDuration& dfDuration = m_dataFiles[ i ].GetDuration();

		if ( bFirst )
		{
			duration = dfDuration;
			bFirst	 = false;
		}
		else if ( !duration.Add( dfDuration ) )
		{
			bStop = true;
		}
	}

	if ( bStop )
	{
		wxLogDebug( "Fail to calculate duration of cue sheet" );
		duration.Invalidate();
	}

	return duration;
}

wxDuration wxCueSheet::GetDuration() const
{
	return GetDuration( m_dataFiles.GetCount() );
}

bool wxCueSheet::CalculateDuration( const wxString& sAlternateExt )
{
	bool bRes = true;

	for ( size_t i = 0, nCount = m_dataFiles.GetCount(); i < nCount; i++ )
	{
		if ( !m_dataFiles[ i ].HasDuration() )
		{
			if ( !m_dataFiles[ i ].GetInfo( sAlternateExt ) )
			{
                wxLogDebug( "Fail to calculate duration for track %" wxSizeTFmtSpec "d", i );
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
		return s1 + ' ' + s2;
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
		wxLogDebug( "wxCueSheet::FindCommonTags - too few tracks" );
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
                        wxString::Format( "%" wxSizeTFmtSpec "u", m_content.GetCount( ) ) ) );

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
					wxCueTag discNumberTag( wxCueTag::TAG_AUTO_GENERATED, wxCueTag::Name::DISCNUMBER, wxString::Format( "%u", i->second ) );
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

