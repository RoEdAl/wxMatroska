/*
	wxTextCueSheetRenderer.cpp
*/

#include "StdWx.h"
#include "wxIndex.h"
#include "wxTrack.h"
#include "wxTextCueSheetRenderer.h"

IMPLEMENT_CLASS( wxTextCueSheetRenderer, wxCueSheetRenderer )

wxTextCueSheetRenderer::wxTextCueSheetRenderer(wxTextOutputStream* pTextOutputStream, int nDumpFlags)
	:m_pTextOutputStream(pTextOutputStream),m_nDumpFlags(nDumpFlags)
{
}

wxTextCueSheetRenderer::~wxTextCueSheetRenderer(void)
{
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

bool wxTextCueSheetRenderer::OnRenderTrack( const wxTrack& track )
{
	InternalRenderTrack( track );
	return wxCueSheetRenderer::OnRenderTrack( track );
}

bool wxTextCueSheetRenderer::OnRenderIndex( const wxTrack& track, const wxIndex& index )
{
	InternalRenderIndex( index );
	return wxCueSheetRenderer::OnRenderIndex( track, index );
}

bool wxTextCueSheetRenderer::OnRenderPreGap( const wxTrack& track, const wxIndex& preGap )
{
	InternalRenderIndex( preGap, wxT("PREGAP") );
	return wxCueSheetRenderer::OnRenderPreGap( track, preGap );
}

bool wxTextCueSheetRenderer::OnRenderPostGap( const wxTrack& track, const wxIndex& postGap )
{
	InternalRenderIndex( postGap, wxT("POSTGAP") );
	return wxCueSheetRenderer::OnRenderPostGap( track, postGap );
}

void wxTextCueSheetRenderer::DumpComponentString(
	const wxCueComponent& component, const wxChar* szEntry, const wxString& text )
{
	if ( !text.IsEmpty() )
	{
		wxString sLine;
		sLine.Printf( wxT("%s %s\n"), szEntry, text.GetData() );
		if ( component.IsTrack() )
		{
			sLine = sLine.Prepend( wxT("\t") );
		}
		m_pTextOutputStream->WriteString( sLine );
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

	// dump CT-TEXT info
	wxCueComponent::wxHashString cdTextInfo = component.GetCdTextInfo();
	for( wxCueComponent::wxHashString::const_iterator i=cdTextInfo.begin(); i != cdTextInfo.end(); i++ )
	{
		DumpComponentString( component, i->first, wxCueComponent::FormatCdTextData( i->first, i->second ) );
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

void wxTextCueSheetRenderer::InternalRenderTrack(const wxTrack& track)
{
	wxString sLine;

	if ( (m_nDumpFlags & DUMP_EMPTY_LINES) != 0 )
	{
		m_pTextOutputStream->WriteString( wxT("\n") );
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

void wxTextCueSheetRenderer::InternalRenderIndex( const wxIndex& idx, wxString desc )
{
	wxString sLine;
	sLine.Printf( wxT("\t\t%s %02d:%02d:%02d\n"), desc, idx.GetMinutes(), idx.GetSeconds(), idx.GetFrames() );
	m_pTextOutputStream->WriteString( sLine );
}

void wxTextCueSheetRenderer::InternalRenderIndex( const wxIndex& idx )
{
	wxString sLine;
	sLine.Printf( wxT("\t\tINDEX %02d %02d:%02d:%02d\n"), idx.GetNumber(), idx.GetMinutes(), idx.GetSeconds(), idx.GetFrames() );
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
