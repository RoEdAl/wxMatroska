/*
	wxXmlCueSheetRenderer.cpp
*/

#include "StdWx.h"
#include "wxConfiguration.h"
#include <wxTagSynonims.h>
#include <wxSamplingInfo.h>
#include <wxIndex.h>
#include <wxTrack.h>
#include <wxCueSheet.h>
#include "wxInputFile.h"
#include "wxXmlCueSheetRenderer.h"

wxIMPLEMENT_DYNAMIC_CLASS( wxXmlCueSheetRenderer, wxCueSheetRenderer )

// ===============================================================================

wxXmlCueSheetRenderer* const wxXmlCueSheetRenderer::Null = (wxXmlCueSheetRenderer* const)wxCueSheetRenderer::Null;

// ===============================================================================

const wxChar* const wxXmlCueSheetRenderer::Tag::ORIGINAL_MEDIA_TYPE = wxT("ORIGINAL_MEDIA_TYPE");
const wxChar* const wxXmlCueSheetRenderer::Tag::CATALOG_NUMBER = wxT("CATALOG_NUMBER");
const wxChar* const wxXmlCueSheetRenderer::Tag::TOTAL_PARTS = wxT("TOTAL_PARTS");
const wxChar* const wxXmlCueSheetRenderer::Tag::PART_NUMBER = wxT("PART_NUMBER");

// ===============================================================================

const wxChar* const wxXmlCueSheetRenderer::Xml::CHAPTER_UID = wxT("ChapterUID");
const wxChar* const wxXmlCueSheetRenderer::Xml::EDITION_ENTRY = wxT("EditionEntry");
const wxChar* const wxXmlCueSheetRenderer::Xml::EDITION_UID = wxT("EditionUID");
const wxChar* const wxXmlCueSheetRenderer::Xml::CHAPTER_TIME_START = wxT("ChapterTimeStart");
const wxChar* const wxXmlCueSheetRenderer::Xml::CHAPTER_TIME_END = wxT("ChapterTimeEnd");
const wxChar* const wxXmlCueSheetRenderer::Xml::CHAPTER_DISPLAY = wxT("ChapterDisplay");
const wxChar* const wxXmlCueSheetRenderer::Xml::CHAPTER_STRING = wxT("ChapterString");
const wxChar* const wxXmlCueSheetRenderer::Xml::CHAPTER_LANGUAGE = wxT("ChapterLanguage");
const wxChar* const wxXmlCueSheetRenderer::Xml::CHAPTER_FLAG_HIDDEN = wxT("ChapterFlagHidden");
const wxChar* const wxXmlCueSheetRenderer::Xml::CHAPTER_ATOM = wxT("ChapterAtom");
const wxChar* const wxXmlCueSheetRenderer::Xml::CHAPTERS = wxT("Chapters");
const wxChar* const wxXmlCueSheetRenderer::Xml::TARGETS = wxT("Targets");
const wxChar* const wxXmlCueSheetRenderer::Xml::TAGS = wxT("Tags");
const wxChar* const wxXmlCueSheetRenderer::Xml::TARGET_TYPE_VALUE = wxT("TargetTypeValue");
const wxChar* const wxXmlCueSheetRenderer::Xml::TARGET_TYPE = wxT("TargetType");
const wxChar* const wxXmlCueSheetRenderer::Xml::TAG = wxT("Tag");
const wxChar* const wxXmlCueSheetRenderer::Xml::TAG_LANGUAGE = wxT("TagLanguage");
const wxChar* const wxXmlCueSheetRenderer::Xml::NAME = wxT("Name");
const wxChar* const wxXmlCueSheetRenderer::Xml::STRING = wxT("String");
const wxChar* const wxXmlCueSheetRenderer::Xml::SIMPLE = wxT("Simple");

// ===============================================================================

const wxChar* const wxXmlCueSheetRenderer::XmlValue::ALBUM = wxT("ALBUM");
const wxChar* const wxXmlCueSheetRenderer::XmlValue::TRACK = wxT("TRACK");

// ===============================================================================

wxXmlNode* wxXmlCueSheetRenderer::create_comment_node( const wxString& sComment )
{
	wxXmlNode* pComment = new wxXmlNode( wxNullXmlNode, wxXML_COMMENT_NODE, wxEmptyString, sComment );
	return pComment;
}

void wxXmlCueSheetRenderer::add_comment_node( wxXmlNode* pNode, const wxString& sComment )
{
	wxASSERT( pNode != wxNullXmlNode );
	pNode->AddChild( create_comment_node( sComment ) );
}

wxXmlNode* wxXmlCueSheetRenderer::get_last_child( wxXmlNode* pNode )
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

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_uid( wxXmlNode* pChapterAtom, const wxULongLong& uid )
{
	wxXmlNode* pChapterUID = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_UID );
	wxXmlNode* pChapterUIDText = new wxXmlNode( pChapterUID, wxXML_TEXT_NODE, wxEmptyString, uid.ToString() );

	pChapterAtom->AddChild( pChapterUID );
	return pChapterUID;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_start( wxXmlNode* pChapterAtom, const wxString& s )
{
	wxXmlNode* pChapterTimeStart = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_TIME_START );
	wxXmlNode* pChapterTimeStartText = new wxXmlNode( pChapterTimeStart, wxXML_TEXT_NODE, wxEmptyString, s );

	pChapterAtom->AddChild( pChapterTimeStart );
	return pChapterTimeStart;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_start( wxXmlNode* pChapterAtom, const wxSamplingInfo& si, wxULongLong frames )
{
	return add_chapter_time_start( pChapterAtom, si.GetSamplesStr( frames ) );
}

wxXmlNode* wxXmlCueSheetRenderer::find_chapter_time_start( wxXmlNode* pChapterAtom )
{
	wxXmlNode* pChild = pChapterAtom->GetChildren();
	wxXmlNode* pPrev = wxNullXmlNode;
	while( pChild != wxNullXmlNode )
	{
		if ( pChild->GetName().CmpNoCase( Xml::CHAPTER_TIME_START ) == 0 )
		{
			return pChild;
		}
		pPrev = pChild;
		pChild = pChild->GetNext();
	}

	wxASSERT( pPrev != wxNullXmlNode );
	return pPrev;
}

bool wxXmlCueSheetRenderer::is_album_tag( wxXmlNode* pTag, long nTargetTypeValue )
{
	wxXmlNode* pChild = pTag->GetChildren();
	while( pChild != wxNullXmlNode  )
	{
		if ( pChild->GetType() == wxXML_ELEMENT_NODE ) break;
		pChild = pChild->GetNext();
	}
	if ( pChild == wxNullXmlNode ) return false;
	if ( pChild->GetName().CmpNoCase( Xml::TARGETS ) != 0 )
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

wxXmlNode* wxXmlCueSheetRenderer::find_disc_tag_node( wxXmlNode* pTags, long nTargetTypeValue )
{
	wxXmlNode* pChild = pTags->GetChildren();
	while( pChild != wxNullXmlNode )
	{
		if ( (pChild->GetType() == wxXML_ELEMENT_NODE) && (pChild->GetName().CmpNoCase( Xml::TAG ) == 0) )
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

bool wxXmlCueSheetRenderer::is_total_parts( wxXmlNode* pSimple )
{
	wxXmlNode* pChild = pSimple->GetChildren();
	while( pChild != wxNullXmlNode )
	{
		if ( (pChild->GetType() == wxXML_ELEMENT_NODE) &&
			 (pChild->GetName().CmpNoCase( Xml::NAME ) == 0) )
		{
			wxXmlNode* pText = pChild->GetChildren();
			if ( ( pText != wxNullXmlNode ) &&
				( pText->GetContent().CmpNoCase( Tag::TOTAL_PARTS ) == 0 ) )
			{
				return true;
			}
		}
		pChild = pChild->GetNext();
	}

	return false;
}

bool wxXmlCueSheetRenderer::set_total_parts( wxXmlNode* pSimple, size_t nTotalParts )
{
	wxXmlNode* pChild = pSimple->GetChildren();
	while( pChild != wxNullXmlNode )
	{
		if ( (pChild->GetType() == wxXML_ELEMENT_NODE) &&
			 (pChild->GetName().CmpNoCase( Xml::STRING ) == 0) )
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

wxXmlNode* wxXmlCueSheetRenderer::find_total_parts_node( wxXmlNode* pTag )
{
	wxXmlNode* pChild = pTag->GetChildren();
	while( pChild != wxNullXmlNode )
	{
		if ( (pChild->GetType() == wxXML_ELEMENT_NODE) && (pChild->GetName().CmpNoCase( Xml::SIMPLE ) == 0) )
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

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_end( wxXmlNode* pChapterAtom, const wxString& text )
{
	wxXmlNode* pChapterTimeStart = find_chapter_time_start( pChapterAtom );

	wxXmlNode* pChapterTimeEnd = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_TIME_END );
	wxXmlNode* pChapterTimeEndText = new wxXmlNode( pChapterTimeEnd, wxXML_TEXT_NODE, wxEmptyString, text );

	pChapterAtom->InsertChildAfter( pChapterTimeEnd, pChapterTimeStart );

	return pChapterTimeEnd;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_end( wxXmlNode* pChapterAtom, const wxSamplingInfo& si, wxULongLong frames )
{
	return add_chapter_time_end( pChapterAtom, si.GetSamplesStr( frames ) );
}

bool wxXmlCueSheetRenderer::has_chapter_time_end( wxXmlNode* pChapterAtom )
{
	wxXmlNode* pChild = pChapterAtom->GetChildren();
	while( pChild != wxNullXmlNode )
	{
		if ( pChild->GetName().CmpNoCase( Xml::CHAPTER_TIME_END ) == 0 )
		{
			return true;
		}
		pChild = pChild->GetNext();
	}
	return false;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_display( wxXmlNode* pChapterAtom, const wxString& sChapterString, const wxString& sLang )
{
	wxXmlNode* pChapterDisplay = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_DISPLAY );

	wxXmlNode* pChapterString = new wxXmlNode( pChapterDisplay, wxXML_ELEMENT_NODE, Xml::CHAPTER_STRING );
	wxXmlNode* pChapterStringText = new wxXmlNode( pChapterString, wxXML_TEXT_NODE, wxEmptyString, sChapterString );

	wxXmlNode* pChapterLanguage = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_LANGUAGE );
	wxXmlNode* pChapterLanguageText = new wxXmlNode( pChapterLanguage, wxXML_TEXT_NODE, wxEmptyString, sLang );
	pChapterDisplay->AddChild( pChapterLanguage );

	pChapterAtom->AddChild( pChapterDisplay );
	return pChapterDisplay;
}

wxXmlNode* wxXmlCueSheetRenderer::add_hidden_flag( wxXmlNode* pChapterAtom, bool bHiddenFlag )
{
	wxXmlNode* pChapterHidden = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_FLAG_HIDDEN );
	wxXmlNode* pChapterHiddenText = new wxXmlNode( pChapterHidden, wxXML_TEXT_NODE, wxEmptyString, 
		 bHiddenFlag? wxT("1") : wxT("0") );
	pChapterAtom->AddChild( pChapterHidden );
	return pChapterHidden;
}

wxXmlNode* wxXmlCueSheetRenderer::add_idx_chapter_atom( 
	wxXmlNode* pChapterAtom,
	const wxSamplingInfo& si,
	const wxULongLong& frames,
	unsigned int nIndexNumber,
	const wxString& sLang,
	bool bHiddenIndexes
)
{
	wxXmlNode* pIdxChapterAtom = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_ATOM );
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

wxXmlCueSheetRenderer::wxXmlCueSheetRenderer()
:
    m_pXmlDoc(wxNullXmlDocument),
	m_pXmlTags(wxNullXmlDocument),
	m_pCfg((wxConfiguration*)NULL),
	m_offset( wxULL(0) ),
	m_nTotalParts(0)
{
	init_synonims();
}

wxXmlCueSheetRenderer* wxXmlCueSheetRenderer::CreateObject( const wxConfiguration& cfg, const wxInputFile& inputFile )
{
	wxClassInfo* ci = wxCLASSINFO(wxXmlCueSheetRenderer);
	wxXmlCueSheetRenderer* pRenderer = wxStaticCast( ci->CreateObject(), wxXmlCueSheetRenderer );
	pRenderer->SetConfiguration( cfg );
	pRenderer->SetInputFile( inputFile );
	return pRenderer;
}

void wxXmlCueSheetRenderer::init_synonims()
{
	wxArrayString as;

	// DISC: TITLE = (TITLE,ALBUM)
	as.Add( wxCueTag::Name::ALBUM );
	wxTagSynonims discSynonim1( wxCueTag::Name::TITLE, as );
	m_discCdTextSynonims.Add( discSynonim1 );

	// DISC: TITLE = (TITLE,ALBUM)
	m_discSynonims.Add( discSynonim1 );

	as.Clear();
	// DISC: ARTIST = (ARTIST,PERFORMER)
	as.Add( wxCueTag::Name::PERFORMER );
	wxTagSynonims discSynonim2( wxCueTag::Name::ARTIST, as );
	m_discCdTextSynonims.Add( discSynonim2 );
	m_trackCdTextSynonims.Add( discSynonim2 );

	as.Clear();
	// DISC: ARTIST = (ARTIST,ALBUM ARTIST)
	as.Add( wxCueTag::Name::ALBUM_ARTIST );
	wxTagSynonims discSynonim3( wxCueTag::Name::ARTIST, as );
	m_discSynonims.Add( discSynonim3 );
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

void wxXmlCueSheetRenderer::SetConfiguration( const wxConfiguration& cfg )
{
	wxASSERT( m_pCfg == (wxConfiguration*)NULL );
	m_pCfg = &cfg;
}

void wxXmlCueSheetRenderer::SetInputFile( const wxInputFile& inputFile )
{
	wxASSERT( m_pCfg != (wxConfiguration*)NULL );

	m_inputFile = inputFile;
	m_pCfg->GetOutputFile( inputFile, m_sOutputFile, m_sTagsFile );
}

const wxConfiguration& wxXmlCueSheetRenderer::GetConfig() const
{
	wxASSERT( m_pCfg != (wxConfiguration*)NULL );
	return *m_pCfg;
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

	if ( GetConfig().GenerateTags() )
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
	return add_chapter_time_end( pChapterAtom, m_si, tframes );
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeEnd( wxXmlNode* pChapterAtom, const wxIndex& idx ) const
{
	return AddChapterTimeEnd( pChapterAtom, m_si.GetIndexOffset( idx ) );
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeEnd( wxXmlNode* pChapterAtom, const wxTrack& track ) const
{
	wxIndex fidx( m_si.ConvertIndex( track.GetFirstIndex() ) );
	fidx -= m_si.GetFramesFromCdFrames( GetConfig().GetChapterOffset() );
	return AddChapterTimeEnd( pChapterAtom, fidx );
}

wxXmlNode* wxXmlCueSheetRenderer::AddIdxChapterAtom( wxXmlNode* pChapterAtom, const wxIndex& idx ) const
{
	wxULongLong frames( m_offset );
	frames += m_si.GetIndexOffset( idx );
	return add_idx_chapter_atom( pChapterAtom, m_si, frames, idx.GetNumber(), GetConfig().GetLang(), GetConfig().HiddenIndexes() );
}

wxXmlDocument* wxXmlCueSheetRenderer::create_xml_document( const wxString& sRootNode )
{
	wxXmlDocument* pXmlDoc = new wxXmlDocument();
	pXmlDoc->SetVersion( wxT("1.0") );
	pXmlDoc->SetFileEncoding( wxT("utf-8") );

	wxXmlNode* pRoot = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, sRootNode );
	pXmlDoc->SetRoot( pRoot );

	return pXmlDoc;
}

bool wxXmlCueSheetRenderer::is_multiline( const wxString& s )
{
	wxStringInputStream is( s );
	wxTextInputStream tis( is, wxT(" \t"), wxConvUTF8 );
	int nLines = 0;
	while ( !( is.Eof() || (nLines>1) ) )
	{
		tis.ReadLine();
		nLines += 1;
	}
	return (nLines>1);
}

wxXmlNode* wxXmlCueSheetRenderer::create_simple_tag( const wxCueTag& tag, const wxString& sLanguage )
{
	wxXmlNode* pSimple = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::SIMPLE );

	wxXmlNode* pName = new wxXmlNode( pSimple, wxXML_ELEMENT_NODE, Xml::NAME );
	wxXmlNode* pNameText = new wxXmlNode( pName, wxXML_TEXT_NODE, wxEmptyString, tag.GetName() );

	if ( tag.GetSource() != wxCueTag::TAG_AUTO_GENERATED )
	{
		add_comment_node( pSimple, wxString::Format( wxT("Source: %s"), tag.GetSourceAsString() ) );
	}

	wxXmlNode* pValue = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::STRING );
	wxXmlNode* pValueText = new wxXmlNode( pValue, 
		tag.IsMultiline()? wxXML_CDATA_SECTION_NODE : wxXML_TEXT_NODE,
		wxEmptyString, tag.GetValue() );

	pSimple->AddChild( pValue );

	wxXmlNode* pLang = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TAG_LANGUAGE );
	wxXmlNode* pLangText = new wxXmlNode( pLang, wxXML_TEXT_NODE, wxEmptyString, sLanguage );

	pSimple->AddChild( pLang );

	return pSimple;
}

bool wxXmlCueSheetRenderer::is_simple( wxXmlNode* pNode, const wxCueTag& tag )
{
	wxASSERT( pNode != wxNullXmlNode );
	wxXmlNode* pChild = pNode->GetChildren();
	bool bName = false;
	bool bValue = false;

	while( pChild != wxNullXmlNode )
	{
		if (pChild->GetType() == wxXML_ELEMENT_NODE)
		{
			if ( pChild->GetName().CmpNoCase( Xml::NAME ) == 0 )
			{
				wxXmlNode* pText = pChild->GetChildren();
				if ( pText == wxNullXmlNode ) return false;
				if ( pText->GetType() != wxXML_TEXT_NODE ) return false;
				bName = ( pText->GetContent().CmpNoCase( tag.GetName() ) == 0 );			
			}

			if ( pChild->GetName().CmpNoCase( Xml::STRING ) == 0 )
			{
				wxXmlNode* pText = pChild->GetChildren();
				if ( pText == wxNullXmlNode ) return false;
				if ( !(( pText->GetType() == wxXML_TEXT_NODE ) || ( pText->GetType() == wxXML_CDATA_SECTION_NODE )) ) return false;
				bValue = ( pText->GetContent().Cmp( tag.GetValue() ) == 0 );			
			}
		}

		pChild = pChild->GetNext();
	}

	return (bName && bValue);
}

wxXmlNode* wxXmlCueSheetRenderer::find_simple_tag( wxXmlNode* pNode, const wxCueTag& tag )
{
	wxASSERT( pNode != wxNullXmlNode );
	wxXmlNode* pChild = pNode->GetChildren();
	while( pChild != wxNullXmlNode )
	{
		if ( is_simple( pChild, tag ) )
		{
			return pChild;
		}

		pChild = pChild->GetNext();
	}

	return wxNullXmlNode;
}

wxXmlNode* wxXmlCueSheetRenderer::add_simple_tag( wxXmlNode* pNode, const wxString& sName, const wxString& sValue, const wxString& sLanguage )
{
	wxCueTag tag( wxCueTag::TAG_AUTO_GENERATED, sName, sValue );
	wxXmlNode* pTagNode = create_simple_tag( tag, sLanguage );
	pNode->AddChild( pTagNode );
	return pTagNode;
}

wxXmlNode* wxXmlCueSheetRenderer::add_simple_tag( wxXmlNode* pNode, const wxCueTag& tag, const wxString& sLanguage )
{
	wxASSERT( pNode != wxNullXmlNode );
	wxXmlNode* pTagNode = find_simple_tag( pNode, tag );
	if ( pTagNode == wxNullXmlNode )
	{
		pTagNode = create_simple_tag( tag, sLanguage );
		pNode->AddChild( pTagNode );
	}
	return pTagNode;
}

void wxXmlCueSheetRenderer::AddTags(
	const wxCueComponent& component,
	const wxTagSynonimsCollection& cdTextSynonims,
	const wxTagSynonimsCollection& synonims,
	wxXmlNode* pTag )
{
	wxArrayCueTag mappedTags;
	wxArrayCueTag rest;

	component.GetTags( cdTextSynonims, synonims, mappedTags, rest );

	size_t numTags = mappedTags.Count();
	for( size_t i = 0; i < numTags; i++ )
	{
		if ( GetConfig().ShouldIgnoreTag( mappedTags[i] ) ) continue;
		wxXmlNode* pSimple = add_simple_tag( pTag, mappedTags[i], GetConfig().GetLang() );
	}

	numTags = rest.Count();
	for( size_t i = 0; i < numTags; i++ )
	{
		if ( GetConfig().ShouldIgnoreTag( rest[i] ) ) continue;
		wxXmlNode* pSimple = add_simple_tag( pTag, rest[i], GetConfig().GetLang() );
	}
}

void wxXmlCueSheetRenderer::AddCdTextInfo( const wxCueComponent& component, wxXmlNode* pTag )
{
	if ( component.IsTrack() )
	{
		AddTags( component, m_trackCdTextSynonims, m_trackSynonims, pTag );
	}
	else
	{
		AddTags( component, m_discCdTextSynonims, m_discSynonims, pTag );
	}
}

wxXmlNode* wxXmlCueSheetRenderer::AddDiscTags(
	const wxCueSheet& cueSheet,
	wxXmlNode* pTags,
	const wxULongLong& editionUID,
	int nTargetTypeValue )
{
	wxXmlNode* pTag = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TAG );
	pTags->AddChild( pTag );

	add_comment_node( pTag, wxT("Disc tag") );

	wxXmlNode* pTargets = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TARGETS );
	pTag->AddChild( pTargets );

	wxXmlNode* pTargetTypeValue = new wxXmlNode( pTargets, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE_VALUE );	
	wxXmlNode* pTargetTypeValueText = new wxXmlNode( pTargetTypeValue, wxXML_TEXT_NODE, wxEmptyString, wxString::Format( wxT("%d"), nTargetTypeValue ) );

	wxXmlNode* pTargetType = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE );	
	wxXmlNode* pTargetTypeText = new wxXmlNode( pTargetType, wxXML_TEXT_NODE, wxEmptyString, XmlValue::ALBUM );
	pTargets->AddChild( pTargetType );

	if ( GetConfig().GenerateEditionUID() )
	{
		wxXmlNode* pEditionUID = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::EDITION_UID );	
		wxXmlNode* pEditionUIDText = new wxXmlNode( pEditionUID, wxXML_TEXT_NODE, wxEmptyString, editionUID.ToString() );
		pTargets->AddChild( pEditionUID );
	}

	wxXmlNode* pSimple = add_simple_tag( 
		pTag,
		Tag::ORIGINAL_MEDIA_TYPE,
		wxT("CD"),
		GetConfig().GetLang() );

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

	add_comment_node( pTag, wxString::Format( wxT("CUE file: \u201C%s\u201D"), m_inputFile.ToString(false) ) );

	AddCdTextInfo( cueSheet, pTag );

	if ( !cueSheet.GetCatalog().IsEmpty() )
	{
		wxXmlNode* pSimple = add_simple_tag( 
			pTag,
			Tag::CATALOG_NUMBER,
			cueSheet.GetCatalog(),
			GetConfig().GetLang() );
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
		add_comment_node( pTag, wxT("Total number of tracks") );

		wxXmlNode* pSimple = add_simple_tag( 
			pTag,
			Tag::TOTAL_PARTS,
			wxString::Format( wxT("%d"), m_nTotalParts ),
			GetConfig().GetLang() );
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
	wxXmlNode* pTag = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TAG );
	pTags->AddChild( pTag );

	add_comment_node( pTag, wxString::Format( wxT("Track %02d"), track.GetNumber() ) );

	wxXmlNode* pTargets = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TARGETS );	
	pTag->AddChild( pTargets );

	wxXmlNode* pTargetTypeValue = new wxXmlNode( pTargets, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE_VALUE );	
	wxXmlNode* pTargetTypeValueText = new wxXmlNode( pTargetTypeValue, wxXML_TEXT_NODE, wxEmptyString, wxString::Format( wxT("%d"), nTargetTypeValue ) );

	wxXmlNode* pTargetType = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE );	
	wxXmlNode* pTargetTypeText = new wxXmlNode( pTargetType, wxXML_TEXT_NODE, wxEmptyString, XmlValue::TRACK );
	pTargets->AddChild( pTargetType );

	wxXmlNode* pChapterUID = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_UID );	
	wxXmlNode* pChapterUIDText = new wxXmlNode( pChapterUID, wxXML_TEXT_NODE, wxEmptyString, chapterUID.ToString() );
	pTargets->AddChild( pChapterUID );

	wxXmlNode* pSimple = add_simple_tag( 
			pTag,
			Tag::PART_NUMBER,
			wxString::Format( wxT("%d"), m_nTotalParts ),
			GetConfig().GetLang() );

	AddCdTextInfo( track, pTag );
	return pTag;
}

bool wxXmlCueSheetRenderer::OnPreRenderDisc( const wxCueSheet& cueSheet )
{
	if ( !GetConfig().GetMerge() )
	{
		wxASSERT( m_pXmlDoc == wxNullXmlDocument );
		wxASSERT( m_pXmlTags == wxNullXmlDocument );
	}

	m_pFirstChapterAtom = wxNullXmlNode;
	wxULongLong editionUID( GenerateUID() );

	if ( m_pXmlDoc == wxNullXmlDocument )
	{
		wxLogInfo( _("Creating XML document") );
		m_pXmlDoc = create_xml_document( Xml::CHAPTERS );
		wxXmlNode* pChapters = m_pXmlDoc->GetRoot();
		GetConfig().BuildXmlComments( m_sOutputFile, pChapters );
		m_pEditionEntry = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::EDITION_ENTRY );
		pChapters->AddChild( m_pEditionEntry );
		if ( GetConfig().GenerateEditionUID() )
		{
			wxXmlNode* pEditionUID = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::EDITION_UID );
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
	add_comment_node( m_pEditionEntry, wxString::Format( wxT("CUE file: \u201C%s\u201D"), m_inputFile.ToString(false) ) );

	if ( m_pXmlTags == wxNullXmlDocument )
	{
		m_pXmlTags = create_xml_document( Xml::TAGS );
		m_pTags = m_pXmlTags->GetRoot();
		GetConfig().BuildXmlComments( m_sTagsFile, m_pTags );
		AddDiscTags( cueSheet, m_pTags, editionUID );
	}
	else
	{
		AppendDiscTags( cueSheet, m_pTags );
	}

	add_comment_node( m_pTags, wxString::Format( wxT("CUE file: \u201C%s\u201D"), m_inputFile.ToString(false) ) );
	add_comment_node( m_pTags, wxString::Format( wxT("Number of tracks: %d"), cueSheet.GetTracks().Count() ) );

	return wxCueSheetRenderer::OnPreRenderDisc( cueSheet );
}

bool wxXmlCueSheetRenderer::OnPreRenderTrack( const wxCueSheet& cueSheet, const wxTrack& track )
{
	wxASSERT( m_pEditionEntry != wxNullXmlNode );
	wxLogInfo( _("Converting track %d"), track.GetNumber() );
	
	if ( (GetConfig().GetUseDataFiles() || GetConfig().IsEmbedded()) && track.HasDataFile() )
	{
		const wxDataFile& dataFile = track.GetDataFile();
		wxSamplingInfo si;
		wxULongLong frames;
		if ( dataFile.GetInfo( si, frames, GetConfig().GetAlternateExtensions() ) )
		{
			m_si = si;
		}
		else
		{
			m_si.SetDefault();
		}
	}

	m_pChapterAtom = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_ATOM );
	if ( m_pFirstChapterAtom == wxNullXmlNode )
	{
		m_pFirstChapterAtom = m_pChapterAtom;
	}

	add_comment_node( m_pChapterAtom, wxString::Format( wxT("Track %02d"), track.GetNumber() ) );
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
		if ( !GetConfig().TrackOneIndexOne() )
		{
			AddChapterTimeStart( m_pChapterAtom, preGap );
		}
	}
	else if ( GetConfig().GetChapterTimeEnd() )
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

	if ( GetConfig().GetChapterTimeEnd() )
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
			if ( !GetConfig().TrackOneIndexOne() )
			{
				AddChapterTimeStart( m_pChapterAtom, idx );
			}
		}
		else if ( GetConfig().GetChapterTimeEnd() )
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
				if ( GetConfig().TrackOneIndexOne() )
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
	bool bOffsetValid = GetConfig().GetMerge() && (m_offset != wxSamplingInfo::wxInvalidNumberOfFrames);

	for( size_t i=0; i<tracksCount; i++ )
	{
		wxDataFile dataFile;
		bool bLastTrackForDataFile = cueSheet.IsLastTrackForDataFile( i, dataFile );

		if ( GetConfig().GetChapterTimeEnd() )
		{
			if ( !has_chapter_time_end( pChapterAtom ) )
			{
				
				if ( GetConfig().GetUseDataFiles() && bLastTrackForDataFile )
				{
					wxLogInfo( _("Calculating end time for track %d using media file \u201C%s\u201D"), tracks[i].GetNumber(), dataFile.GetFileName() );
					wxSamplingInfo si;
					wxULongLong frames;
					if ( dataFile.GetInfo( si, frames, GetConfig().GetAlternateExtensions() ) )
					{
						wxLogDebug( _("Number of frames - %s"), frames.ToString() );
						AddChapterTimeEnd( pChapterAtom, frames );
					}
				}
				else if ( GetConfig().GetUnknownChapterTimeEndToNextChapter() && ((i+1) < tracksCount) )
				{
					wxLogInfo( _("Calculating end time for track %d using offset (%d CD frames)"), tracks[i].GetNumber(), GetConfig().GetChapterOffset() );
					const wxTrack& nextTrack = tracks[i+1];
					AddChapterTimeEnd( pChapterAtom, nextTrack );
				}
			}

			wxString s( cueSheet.FormatTrack( i, GetConfig().GetTrackNameFormat() ) );
			add_chapter_display( pChapterAtom, s, GetConfig().GetLang() );
		}

		if ( bOffsetValid && bLastTrackForDataFile )
		{
			wxSamplingInfo si;
			wxULongLong frames;
			if ( dataFile.GetInfo( si, frames, GetConfig().GetAlternateExtensions() ) )
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
		wxLogInfo( _("Offset: %s frames: %s"), m_offset.ToString(), m_si.GetSamplesStr( m_offset ) );
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
