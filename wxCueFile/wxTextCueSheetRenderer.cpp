/*
 * wxTextCueSheetRenderer.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>
#include <wxEncodingDetection/wxTextOutputStreamOnString.h>

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxTextCueSheetRenderer, wxCueSheetRenderer );

// ===============================================================================

wxTextCueSheetRenderer::wxTextCueSheetRenderer( wxTextOutputStream* pTextOutputStream, int nDumpFlags ):
	m_pTextOutputStream( pTextOutputStream ),
	m_nDumpFlags( nDumpFlags ),
	m_nDataFileIdx( wxIndex::UnknownDataFileIdx ),
	m_reSpace( "[[:space:]]+", wxRE_ADVANCED | wxRE_NOSUB )
{
	wxASSERT( m_reSpace.IsValid() );
}

void wxTextCueSheetRenderer::Assign( wxTextOutputStream* pTextOutputStream, int nDumpFlags )
{
	wxASSERT( pTextOutputStream != (wxTextOutputStream*)NULL );

	m_pTextOutputStream = pTextOutputStream;
	m_nDumpFlags		= nDumpFlags;
}

bool wxTextCueSheetRenderer::OnPreRenderDisc( const wxCueSheet& cueSheet )
{
	if ( m_pTextOutputStream == (wxTextOutputStream*)NULL )
	{
		wxLogError( _( "wxTextCueSheetRenderer: output stream not specified" ) );
		return false;
	}

	return true;
}

bool wxTextCueSheetRenderer::OnRenderDisc( const wxCueSheet& cueSheet )
{
	InternalRenderCueSheet( cueSheet );
	return wxCueSheetRenderer::OnRenderDisc( cueSheet );
}

bool wxTextCueSheetRenderer::OnRenderTrack( const wxCueSheet& cueSheet, const wxTrack& track )
{
	InternalRenderTrack( cueSheet, track );
	return wxCueSheetRenderer::OnRenderTrack( cueSheet, track );
}

bool wxTextCueSheetRenderer::OnRenderIndex( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& index )
{
	InternalRenderIndex( cueSheet, track, index );
	return wxCueSheetRenderer::OnRenderIndex( cueSheet, track, index );
}

bool wxTextCueSheetRenderer::OnRenderPreGap( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& preGap )
{
	InternalRenderIndex( cueSheet, track, preGap, "PREGAP" );
	return wxCueSheetRenderer::OnRenderPreGap( cueSheet, track, preGap );
}

bool wxTextCueSheetRenderer::OnRenderPostGap( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& postGap )
{
	InternalRenderIndex( cueSheet, track, postGap, "POSTGAP" );
	return wxCueSheetRenderer::OnRenderPostGap( cueSheet, track, postGap );
}

void wxTextCueSheetRenderer::DumpComponentTag( const wxCueComponent& component, const wxCueTag& tag )
{
	wxString sName;

	if ( m_reSpace.Matches( tag.GetName() ) )
	{
		sName = wxCueTag::Quote( tag.GetName() );
	}
	else
	{
		sName = tag.GetName();
	}

	DumpComponentString( component, "REM", wxString::Format( "%s %s", sName, tag.GetFlattenValue() ) );
}

void wxTextCueSheetRenderer::DumpComponentString(
		const wxCueComponent& component, const wxString& sEntry,
		const wxString& text )
{
	if ( !text.IsEmpty() )
	{
		if ( component.IsTrack() )
		{
			*m_pTextOutputStream << '\t';
		}

		*m_pTextOutputStream << ' ' << sEntry << ' ' << text << endl;
	}
}

void wxTextCueSheetRenderer::InternalRenderComponent( const wxCueComponent& component )
{
	// dump comments
	if ( ( m_nDumpFlags & DUMP_COMMENTS ) != 0 )
	{
		const wxArrayString& comments = component.GetComments();
		for ( wxArrayString::const_iterator i = comments.begin(); i != comments.end(); i++ )
		{
			DumpComponentString( component, "REM", *i );
		}
	}

	// dump tags
	if ( ( m_nDumpFlags & DUMP_TAGS ) != 0 )
	{
		const wxArrayCueTag& tags = component.GetTags();
		for ( size_t i = 0, nCount = tags.GetCount(); i < nCount; i++ )
		{
			DumpComponentTag( component, tags[ i ] );
		}
	}

	// dump CT-TEXT info
	const wxArrayCueTag& tags = component.GetCdTextTags();
	for ( size_t i = 0, nCount = tags.GetCount(); i < nCount; i++ )
	{
		DumpComponentString( component,
				tags[ i ].GetName(),
				wxCueComponent::FormatCdTextData( tags[ i ].GetName(),
						tags[ i ].GetValue() ) );
	}

	// dump garbage
	if ( ( m_nDumpFlags & DUMP_GARBAGE ) != 0 )
	{
		const wxArrayString garbage = component.GetGarbage();
		for ( wxArrayString::const_iterator i = garbage.begin(); i != garbage.end(); i++ )
		{
			*m_pTextOutputStream << *i << endl;
		}
	}
}

void wxTextCueSheetRenderer::InternalRenderCueSheet( const wxCueSheet& cueSheet )
{
	m_nDataFileIdx = wxIndex::UnknownDataFileIdx;
	InternalRenderComponent( cueSheet );

	const wxArrayCueTag& catalogs = cueSheet.GetCatalogs();
	for ( size_t i = 0, nCount = catalogs.GetCount(); i < nCount; i++ )
	{
		DumpComponentString( cueSheet, catalogs[ i ].GetName(), catalogs[ i ].GetValue() );
	}

	const wxArrayFileName& cdtextfiles = cueSheet.GetCdTextFiles();
	for ( size_t i = 0, nCount = cdtextfiles.GetCount(); i < nCount; i++ )
	{
		DumpComponentString( cueSheet, wxCueTag::Name::CDTEXTFILE, wxCueTag::Quote( cdtextfiles[ i ].GetFullName() ) );
	}
}

void wxTextCueSheetRenderer::InternalRenderTrack( const wxCueSheet& cueSheet, const wxTrack& track )
{
	wxString sLine;

	if ( ( m_nDumpFlags & DUMP_EMPTY_LINES ) != 0 )
	{
		*m_pTextOutputStream << endl;
	}

	if ( track.GetNumber() == 1u )
	{
		size_t nDataFileIdx = track.GetMinDataFileIdx( true );
		InternalRenderDataFile( cueSheet, nDataFileIdx );
	}
	else if ( !track.HasZeroIndex() )
	{
		size_t nDataFileIdx = track.GetMinDataFileIdx( false );
		InternalRenderDataFile( cueSheet, nDataFileIdx );
	}

	sLine.Printf( "%02" wxSizeTFmtSpec "d %s", track.GetNumber(), track.GetModeAsString() );
	DumpComponentString( track, "TRACK", sLine );
	InternalRenderComponent( track );

	// flags
	if ( track.HasFlags() )
	{
		DumpComponentString( track, "FLAGS", track.GetFlagsAsString() );
	}
}

void wxTextCueSheetRenderer::InternalRenderIndex( const wxCueSheet& cueSheet, const wxTrack& WXUNUSED( track ), const wxIndex& idx, wxString desc )
{
	if ( idx.HasDataFileIdx() )
	{
		InternalRenderDataFile( cueSheet, idx.GetDataFileIdx() );
	}

	wxString sIdx( m_si.GetIndexOffsetFramesStr( idx ) );

	*m_pTextOutputStream << "\t\t " << desc << ' ' << sIdx << endl;
}

void wxTextCueSheetRenderer::InternalRenderIndex( const wxCueSheet& cueSheet, const wxTrack& WXUNUSED( track ), const wxIndex& idx )
{
	if ( idx.HasDataFileIdx() )
	{
		InternalRenderDataFile( cueSheet, idx.GetDataFileIdx() );
	}

	wxString sIdxNo;
	wxString sIdx( m_si.GetIndexOffsetFramesStr( idx ) );

	sIdxNo.Printf( "%02" wxSizeTFmtSpec "d", idx.GetNumber() );
	*m_pTextOutputStream << "\t\t INDEX " << sIdxNo << ' ' <<
		sIdx << endl;
}

void wxTextCueSheetRenderer::InternalRenderDataFile( const wxCueSheet& cueSheet, size_t nDataFileIdx )
{
	if ( ( m_nDataFileIdx == wxIndex::UnknownDataFileIdx && nDataFileIdx != wxIndex::UnknownDataFileIdx ) ||
		 ( nDataFileIdx > m_nDataFileIdx ) )
	{
		const wxDataFile& dataFile = cueSheet.GetDataFiles().Item( nDataFileIdx );
		*m_pTextOutputStream <<
			"FILE \"" << dataFile.GetFileName().GetFullName() <<
			"\" " << dataFile.GetFileTypeAsString() << endl;
		m_nDataFileIdx = nDataFileIdx;
	}
}

wxString wxTextCueSheetRenderer::ToString( const wxCueSheet& cueSheet, int nDumpFlags )
{
	wxTextOutputStreamOnString tos;
	wxTextCueSheetRenderer	   renderer( &( tos.GetStream() ), nDumpFlags );

	if ( renderer.Render( cueSheet ) )
	{
		return tos.GetString();
	}
	else
	{
		return wxEmptyString;
	}
}

bool wxTextCueSheetRenderer::ToString( wxTextOutputStream& tos, const wxCueSheet& cueSheet, int nDumpFlags )
{
	wxTextCueSheetRenderer renderer( &tos, nDumpFlags );

	return renderer.Render( cueSheet );
}

