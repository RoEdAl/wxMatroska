/*
 * wxXmlCueSheetRenderer.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxTagSynonims.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxDuration.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include "wxConfiguration.h"
#include "wxInputFile.h"
#include "wxXmlCueSheetRenderer.h"
#include "wxApp.h"

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxXmlCueSheetRenderer, wxCueSheetRenderer );

// ===============================================================================

wxXmlCueSheetRenderer* const wxXmlCueSheetRenderer::Null = (wxXmlCueSheetRenderer* const)wxCueSheetRenderer::Null;

// ===============================================================================

const wxChar wxXmlCueSheetRenderer::Tag::ORIGINAL_MEDIA_TYPE[] = wxS( "ORIGINAL_MEDIA_TYPE" );
const wxChar wxXmlCueSheetRenderer::Tag::CATALOG_NUMBER[]	   = wxS( "CATALOG_NUMBER" );
const wxChar wxXmlCueSheetRenderer::Tag::TOTAL_PARTS[]		   = wxS( "TOTAL_PARTS" );
const wxChar wxXmlCueSheetRenderer::Tag::PART_NUMBER[]		   = wxS( "PART_NUMBER" );

// ===============================================================================

const wxChar wxXmlCueSheetRenderer::Xml::CHAPTER_UID[]		   = wxS( "ChapterUID" );
const wxChar wxXmlCueSheetRenderer::Xml::EDITION_ENTRY[]	   = wxS( "EditionEntry" );
const wxChar wxXmlCueSheetRenderer::Xml::EDITION_UID[]		   = wxS( "EditionUID" );
const wxChar wxXmlCueSheetRenderer::Xml::CHAPTER_TIME_START[]  = wxS( "ChapterTimeStart" );
const wxChar wxXmlCueSheetRenderer::Xml::CHAPTER_TIME_END[]	   = wxS( "ChapterTimeEnd" );
const wxChar wxXmlCueSheetRenderer::Xml::CHAPTER_DISPLAY[]	   = wxS( "ChapterDisplay" );
const wxChar wxXmlCueSheetRenderer::Xml::CHAPTER_STRING[]	   = wxS( "ChapterString" );
const wxChar wxXmlCueSheetRenderer::Xml::CHAPTER_LANGUAGE[]	   = wxS( "ChapterLanguage" );
const wxChar wxXmlCueSheetRenderer::Xml::CHAPTER_FLAG_HIDDEN[] = wxS( "ChapterFlagHidden" );
const wxChar wxXmlCueSheetRenderer::Xml::CHAPTER_ATOM[]		   = wxS( "ChapterAtom" );
const wxChar wxXmlCueSheetRenderer::Xml::CHAPTERS[]			   = wxS( "Chapters" );
const wxChar wxXmlCueSheetRenderer::Xml::TARGETS[]			   = wxS( "Targets" );
const wxChar wxXmlCueSheetRenderer::Xml::TAGS[]				   = wxS( "Tags" );
const wxChar wxXmlCueSheetRenderer::Xml::TARGET_TYPE_VALUE[]   = wxS( "TargetTypeValue" );
const wxChar wxXmlCueSheetRenderer::Xml::TARGET_TYPE[]		   = wxS( "TargetType" );
const wxChar wxXmlCueSheetRenderer::Xml::TAG[]				   = wxS( "Tag" );
const wxChar wxXmlCueSheetRenderer::Xml::TAG_LANGUAGE[]		   = wxS( "TagLanguage" );
const wxChar wxXmlCueSheetRenderer::Xml::NAME[]				   = wxS( "Name" );
const wxChar wxXmlCueSheetRenderer::Xml::STRING[]			   = wxS( "String" );
const wxChar wxXmlCueSheetRenderer::Xml::SIMPLE[]			   = wxS( "Simple" );

// ===============================================================================

const wxChar wxXmlCueSheetRenderer::XmlValue::ALBUM[] = wxS( "ALBUM" );
const wxChar wxXmlCueSheetRenderer::XmlValue::TRACK[] = wxS( "TRACK" );

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
	wxXmlNode* pPrev  = wxNullXmlNode;

	while ( pChild != wxNullXmlNode )
	{
		pPrev  = pChild;
		pChild = pChild->GetNext();
	}

	return pPrev;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_uid( wxXmlNode* pChapterAtom, const wxULongLong& uid )
{
	wxXmlNode* pChapterUID	   = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_UID );
	wxXmlNode* pChapterUIDText = new wxXmlNode( pChapterUID, wxXML_TEXT_NODE, wxEmptyString, uid.ToString() );

	pChapterAtom->AddChild( pChapterUID );
	return pChapterUID;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_start( wxXmlNode* pChapterAtom, const wxString& s, const wxString& sComment )
{
	wxXmlNode* pChapterTimeStart	 = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_TIME_START );
	wxXmlNode* pChapterTimeStartText = new wxXmlNode( pChapterTimeStart, wxXML_TEXT_NODE, wxEmptyString, s );
	wxXmlNode* pCommentNode			 = create_comment_node( sComment );

	pChapterAtom->AddChild( pCommentNode );
	pChapterAtom->AddChild( pChapterTimeStart );

	return pChapterTimeStart;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_start( wxXmlNode* pChapterAtom, const wxDuration& duration, const wxString& sComment )
{
	return add_chapter_time_start( pChapterAtom, duration.GetSamplesStr(), sComment );
}

wxXmlNode* wxXmlCueSheetRenderer::find_chapter_time_start( wxXmlNode* pChapterAtom )
{
	wxXmlNode* pChild = pChapterAtom->GetChildren();
	wxXmlNode* pPrev  = wxNullXmlNode;

	while ( pChild != wxNullXmlNode )
	{
		if ( pChild->GetName().CmpNoCase( Xml::CHAPTER_TIME_START ) == 0 )
		{
			return pChild;
		}

		pPrev  = pChild;
		pChild = pChild->GetNext();
	}

	wxASSERT( pPrev != wxNullXmlNode );
	return pPrev;
}

bool wxXmlCueSheetRenderer::is_album_tag( wxXmlNode* pTag, long nTargetTypeValue )
{
	wxXmlNode* pChild = pTag->GetChildren();

	while ( pChild != wxNullXmlNode )
	{
		if ( pChild->GetType() == wxXML_ELEMENT_NODE )
		{
			break;
		}

		pChild = pChild->GetNext();
	}

	if ( pChild == wxNullXmlNode )
	{
		return false;
	}

	if ( pChild->GetName().CmpNoCase( Xml::TARGETS ) != 0 )
	{
		return false;
	}

	wxXmlNode* pTargetTypeValue = pChild->GetChildren();

	if ( pTargetTypeValue == wxNullXmlNode )
	{
		return false;
	}

	wxXmlNode* pText = pTargetTypeValue->GetChildren();

	if ( pText == wxNullXmlNode )
	{
		return false;
	}

	long nParsedTargetTypeValue;

	if ( !pText->GetContent().ToCLong( &nParsedTargetTypeValue ) )
	{
		return false;
	}

	return ( nParsedTargetTypeValue == nTargetTypeValue );
}

wxXmlNode* wxXmlCueSheetRenderer::find_disc_tag_node( wxXmlNode* pTags, long nTargetTypeValue )
{
	wxXmlNode* pChild = pTags->GetChildren();

	while ( pChild != wxNullXmlNode )
	{
		if ( ( pChild->GetType() == wxXML_ELEMENT_NODE ) && ( pChild->GetName().CmpNoCase( Xml::TAG ) == 0 ) )
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

bool wxXmlCueSheetRenderer::set_total_parts( wxXmlNode* pSimple, size_t nTotalParts )
{
	wxXmlNode* pChild = pSimple->GetChildren();

	while ( pChild != wxNullXmlNode )
	{
		if ( ( pChild->GetType() == wxXML_ELEMENT_NODE ) && ( pChild->GetName().CmpNoCase( Xml::STRING ) == 0 ) )
		{
			wxXmlNode* pText = pChild->GetChildren();
			wxASSERT( pText != wxNullXmlNode );
			pText->SetContent( wxString::Format( wxS( "%u" ), nTotalParts ) );
			return true;
		}

		pChild = pChild->GetNext();
	}

	return false;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_end( wxXmlNode* pChapterAtom, const wxString& text, const wxString& sComment )
{
	wxXmlNode* pChapterTimeStart   = find_chapter_time_start( pChapterAtom );
	wxXmlNode* pChapterTimeEnd	   = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_TIME_END );
	wxXmlNode* pChapterTimeEndText = new wxXmlNode( pChapterTimeEnd, wxXML_TEXT_NODE, wxEmptyString, text );
	wxXmlNode* pCommentNode		   = create_comment_node( sComment );

	pChapterAtom->InsertChildAfter( pCommentNode, pChapterTimeStart );
	pChapterAtom->InsertChildAfter( pChapterTimeEnd, pCommentNode );

	return pChapterTimeEnd;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_end( wxXmlNode* pChapterAtom, const wxDuration& duration, const wxString& sComment )
{
	return add_chapter_time_end( pChapterAtom, duration.GetSamplesStr(), sComment );
}

bool wxXmlCueSheetRenderer::has_chapter_time_end( wxXmlNode* pChapterAtom )
{
	wxXmlNode* pChild = pChapterAtom->GetChildren();

	while ( pChild != wxNullXmlNode )
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
	wxXmlNode* pChapterDisplay		= new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_DISPLAY );
	wxXmlNode* pChapterString		= new wxXmlNode( pChapterDisplay, wxXML_ELEMENT_NODE, Xml::CHAPTER_STRING );
	wxXmlNode* pChapterStringText	= new wxXmlNode( pChapterString, wxXML_TEXT_NODE, wxEmptyString, sChapterString );
	wxXmlNode* pChapterLanguage		= new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_LANGUAGE );
	wxXmlNode* pChapterLanguageText = new wxXmlNode( pChapterLanguage, wxXML_TEXT_NODE, wxEmptyString, sLang );

	pChapterDisplay->AddChild( pChapterLanguage );
	pChapterAtom->AddChild( pChapterDisplay );
	return pChapterDisplay;
}

wxXmlNode* wxXmlCueSheetRenderer::add_hidden_flag( wxXmlNode* pChapterAtom, bool bHiddenFlag )
{
	wxXmlNode* pChapterHidden	  = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_FLAG_HIDDEN );
	wxXmlNode* pChapterHiddenText = new wxXmlNode( pChapterHidden, wxXML_TEXT_NODE, wxEmptyString, bHiddenFlag ? wxS( "1" ) : wxS( "0" ) );

	pChapterAtom->AddChild( pChapterHidden );
	return pChapterHidden;
}

wxXmlNode* wxXmlCueSheetRenderer::add_idx_chapter_atom(
		wxXmlNode* pChapterAtom,
		const wxDuration& duration,
		size_t nIndexNumber,
		const wxString& sComment,
		const wxString& sLang,
		bool bHiddenIndexes
		)
{
	wxASSERT( nIndexNumber > 1u );

	wxXmlNode* pIdxChapterAtom	 = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_ATOM );
	wxXmlNode* pChapterTimeStart = add_chapter_time_start( pIdxChapterAtom, duration, wxString::Format( wxS( "INDEX %d %s" ), nIndexNumber, sComment ) );

	add_chapter_display( pIdxChapterAtom, wxString::Format( wxS( "INDEX %02d" ), nIndexNumber - 1u ), sLang );
	add_hidden_flag( pIdxChapterAtom, bHiddenIndexes );

	pChapterAtom->AddChild( pIdxChapterAtom );
	return pIdxChapterAtom;
}

// ===============================================================================

wxULongLong wxXmlCueSheetRenderer::GenerateUID()
{
	return wxULongLong( (wxUint32)rand(), (wxUint32)rand() );
}

wxXmlCueSheetRenderer::wxXmlCueSheetRenderer():
	m_pCfg( ( wxConfiguration* )NULL )
{
	init_synonims();
}

wxXmlCueSheetRenderer* wxXmlCueSheetRenderer::CreateObject( const wxConfiguration& cfg, const wxInputFile& inputFile )
{
	wxClassInfo*		   ci		 = wxCLASSINFO( wxXmlCueSheetRenderer );
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

	// DISC: ARTIST = (ARTIST,ALBUM ARTIST,ALBUMARTIST)
	as.Add( wxCueTag::Name::ALBUM_ARTIST );
	as.Add( wxCueTag::Name::ALBUMARTIST );
	wxTagSynonims discSynonim3( wxCueTag::Name::ARTIST, as );
	m_discSynonims.Add( discSynonim3 );
	m_trackSynonims.Add( discSynonim3 );
}

void wxXmlCueSheetRenderer::SetConfiguration( const wxConfiguration& cfg )
{
	wxASSERT( m_pCfg == (wxConfiguration*)NULL );
	m_pCfg = &cfg;
}

void wxXmlCueSheetRenderer::SetInputFile( const wxInputFile& inputFile )
{
	m_inputFile = inputFile;
	GetConfig().GetOutputFile( inputFile, m_outputFile, m_tagsFile );
}

const wxConfiguration& wxXmlCueSheetRenderer::GetConfig() const
{
	wxASSERT( m_pCfg != (wxConfiguration*)NULL );
	return *m_pCfg;
}

bool wxXmlCueSheetRenderer::HasXmlDoc() const
{
	return m_pXmlDoc;
}

wxXmlDocument& wxXmlCueSheetRenderer::GetXmlDoc() const
{
	wxASSERT( HasXmlDoc() );
	return *m_pXmlDoc;
}

bool wxXmlCueSheetRenderer::HasXmlTags() const
{
	return m_pXmlTags;
}

wxXmlDocument& wxXmlCueSheetRenderer::GetXmlTags() const
{
	wxASSERT( HasXmlTags() );
	return *m_pXmlTags;
}

const wxFileName& wxXmlCueSheetRenderer::GetOutputFile() const
{
	return m_outputFile;
}

const wxFileName& wxXmlCueSheetRenderer::GetTagsFile() const
{
	return m_tagsFile;
}

bool wxXmlCueSheetRenderer::SaveXmlDoc( const wxScopedPtr< wxXmlDocument >& pXmlDoc, const wxFileName& fileName ) const
{
	wxSharedPtr< wxMBConv > pConv( GetConfig().GetXmlEncoding() );
	wxStringOutputStream	outputStream( NULL, *pConv );

	if ( !pXmlDoc->Save( outputStream ) )
	{
		wxLogError( _( "Fail to stringify XML document" ) );
		return false;
	}

	wxFileOutputStream fos( fileName.GetFullPath() );

	if ( fos.IsOk() )
	{
		wxSharedPtr< wxTextOutputStream > pStream( GetConfig().GetOutputTextStream( fos ) );
		pStream->WriteString( outputStream.GetString() );
		return true;
	}
	else
	{
		wxLogDebug( wxS( "Fail to save XML document to file \u201C%s\u201D" ), fileName.GetFullName() );
		return false;
	}
}

bool wxXmlCueSheetRenderer::SaveXmlDoc()
{
	wxASSERT( HasXmlDoc() );
	wxASSERT( HasXmlTags() );

	if ( !SaveXmlDoc( m_pXmlDoc, m_outputFile ) )
	{
		wxLogError( _( "Fail to save chapters to \u201C%s\u201D" ), m_outputFile.GetFullName() );
		return false;
	}

	if ( GetConfig().GenerateTags() )
	{
		wxASSERT( m_tagsFile.IsOk() );

		if ( !SaveXmlDoc( m_pXmlTags, m_tagsFile ) )
		{
			wxLogError( _( "Fail to save tags to \u201C%s\u201D" ), m_tagsFile.GetFullName() );
			return false;
		}
	}

	return true;
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeStart( wxXmlNode* pChapterAtom, const wxCueSheet& cueSheet, const wxIndex& idx ) const
{
	wxASSERT( idx.HasDataFileIdx() );

	wxDuration duration( cueSheet.GetDuration( idx.GetDataFileIdx() ) );

	if ( duration.IsValid() )
	{
		wxULongLong indexOffset( duration.GetSamplingInfo().GetIndexOffset( idx ) );
		duration.Add( indexOffset );
		return add_chapter_time_start( pChapterAtom, duration, duration.GetCdFramesStr() );
	}
	else
	{
		const wxDataFile& dataFile = cueSheet.GetDataFiles().Item( idx.GetDataFileIdx() );
		wxLogError( _( "Cannot calulate duration of data file \u201C%s\u201D (index %d)" ), dataFile.GetRealFileName().GetFullName(), idx.GetDataFileIdx() );
		return wxNullXmlNode;
	}
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeEnd( wxXmlNode* pChapterAtom, const wxDuration& duration, const wxString& sComment ) const
{
	return add_chapter_time_end( pChapterAtom, duration, sComment );
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeEnd( wxXmlNode* pChapterAtom, const wxCueSheet& cueSheet, const wxIndex& idx, const wxString& sDescription ) const
{
	wxASSERT( idx.HasDataFileIdx() );

	wxDuration duration( cueSheet.GetDuration( idx.GetDataFileIdx() ) );

	if ( duration.IsValid() )
	{
		wxULongLong indexOffset( duration.GetSamplingInfo().GetIndexOffset( idx ) );
		wxString	sComment;
		duration.Add( indexOffset );
		sComment.Printf( wxS( "%s : %s" ), duration.GetCdFramesStr(), sDescription );
		return AddChapterTimeEnd( pChapterAtom, duration, sComment );
	}
	else
	{
		const wxDataFile& dataFile = cueSheet.GetDataFiles().Item( idx.GetDataFileIdx() );
		wxLogError( _( "Cannot calulate duration of data file \u201C%s\u201D (index %d)" ), dataFile.GetRealFileName().GetFullName(), idx.GetDataFileIdx() );
		return wxNullXmlNode;
	}
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeEnd( wxXmlNode* pChapterAtom, const wxCueSheet& cueSheet, const wxTrack& track ) const
{
	const wxIndex& idx = track.GetFirstIndex();

	wxASSERT( idx.HasDataFileIdx() );

	const wxSamplingInfo& si = cueSheet.GetDataFiles().Item( idx.GetDataFileIdx() ).GetDuration().GetSamplingInfo();
	wxIndex				  fidx( si.ConvertIndex( idx, si.GetFramesFromCdFrames( GetConfig().GetChapterOffset() ), false ) );

	wxASSERT( fidx.HasDataFileIdx() );

	return AddChapterTimeEnd( pChapterAtom, cueSheet, fidx, wxString::Format( wxS( "from next track %d" ), track.GetNumber() ) );
}

wxXmlNode* wxXmlCueSheetRenderer::AddIdxChapterAtom( wxXmlNode* pChapterAtom, const wxCueSheet& cueSheet, const wxIndex& idx ) const
{
	wxDuration duration( cueSheet.GetDuration( idx.GetDataFileIdx() ) );

	if ( duration.IsValid() )
	{
		wxULongLong indexOffset( duration.GetSamplingInfo().GetIndexOffset( idx ) );
		duration.Add( indexOffset );
		return add_idx_chapter_atom( pChapterAtom, duration, idx.GetNumber(), duration.GetCdFramesStr(), GetConfig().GetLang(), GetConfig().HiddenIndexes() );
	}
	else
	{
		const wxDataFile& dataFile = cueSheet.GetDataFiles().Item( idx.GetDataFileIdx() );
		wxLogError( _( "Cannot calulate duration of data file \u201C%s\u201D (index %d)" ), dataFile.GetRealFileName().GetFullName(), idx.GetDataFileIdx() );
		return wxNullXmlNode;
	}
}

wxXmlDocument* wxXmlCueSheetRenderer::create_xml_document( const wxString& sRootNode )
{
	wxXmlDocument* pXmlDoc = new wxXmlDocument();

	pXmlDoc->SetVersion( wxS( "1.0" ) );
	pXmlDoc->SetFileEncoding( GetConfig().GetXmlFileEncoding() );
	wxXmlNode* pRoot = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, sRootNode );
	pXmlDoc->SetRoot( pRoot );
	return pXmlDoc;
}

wxXmlNode* wxXmlCueSheetRenderer::create_simple_tag( const wxCueTag& tag, const wxString& sLanguage )
{
	wxXmlNode* pSimple = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::SIMPLE );

	wxXmlNode* pName	 = new wxXmlNode( pSimple, wxXML_ELEMENT_NODE, Xml::NAME );
	wxXmlNode* pNameText = new wxXmlNode( pName, wxXML_TEXT_NODE, wxEmptyString, tag.GetName() );

	add_comment_node( pSimple, wxString::Format( wxS( "Source: %s" ), tag.GetSourceAsString() ) );

	wxXmlNode* pValue	  = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::STRING );
	wxXmlNode* pValueText = new wxXmlNode( pValue, tag.IsMultiline() ? wxXML_CDATA_SECTION_NODE : wxXML_TEXT_NODE, wxEmptyString, tag.GetValue() );

	pSimple->AddChild( pValue );

	wxXmlNode* pLang	 = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TAG_LANGUAGE );
	if ( tag.TestSource( wxCueTag::TAG_AUTO_GENERATED ) )
	{ // automatically generated tags are language agnostic
		wxXmlNode* pLangText = new wxXmlNode( pLang, wxXML_TEXT_NODE, wxEmptyString, wxConfiguration::LANG_UND );
	}
	else
	{
		wxXmlNode* pLangText = new wxXmlNode( pLang, wxXML_TEXT_NODE, wxEmptyString, sLanguage );
	}


	pSimple->AddChild( pLang );

	return pSimple;
}

bool wxXmlCueSheetRenderer::is_simple( wxXmlNode* pNode, const wxCueTag& tag )
{
	wxASSERT( pNode != wxNullXmlNode );

	wxXmlNode* pChild = pNode->GetChildren();
	bool	   bName  = false;
	bool	   bValue = false;

	while ( pChild != wxNullXmlNode )
	{
		if ( pChild->GetType() == wxXML_ELEMENT_NODE )
		{
			if ( pChild->GetName().CmpNoCase( Xml::NAME ) == 0 )
			{
				wxXmlNode* pText = pChild->GetChildren();

				if ( pText == wxNullXmlNode )
				{
					return false;
				}

				if ( pText->GetType() != wxXML_TEXT_NODE )
				{
					return false;
				}

				bName = ( pText->GetContent().CmpNoCase( tag.GetName() ) == 0 );
			}

			if ( pChild->GetName().CmpNoCase( Xml::STRING ) == 0 )
			{
				wxXmlNode* pText = pChild->GetChildren();

				if ( pText == wxNullXmlNode )
				{
					return false;
				}

				if ( !( ( pText->GetType() == wxXML_TEXT_NODE ) ||
						( pText->GetType() == wxXML_CDATA_SECTION_NODE ) ) )
				{
					return false;
				}

				bValue = ( pText->GetContent().Cmp( tag.GetValue() ) == 0 );
			}
		}

		pChild = pChild->GetNext();
	}

	return ( bName && bValue );
}

wxXmlNode* wxXmlCueSheetRenderer::find_simple_tag( wxXmlNode* pNode, const wxCueTag& tag )
{
	wxASSERT( pNode != wxNullXmlNode );

	wxXmlNode* pChild = pNode->GetChildren();
	while ( pChild != wxNullXmlNode )
	{
		if ( is_simple( pChild, tag ) )
		{
			return pChild;
		}

		pChild = pChild->GetNext();
	}

	return wxNullXmlNode;
}

wxXmlNode* wxXmlCueSheetRenderer::add_simple_tag( wxXmlNode* pNode,
		const wxString& sName, const wxString& sValue,
		const wxString& sLanguage )
{
	wxASSERT( pNode != wxNullXmlNode );

	wxCueTag   tag( wxCueTag::TAG_AUTO_GENERATED, sName, sValue );
	wxXmlNode* pTagNode = create_simple_tag( tag, sLanguage );

	pNode->AddChild( pTagNode );
	return pTagNode;
}

wxXmlNode* wxXmlCueSheetRenderer::add_simple_tag( wxXmlNode* pNode,
		const wxString& sName, size_t nValue,
		const wxString& sLanguage )
{
    return add_simple_tag( pNode, sName, wxString::Format( "%" wxSizeTFmtSpec "u", nValue ), sLanguage );
}

wxXmlNode* wxXmlCueSheetRenderer::add_simple_tag( wxXmlNode* pNode, const wxCueTag& tag, const wxString& sLanguage )
{
	wxASSERT( pNode != wxNullXmlNode );

	wxXmlNode* pTagNode = create_simple_tag( tag, sLanguage );
	pNode->AddChild( pTagNode );
	return pTagNode;
}

void wxXmlCueSheetRenderer::add_simple_tags( wxXmlNode* pNode, const wxArrayCueTag& tags, const wxString& sLanguage )
{
	wxASSERT( pNode != wxNullXmlNode );

	for ( size_t i = 0, nCount = tags.GetCount(); i < nCount; i++ )
	{
		wxXmlNode* pTagNode = add_simple_tag( pNode, tags[ i ], sLanguage );
	}
}

void wxXmlCueSheetRenderer::AddTags(
		const wxCueComponent& component,
		const wxTagSynonimsCollection& cdTextSynonims,
		const wxTagSynonimsCollection& synonims,
		wxXmlNode* pTag )
{
	wxASSERT( pTag != wxNullXmlNode );

	wxArrayCueTag mappedTags;
	wxArrayCueTag rest;

	component.GetTags( GetConfig().GetTagSources(), cdTextSynonims, synonims, mappedTags, rest );
	add_simple_tags( pTag, mappedTags, GetConfig().GetLang() );
	add_simple_tags( pTag, rest, GetConfig().GetLang() );
}

void wxXmlCueSheetRenderer::AddCdTextInfo( const wxCueComponent& component, wxXmlNode* pTag )
{
	wxASSERT( pTag != wxNullXmlNode );

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
	wxASSERT( pTags != wxNullXmlNode );

	wxXmlNode* pTag = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TAG );

	pTags->AddChild( pTag );

	add_comment_node( pTag, wxS( "Disc tag" ) );

	wxXmlNode* pTargets = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TARGETS );
	pTag->AddChild( pTargets );

	wxXmlNode* pTargetTypeValue		= new wxXmlNode( pTargets, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE_VALUE );
	wxXmlNode* pTargetTypeValueText = new wxXmlNode( pTargetTypeValue, wxXML_TEXT_NODE, wxEmptyString, wxString::Format( wxS( "%d" ), nTargetTypeValue ) );

	wxXmlNode* pTargetType	   = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE );
	wxXmlNode* pTargetTypeText = new wxXmlNode( pTargetType, wxXML_TEXT_NODE, wxEmptyString, XmlValue::ALBUM );
	pTargets->AddChild( pTargetType );

	if ( GetConfig().GenerateEditionUID() )
	{
		wxXmlNode* pEditionUID	   = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::EDITION_UID );
		wxXmlNode* pEditionUIDText = new wxXmlNode( pEditionUID, wxXML_TEXT_NODE, wxEmptyString, editionUID.ToString() );
		pTargets->AddChild( pEditionUID );
	}

	wxXmlNode* pSimple = add_simple_tag( pTag, Tag::ORIGINAL_MEDIA_TYPE, wxS( "CD" ), GetConfig().GetLang() );
	AppendDiscTags( cueSheet, pTags, nTargetTypeValue );
	return pTag;
}

wxXmlNode* wxXmlCueSheetRenderer::AppendDiscTags(
		const wxCueSheet& cueSheet,
		wxXmlNode* pTags,
		long nTargetTypeValue )
{
	wxASSERT( pTags != wxNullXmlNode );

	wxXmlNode* pTag = find_disc_tag_node( pTags, nTargetTypeValue );

	wxASSERT( pTag != wxNullXmlNode );

	add_comment_node( pTag, wxString::Format( wxS( "CUE file: \u201C%s\u201D" ), m_inputFile.ToString( false ) ) );

	AddCdTextInfo( cueSheet, pTag );

	const wxArrayCueTag& catalogs = cueSheet.GetCatalogs();
	for ( size_t i = 0, nCount = catalogs.GetCount(); i < nCount; i++ )
	{
		wxXmlNode* pSimple = add_simple_tag( pTag, Tag::CATALOG_NUMBER, catalogs[ i ].GetValue(), GetConfig().GetLang() );
	}

	return pTag;
}

wxXmlNode* wxXmlCueSheetRenderer::SetTotalParts(
		size_t nTotalParts,
		wxXmlNode* pTags,
		long nTargetTypeValue )
{
	wxASSERT( pTags != wxNullXmlNode );

	wxXmlNode* pTag = find_disc_tag_node( pTags, nTargetTypeValue );

	wxASSERT( pTag != wxNullXmlNode );

	add_comment_node( pTag, wxS( "Total number of tracks" ) );
	wxXmlNode* pSimple = add_simple_tag( pTag, Tag::TOTAL_PARTS, nTotalParts, GetConfig().GetLang() );

	return pTag;
}

wxXmlNode* wxXmlCueSheetRenderer::AddTrackTags(
		const wxTrack& track,
		const wxULongLong& chapterUID,
		wxXmlNode* pTags,
		int nTargetTypeValue )
{
	wxASSERT( pTags != wxNullXmlNode );

	wxXmlNode* pTag = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TAG );

	pTags->AddChild( pTag );

    add_comment_node( pTag, wxString::Format( "Track %02" wxSizeTFmtSpec "d", track.GetNumber( ) ) );

	wxXmlNode* pTargets = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TARGETS );
	pTag->AddChild( pTargets );

	wxXmlNode* pTargetTypeValue		= new wxXmlNode( pTargets, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE_VALUE );
	wxXmlNode* pTargetTypeValueText = new wxXmlNode( pTargetTypeValue, wxXML_TEXT_NODE, wxEmptyString, wxString::Format( wxS( "%d" ), nTargetTypeValue ) );

	wxXmlNode* pTargetType	   = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE );
	wxXmlNode* pTargetTypeText = new wxXmlNode( pTargetType, wxXML_TEXT_NODE, wxEmptyString, XmlValue::TRACK );
	pTargets->AddChild( pTargetType );

	wxXmlNode* pChapterUID	   = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_UID );
	wxXmlNode* pChapterUIDText = new wxXmlNode( pChapterUID, wxXML_TEXT_NODE, wxEmptyString, chapterUID.ToString() );
	pTargets->AddChild( pChapterUID );

	wxXmlNode* pSimple = add_simple_tag( pTag, Tag::PART_NUMBER, track.GetNumber(), GetConfig().GetLang() );
	AddCdTextInfo( track, pTag );

	return pTag;
}

bool wxXmlCueSheetRenderer::OnPreRenderDisc( const wxCueSheet& cueSheet )
{
	wxASSERT( !HasXmlDoc() );
	wxASSERT( !HasXmlTags() );

	m_pFirstChapterAtom = wxNullXmlNode;
	wxULongLong editionUID( GenerateUID() );

	wxLogInfo( _( "Creating XML document" ) );
	m_pXmlDoc.reset( create_xml_document( Xml::CHAPTERS ) );
	wxXmlNode* pChapters = m_pXmlDoc->GetRoot();
	GetConfig().BuildXmlComments( m_outputFile, pChapters );
	m_pEditionEntry = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::EDITION_ENTRY );
	pChapters->AddChild( m_pEditionEntry );

	if ( GetConfig().GenerateEditionUID() )
	{
		wxXmlNode* pEditionUID	   = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::EDITION_UID );
		wxXmlNode* pEditionUIDText = new wxXmlNode( pEditionUID, wxXML_TEXT_NODE, wxEmptyString, editionUID.ToString() );
		m_pEditionEntry->AddChild( pEditionUID );
	}

	m_pChapterAtom	   = wxNullXmlNode;
	m_pPrevChapterAtom = wxNullXmlNode;

	wxASSERT( m_pEditionEntry != wxNullXmlNode );
	add_comment_node( m_pEditionEntry, wxString::Format( wxS( "CUE file: \u201C%s\u201D" ), m_inputFile.ToString( false ) ) );

	m_pXmlTags.reset( create_xml_document( Xml::TAGS ) );
	m_pTags = m_pXmlTags->GetRoot();
	GetConfig().BuildXmlComments( m_tagsFile, m_pTags );
	AddDiscTags( cueSheet, m_pTags, editionUID );

	add_comment_node( m_pTags, wxString::Format( wxS( "CUE file: \u201C%s\u201D" ), m_inputFile.ToString( false ) ) );
    add_comment_node( m_pTags, wxString::Format( "Number of tracks: %" wxSizeTFmtSpec "u", cueSheet.GetTracksCount() ) );

	return wxCueSheetRenderer::OnPreRenderDisc( cueSheet );
}

bool wxXmlCueSheetRenderer::OnPreRenderTrack( const wxCueSheet& cueSheet, const wxTrack& track )
{
	wxASSERT( m_pEditionEntry != wxNullXmlNode );
    wxLogInfo( _( "Converting track %" wxSizeTFmtSpec "u" ), track.GetNumber( ) );

	m_pChapterAtom = new wxXmlNode( wxNullXmlNode, wxXML_ELEMENT_NODE, Xml::CHAPTER_ATOM );

	if ( m_pFirstChapterAtom == wxNullXmlNode )
	{
		m_pFirstChapterAtom = m_pChapterAtom;
	}

	add_comment_node( m_pChapterAtom, wxString::Format( "Track %02" wxSizeTFmtSpec "u", track.GetNumber() ) );
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
	size_t nTrackIdx = cueSheet.GetTrackIdxFromNumber( track.GetNumber() );
	add_chapter_display( m_pChapterAtom, cueSheet.FormatTrack( nTrackIdx, GetConfig().GetTrackNameFormat() ), GetConfig().GetLang() );
	m_pEditionEntry->AddChild( m_pChapterAtom );
	m_pPrevChapterAtom = m_pChapterAtom;
	return wxCueSheetRenderer::OnPostRenderTrack( cueSheet, track );
}

bool wxXmlCueSheetRenderer::OnRenderPreGap( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& preGap )
{
    wxLogInfo( _( "Converting pre-gap of track %" wxSizeTFmtSpec "u" ), track.GetNumber( ) );

	if ( track.GetNumber() == 1u )
	{
		if ( !GetConfig().TrackOneIndexOne() )
		{
			if ( AddChapterTimeStart( m_pChapterAtom, cueSheet, preGap ) == wxNullXmlNode )
			{
				return false;
			}
		}
	}
	else if ( GetConfig().GetChapterTimeEnd() )
	{
		if ( m_pPrevChapterAtom != wxNullXmlNode )
		{
            if (AddChapterTimeEnd( m_pPrevChapterAtom, cueSheet, preGap, wxString::Format( "from pre-gap of track %" wxSizeTFmtSpec "u", track.GetNumber( ) ) ) == wxNullXmlNode)
			{
				return false;
			}
		}
	}

	return wxCueSheetRenderer::OnRenderPreGap( cueSheet, track, preGap );
}

bool wxXmlCueSheetRenderer::OnRenderPostGap( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& postGap )
{
    wxLogInfo( _( "Converting post-gap of track %" wxSizeTFmtSpec "u" ), track.GetNumber( ) );

	if ( GetConfig().GetChapterTimeEnd() )
	{
        if (AddChapterTimeEnd( m_pChapterAtom, cueSheet, postGap, wxString::Format( "from post-gap of track %" wxSizeTFmtSpec "d", track.GetNumber( ) )) == wxNullXmlNode )
		{
			return false;
		}
	}

	return wxCueSheetRenderer::OnRenderPostGap( cueSheet, track, postGap );
}

bool wxXmlCueSheetRenderer::OnRenderIndex( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& idx )
{
    wxLogInfo( _( "Converting index %" wxSizeTFmtSpec "d of track %" wxSizeTFmtSpec "d" ), idx.GetNumber( ), track.GetNumber( ) );

	switch ( idx.GetNumber() )
	{
		case 0:	// pre-gap
		{
			if ( track.GetNumber() == 1u )
			{
				if ( !GetConfig().TrackOneIndexOne() )
				{
					if ( AddChapterTimeStart( m_pChapterAtom, cueSheet, idx ) == wxNullXmlNode )
					{
						return false;
					}
				}
			}
			else if ( GetConfig().GetChapterTimeEnd() )
			{
				if ( m_pPrevChapterAtom != wxNullXmlNode )
				{
                    if (AddChapterTimeEnd( m_pPrevChapterAtom, cueSheet, idx, wxString::Format( "from index 0 of track %" wxSizeTFmtSpec "d", track.GetNumber( ) )) == wxNullXmlNode )
					{
						return false;
					}
				}
			}

			break;
		}

		case 1:	// start
		{
			if ( track.GetNumber() == 1u )
			{
				if ( GetConfig().TrackOneIndexOne() )
				{
					if ( AddChapterTimeStart( m_pChapterAtom, cueSheet, idx ) == wxNullXmlNode )
					{
						return false;
					}
				}
			}
			else if ( AddChapterTimeStart( m_pChapterAtom, cueSheet, idx ) == wxNullXmlNode )
			{
				return false;
			}

			break;
		}

		default:
		{
			if ( AddIdxChapterAtom( m_pChapterAtom, cueSheet, idx ) == wxNullXmlNode )
			{
				return false;
			}

			break;
		}
	}

	return wxCueSheetRenderer::OnRenderIndex( cueSheet, track, idx );
}

bool wxXmlCueSheetRenderer::OnPostRenderDisc( const wxCueSheet& cueSheet )
{
	wxLogInfo( _( "Calculating chapter names and end time from data file(s)" ) );

	size_t nTracksCount = cueSheet.GetTracksCount();
	SetTotalParts( nTracksCount, m_pTags );

	if ( GetConfig().GetChapterTimeEnd() )
	{
		wxASSERT( m_pFirstChapterAtom != wxNullXmlNode );
		wxXmlNode* pChapterAtom = m_pFirstChapterAtom;

		for ( size_t i = 0; i < nTracksCount; i++ )
		{
			if ( !has_chapter_time_end( pChapterAtom ) )
			{
				if ( GetConfig().GetUseDataFiles() )
				{
					size_t nDataFileIdx = cueSheet.GetDataFileIdxIfLastForTrack( i );

					if ( nDataFileIdx != wxIndex::UnknownDataFileIdx )
					{
						const wxDataFile& dataFile = cueSheet.GetDataFiles().Item( nDataFileIdx );
						wxDuration		  duration( cueSheet.GetDuration( nDataFileIdx + 1u ) );

						if ( !duration.IsValid() )
						{
                            wxString s;
                            s << _("Cannot calulate duration for data file \u201C") <<
                                dataFile.GetRealFileName( ).GetFullName( ) <<
                                _("\u201D (index ") << nDataFileIdx << ')';
                            wxLogError( s );
							return false;
						}

						wxString sComment;
                        sComment << duration.GetCdFramesStr() << " : from media file(s) (" <<
                            (nDataFileIdx + 1u) << " \u201C" << dataFile.GetRealFileName().GetFullName() << "\u201D)";
 
						if ( AddChapterTimeEnd( pChapterAtom, duration, sComment ) == wxNullXmlNode )
						{
							return false;
						}
					}
				}
				else if ( GetConfig().GetUnknownChapterTimeEndToNextChapter() )
				{
					if ( ( i + 1u ) < cueSheet.GetTracksCount() )
					{
						const wxTrack& nextTrack = cueSheet.GetTrack( i + 1u );

						if ( AddChapterTimeEnd( pChapterAtom, cueSheet, nextTrack ) == wxNullXmlNode )
						{
							return false;
						}
					}
				}
			}

			pChapterAtom = pChapterAtom->GetNext();
		}
	}

	wxLogInfo( _( "XML document created" ) );
	return wxCueSheetRenderer::OnPostRenderDisc( cueSheet );
}

