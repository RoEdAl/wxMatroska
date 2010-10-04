/*
	wxXmlCueSheetRenderer.cpp
*/

#include "StdWx.h"
#include "wxConfiguration.h"
#include <wxSamplingInfo.h>
#include <wxIndex.h>
#include <wxTrack.h>
#include "wxInputFile.h"
#include "wxXmlCueSheetRenderer.h"

IMPLEMENT_CLASS( wxXmlCueSheetRenderer, wxCueSheetRenderer )

wxXmlCueSheetRenderer* const wxXmlCueSheetRenderer::Null = (wxXmlCueSheetRenderer* const)wxCueSheetRenderer::Null;

// ===============================================================================

static wxXmlNode* get_last_child( wxXmlNode* pNode )
{
	wxXmlNode* pChild = pNode->GetChildren();
	wxXmlNode* pPrev = wxNullXmlNode;
	while( pChild != wxNullXmlNode )
	{
		pPrev = pChild;
		pChild = pChild->GetNext();
	}
	return pPrev;
}

static wxXmlNode* add_chapter_uid( wxXmlNode* pChapterAtom, const wxULongLong& uid )
{
	wxXmlNode* pChapterUID = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("ChapterUID") );
	wxXmlNode* pChapterUIDText = new wxXmlNode( pChapterUID, wxXML_TEXT_NODE, wxEmptyString, uid.ToString() );

	pChapterAtom->AddChild( pChapterUID );
	return pChapterUID;
}

static wxXmlNode* add_chapter_time_start( wxXmlNode* pChapterAtom, const wxString& s )
{
	wxXmlNode* pChapterTimeStart = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("ChapterTimeStart") );
	wxXmlNode* pChapterTimeStartText = new wxXmlNode( pChapterTimeStart, wxXML_TEXT_NODE, wxEmptyString, s );

	pChapterAtom->AddChild( pChapterTimeStart );
	return pChapterTimeStart;
}

static wxXmlNode* add_chapter_time_start( wxXmlNode* pChapterAtom, const wxSamplingInfo& si, wxULongLong frames )
{
	return add_chapter_time_start( pChapterAtom, si.GetSamplesStr( frames ) );
}

static wxXmlNode* find_chapter_time_start( wxXmlNode* pChapterAtom )
{
	wxXmlNode* pChild = pChapterAtom->GetChildren();
	wxXmlNode* pPrev = wxNullXmlNode;
	while( pChild != wxNullXmlNode )
	{
		if ( pChild->GetName().CmpNoCase( wxT("ChapterTimeStart") ) == 0 )
		{
			return pChild;
		}
		pPrev = pChild;
		pChild = pChild->GetNext();
	}

	wxASSERT( pPrev != wxNullXmlNode );
	return pPrev;
}

static bool is_album_tag( wxXmlNode* pTag, long nTargetTypeValue )
{
	wxXmlNode* pChild = pTag->GetChildren();
	while( pChild != wxNullXmlNode  )
	{
		if ( pChild->GetType() == wxXML_ELEMENT_NODE ) break;
		pChild = pChild->GetNext();
	}
	if ( pChild == wxNullXmlNode ) return false;
	if ( pChild->GetName().CmpNoCase( wxT("Targets") ) != 0 )
	{
		return false;
	}

	wxXmlNode* pTargetTypeValue = pChild->GetChildren();
	if ( pTargetTypeValue == wxNullXmlNode ) return false;
	wxXmlNode* pText = pTargetTypeValue->GetChildren();
	if ( pText == wxNullXmlNode ) return false;

	long nParsedTargetTypeValue;
	if ( !pText->GetContent().ToCLong( &nParsedTargetTypeValue ) )
	{
		return false;
	}

	return (nParsedTargetTypeValue == nTargetTypeValue);
}

static wxXmlNode* find_disc_tag_node( wxXmlNode* pTags, long nTargetTypeValue )
{
	wxXmlNode* pChild = pTags->GetChildren();
	while( pChild != wxNullXmlNode )
	{
		if ( (pChild->GetType() == wxXML_ELEMENT_NODE) && (pChild->GetName().CmpNoCase( wxT("Tag") ) == 0) )
		{
			if ( is_album_tag( pChild, nTargetTypeValue ) )
			{
				return pChild;
			}
		}
		pChild = pChild->GetNext();
	}
	return wxNullXmlNode;
}

static bool is_total_parts( wxXmlNode* pSimple )
{
	wxXmlNode* pChild = pSimple->GetChildren();
	while( pChild != wxNullXmlNode )
	{
		if ( (pChild->GetType() == wxXML_ELEMENT_NODE) &&
			 (pChild->GetName().CmpNoCase( wxT("Name") ) == 0) )
		{
			wxXmlNode* pText = pChild->GetChildren();
			if ( ( pText != wxNullXmlNode ) &&
				( pText->GetContent().CmpNoCase(wxT("TOTAL_PARTS")) == 0 ) )
			{
				return true;
			}
		}
		pChild = pChild->GetNext();
	}

	return false;
}

static bool set_total_parts( wxXmlNode* pSimple, size_t nTotalParts )
{
	wxXmlNode* pChild = pSimple->GetChildren();
	while( pChild != wxNullXmlNode )
	{
		if ( (pChild->GetType() == wxXML_ELEMENT_NODE) &&
			 (pChild->GetName().CmpNoCase( wxT("String") ) == 0) )
		{
			wxXmlNode* pText = pChild->GetChildren();
			wxASSERT( pText != wxNullXmlNode );
			pText->SetContent( wxString::Format( wxT("%d"), nTotalParts ) );
			return true;
		}
		pChild = pChild->GetNext();
	}

	return false;
}

static wxXmlNode* find_total_parts_node( wxXmlNode* pTag )
{
	wxXmlNode* pChild = pTag->GetChildren();
	while( pChild != wxNullXmlNode )
	{
		if ( (pChild->GetType() == wxXML_ELEMENT_NODE) && (pChild->GetName().CmpNoCase( wxT("Simple") ) == 0) )
		{
			if ( is_total_parts( pChild ) )
			{
				return pChild;
			}
		}
		pChild = pChild->GetNext();
	}
	return wxNullXmlNode;
}

static wxXmlNode* add_chapter_time_end( wxXmlNode* pChapterAtom, const wxString& text )
{
	wxXmlNode* pChapterTimeStart = find_chapter_time_start( pChapterAtom );

	wxXmlNode* pChapterTimeEnd = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("ChapterTimeEnd") );
	wxXmlNode* pChapterTimeEndText = new wxXmlNode( pChapterTimeEnd, wxXML_TEXT_NODE, wxEmptyString, text );

	pChapterAtom->InsertChildAfter( pChapterTimeEnd, pChapterTimeStart );

	return pChapterTimeEnd;
}

static wxXmlNode* add_chapter_time_end( wxXmlNode* pChapterAtom, const wxSamplingInfo& si, wxULongLong frames )
{
	return add_chapter_time_end( pChapterAtom, si.GetSamplesStr( frames ) );
}

static bool has_chapter_time_end( wxXmlNode* pChapterAtom )
{
	wxXmlNode* pChild = pChapterAtom->GetChildren();
	while( pChild != wxNullXmlNode )
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
	wxXmlNode* pChapterDisplay = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("ChapterDisplay") );

	wxXmlNode* pChapterString = new wxXmlNode( pChapterDisplay, wxXML_ELEMENT_NODE, wxT("ChapterString") );
	wxXmlNode* pChapterStringText = new wxXmlNode( pChapterString, wxXML_TEXT_NODE, wxEmptyString, sChapterString );

	wxXmlNode* pChapterLanguage = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("ChapterLanguage") );
	wxXmlNode* pChapterLanguageText = new wxXmlNode( pChapterLanguage, wxXML_TEXT_NODE, wxEmptyString, sLang );
	pChapterDisplay->AddChild( pChapterLanguage );

	pChapterAtom->AddChild( pChapterDisplay );
	return pChapterDisplay;
}

static wxXmlNode* add_hidden_flag( wxXmlNode* pChapterAtom, bool bHiddenFlag )
{
	wxXmlNode* pChapterHidden = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("ChapterFlagHidden") );
	wxXmlNode* pChapterHiddenText = new wxXmlNode( pChapterHidden, wxXML_TEXT_NODE, wxEmptyString, 
		 bHiddenFlag? wxT("1") : wxT("0") );
	pChapterAtom->AddChild( pChapterHidden );
	return pChapterHidden;
}

static wxXmlNode* add_idx_chapter_atom( 
	wxXmlNode* pChapterAtom,
	const wxSamplingInfo& si,
	const wxULongLong& frames,
	unsigned int nIndexNumber,
	const wxString& sLang,
	bool bHiddenIndexes
)
{
	wxXmlNode* pIdxChapterAtom = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("ChapterAtom") );
	wxXmlNode* pChapterTimeStart = add_chapter_time_start( pIdxChapterAtom, si, frames );

	add_chapter_display( pIdxChapterAtom, wxString::Format( wxT("INDEX %02d"), nIndexNumber ), sLang );
	add_hidden_flag( pIdxChapterAtom, bHiddenIndexes );

	pChapterAtom->AddChild( pIdxChapterAtom );
	return pIdxChapterAtom;
}

// ===============================================================================

wxULongLong wxXmlCueSheetRenderer::GenerateUID()
{
	return wxULongLong( (wxUint32)rand(), (wxUint32)rand() );
}

wxXmlCueSheetRenderer::wxXmlCueSheetRenderer(
	const wxConfiguration& cfg, const wxInputFile& inputFile ):
    m_pXmlDoc(wxNullXmlDocument),
	m_pXmlTags(wxNullXmlDocument),
	m_cfg(cfg),
	m_inputFile( inputFile ),
	m_offset( wxULL(0) ),
	m_nTotalParts(0)
{
	cfg.GetOutputFile( inputFile, m_sOutputFile, m_sTagsFile );
}

wxXmlCueSheetRenderer::~wxXmlCueSheetRenderer(void)
{
	if ( m_pXmlDoc != wxNullXmlDocument )
	{
		delete m_pXmlDoc;
	}

	if ( m_pXmlTags != wxNullXmlDocument )
	{
		delete m_pXmlTags;
	}
}

void wxXmlCueSheetRenderer::SetInputFile( const wxInputFile& inputFile )
{
	m_inputFile = inputFile;
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
	wxASSERT( m_pXmlDoc != wxNullXmlDocument );
	wxASSERT( m_pXmlTags != wxNullXmlDocument );

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

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeStart( wxXmlNode* pChapterAtom, const wxIndex& idx ) const
{
	wxULongLong frames( m_offset );
	frames += m_si.GetIndexOffset( idx );
	return add_chapter_time_start( pChapterAtom, m_si, frames );
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeEnd( wxXmlNode* pChapterAtom, const wxULongLong& frames ) const
{
	wxULongLong tframes( m_offset );
	tframes += frames;
	return add_chapter_time_end( pChapterAtom, m_si, frames );
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeEnd( wxXmlNode* pChapterAtom, const wxIndex& idx ) const
{
	return AddChapterTimeEnd( pChapterAtom, m_si.GetIndexOffset( idx ) );
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeEnd( wxXmlNode* pChapterAtom, const wxTrack& track ) const
{
	wxIndex fidx( m_si.ConvertIndex( track.GetFirstIndex() ) );
	fidx -= m_si.GetFramesFromCdFrames( m_cfg.GetChapterOffset() );
	return AddChapterTimeEnd( pChapterAtom, fidx );
}

wxXmlNode* wxXmlCueSheetRenderer::AddIdxChapterAtom( wxXmlNode* pChapterAtom, const wxIndex& idx ) const
{
	wxULongLong frames( m_offset );
	frames += m_si.GetIndexOffset( idx );
	return add_idx_chapter_atom( pChapterAtom, m_si, frames, idx.GetNumber(), m_cfg.GetLang(), m_cfg.HiddenIndexes() );
}

static wxXmlDocument* create_xml_document( const wxString& sRootNode )
{
	wxXmlDocument* pXmlDoc = new wxXmlDocument();
	pXmlDoc->SetVersion( wxT("1.0") );
	pXmlDoc->SetFileEncoding( wxT("utf-8") );

	wxXmlNode* pRoot = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, sRootNode );
	pXmlDoc->SetRoot( pRoot );

	return pXmlDoc;
}

static wxXmlNode* create_simple_tag( const wxString& sName, const wxString& sValue, const wxString& sLanguage )
{
	wxXmlNode* pSimple = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("Simple") );

	wxXmlNode* pName = new wxXmlNode( pSimple, wxXML_ELEMENT_NODE, wxT("Name") );
	wxXmlNode* pNameText = new wxXmlNode( pName, wxXML_TEXT_NODE, wxEmptyString, sName );

	wxXmlNode* pValue = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("String") );
	wxXmlNode* pValueText = new wxXmlNode( pValue, wxXML_TEXT_NODE, wxEmptyString, sValue );

	pSimple->AddChild( pValue );

	wxXmlNode* pLang = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("TagLanguage") );
	wxXmlNode* pLangText = new wxXmlNode( pLang, wxXML_TEXT_NODE, wxEmptyString, sLanguage );

	pSimple->AddChild( pLang );

	return pSimple;
}

static wxXmlNode* create_simple_tag( const wxCueTag& tag, const wxString& sLanguage )
{
	return create_simple_tag( tag.GetName(), tag.GetValue(), sLanguage );
}

static bool is_simple( wxXmlNode* pNode, const wxString& sName, const wxString& sValue )
{
	wxASSERT( pNode != wxNullXmlNode );
	wxXmlNode* pChild = pNode->GetChildren();
	bool bName = false;
	bool bValue = false;

	while( pChild != wxNullXmlNode )
	{
		if (pChild->GetType() == wxXML_ELEMENT_NODE)
		{
			if ( pChild->GetName().CmpNoCase( wxT("Name") ) == 0 )
			{
				wxXmlNode* pText = pChild->GetChildren();
				if ( pText == wxNullXmlNode ) return false;
				if ( pText->GetType() != wxXML_TEXT_NODE ) return false;
				bName = ( pText->GetContent().CmpNoCase( sName ) == 0 );			
			}

			if ( pChild->GetName().CmpNoCase( wxT("String") ) == 0 )
			{
				wxXmlNode* pText = pChild->GetChildren();
				if ( pText == wxNullXmlNode ) return false;
				if ( pText->GetType() != wxXML_TEXT_NODE ) return false;
				bValue = ( pText->GetContent().Cmp( sValue ) == 0 );			
			}
		}

		pChild = pChild->GetNext();
	}

	return (bName && bValue);
}

static wxXmlNode* find_simple_tag( wxXmlNode* pNode, const wxString& sName, const wxString& sValue )
{
	wxASSERT( pNode != wxNullXmlNode );
	wxXmlNode* pChild = pNode->GetChildren();
	while( pChild != wxNullXmlNode )
	{
		if ( is_simple( pChild, sName, sValue ) )
		{
			return pChild;
		}

		pChild = pChild->GetNext();
	}

	return wxNullXmlNode;
}

static wxXmlNode* find_simple_tag( wxXmlNode* pNode, const wxCueTag& tag )
{
	return find_simple_tag( pNode, tag.GetName(), tag.GetValue() );
}

static wxXmlNode* add_simple_tag( wxXmlNode* pNode, const wxString& sName, const wxString& sValue, const wxString& sLanguage )
{
	wxASSERT( pNode != wxNullXmlNode );
	wxXmlNode* pTagNode = find_simple_tag( pNode, sName, sValue );
	if ( pTagNode == wxNullXmlNode )
	{
		pTagNode = create_simple_tag( sName, sValue, sLanguage );
		pNode->AddChild( pTagNode );
	}
	return pTagNode;
}

static wxXmlNode* add_simple_tag( wxXmlNode* pNode, const wxCueTag& tag, const wxString& sLanguage )
{
	return add_simple_tag( pNode, tag.GetName(), tag.GetValue(), sLanguage );
}

void wxXmlCueSheetRenderer::AddCdTextInfo( const wxCueComponent& component, wxXmlNode* pTag )
{
	const wxArrayCueTag& cdTextTags = component.GetCdTextTags();
	size_t numTags = cdTextTags.Count();
	for( size_t i=0; i<numTags; i++ )
	{
		wxCueComponent::ENTRY_FORMAT entryFormat;
		wxCueComponent::ENTRY_TYPE entryType;

		wxCueComponent::GetCdTextInfoFormat( cdTextTags[i].GetName(), entryFormat );
		wxCueComponent::GetCdTextInfoType( cdTextTags[i].GetName(), entryType );

		if ( ((entryFormat == wxCueComponent::CHARACTER) || (entryFormat == wxCueComponent::BINARY)) && component.CheckEntryType( entryType ) )
		{ // we can save this entry
			wxXmlNode* pSimple = add_simple_tag( pTag, cdTextTags[i], m_cfg.GetLang() );
		}
	}

	wxArrayCueTag tags;
	component.GetTags( tags );
	numTags = tags.Count();
	for( size_t i = 0; i < numTags; i++ )
	{
		wxXmlNode* pSimple = add_simple_tag( pTag, tags[i], m_cfg.GetLang() );
	}
}

wxXmlNode* wxXmlCueSheetRenderer::AddDiscTags(
	const wxCueSheet& cueSheet,
	wxXmlNode* pTags,
	const wxULongLong& editionUID,
	int nTargetTypeValue )
{
	wxXmlNode* pTag = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("Tag") );
	pTags->AddChild( pTag );

	wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString, wxT("Disc tag") );
	pTag->AddChild( pComment );

	wxXmlNode* pTargets = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("Targets") );
	pTag->AddChild( pTargets );

	wxXmlNode* pTargetTypeValue = new wxXmlNode( pTargets, wxXML_ELEMENT_NODE, wxT("TargetTypeValue") );	
	wxXmlNode* pTargetTypeValueText = new wxXmlNode( pTargetTypeValue, wxXML_TEXT_NODE, wxEmptyString, wxString::Format( wxT("%d"), nTargetTypeValue ) );

	wxXmlNode* pTargetType = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("TargetType") );	
	wxXmlNode* pTargetTypeText = new wxXmlNode( pTargetType, wxXML_TEXT_NODE, wxEmptyString, wxT("ALBUM") );
	pTargets->AddChild( pTargetType );

	if ( m_cfg.GenerateEditionUID() )
	{
		wxXmlNode* pEditionUID = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("EditionUID") );	
		wxXmlNode* pEditionUIDText = new wxXmlNode( pEditionUID, wxXML_TEXT_NODE, wxEmptyString, editionUID.ToString() );
		pTargets->AddChild( pEditionUID );
	}

	wxXmlNode* pSimple = add_simple_tag( 
		pTag,
		wxT("ORIGINAL_MEDIA_TYPE"),
		wxT("CD"),
		m_cfg.GetLang() );

	AppendDiscTags( cueSheet, pTags, nTargetTypeValue );
	return pTag;
}

wxXmlNode* wxXmlCueSheetRenderer::AppendDiscTags(
	const wxCueSheet& cueSheet,
	wxXmlNode* pTags,
	long nTargetTypeValue )
{
	wxXmlNode* pTag = find_disc_tag_node( pTags, nTargetTypeValue );
	wxASSERT( pTag != wxNullXmlNode );

	wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString,
		wxString::Format( wxT("CUE file: \u201C%s\u201D"), m_inputFile.ToString(false) ) );
	pTag->AddChild( pComment );

	AddCdTextInfo( cueSheet, pTag );

	if ( !cueSheet.GetCatalog().IsEmpty() )
	{
		wxXmlNode* pSimple = add_simple_tag( 
			pTag,
			wxT("CATALOG_NUMBER"),
			cueSheet.GetCatalog(),
			m_cfg.GetLang() );
	}

	return pTag;
}

wxXmlNode* wxXmlCueSheetRenderer::SetTotalParts(
	wxXmlNode* pTags,
	long nTargetTypeValue )
{
	wxXmlNode* pTag = find_disc_tag_node( pTags, nTargetTypeValue );
	wxASSERT( pTag != wxNullXmlNode );

	wxXmlNode* pSimple = find_total_parts_node( pTag );
	if ( pSimple == wxNullXmlNode )
	{
		wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString, wxT("Total number of tracks") );
		pTag->AddChild( pComment );

		wxXmlNode* pSimple = add_simple_tag( 
			pTag,
			wxT("TOTAL_PARTS"),
			wxString::Format( wxT("%d"), m_nTotalParts ),
			m_cfg.GetLang() );
	}
	else
	{
		set_total_parts( pSimple, m_nTotalParts );
	}

	return pTag;
}

wxXmlNode* wxXmlCueSheetRenderer::AddTrackTags(
	const wxTrack& track,
	const wxULongLong& chapterUID,
	wxXmlNode* pTags,
	int nTargetTypeValue )
{
	wxXmlNode* pTag = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("Tag") );
	pTags->AddChild( pTag );

	wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString, wxString::Format( wxT("Track %02d"), track.GetNumber() ) );
	pTag->AddChild( pComment );

	wxXmlNode* pTargets = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("Targets") );	
	pTag->AddChild( pTargets );

	wxXmlNode* pTargetTypeValue = new wxXmlNode( pTargets, wxXML_ELEMENT_NODE, wxT("TargetTypeValue") );	
	wxXmlNode* pTargetTypeValueText = new wxXmlNode( pTargetTypeValue, wxXML_TEXT_NODE, wxEmptyString, wxString::Format( wxT("%d"), nTargetTypeValue ) );

	wxXmlNode* pTargetType = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("TargetType") );	
	wxXmlNode* pTargetTypeText = new wxXmlNode( pTargetType, wxXML_TEXT_NODE, wxEmptyString, wxT("TRACK") );
	pTargets->AddChild( pTargetType );

	wxXmlNode* pChapterUID = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("ChapterUID") );	
	wxXmlNode* pChapterUIDText = new wxXmlNode( pChapterUID, wxXML_TEXT_NODE, wxEmptyString, chapterUID.ToString() );
	pTargets->AddChild( pChapterUID );

	wxXmlNode* pSimple = add_simple_tag( 
			pTag,
			wxT("PART_NUMBER"),
			wxString::Format( wxT("%d"), m_nTotalParts ),
			m_cfg.GetLang() );

	AddCdTextInfo( track, pTag );
	return pTag;
}

bool wxXmlCueSheetRenderer::OnPreRenderDisc( const wxCueSheet& cueSheet )
{
	if ( !m_cfg.GetMerge() )
	{
		wxASSERT( m_pXmlDoc == wxNullXmlDocument );
		wxASSERT( m_pXmlTags == wxNullXmlDocument );
	}

	m_pFirstChapterAtom = wxNullXmlNode;
	wxULongLong editionUID( GenerateUID() );

	if ( m_pXmlDoc == wxNullXmlDocument )
	{
		wxLogInfo( _("Creating XML document") );
		m_pXmlDoc = create_xml_document( wxT("Chapters") );
		wxXmlNode* pChapters = m_pXmlDoc->GetRoot();
		m_cfg.BuildXmlComments( m_sOutputFile, pChapters );
		m_pEditionEntry = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("EditionEntry") );
		pChapters->AddChild( m_pEditionEntry );
		if ( m_cfg.GenerateEditionUID() )
		{
			wxXmlNode* pEditionUID = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("EditionUID") );
			wxXmlNode* pEditionUIDText = new wxXmlNode( pEditionUID, wxXML_TEXT_NODE, wxEmptyString, editionUID.ToString() );
			m_pEditionEntry->AddChild( pEditionUID );
		}

		m_pChapterAtom = wxNullXmlNode;
		m_pPrevChapterAtom = wxNullXmlNode;
	}
	else
	{
		m_pPrevChapterAtom = wxNullXmlNode;
	}

	wxASSERT( m_pEditionEntry != wxNullXmlNode );
	wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString,
		wxString::Format( wxT("CUE file: \u201C%s\u201D"), m_inputFile.ToString(false) ) );
	m_pEditionEntry->AddChild( pComment );

	if ( m_pXmlTags == wxNullXmlDocument )
	{
		m_pXmlTags = create_xml_document( wxT("Tags") );
		m_pTags = m_pXmlTags->GetRoot();
		m_cfg.BuildXmlComments( m_sTagsFile, m_pTags );
		AddDiscTags( cueSheet, m_pTags, editionUID );
	}
	else
	{
		AppendDiscTags( cueSheet, m_pTags );
	}

	pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString,
		wxString::Format( wxT("CUE file: \u201C%s\u201D"), m_inputFile.ToString(false) ) );
	m_pTags->AddChild( pComment );
	pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString,
		wxString::Format( wxT("Number of tracks: %d"), cueSheet.GetTracks().Count() ) );
	m_pTags->AddChild( pComment );

	return wxCueSheetRenderer::OnPreRenderDisc( cueSheet );
}

bool wxXmlCueSheetRenderer::OnPreRenderTrack( const wxCueSheet& cueSheet, const wxTrack& track )
{
	wxASSERT( m_pEditionEntry != wxNullXmlNode );
	wxLogInfo( _("Converting track %d"), track.GetNumber() );
	
	if ( (m_cfg.GetUseDataFiles() || m_cfg.IsEmbedded()) && track.HasDataFile() )
	{
		const wxDataFile& dataFile = track.GetDataFile();
		wxSamplingInfo si;
		wxULongLong frames;
		if ( dataFile.GetInfo( si, frames, m_cfg.GetAlternateExtensions() ) )
		{
			m_si = si;
		}
		else
		{
			m_si.SetDefault();
		}
	}

	m_pChapterAtom = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, wxT("ChapterAtom") );
	if ( m_pFirstChapterAtom == wxNullXmlNode )
	{
		m_pFirstChapterAtom = m_pChapterAtom;
	}

	wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString, wxString::Format( wxT("Track %02d"), track.GetNumber() ) );
	m_pChapterAtom->AddChild( pComment );

	m_nTotalParts += 1;

	return wxCueSheetRenderer::OnPreRenderTrack( cueSheet, track );
}

bool wxXmlCueSheetRenderer::OnRenderTrack( const wxCueSheet& cueSheet, const wxTrack& track )
{
	wxULongLong chapterUID( GenerateUID() );
	add_chapter_uid( m_pChapterAtom, chapterUID );
	AddTrackTags( track, chapterUID, m_pTags );
	return wxCueSheetRenderer::OnRenderTrack( cueSheet, track );
}

bool wxXmlCueSheetRenderer::OnPostRenderTrack( const wxCueSheet& cueSheet, const wxTrack& track )
{
	wxASSERT( m_pEditionEntry != wxNullXmlNode );
	m_pEditionEntry->AddChild( m_pChapterAtom );
	m_pPrevChapterAtom = m_pChapterAtom;
	return wxCueSheetRenderer::OnPostRenderTrack( cueSheet, track );
}

bool wxXmlCueSheetRenderer::OnRenderPreGap( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& preGap )
{
	wxLogInfo( _("Converting pre-gap of track %d"), track.GetNumber() );

	if ( track.GetNumber() == 1u )
	{
		if ( !m_cfg.TrackOneIndexOne() )
		{
			AddChapterTimeStart( m_pChapterAtom, preGap );
		}
	}
	else if ( m_cfg.GetChapterTimeEnd() )
	{
		if ( m_pPrevChapterAtom != wxNullXmlNode )
		{
			AddChapterTimeEnd( m_pPrevChapterAtom, preGap );
		}
	}

	return wxCueSheetRenderer::OnRenderPreGap( cueSheet, track, preGap );
}

bool wxXmlCueSheetRenderer::OnRenderPostGap( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& postGap )
{
	wxLogInfo( _("Converting post-gap of track %d"), track.GetNumber() );

	if ( m_cfg.GetChapterTimeEnd() )
	{
		AddChapterTimeEnd( m_pChapterAtom, postGap );
	}
	return wxCueSheetRenderer::OnRenderPostGap( cueSheet, track, postGap );
}

bool wxXmlCueSheetRenderer::OnRenderIndex( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& idx )
{
	wxLogInfo( _("Converting index %d of track %d"), idx.GetNumber(), track.GetNumber() );

	switch ( idx.GetNumber() )
	{
		case 0: // pre-gap
		if ( track.GetNumber() == 1u )
		{
			if ( !m_cfg.TrackOneIndexOne() )
			{
				AddChapterTimeStart( m_pChapterAtom, idx );
			}
		}
		else if ( m_cfg.GetChapterTimeEnd() )
		{
			if ( m_pPrevChapterAtom != wxNullXmlNode )
			{
				AddChapterTimeEnd( m_pPrevChapterAtom, idx );
			}
		}
		break;

		case 1: // start
		{
			if ( track.GetNumber() == 1u )
			{
				if ( m_cfg.TrackOneIndexOne() )
				{
					AddChapterTimeStart( m_pChapterAtom, idx );
				}
			}
			else
			{
				AddChapterTimeStart( m_pChapterAtom, idx );
			}
		}
		break;

		default:
		AddIdxChapterAtom( m_pChapterAtom, idx );
		break;
	}
	return wxCueSheetRenderer::OnRenderIndex( cueSheet, track, idx );
}

bool wxXmlCueSheetRenderer::OnPostRenderDisc( const wxCueSheet& cueSheet )
{
	wxLogInfo( _("Calculating chapter names and end time from data file(s)") );

	SetTotalParts( m_pTags );

	wxASSERT( m_pFirstChapterAtom != wxNullXmlNode );
	wxXmlNode* pChapterAtom = m_pFirstChapterAtom;

	const wxArrayTrack& tracks = cueSheet.GetTracks();
	size_t tracksCount = tracks.Count();
	wxULongLong offset( wxULL(0) );
	bool bOffsetValid = m_cfg.GetMerge() && (m_offset != wxSamplingInfo::wxInvalidNumberOfFrames);

	for( size_t i=0; i<tracksCount; i++ )
	{
		wxDataFile dataFile;
		bool bLastTrackForDataFile = cueSheet.IsLastTrackForDataFile( i, dataFile );

		if ( m_cfg.GetChapterTimeEnd() )
		{
			if ( !has_chapter_time_end( pChapterAtom ) )
			{
				
				if ( m_cfg.GetUseDataFiles() && bLastTrackForDataFile )
				{
					wxLogInfo( _("Calculating end time for track %d using media file \u201C%s\u201D"), tracks[i].GetNumber(), dataFile.GetFileName() );
					wxSamplingInfo si;
					wxULongLong frames;
					if ( dataFile.GetInfo( si, frames, m_cfg.GetAlternateExtensions() ) )
					{
						wxLogDebug( _("Number of frames - %s"), frames.ToString() );
						AddChapterTimeEnd( pChapterAtom, frames );
					}
				}
				else if ( m_cfg.GetUnknownChapterTimeEndToNextChapter() && ((i+1) < tracksCount) )
				{
					wxLogInfo( _("Calculating end time for track %d using offset (%d CD frames)"), tracks[i].GetNumber(), m_cfg.GetChapterOffset() );
					const wxTrack& nextTrack = tracks[i+1];
					AddChapterTimeEnd( pChapterAtom, nextTrack );
				}
			}

			wxString s( cueSheet.FormatTrack( i, m_cfg.GetTrackNameFormat() ) );
			add_chapter_display( pChapterAtom, s, m_cfg.GetLang() );
		}

		if ( bOffsetValid && bLastTrackForDataFile )
		{
			wxSamplingInfo si;
			wxULongLong frames;
			if ( dataFile.GetInfo( si, frames, m_cfg.GetAlternateExtensions() ) )
			{
				offset += frames;
			}
			else
			{
				//wxLogInfo( _("Cannot get number of samples for track %d"), tracks[i].GetNumber() );
				bOffsetValid = false;
			}
		}

		pChapterAtom = pChapterAtom->GetNext();
	}

	if ( bOffsetValid )
	{
		m_offset += offset;
		wxLogInfo( _("Offset - %s frames - %s"), m_offset.ToString(), m_si.GetSamplesStr( m_offset ) );
	}
	else
	{
		m_offset = wxSamplingInfo::wxInvalidNumberOfFrames;
	}

	wxLogInfo( _("Conversion done") );
	return wxCueSheetRenderer::OnPostRenderDisc( cueSheet );
}

bool wxXmlCueSheetRenderer::IsOffsetValid() const
{
	return (m_offset != wxSamplingInfo::wxInvalidNumberOfFrames);
}
