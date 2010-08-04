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

static wxXmlNode* add_chapter_uid( wxXmlNode* pChapterAtom, const wxULongLong& uid )
{
	wxXmlNode* pChapterUID = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("ChapterUID") );
	wxXmlNode* pChapterUIDText = new wxXmlNode( pChapterUID, wxXML_TEXT_NODE, wxEmptyString, uid.ToString() );

	pChapterAtom->AddChild( pChapterUID );
	return pChapterUID;
}

static wxXmlNode* add_chapter_time_start( wxXmlNode* pChapterAtom, const wxIndex& idx )
{
	wxXmlNode* pChapterTimeStart = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("ChapterTimeStart") );
	wxXmlNode* pChapterTimeStartText = new wxXmlNode( pChapterTimeStart, wxXML_TEXT_NODE, wxEmptyString, idx.GetTimeStr() );

	pChapterAtom->AddChild( pChapterTimeStart );
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

wxULongLong wxXmlCueSheetRenderer::GenerateUID()
{
	return wxULongLong( (wxUint32)rand(), (wxUint32)rand() );
}

wxXmlCueSheetRenderer::wxXmlCueSheetRenderer(
	const wxConfiguration& cfg, const wxInputFile& inputFile ):
    m_pXmlDoc((wxXmlDocument*)NULL),
	m_pXmlTags((wxXmlDocument*)NULL),
	m_cfg(cfg),
	m_inputFile( inputFile )
{
	cfg.GetOutputFile( inputFile, m_sOutputFile, m_sTagsFile );
}

wxXmlCueSheetRenderer::~wxXmlCueSheetRenderer(void)
{
	if ( m_pXmlDoc != (wxXmlDocument*)NULL )
	{
		delete m_pXmlDoc;
	}

	if ( m_pXmlTags != (wxXmlDocument*)NULL )
	{
		delete m_pXmlTags;
	}
}

wxXmlDocument* wxXmlCueSheetRenderer::GetXmlDoc() const
{
	return m_pXmlDoc;
}

wxXmlDocument* wxXmlCueSheetRenderer::GetXmlTags() const
{
	return m_pXmlTags;
}

const wxString& wxXmlCueSheetRenderer::GetOutputFile() const
{
	return m_sOutputFile;
}

const wxString& wxXmlCueSheetRenderer::GetTagsFile() const
{
	return m_sTagsFile;
}

bool wxXmlCueSheetRenderer::SaveXmlDoc()
{
	wxASSERT( m_pXmlDoc != (wxXmlDocument*)NULL );
	wxASSERT( m_pXmlTags != (wxXmlDocument*)NULL );

	if ( !m_pXmlDoc->Save( m_sOutputFile ) )
	{
		wxLogError( _("Fail to save chapters to \u201C%s\u201D"), m_sOutputFile );
		return false;
	}

	if ( m_cfg.GenerateTags() )
	{
		wxASSERT( !m_sTagsFile.IsEmpty() );
		if ( !m_pXmlTags->Save( m_sTagsFile ) )
		{
			wxLogError( _("Fail to save tags to \u201C%s\u201D"), m_sTagsFile );
			return false;
		}
	}

	return true;
}

static wxXmlDocument* create_xml_document( const wxString& sRootNode )
{
	wxXmlDocument* pXmlDoc = new wxXmlDocument();
	pXmlDoc->SetVersion( wxT("1.0") );
	pXmlDoc->SetFileEncoding( wxT("utf-8") );

	wxXmlNode* pRoot = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, sRootNode );
	pXmlDoc->SetRoot( pRoot );

	return pXmlDoc;
}

static wxXmlNode* create_simple_tag( const wxString& sName, const wxString& sValue, const wxString& sLanguage )
{
	wxXmlNode* pSimple = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("Simple") );

	wxXmlNode* pName = new wxXmlNode( pSimple, wxXML_ELEMENT_NODE, wxT("Name") );
	wxXmlNode* pNameText = new wxXmlNode( pName, wxXML_TEXT_NODE, wxEmptyString, sName );

	wxXmlNode* pValue = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("String") );
	wxXmlNode* pValueText = new wxXmlNode( pValue, wxXML_TEXT_NODE, wxEmptyString, sValue );

	pSimple->AddChild( pValue );

	wxXmlNode* pLang = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("TagLanguage") );
	wxXmlNode* pLangText = new wxXmlNode( pLang, wxXML_TEXT_NODE, wxEmptyString, sLanguage );

	pSimple->AddChild( pLang );

	return pSimple;
}

static void add_cd_text_info( const wxCueComponent& component, wxXmlNode* pTag, const wxString& sLanguage )
{
	const wxCueComponent::wxHashString& cdTextInfo = component.GetCdTextInfo();
	for( wxCueComponent::wxHashString::const_iterator i=cdTextInfo.begin();
		i != cdTextInfo.end(); i++ )
	{
		wxCueComponent::ENTRY_FORMAT entryFormat;
		wxCueComponent::ENTRY_TYPE entryType;

		wxCueComponent::GetCdTextInfoFormat( i->first, entryFormat );
		wxCueComponent::GetCdTextInfoType( i->first, entryType );

		if ( (entryFormat == wxCueComponent::CHARACTER) && component.CheckEntryType( entryType ) )
		{ // we can save this entry
			wxXmlNode* pSimple = create_simple_tag( i->first, i->second, sLanguage );
			pTag->AddChild( pSimple );
		}
	}
}

static wxXmlNode* add_disc_tags(
	const wxCueSheet& cueSheet,
	wxXmlNode* pTags,
	const wxULongLong& editionUID,
	const wxString& sLanguage,
	int nTargetTypeValue = 50 )
{
	wxXmlNode* pTag = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("Tag") );
	pTags->AddChild( pTag );

	wxXmlNode* pComment = new wxXmlNode( (wxXmlNode*)NULL, wxXML_COMMENT_NODE, wxEmptyString, wxT("Disc tag") );
	pTag->AddChild( pComment );

	wxXmlNode* pTargets = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("Targets") );
	pTag->AddChild( pTargets );

	wxXmlNode* pTargetTypeValue = new wxXmlNode( pTargets, wxXML_ELEMENT_NODE, wxT("TargetTypeValue") );	
	wxXmlNode* pTargetTypeValueText = new wxXmlNode( pTargetTypeValue, wxXML_TEXT_NODE, wxEmptyString, wxString::Format( wxT("%d"), nTargetTypeValue ) );

	wxXmlNode* pTargetType = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("TargetType") );	
	wxXmlNode* pTargetTypeText = new wxXmlNode( pTargetType, wxXML_TEXT_NODE, wxEmptyString, wxT("ALBUM") );
	pTargets->AddChild( pTargetType );

	wxXmlNode* pEditionUID = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("EditionUID") );	
	wxXmlNode* pEditionUIDText = new wxXmlNode( pEditionUID, wxXML_TEXT_NODE, wxEmptyString, editionUID.ToString() );
	pTargets->AddChild( pEditionUID );

	add_cd_text_info( cueSheet, pTag, sLanguage );
	wxXmlNode* pSimple = create_simple_tag( 
		wxT("TOTAL_PARTS"),
		wxString::Format( wxT("%d"), cueSheet.GetTracks().Count() ),
		sLanguage );
	pTag->AddChild( pSimple );

	pSimple = create_simple_tag( 
		wxT("ORIGINAL_MEDIA_TYPE"),
		wxT("CD"),
		sLanguage );
	pTag->AddChild( pSimple );

	if ( !cueSheet.GetCatalog().IsEmpty() )
	{
		pSimple = create_simple_tag( 
			wxT("CATALOG_NUMBER"),
			cueSheet.GetCatalog(),
			sLanguage );
		pTag->AddChild( pSimple );
	}

	return pTag;
}

static wxXmlNode* add_track_tags( const wxTrack& track, const wxULongLong& chapterUID, wxXmlNode* pTags, const wxString& sLanguage, int nTargetTypeValue = 30 )
{
	wxXmlNode* pTag = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("Tag") );
	pTags->AddChild( pTag );

	wxXmlNode* pComment = new wxXmlNode( (wxXmlNode*)NULL, wxXML_COMMENT_NODE, wxEmptyString, wxString::Format( wxT("Track %02d"), track.GetNumber() ) );
	pTag->AddChild( pComment );

	wxXmlNode* pTargets = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("Targets") );	
	pTag->AddChild( pTargets );

	wxXmlNode* pTargetTypeValue = new wxXmlNode( pTargets, wxXML_ELEMENT_NODE, wxT("TargetTypeValue") );	
	wxXmlNode* pTargetTypeValueText = new wxXmlNode( pTargetTypeValue, wxXML_TEXT_NODE, wxEmptyString, wxString::Format( wxT("%d"), nTargetTypeValue ) );

	wxXmlNode* pTargetType = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("TargetType") );	
	wxXmlNode* pTargetTypeText = new wxXmlNode( pTargetType, wxXML_TEXT_NODE, wxEmptyString, wxT("TRACK") );
	pTargets->AddChild( pTargetType );

	wxXmlNode* pChapterUID = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("ChapterUID") );	
	wxXmlNode* pChapterUIDText = new wxXmlNode( pChapterUID, wxXML_TEXT_NODE, wxEmptyString, chapterUID.ToString() );
	pTargets->AddChild( pChapterUID );
	add_cd_text_info( track, pTag, sLanguage );
	return pTag;
}

bool wxXmlCueSheetRenderer::OnPreRenderDisc( const wxCueSheet& cueSheet )
{
	wxASSERT( m_pXmlDoc == (wxXmlDocument*)NULL );
	wxLogInfo( _("Creating XML document") );

	m_pXmlDoc = create_xml_document( wxT("Chapters") );
	wxXmlNode* pChapters = m_pXmlDoc->GetRoot();

	m_cfg.BuildXmlComments( m_inputFile, m_sOutputFile, pChapters );

	wxULongLong editionUID( GenerateUID() );
	wxXmlNode* pEditionUID = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("EditionUID") );
	wxXmlNode* pEditionUIDText = new wxXmlNode( pEditionUID, wxXML_TEXT_NODE, editionUID.ToString() );
	pChapters->AddChild( pEditionUID );

	m_pEditionEntry = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("EditionEntry") );
	pChapters->AddChild( m_pEditionEntry );

	m_pChapterAtom = (wxXmlNode*)NULL;
	m_pPrevChapterAtom = (wxXmlNode*)NULL;

	wxASSERT( m_pXmlTags == (wxXmlDocument*)NULL );
	m_pXmlTags = create_xml_document( wxT("Tags") );

	m_pTags = m_pXmlTags->GetRoot();
	m_cfg.BuildXmlComments( m_inputFile, m_sTagsFile, m_pTags );
	add_disc_tags( cueSheet, m_pTags, editionUID, m_cfg.GetLang() );

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

bool wxXmlCueSheetRenderer::OnRenderTrack( const wxTrack& track )
{
	wxULongLong chapterUID( GenerateUID() );
	add_chapter_uid( m_pChapterAtom, chapterUID );
	add_track_tags( track, chapterUID, m_pTags, m_cfg.GetLang() );
	return wxCueSheetRenderer::OnRenderTrack( track );
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