/*
 * wxCuePointsRenderer.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxDataFile.h>
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include <wxCueFile/wxTextOutputStreamOnString.h>
#include <wxCueFile/wxTextInputStreamOnString.h>
#include <wxCueFile/wxTextCueSheetRenderer.h>
#include "wxConfiguration.h"
#include "wxInputFile.h"
#include "wxPrimitiveRenderer.h"
#include "wxCuePointsRenderer.h"
#include "wxApp.h"

// ===============================================================================

wxCuePointsRenderer::wxCuePointsRenderer( const wxConfiguration& cfg ):
	wxPrimitiveRenderer( cfg )
{}

static wxString get_track_title( const wxTrack& track )
{
	wxArrayCueTag tags;
	size_t nTags = wxCueTag::GetTags( track.GetCdTextTags(), wxCueTag::Name::TITLE, tags );
	if ( nTags > 0u )
	{
		wxString s;
		for( size_t i = 0, nCount = tags.GetCount(); i < nCount; i++ )
		{
			s += tags[i].GetFlattenValue();
			s += wxS(';');
		}

		return s.RemoveLast();
	}
	else
	{
		return wxEmptyString;
	}
}

void wxCuePointsRenderer::RenderDisc( const wxCueSheet& cueSheet )
{
	wxSamplingInfo	si;
	wxDateTime dtNow( wxDateTime::Now() );

	*m_os <<
	wxS( "# This file was created by " ) << wxGetApp().GetAppDisplayName() << wxS( " tool" ) << endl <<
	wxS( "# Application version: " ) << wxGetApp().APP_VERSION << endl <<
	wxS( "# Application vendor: " ) << wxGetApp().GetVendorDisplayName() << endl <<
	wxS( "# Creation time: " ) << dtNow.FormatISODate() << wxS( ' ' ) << dtNow.FormatISOTime() << endl;

	if ( cueSheet.GetContentsCount() > 0u )
	{
		const wxCueSheetContent& cnt = cueSheet.GetContents()[0];
		if ( cnt.HasSource() )
		{
			const wxDataFile& df = cnt.GetSource();
			*m_os << wxS( "# Source file \u201C" ) << df.GetFileName().GetFullName() << wxS("\u201D") << endl;
		}
	}

	const wxArrayTrack& tracks = cueSheet.GetTracks();
	for( size_t i=0, nCount = tracks.GetCount(); i < nCount; i++ )
	{
		const wxTrack& track = tracks[i];
		wxString sTitle( get_track_title( track ) );
		wxIndex idx;
		wxString sIdx;

		if ( track.GetNumber() == 1u && !m_cfg.TrackOneIndexOne() && track.HasZeroIndex() )
		{
			idx = track.GetPreGap();
		}
		else
		{
			idx = track.GetFirstIndex();
		}

		wxASSERT( idx.HasDataFileIdx() );
		wxDuration duration( cueSheet.GetDuration( idx.GetDataFileIdx() ) );
		if ( !duration.IsValid() )
		{
			wxLogWarning( _("Cannot render cue point for track %u"), i );
			*m_os << _("# Cannot render cue point for track ") << i << endl;
			continue;
		}

		wxULongLong indexOffset( duration.GetSamplingInfo().GetIndexOffset( idx ) );

		if ( track.GetNumber() == 1u && idx.GetDataFileIdx() == 0u && indexOffset == wxULL(0) )
		{
			continue;
		}

		duration.Add( indexOffset );
		sIdx = duration.GetCdFramesStr();

		if ( sTitle.IsEmpty() )
		{
			*m_os << sIdx << endl;
		}
		else
		{
			*m_os << sIdx << wxS("\t# ") << sTitle << endl;
		}
	}
}

bool wxCuePointsRenderer::Save( const wxFileName& outputFile )
{
	wxFileOutputStream os( outputFile.GetFullPath() );

	if ( os.IsOk() )
	{
		wxLogInfo( _( "Creating cue points file \u201C%s\u201D" ), outputFile.GetFullName() );
		wxSharedPtr< wxTextOutputStream > pStream( m_cfg.GetOutputTextStream( os ) );
		save_string_to_stream( *pStream, m_os.GetString() );
		return true;
	}
	else
	{
		wxLogError( _( "Fail to save cue points to \u201C%s\u201D" ), outputFile.GetFullName() );
		return false;
	}
}
