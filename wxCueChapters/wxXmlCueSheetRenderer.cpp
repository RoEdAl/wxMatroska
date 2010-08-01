/*
	wxXmlCueSheetRenderer.cpp
*/

#include "StdWx.h"
#include "wxConfiguration.h"
#include "wxIndex.h"
#include "wxTrack.h"
#include "wxInputFile.h"
#include "wxXmlCueSheetRenderer.h"

IMPLEMENT_CLASS( wxXmlCueSheetRenderer, wxCueSheetRenderer )

// ===============================================================================

static wxXmlNode* add_chapter_time_start( wxXmlNode* pChapterAtom, const wxIndex& idx )
{
	wxXmlNode* pChapterTimeStart = new wxXmlNode( pChapterAtom, wxXML_ELEMENT_NODE, wxT("ChapterTimeStart") );
	wxXmlNode* pChapterTimeStartText = new wxXmlNode( pChapterTimeStart, wxXML_TEXT_NODE, wxEmptyString, idx.GetTimeStr() );

	return pChapterTimeStart;
}

static wxXmlNode* find_chapter_time_start( wxXmlNode* pChapterAtom )
{
	wxXmlNode* pChild = pChapterAtom->GetChildren();
	wxXmlNode* pPrev = (wxXmlNode*)NULL;
	while( pChild != (wxXmlNode*)NULL )
	{
		if ( pChild->GetName().CmpNoCase( wxT("ChapterTimeStart") ) == 0 )
		{
			return pChild;
		}
		pPrev = pChild;
		pChild = pChild->GetNext();
	}

	wxASSERT( pPrev != (wxXmlNode*)NULL );
	return pPrev;
}

static wxXmlNode* add_chapter_time_end( wxXmlNode* pChapterAtom, const wxString& text )
{
	wxXmlNode* pChapterTimeStart = find_chapter_time_start( pChapterAtom );

	wxXmlNode* pChapterTimeEnd = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("ChapterTimeEnd") );
	wxXmlNode* pChapterTimeEndText = new wxXmlNode( pChapterTimeEnd, wxXML_TEXT_NODE, wxEmptyString, text );

	pChapterAtom->InsertChildAfter( pChapterTimeEnd, pChapterTimeStart );

	return pChapterTimeEnd;
}

static wxXmlNode* add_chapter_time_end( wxXmlNode* pChapterAtom, const wxIndex& idx )
{
	return add_chapter_time_end( pChapterAtom, idx.GetTimeStr() );
}

static wxXmlNode* add_chapter_time_end( wxXmlNode* pChapterAtom, wxULongLong samples )
{
	return add_chapter_time_end( pChapterAtom, wxDataFile::GetSamplesStr( samples ) );
}

static wxXmlNode* add_chapter_time_end( wxXmlNode* pChapterAtom, const wxTrack& track, unsigned long nOffset )
{
	wxIndex fidx( track.GetFirstIndex() );
	fidx -= nOffset;
	return add_chapter_time_end( pChapterAtom, fidx.GetTimeStr() );
}

static bool has_chapter_time_end( wxXmlNode* pChapterAtom )
{
	wxXmlNode* pChild = pChapterAtom->GetChildren();
	while( pChild != (wxXmlNode*)NULL )
	{
		if ( pChild->GetName().CmpNoCase( wxT("ChapterTimeEnd") ) == 0 )
		{
			return true;
		}
		pChild = pChild->GetNext();
	}
	return false;
}

static wxXmlNode* get_last_child( wxXmlNode* pNode )
{
	wxXmlNode* pChild = pNode->GetChildren();
	wxXmlNode* pPrev = (wxXmlNode*)NULL;
	while( pChild != (wxXmlNode*)NULL )
	{
		pPrev = pChild;
		pChild = pChild->GetNext();
	}
	return pPrev;
}

static wxXmlNode* add_chapter_display( wxXmlNode* pChapterAtom, const wxString& sChapterString, const wxString& sLang )
{
	wxXmlNode* pChapterDisplay = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("ChapterDisplay") );
	wxXmlNode* pLastChild = get_last_child( pChapterAtom );
	if ( pLastChild != (wxXmlNode*)NULL )
	{
		pLastChild->SetNext( pChapterDisplay );
	}
	else
	{
		pChapterAtom->SetChildren( pChapterDisplay );
	}

	wxXmlNode* pChapterString = new wxXmlNode( pChapterDisplay, wxXML_ELEMENT_NODE, wxT("ChapterString") );
	wxXmlNode* pChapterStringText = new wxXmlNode( pChapterString, wxXML_TEXT_NODE, wxEmptyString, sChapterString );

	wxXmlNode* pChapterLanguage = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("ChapterLanguage") );
	wxXmlNode* pChapterLanguageText = new wxXmlNode( pChapterLanguage, wxXML_TEXT_NODE, wxEmptyString, sLang );
	pChapterString->SetNext( pChapterLanguage );

	return pChapterDisplay;

}

static wxXmlNode* add_hidden_flag( wxXmlNode* pChapterAtom, bool bHiddenFlag )
{
	wxXmlNode* pChapterHidden = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("ChapterFlagHidden") );
	wxXmlNode* pChapterHiddenText = new wxXmlNode( pChapterHidden, wxXML_TEXT_NODE, wxEmptyString, 
		 bHiddenFlag? wxT("1") : wxT("0") );
	wxXmlNode* pLastChild = get_last_child( pChapterAtom );
	if ( pLastChild != (wxXmlNode*)NULL )
	{
		pLastChild->SetNext( pChapterHidden );
	}
	else
	{
		pChapterAtom->SetChildren( pChapterHidden );
	}
	return pChapterHidden;
}

static wxXmlNode* add_idx_chapter_atom( 
	wxXmlNode* pChapterAtom,
	const wxIndex& idx,
	const wxString& sLang,
	bool bHiddenIndexes
)
{
	wxXmlNode* pIdxChapterAtom = new wxXmlNode( pChapterAtom, wxXML_ELEMENT_NODE, wxT("ChapterAtom") );
	wxXmlNode* pChapterTimeStart = add_chapter_time_start( pIdxChapterAtom, idx );

	wxString sChapterString;
	sChapterString.Format( wxT("INDEX %02d"), idx.GetNumber() );

	add_chapter_display( pChapterAtom, sChapterString, sLang );
	add_hidden_flag( pChapterAtom, bHiddenIndexes );
	return pIdxChapterAtom;
}

// ===============================================================================

wxXmlCueSheetRenderer::wxXmlCueSheetRenderer(
	const wxConfiguration& cfg, const wxInputFile& inputFile, const wxString& sOutputFile ):
    m_pXmlDoc((wxXmlDocument*)NULL),
	m_cfg(cfg),
	m_inputFile( inputFile ),
	m_sOutputFile( sOutputFile )
{
}

wxXmlCueSheetRenderer::~wxXmlCueSheetRenderer(void)
{
	if ( m_pXmlDoc != (wxXmlDocument*)NULL )
	{
		delete m_pXmlDoc;
	}
}

wxXmlDocument* wxXmlCueSheetRenderer::GetXmlDoc() const
{
	return m_pXmlDoc;
}

bool wxXmlCueSheetRenderer::SaveXmlDoc()
{
	wxASSERT( m_pXmlDoc != (wxXmlDocument*)NULL );

	if ( !m_pXmlDoc->Save( m_sOutputFile ) )
	{
		wxLogError( _("Fail to save chapters to \u201C%s\u201D"), m_sOutputFile );
		return false;
	}

	return true;
}

bool wxXmlCueSheetRenderer::OnPreRenderDisc( const wxCueSheet& cueSheet )
{
	wxASSERT( m_pXmlDoc == (wxXmlDocument*)NULL );
	wxLogInfo( _("Creating XML document") );

	m_pXmlDoc = new wxXmlDocument();
	m_pXmlDoc->SetVersion( wxT("1.0") );
	m_pXmlDoc->SetFileEncoding( wxT("utf-8") );

	wxXmlNode* pChapters = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("Chapters") );
	m_pXmlDoc->SetRoot( pChapters );

	wxXmlNode* pLastComment;
	wxXmlNode* pComment = m_cfg.BuildXmlComments( m_inputFile, m_sOutputFile, pLastComment );
	pChapters->SetChildren( pComment );

	m_pEditionEntry = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("EditionEntry") );
	pLastComment->SetNext( m_pEditionEntry );

	m_pChapterAtom = (wxXmlNode*)NULL;
	m_pPrevChapterAtom = (wxXmlNode*)NULL;

	return wxCueSheetRenderer::OnPreRenderDisc( cueSheet );
}

bool wxXmlCueSheetRenderer::OnPreRenderTrack( const wxTrack& track )
{
	wxASSERT( m_pEditionEntry != (wxXmlNode*)NULL );
	wxLogInfo( _("Converting track %d"), track.GetNumber() );

	m_pChapterAtom = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("ChapterAtom") );
	if ( m_pPrevChapterAtom == (wxXmlNode*)NULL )
	{
		m_pEditionEntry->SetChildren( m_pChapterAtom );
	}
	else
	{
		m_pPrevChapterAtom->SetNext( m_pChapterAtom );
	}

	return wxCueSheetRenderer::OnPreRenderTrack( track );
}

bool wxXmlCueSheetRenderer::OnPostRenderTrack( const wxTrack& track )
{
	m_pPrevChapterAtom = m_pChapterAtom;
	return wxCueSheetRenderer::OnPostRenderTrack( track );
}

bool wxXmlCueSheetRenderer::OnRenderPreGap( const wxTrack& track, const wxIndex& preGap )
{
	wxLogInfo( _("Converting pre-gap of track %d"), track.GetNumber() );

	if ( track.GetNumber() == 1u )
	{
		if ( !m_cfg.TrackOneIndexOne() )
		{
			add_chapter_time_start( m_pChapterAtom, preGap );
		}
	}
	else if ( m_cfg.GetChapterTimeEnd() )
	{
		if ( m_pPrevChapterAtom != (wxXmlNode*)NULL )
		{
			add_chapter_time_end( m_pPrevChapterAtom, preGap );
		}
	}

	return wxCueSheetRenderer::OnRenderPreGap( track, preGap );
}

bool wxXmlCueSheetRenderer::OnRenderPostGap( const wxTrack& track, const wxIndex& postGap )
{
	wxLogInfo( _("Converting post-gap of track %d"), track.GetNumber() );

	if ( m_cfg.GetChapterTimeEnd() )
	{
		add_chapter_time_end( m_pPrevChapterAtom, postGap );
	}
	return wxCueSheetRenderer::OnRenderPostGap( track, postGap );
}

bool wxXmlCueSheetRenderer::OnRenderIndex( const wxTrack& track, const wxIndex& idx )
{
	wxLogInfo( _("Converting index %d of track %d"), idx.GetNumber(), track.GetNumber() );

	switch ( idx.GetNumber() )
	{
		case 0: // pre-gap
		if ( track.GetNumber() == 1u )
		{
			if ( !m_cfg.TrackOneIndexOne() )
			{
				add_chapter_time_start( m_pChapterAtom, idx );
			}
		}
		else if ( m_cfg.GetChapterTimeEnd() )
		{
			if ( m_pPrevChapterAtom != (wxXmlNode*)NULL )
			{
				add_chapter_time_end( m_pPrevChapterAtom, idx );
			}
		}
		break;

		case 1: // start
		{
			if ( track.GetNumber() == 1u )
			{
				if ( m_cfg.TrackOneIndexOne() )
				{
					add_chapter_time_start( m_pChapterAtom, idx );
				}
			}
			else
			{
				add_chapter_time_start( m_pChapterAtom, idx );
			}
		}
		break;

		default:
		add_idx_chapter_atom( m_pChapterAtom, idx, m_cfg.GetLang(), m_cfg.HiddenIndexes() );
		break;
	}
	return wxCueSheetRenderer::OnRenderIndex( track, idx );
}

bool wxXmlCueSheetRenderer::OnPostRenderDisc( const wxCueSheet& cueSheet )
{
	wxLogInfo( _("Calculating chapter names and end time from data file(s)") );

	wxXmlNode* pChapterAtom = m_pEditionEntry->GetChildren();
	const wxArrayTrack& tracks = cueSheet.GetTracks();
	size_t tracksCount = tracks.Count();

	for( size_t i=0; i<tracksCount; i++ )
	{
		if ( m_cfg.GetChapterTimeEnd() )
		{
			if ( !has_chapter_time_end( pChapterAtom ) )
			{
				wxDataFile dataFile;
				if ( m_cfg.GetUseDataFiles() && cueSheet.IsLastTrackForDataFile( i, dataFile ) )
				{
					wxLogInfo( _("Calculating end time for track %d using media file \u201C%s\u201D"), tracks[i].GetNumber(), dataFile.GetFileName() );
					wxULongLong samples( dataFile.GetNumberOfSamples( m_cfg.GetAlternateExtensions(), m_cfg.RoundDownToFullFrames() ) );
					if ( samples != wxDataFile::wxInvalidNumberOfSamples )
					{
						wxLogDebug( _("Number of samples - %s"), samples.ToString() );
						add_chapter_time_end( pChapterAtom, samples );
					}
				}
				else if ( m_cfg.GetUnknownChapterTimeEndToNextChapter() && ((i+1) < tracksCount) )
				{
					wxLogInfo( _("Calculating end time for track %d using offset (%d frames)"), tracks[i].GetNumber(), m_cfg.GetChapterOffset() );
					const wxTrack& nextTrack = tracks[i+1];
					add_chapter_time_end( pChapterAtom, nextTrack, m_cfg.GetChapterOffset() );
				}
			}

			wxString s( cueSheet.FormatTrack( i, m_cfg.GetTrackNameFormat() ) );
			add_chapter_display( pChapterAtom, s, m_cfg.GetLang() );
		}
		pChapterAtom = pChapterAtom->GetNext();
	}

	wxLogInfo( _("Conversion done") );
	return wxCueSheetRenderer::OnPostRenderDisc( cueSheet );
}