/*
	wxTextCueSheetRenderer.cpp
*/

#include "StdWx.h"
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>

wxIMPLEMENT_DYNAMIC_CLASS( wxTextCueSheetRenderer, wxCueSheetRenderer )

wxTextCueSheetRenderer::wxTextCueSheetRenderer(wxTextOutputStream* pTextOutputStream, int nDumpFlags)
	:m_pTextOutputStream(pTextOutputStream),
	 m_nDumpFlags(nDumpFlags),
	 m_reSpace( wxT("[[:space:]]+"), wxRE_ADVANCED|wxRE_NOSUB )
{
	wxASSERT( m_reSpace.IsValid() );
}

void wxTextCueSheetRenderer::Assign( wxTextOutputStream* pTextOutputStream, int nDumpFlags )
{
	wxASSERT( pTextOutputStream != (wxTextOutputStream*)NULL );
	m_pTextOutputStream = pTextOutputStream;
	m_nDumpFlags = nDumpFlags;
}

bool wxTextCueSheetRenderer::OnPreRenderDisc( const wxCueSheet& cueSheet )
{
	if ( m_pTextOutputStream == (wxTextOutputStream*)NULL )
	{
		wxLogError( _("wxTextCueSheetRenderer: output stream not specified") );
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
	InternalRenderIndex( cueSheet, track, preGap, wxT("PREGAP") );
	return wxCueSheetRenderer::OnRenderPreGap( cueSheet, track, preGap );
}

bool wxTextCueSheetRenderer::OnRenderPostGap( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& postGap )
{
	InternalRenderIndex( cueSheet, track, postGap, wxT("POSTGAP") );
	return wxCueSheetRenderer::OnRenderPostGap( cueSheet, track, postGap );
}

void wxTextCueSheetRenderer::DumpComponentTag(
	const wxCueComponent& component, const wxCueTag& tag )
{
	wxString sName;
	if ( m_reSpace.Matches( tag.GetName() ) )
	{
		sName = wxString::Format( wxT("\"%s\""), tag.GetName() );
	}
	else
	{
		sName = tag.GetName();
	}

	wxString sValue;
	if ( tag.IsMultiline() )
	{
		sValue = tag.GetFlattenValue();
	}
	else
	{
		sValue = tag.GetValue();
	}

	DumpComponentString( component, wxT("REM"), wxString::Format( wxT("%s %s"), sName, sValue ) );
}

void wxTextCueSheetRenderer::DumpComponentString(
	const wxCueComponent& component, const wxChar* szEntry, const wxString& text )
{
	if ( !text.IsEmpty() )
	{
		wxString sLine;
		sLine.Printf( wxT("%s %s"), szEntry, text );
		if ( component.IsTrack() )
		{
			*m_pTextOutputStream << wxT("\t");
		}
		*m_pTextOutputStream << sLine << endl;
	}
}


void wxTextCueSheetRenderer::InternalRenderComponent(const wxCueComponent& component )
{
	// dump comments
	if ( (m_nDumpFlags & DUMP_COMMENTS) != 0 )
	{
		const wxArrayString& comments = component.GetComments();
		for( wxArrayString::const_iterator i = comments.begin(); i != comments.end(); i++ )
		{
			DumpComponentString( component, wxT("REM"), *i );
		}
	}

	// dump tags
	if ( (m_nDumpFlags & DUMP_TAGS) != 0 )
	{
		const wxArrayCueTag& tags = component.GetTags();
		size_t numTags = tags.Count();
		for( size_t i=0; i<numTags; i++ )
		{
			DumpComponentTag( component, tags[i] );
		}
	}

	// dump CT-TEXT info
	const wxArrayCueTag& tags = component.GetCdTextTags();
	size_t numTags = tags.Count();
	for( size_t i=0; i<numTags; i++ )
	{
		DumpComponentString( component, tags[i].GetName(), wxCueComponent::FormatCdTextData( tags[i].GetName(), tags[i].GetValue() ) );
	}

	// dump garbage
	if ( (m_nDumpFlags & DUMP_GARBAGE) != 0 )
	{
		const wxArrayString garbage = component.GetGarbage();
		for( wxArrayString::const_iterator i = garbage.begin(); i != garbage.end(); i++ )
		{
			m_pTextOutputStream->WriteString( *i );
		}
	}
}

void wxTextCueSheetRenderer::InternalRenderCueSheet(const wxCueSheet& cueSheet )
{
	InternalRenderComponent( cueSheet );
	DumpComponentString( cueSheet, wxT("CATALOG"), cueSheet.GetCatalog() );

	if ( !cueSheet.GetCdTextFile().IsEmpty() )
	{
		wxString sQuoted( cueSheet.GetCdTextFile() );
		sQuoted.Prepend( wxT('"') ).Append( wxT('"') );
		DumpComponentString( cueSheet, wxT("CDTEXTFILE"), sQuoted );
	}
}

void wxTextCueSheetRenderer::InternalRenderTrack(const wxCueSheet& WXUNUSED(cueSheet), const wxTrack& track)
{
	wxString sLine;

	if ( (m_nDumpFlags & DUMP_EMPTY_LINES) != 0 )
	{
		*m_pTextOutputStream << endl;
	}

	if ( track.HasDataFile() )
	{
		InternalRenderDataFile( track.GetDataFile() );
	}

	sLine.Printf( wxT("%02d %s"), track.GetNumber(), track.GetModeAsString() );
	DumpComponentString( track, wxT("TRACK"), sLine );
	InternalRenderComponent( track );

	// flags
	if ( track.HasFlags() )
	{
		DumpComponentString( track, wxT("FLAGS"), track.GetFlagsAsString() );
	}
}

void wxTextCueSheetRenderer::InternalRenderIndex( const wxCueSheet& cueSheet, const wxTrack& WXUNUSED(track), const wxIndex& idx, wxString desc )
{
	wxString sLine;
	wxString sIdx( m_si.GetIndexOffsetFramesStr( idx ) );
	sLine.Printf( wxT("\t\t%s %s\n"), desc, sIdx );
	m_pTextOutputStream->WriteString( sLine );
}

void wxTextCueSheetRenderer::InternalRenderIndex( const wxCueSheet& cueSheet, const wxTrack& WXUNUSED(track), const wxIndex& idx )
{
	wxString sLine;
	wxString sIdx( m_si.GetIndexOffsetFramesStr( idx ) );
	sLine.Printf( wxT("\t\tINDEX %02d %s\n"), idx.GetNumber(), sIdx );
	m_pTextOutputStream->WriteString( sLine );
}

void wxTextCueSheetRenderer::InternalRenderDataFile( const wxDataFile& dataFile )
{
	wxString s;
	s.Printf( wxT("FILE \"%s\" %s\n"), dataFile.GetFileName(), dataFile.GetFileTypeAsString() );
	m_pTextOutputStream->WriteString( s );
}

wxString wxTextCueSheetRenderer::ToString( const wxCueSheet& cueSheet, int nDumpFlags)
{
	wxStringOutputStream sout;
	wxTextOutputStream ts( sout );
	wxTextCueSheetRenderer renderer( &ts, nDumpFlags );
	if ( renderer.Render( cueSheet ) )
	{
		return sout.GetString();
	}
	else
	{
		return wxEmptyString;
	}
}
