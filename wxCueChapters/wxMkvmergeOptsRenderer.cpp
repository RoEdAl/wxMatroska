/*
	wxMkvmergeOptsRenderer.cpp
*/

#include "StdWx.h"
#include <wxCueFile/wxCueComponent.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include "wxConfiguration.h"
#include "wxInputFile.h"
#include "wxMkvmergeOptsRenderer.h"
#include "wxApp.h"

// ===============================================================================

wxMkvmergeOptsRenderer* const wxMkvmergeOptsRenderer::Null = (wxMkvmergeOptsRenderer* const)NULL;

// ===============================================================================

wxMkvmergeOptsRenderer::wxMkvmergeOptsRenderer( const wxConfiguration& cfg )
	:m_cfg( cfg )
{
}

const wxString& wxMkvmergeOptsRenderer::GetMkvmergeOptsFile() const
{
	return m_sMatroskaOptsFile;
}

wxString wxMkvmergeOptsRenderer::mkvmerge_escape( const wxString& s )
{
	wxString sRes( s );
	sRes.Replace( wxT('\\'), wxT("\\\\") );
	sRes.Replace( wxT(' '), wxT("\\s") );
	sRes.Replace( wxT('\"'), wxT("\\2") );
	sRes.Replace( wxT(':'), wxT("\\c") );
	sRes.Replace( wxT('#'), wxT("\\h") );
	return sRes;
}

void wxMkvmergeOptsRenderer::write_as( wxTextOutputStream& stream, const wxArrayString& as )
{
	for( wxArrayString::const_iterator i = as.begin(); i != as.end(); i++ )
	{
		stream << *i << endl;
	}
}

void wxMkvmergeOptsRenderer::RenderDisc( const wxInputFile& inputFile, const wxCueSheet& cueSheet )
{
	wxString sOutputFile, sTagsFile, sMatroskaFile;

	m_cfg.GetOutputFile( inputFile, sOutputFile, sTagsFile );
	m_cfg.GetOutputMatroskaFile( inputFile, sMatroskaFile, m_sMatroskaOptsFile );

	// pre
	wxDateTime dtNow( wxDateTime::Now() );

	m_asMmcPre.Empty();
	m_asMmcPre.Add( wxString::Format( wxT("# This file was created by %s tool"), wxGetApp().GetAppDisplayName() ) );
	m_asMmcPre.Add( wxString::Format( wxT("# Application version: %s"), wxGetApp().APP_VERSION ) );
	m_asMmcPre.Add( wxString::Format( wxT("# Application vendor: %s"), wxGetApp().GetVendorDisplayName() ) );
	m_asMmcPre.Add( wxString::Format( wxT("# Creation time: %s %s"), dtNow.FormatISODate(), dtNow.FormatISOTime() ) );
	m_asMmcPre.Add( wxT("# Output file") );
	m_asMmcPre.Add( wxT("-o") );
	if ( m_cfg.UseFullPaths() )
	{
		m_asMmcPre.Add( mkvmerge_escape( sMatroskaFile ) );
	}
	else
	{
		m_asMmcPre.Add( mkvmerge_escape( wxMyApp::GetFileName( sMatroskaFile ) ) );
	}
	m_asMmcPre.Add( wxT("--language") );
	m_asMmcPre.Add( wxString::Format( wxT("0:%s"), m_cfg.GetLang() ) );
	m_asMmcPre.Add( wxT("--default-track") );
	m_asMmcPre.Add( wxT("0:yes") );
	m_asMmcPre.Add( wxT("--track-name") );
	m_asMmcPre.Add( wxString::Format( wxT("0:%s"), cueSheet.Format( m_cfg.GetMatroskaNameFormat() ) ) );
	m_asMmcPre.Add( wxT("# Input file(s)") );

	// tracks
	const wxArrayTrack& tracks = cueSheet.GetTracks();
	size_t nTracks = tracks.Count();
	for( size_t i=0; i<nTracks; i++ )
	{
		if ( tracks[i].HasDataFile() )
		{
			bool bNext = !m_asMmcInputFiles.IsEmpty();

			m_asMmcInputFiles.Add( wxT("-a") );
			m_asMmcInputFiles.Add( wxT("0") );
			m_asMmcInputFiles.Add( wxT("-D") );
			m_asMmcInputFiles.Add( wxT("-S") );
			m_asMmcInputFiles.Add( wxT("-T") );
			m_asMmcInputFiles.Add( wxT("--no-global-tags") );
			m_asMmcInputFiles.Add( wxT("--no-chapters" ) );
			if ( bNext )
			{
				wxString sLine( tracks[i].GetDataFile().GetFullPath() );
				sLine.Prepend( wxT('+') );
				m_asMmcInputFiles.Add( mkvmerge_escape( sLine ) );
			}
			else
			{
				m_asMmcInputFiles.Add( mkvmerge_escape( tracks[i].GetDataFile().GetFullPath() ) );
			}
		}
	}

	// poost
	m_asMmcPost.Empty();
	m_asMmcPost.Add( wxT("# General options") );
	m_asMmcPost.Add( wxT("--default-language") );
	m_asMmcPost.Add( m_cfg.GetLang() );
	m_asMmcPost.Add( wxT("--title") );
	m_asMmcPost.Add( cueSheet.Format( m_cfg.GetMatroskaNameFormat() ) );
	m_asMmcPost.Add( wxT("--chapters") );
	if ( m_cfg.UseFullPaths() )
	{
		m_asMmcPost.Add( mkvmerge_escape( sOutputFile ) );
	}
	else
	{
		m_asMmcPost.Add( mkvmerge_escape( wxMyApp::GetFileName(sOutputFile) ) );
	}

	if ( m_cfg.GenerateTags() )
	{
		m_asMmcPost.Add( wxT("--global-tags") );
		if ( m_cfg.UseFullPaths() )
		{
			m_asMmcPost.Add( mkvmerge_escape( sTagsFile ) );
		}
		else
		{
			m_asMmcPost.Add( mkvmerge_escape( wxMyApp::GetFileName( sTagsFile ) ) );
		}
	}
}

bool wxMkvmergeOptsRenderer::Save()
{
	wxASSERT( m_cfg.GenerateMkvmergeOpts() );
	wxFileOutputStream os( m_sMatroskaOptsFile );
	if ( os.IsOk() )
	{
		wxSharedPtr<wxTextOutputStream> pStream( wxTextOutputStreamWithBOMFactory::CreateUTF8( os, wxEOL_NATIVE, true, m_cfg.UseMLang() ) );
		write_as( *pStream, m_asMmcPre );
		write_as( *pStream, m_asMmcInputFiles );
		write_as( *pStream, m_asMmcPost );
		pStream->Flush();
		return true;
	}
	else
	{
		wxLogError( _("Fail to save options to \u201C%s\u201D"), m_sMatroskaOptsFile );
		return false;
	}
}
