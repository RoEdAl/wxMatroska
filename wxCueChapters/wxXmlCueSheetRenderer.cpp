/*
 * wxXmlCueSheetRenderer.cpp
 */

#include <wxCueFile/wxTagSynonims.h>
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxDuration.h>
#include <wxCueFile/wxIndex.h>
#include <wxCueFile/wxTrack.h>
#include <wxCueFile/wxCueSheet.h>
#include <wxCueFile/wxCueSheetReader.h>
#include <wxCueFile/wxStringProcessor.h>
#include <wxEncodingDetection/wxTextOutputStreamWithBOM.h>
#include "wxConfiguration.h"
#include "wxInputFile.h"
#include "wxXmlCueSheetRenderer.h"
#include "wxApp.h"

// ===============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxXmlCueSheetRenderer, wxCueSheetRenderer );

// ===============================================================================

const char wxXmlCueSheetRenderer::NON_ALPHA_REG_EX[] = "^\\P{L&}+$";

// ===============================================================================

const char wxXmlCueSheetRenderer::Tag::PREPARER[]       = "PREPARER";
const char wxXmlCueSheetRenderer::Tag::CATALOG_NUMBER[] = "CATALOG_NUMBER";
const char wxXmlCueSheetRenderer::Tag::TOTAL_PARTS[]    = "TOTAL_PARTS";
const char wxXmlCueSheetRenderer::Tag::PART_NUMBER[]    = "PART_NUMBER";

// ===============================================================================

const char wxXmlCueSheetRenderer::Xml::BINARY[]              = "Binary";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_UID[]         = "ChapterUID";
const char wxXmlCueSheetRenderer::Xml::EDITION_ENTRY[]       = "EditionEntry";
const char wxXmlCueSheetRenderer::Xml::EDITION_UID[]         = "EditionUID";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_TIME_START[]  = "ChapterTimeStart";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_TIME_END[]    = "ChapterTimeEnd";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_DISPLAY[]     = "ChapterDisplay";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_STRING[]      = "ChapterString";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_LANGUAGE[]    = "ChapterLanguage";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_FLAG_HIDDEN[] = "ChapterFlagHidden";
const char wxXmlCueSheetRenderer::Xml::CHAPTER_ATOM[]        = "ChapterAtom";
const char wxXmlCueSheetRenderer::Xml::CHAPTERS[]            = "Chapters";
const char wxXmlCueSheetRenderer::Xml::TARGETS[]             = "Targets";
const char wxXmlCueSheetRenderer::Xml::TAGS[]                = "Tags";
const char wxXmlCueSheetRenderer::Xml::TARGET_TYPE_VALUE[]   = "TargetTypeValue";
const char wxXmlCueSheetRenderer::Xml::TARGET_TYPE[]         = "TargetType";
const char wxXmlCueSheetRenderer::Xml::TAG[]                 = "Tag";
const char wxXmlCueSheetRenderer::Xml::TAG_LANGUAGE[]        = "TagLanguage";
const char wxXmlCueSheetRenderer::Xml::NAME[]                = "Name";
const char wxXmlCueSheetRenderer::Xml::STRING[]              = "String";
const char wxXmlCueSheetRenderer::Xml::SIMPLE[]              = "Simple";

// ===============================================================================

const char wxXmlCueSheetRenderer::XmlValue::ALBUM[] = "ALBUM";
const char wxXmlCueSheetRenderer::XmlValue::TRACK[] = "TRACK";

// ===============================================================================

wxXmlNode* wxXmlCueSheetRenderer::create_comment_node( const wxString& sComment )
{
	wxXmlNode* const pComment = new wxXmlNode( nullptr, wxXML_COMMENT_NODE, wxEmptyString, sComment );

	return pComment;
}

void wxXmlCueSheetRenderer::add_comment_node( wxXmlNode* pNode, const wxString& sComment )
{
	wxASSERT( pNode != nullptr );
	pNode->AddChild( create_comment_node( sComment ) );
}

wxXmlNode* wxXmlCueSheetRenderer::get_last_child( wxXmlNode* pNode )
{
	wxXmlNode* pChild = pNode->GetChildren();
	wxXmlNode* pPrev  = nullptr;

	while ( pChild != nullptr )
	{
		pPrev  = pChild;
		pChild = pChild->GetNext();
	}

	return pPrev;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_uid( wxXmlNode* pChapterAtom, const wxULongLong& uid )
{
	wxXmlNode* const pChapterUID     = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_UID );
	wxXmlNode* const pChapterUIDText = new wxXmlNode( pChapterUID, wxXML_TEXT_NODE, wxEmptyString, uid.ToString() );

	pChapterAtom->AddChild( pChapterUID );
	return pChapterUID;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_start( wxXmlNode* pChapterAtom, const wxString& s, const wxString& sComment )
{
	wxXmlNode* const pChapterTimeStart     = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_TIME_START );
	wxXmlNode* const pChapterTimeStartText = new wxXmlNode( pChapterTimeStart, wxXML_TEXT_NODE, wxEmptyString, s );
	wxXmlNode* const pCommentNode          = create_comment_node( sComment );

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
	wxXmlNode* pPrev  = nullptr;

	while ( pChild != nullptr )
	{
		if ( pChild->GetName().CmpNoCase( Xml::CHAPTER_TIME_START ) == 0 ) return pChild;

		pPrev  = pChild;
		pChild = pChild->GetNext();
	}

	wxASSERT( pPrev != nullptr );
	return pPrev;
}

bool wxXmlCueSheetRenderer::is_album_tag( wxXmlNode* pTag, long nTargetTypeValue )
{
	wxXmlNode* pChild = pTag->GetChildren();

	while ( pChild != nullptr )
	{
		if ( pChild->GetType() == wxXML_ELEMENT_NODE ) break;

		pChild = pChild->GetNext();
	}

	if ( pChild == nullptr ) return false;

	if ( pChild->GetName().CmpNoCase( Xml::TARGETS ) != 0 ) return false;

	wxXmlNode* const pTargetTypeValue = pChild->GetChildren();

	if ( pTargetTypeValue == nullptr ) return false;

	wxXmlNode* const pText = pTargetTypeValue->GetChildren();

	if ( pText == nullptr ) return false;

	long nParsedTargetTypeValue;

	if ( !pText->GetContent().ToCLong( &nParsedTargetTypeValue ) ) return false;

	return ( nParsedTargetTypeValue == nTargetTypeValue );
}

wxXmlNode* wxXmlCueSheetRenderer::find_disc_tag_node( wxXmlNode* pTags, long nTargetTypeValue )
{
	wxXmlNode* pChild = pTags->GetChildren();

	while ( pChild != nullptr )
	{
		if ( ( pChild->GetType() == wxXML_ELEMENT_NODE ) && ( pChild->GetName().CmpNoCase( Xml::TAG ) == 0 ) )
			if ( is_album_tag( pChild, nTargetTypeValue ) ) return pChild;

		pChild = pChild->GetNext();
	}

	return nullptr;
}

bool wxXmlCueSheetRenderer::set_total_parts( wxXmlNode* pSimple, size_t nTotalParts )
{
	wxXmlNode* pChild = pSimple->GetChildren();

	while ( pChild != nullptr )
	{
		if ( ( pChild->GetType() == wxXML_ELEMENT_NODE ) && ( pChild->GetName().CmpNoCase( Xml::STRING ) == 0 ) )
		{
			wxXmlNode* pText = pChild->GetChildren();
			wxASSERT( pText != nullptr );
			pText->SetContent( wxString::Format( "%" wxSizeTFmtSpec "u", nTotalParts ) );
			return true;
		}

		pChild = pChild->GetNext();
	}

	return false;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_time_end( wxXmlNode* pChapterAtom, const wxString& text, const wxString& sComment )
{
	wxXmlNode* const pChapterTimeStart   = find_chapter_time_start( pChapterAtom );
	wxXmlNode* const pChapterTimeEnd     = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_TIME_END );
	wxXmlNode* const pChapterTimeEndText = new wxXmlNode( pChapterTimeEnd, wxXML_TEXT_NODE, wxEmptyString, text );
	wxXmlNode* const pCommentNode        = create_comment_node( sComment );

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

	while ( pChild != nullptr )
	{
		if ( pChild->GetName().CmpNoCase( Xml::CHAPTER_TIME_END ) == 0 ) return true;

		pChild = pChild->GetNext();
	}

	return false;
}

wxXmlNode* wxXmlCueSheetRenderer::add_chapter_display( wxXmlNode* pChapterAtom, const wxString& sChapterString, const wxString& sLang )
{
	wxXmlNode* const pChapterDisplay      = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_DISPLAY );
	wxXmlNode* const pChapterString       = new wxXmlNode( pChapterDisplay, wxXML_ELEMENT_NODE, Xml::CHAPTER_STRING );
	wxXmlNode* const pChapterStringText   = new wxXmlNode( pChapterString, wxXML_TEXT_NODE, wxEmptyString, sChapterString );
	wxXmlNode* const pChapterLanguage     = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_LANGUAGE );
	wxXmlNode* const pChapterLanguageText = new wxXmlNode( pChapterLanguage, wxXML_TEXT_NODE, wxEmptyString, sLang );

	pChapterDisplay->AddChild( pChapterLanguage );
	pChapterAtom->AddChild( pChapterDisplay );
	return pChapterDisplay;
}

wxXmlNode* wxXmlCueSheetRenderer::add_hidden_flag( wxXmlNode* pChapterAtom, bool bHiddenFlag )
{
	wxXmlNode* const pChapterHidden     = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_FLAG_HIDDEN );
	wxXmlNode* const pChapterHiddenText = new wxXmlNode( pChapterHidden, wxXML_TEXT_NODE, wxEmptyString, bHiddenFlag ? "1" : "0" );

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

	wxXmlNode* const pIdxChapterAtom   = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_ATOM );
	wxXmlNode* const pChapterTimeStart = add_chapter_time_start( pIdxChapterAtom, duration, wxString::Format( "INDEX %" wxSizeTFmtSpec "u %s", nIndexNumber, sComment ) );

	add_chapter_display( pIdxChapterAtom, wxString::Format( "INDEX %02" wxSizeTFmtSpec "d", nIndexNumber - 1u ), sLang );
	add_hidden_flag( pIdxChapterAtom, bHiddenIndexes );

	pChapterAtom->AddChild( pIdxChapterAtom );
	return pIdxChapterAtom;
}

// ===============================================================================

wxULongLong wxXmlCueSheetRenderer::GenerateUID()
{
	return wxULongLong( (wxUint32)rand(), (wxUint32)rand() );
}

bool wxXmlCueSheetRenderer::IsNonAlphaTag( const wxCueTag& tag ) const
{
	return m_nonAlphaRegEx.Matches( tag.GetValue() );
}

wxXmlCueSheetRenderer::wxXmlCueSheetRenderer() :
	m_pCfg( nullptr ), m_nonAlphaRegEx( NON_ALPHA_REG_EX, wxRE_NOSUB )
{
	init_synonims();
	wxASSERT( m_nonAlphaRegEx.IsValid() );
}

wxXmlCueSheetRenderer* wxXmlCueSheetRenderer::CreateObject( const wxConfiguration& cfg, const wxInputFile& inputFile )
{
	wxClassInfo* const           ci        = wxCLASSINFO( wxXmlCueSheetRenderer );
	wxXmlCueSheetRenderer* const pRenderer = wxStaticCast( ci->CreateObject(), wxXmlCueSheetRenderer );

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
	wxASSERT( m_pCfg == nullptr );
	m_pCfg = &cfg;
	m_pStringProcessor.reset(cfg.CreateStringProcessor());
}

void wxXmlCueSheetRenderer::SetInputFile( const wxInputFile& inputFile )
{
	m_inputFile = inputFile;
	GetConfig().GetOutputFile( inputFile, m_outputFile, m_tagsFile );
}

const wxConfiguration& wxXmlCueSheetRenderer::GetConfig() const
{
	wxASSERT( m_pCfg != nullptr );
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
	wxStringOutputStream    outputStream( NULL, *pConv );

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
		return nullptr;
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
		const wxULongLong indexOffset( duration.GetSamplingInfo().GetIndexOffset( idx ) );
		wxString          sComment;
		duration.Add( indexOffset );
		sComment.Printf( "%s : %s", duration.GetCdFramesStr(), sDescription );
		return AddChapterTimeEnd( pChapterAtom, duration, sComment );
	}
	else
	{
		const wxDataFile& dataFile = cueSheet.GetDataFiles().Item( idx.GetDataFileIdx() );
		wxLogError( _( "Cannot calulate duration of data file \u201C%s\u201D (index %" wxSizeTFmtSpec "u)" ), dataFile.GetRealFileName().GetFullName(), idx.GetDataFileIdx() );
		return nullptr;
	}
}

wxXmlNode* wxXmlCueSheetRenderer::AddChapterTimeEnd( wxXmlNode* pChapterAtom, const wxCueSheet& cueSheet, const wxTrack& track ) const
{
	const wxIndex& idx = track.GetFirstIndex();

	wxASSERT( idx.HasDataFileIdx() );

	const wxSamplingInfo& si = cueSheet.GetDataFiles().Item( idx.GetDataFileIdx() ).GetDuration().GetSamplingInfo();
	const wxIndex         fidx( si.ConvertIndex( idx, si.GetFramesFromCdFrames( GetConfig().GetChapterOffset() ), false ) );

	wxASSERT( fidx.HasDataFileIdx() );

	return AddChapterTimeEnd( pChapterAtom, cueSheet, fidx, wxString::Format( "from next track %" wxSizeTFmtSpec "u", track.GetNumber() ) );
}

wxXmlNode* wxXmlCueSheetRenderer::AddIdxChapterAtom( wxXmlNode* pChapterAtom, const wxCueSheet& cueSheet, const wxIndex& idx ) const
{
	wxDuration duration( cueSheet.GetDuration( idx.GetDataFileIdx() ) );

	if ( duration.IsValid() )
	{
		const wxULongLong indexOffset( duration.GetSamplingInfo().GetIndexOffset( idx ) );
		duration.Add( indexOffset );
		return add_idx_chapter_atom( pChapterAtom, duration, idx.GetNumber(), duration.GetCdFramesStr(), GetConfig().GetLang(), GetConfig().HiddenIndexes() );
	}
	else
	{
		const wxDataFile& dataFile = cueSheet.GetDataFiles().Item( idx.GetDataFileIdx() );
		wxLogError( _( "Cannot calulate duration of data file \u201C%s\u201D (index %" wxSizeTFmtSpec "u)" ), dataFile.GetRealFileName().GetFullName(), idx.GetDataFileIdx() );
		return nullptr;
	}
}

wxXmlDocument* wxXmlCueSheetRenderer::create_xml_document( const wxString& sRootNode )
{
	wxXmlDocument* const pXmlDoc = new wxXmlDocument();

	pXmlDoc->SetVersion( "1.0" );
	pXmlDoc->SetFileEncoding( GetConfig().GetXmlFileEncoding() );
	wxXmlNode* const pRoot = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, sRootNode );

	pXmlDoc->SetRoot( pRoot );
	return pXmlDoc;
}

wxXmlNode* wxXmlCueSheetRenderer::create_simple_tag( const wxCueTag& tag ) const
{
	if (!GetConfig().RenderReplayGainTags())
	{
		if (tag.IsReplayGain())
		{
			wxLogInfo("XML Renderer: skipping RG tag %s", tag.GetName());
			return nullptr;
		}
	}

	if (!GetConfig().RenderMultilineTags())
	{
		if (tag.IsMultiline())
		{
			wxLogInfo("XML renderer: skipping multiline tag %s", tag.GetName());
			return nullptr;
		}
	}

	wxXmlNode* const pSimple = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::SIMPLE );
	wxXmlNode* const pName     = new wxXmlNode( pSimple, wxXML_ELEMENT_NODE, Xml::NAME );
	wxXmlNode* const pNameText = new wxXmlNode( pName, wxXML_TEXT_NODE, wxEmptyString, tag.GetName() );

	add_comment_node( pSimple, wxString::Format( "Source: %s", tag.GetSourceAsString() ) );

	wxXmlNode* pValue = nullptr;

	if (tag.IsMultiline())
	{
		wxString      sSeparator('=', 65);
		wxArrayString as;
		tag.GetValue(as);

		add_comment_node(pSimple, sSeparator);
		for (size_t i = 0, n = as.GetCount(); i < n; ++i)
		{
			add_comment_node(pSimple, as[i]);
		}

		add_comment_node(pSimple, sSeparator);

		wxString s;
		tag.GetValueBase64(76, as);
		wxASSERT(!as.IsEmpty());

		size_t nCount = as.GetCount();

		if (nCount > 1)
		{
			for (size_t i = 0, n = nCount - 1; i < n; ++i)
			{
				s << as[i] << "\r\n";
			}
		}
		s << as[nCount - 1];

		pValue = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, Xml::BINARY);
		wxXmlNode* pValueText = new wxXmlNode(pValue, wxXML_TEXT_NODE, wxEmptyString, s);
	}
	else
	{
		pValue = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::STRING );
		wxXmlNode* pValueText = new wxXmlNode( pValue, wxXML_TEXT_NODE, wxEmptyString, tag.GetValue() );
	}

	wxASSERT( pValue != nullptr );
	pSimple->AddChild( pValue );

	wxXmlNode* const pLang            = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::TAG_LANGUAGE );
	const bool       languageAgnostic = tag.IsReplayGain() || IsNonAlphaTag( tag ) || tag.TestSource( wxCueTag::TAG_AUTO_GENERATED | wxCueTag::TAG_UNKNOWN );
	wxXmlNode* const pLangText        = new wxXmlNode( pLang, wxXML_TEXT_NODE, wxEmptyString, languageAgnostic ? wxConfiguration::LANG_UND : GetConfig().GetLang() );

	pSimple->AddChild( pLang );

	return pSimple;
}

bool wxXmlCueSheetRenderer::is_simple( wxXmlNode* pNode, const wxCueTag& tag )
{
	wxASSERT( pNode != nullptr );

	wxXmlNode* pChild = pNode->GetChildren();
	bool       bName  = false;
	bool       bValue = false;

	while ( pChild != nullptr )
	{
		if ( pChild->GetType() == wxXML_ELEMENT_NODE )
		{
			if ( pChild->GetName().CmpNoCase( Xml::NAME ) == 0 )
			{
				wxXmlNode* pText = pChild->GetChildren();

				if ( pText == nullptr ) return false;

				if ( pText->GetType() != wxXML_TEXT_NODE ) return false;

				bName = ( pText->GetContent().CmpNoCase( tag.GetName() ) == 0 );
			}

			if ( pChild->GetName().CmpNoCase( Xml::STRING ) == 0 )
			{
				wxXmlNode* pText = pChild->GetChildren();

				if ( pText == nullptr ) return false;

				if ( !( ( pText->GetType() == wxXML_TEXT_NODE ) ||
						( pText->GetType() == wxXML_CDATA_SECTION_NODE ) ) ) return false;

				bValue = ( pText->GetContent().Cmp( tag.GetValue() ) == 0 );
			}
		}

		pChild = pChild->GetNext();
	}

	return ( bName && bValue );
}

wxXmlNode* wxXmlCueSheetRenderer::find_simple_tag( wxXmlNode* pNode, const wxCueTag& tag )
{
	wxASSERT( pNode != nullptr );

	wxXmlNode* pChild = pNode->GetChildren();

	while ( pChild != nullptr )
	{
		if ( is_simple( pChild, tag ) ) return pChild;

		pChild = pChild->GetNext();
	}

	return nullptr;
}

void wxXmlCueSheetRenderer::add_simple_tag( wxXmlNode* pNode,
		const wxString& sName, const wxString& sValue ) const
{
	wxASSERT( pNode != nullptr );

	const wxCueTag   tag( wxCueTag::TAG_AUTO_GENERATED, sName, sValue );
	wxXmlNode* const pTagNode = create_simple_tag( tag );
	if (pTagNode != nullptr) pNode->AddChild( pTagNode );
}

void wxXmlCueSheetRenderer::add_simple_tag( wxXmlNode* pNode,
		const wxString& sName, size_t nValue ) const
{
	add_simple_tag( pNode, sName, wxString::Format( "%" wxSizeTFmtSpec "u", nValue ) );
}

void wxXmlCueSheetRenderer::add_simple_tag( wxXmlNode* pNode, const wxCueTag& tag ) const
{
	wxASSERT( pNode != nullptr );

	wxXmlNode* const pTagNode = create_simple_tag( tag );
	if (pTagNode != nullptr) pNode->AddChild( pTagNode );
}

void wxXmlCueSheetRenderer::add_simple_tags( wxXmlNode* pNode, const wxArrayCueTag& tags ) const
{
	wxASSERT( pNode != nullptr );

	for ( size_t i = 0, nCount = tags.GetCount(); i < nCount; ++i )
	{
		add_simple_tag( pNode, tags[ i ] );
	}
}

void wxXmlCueSheetRenderer::AddTags(
		const wxCueComponent& component,
		const wxTagSynonimsCollection& cdTextSynonims,
		const wxTagSynonimsCollection& synonims,
		wxXmlNode* pTag )
{
	wxASSERT( pTag != nullptr );

	wxArrayCueTag mappedTags;
	wxArrayCueTag rest;

	component.GetTags( GetConfig().GetTagSources(), cdTextSynonims, synonims, mappedTags, rest );

	if ( GetConfig().RenderArtistForTrack() )
	{
		// wxCueTag artistTag( wxCueTag::TAG_UNKNOWN, wxCueTag::Name::ARTIST, wxEmptyString );

		if ( component.IsTrack() )
		{
			wxArrayCueTag artists;
			const size_t  res = wxCueTag::GetTags( mappedTags, wxCueTag::Name::ARTIST, artists );

			if ( res == 0 && !m_artistTags.IsEmpty() ) wxCueTag::AddTags( mappedTags, m_artistTags );
		}
		else
		{
			wxCueTag::GetTags( mappedTags, wxCueTag::Name::ARTIST, m_artistTags );
		}
	}

	add_simple_tags( pTag, mappedTags );
	add_simple_tags( pTag, rest );
}

void wxXmlCueSheetRenderer::AddCdTextInfo( const wxCueComponent& component, wxXmlNode* pTag )
{
	wxASSERT( pTag != nullptr );

	if ( component.IsTrack() ) AddTags( component, m_trackCdTextSynonims, m_trackSynonims, pTag );

	// wxCueTag::FindTag(  )
	else AddTags( component, m_discCdTextSynonims, m_discSynonims, pTag );
}

wxXmlNode* wxXmlCueSheetRenderer::AddDiscTags(
		const wxCueSheet& cueSheet,
		wxXmlNode* pTags,
		const wxULongLong& editionUID,
		int nTargetTypeValue )
{
	wxASSERT( pTags != nullptr );

	wxXmlNode* const pTag = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::TAG );

	pTags->AddChild( pTag );

	add_comment_node( pTag, "Disc tag" );

	wxXmlNode* const pTargets = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::TARGETS );

	pTag->AddChild( pTargets );

	wxXmlNode* const pTargetTypeValue     = new wxXmlNode( pTargets, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE_VALUE );
	wxXmlNode* const pTargetTypeValueText = new wxXmlNode( pTargetTypeValue, wxXML_TEXT_NODE, wxEmptyString, wxString::Format( "%d", nTargetTypeValue ) );

	wxXmlNode* const pTargetType     = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE );
	wxXmlNode* const pTargetTypeText = new wxXmlNode( pTargetType, wxXML_TEXT_NODE, wxEmptyString, XmlValue::ALBUM );

	pTargets->AddChild( pTargetType );

	if ( GetConfig().GenerateEditionUID() )
	{
		wxXmlNode* const pEditionUID     = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::EDITION_UID );
		wxXmlNode* const pEditionUIDText = new wxXmlNode( pEditionUID, wxXML_TEXT_NODE, wxEmptyString, editionUID.ToString() );
		pTargets->AddChild( pEditionUID );
	}

	const wxCueTag preparerTag(
		wxCueTag::TAG_AUTO_GENERATED,
		Tag::PREPARER,
		wxString::Format( wxS( "%s\u00A0%s" ), wxGetApp().GetAppDisplayName(), wxGetApp().APP_VERSION ) );
	add_simple_tag( pTag, preparerTag );

	AppendDiscTags( cueSheet, pTags, nTargetTypeValue );
	return pTag;
}

wxXmlNode* wxXmlCueSheetRenderer::AppendDiscTags(
		const wxCueSheet& cueSheet,
		wxXmlNode* pTags,
		long nTargetTypeValue )
{
	wxASSERT( pTags != nullptr );

	wxXmlNode* const pTag = find_disc_tag_node( pTags, nTargetTypeValue );

	wxASSERT( pTag != nullptr );

	add_comment_node( pTag, wxString::Format( wxS( "CUE file: \u201C%s\u201D" ), m_inputFile.ToString( false ) ) );

	AddCdTextInfo( cueSheet, pTag );

	const wxArrayCueTag& catalogs = cueSheet.GetCatalogs();

	for ( size_t i = 0, nCount = catalogs.GetCount(); i < nCount; ++i )
	{
		const wxCueTag   catalogNumberTag( catalogs[ i ].GetSource(), Tag::CATALOG_NUMBER, catalogs[ i ].GetValue() );
		add_simple_tag( pTag, catalogNumberTag );
	}

	return pTag;
}

wxXmlNode* wxXmlCueSheetRenderer::SetTotalParts(
		size_t nTotalParts,
		wxXmlNode* pTags,
		long nTargetTypeValue )
{
	wxASSERT( pTags != nullptr );

	wxXmlNode* const pTag = find_disc_tag_node( pTags, nTargetTypeValue );

	wxASSERT( pTag != nullptr );

	add_comment_node( pTag, "Total number of tracks" );
	add_simple_tag( pTag, Tag::TOTAL_PARTS, nTotalParts );

	return pTag;
}

wxXmlNode* wxXmlCueSheetRenderer::AddTrackTags(
		const wxTrack& track,
		const wxULongLong& chapterUID,
		wxXmlNode* pTags,
		int nTargetTypeValue )
{
	wxASSERT( pTags != nullptr );

	wxXmlNode* const pTag = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::TAG );

	pTags->AddChild( pTag );

	add_comment_node( pTag, wxString::Format( "Track %02" wxSizeTFmtSpec "d", track.GetNumber() ) );

	wxXmlNode* const pTargets = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::TARGETS );

	pTag->AddChild( pTargets );

	wxXmlNode* const pTargetTypeValue     = new wxXmlNode( pTargets, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE_VALUE );
	wxXmlNode* const pTargetTypeValueText = new wxXmlNode( pTargetTypeValue, wxXML_TEXT_NODE, wxEmptyString, wxString::Format( "%d", nTargetTypeValue ) );

	wxXmlNode* const pTargetType     = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::TARGET_TYPE );
	wxXmlNode* const pTargetTypeText = new wxXmlNode( pTargetType, wxXML_TEXT_NODE, wxEmptyString, XmlValue::TRACK );

	pTargets->AddChild( pTargetType );

	wxXmlNode* const pChapterUID     = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_UID );
	wxXmlNode* const pChapterUIDText = new wxXmlNode( pChapterUID, wxXML_TEXT_NODE, wxEmptyString, chapterUID.ToString() );

	pTargets->AddChild( pChapterUID );

	add_simple_tag( pTag, Tag::PART_NUMBER, track.GetNumber() );

	AddCdTextInfo( track, pTag );

	return pTag;
}

bool wxXmlCueSheetRenderer::OnPreRenderDisc( const wxCueSheet& cueSheet )
{
	wxASSERT( !HasXmlDoc() );
	wxASSERT( !HasXmlTags() );

	m_pFirstChapterAtom = nullptr;
	const wxULongLong editionUID( GenerateUID() );

	wxLogInfo( _( "Creating XML document" ) );
	m_pXmlDoc.reset( create_xml_document( Xml::CHAPTERS ) );
	wxXmlNode* const pChapters = m_pXmlDoc->GetRoot();

	GetConfig().BuildXmlComments( m_outputFile, pChapters );
	m_pEditionEntry = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::EDITION_ENTRY );
	pChapters->AddChild( m_pEditionEntry );

	if ( GetConfig().GenerateEditionUID() )
	{
		wxXmlNode* const pEditionUID     = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::EDITION_UID );
		wxXmlNode* const pEditionUIDText = new wxXmlNode( pEditionUID, wxXML_TEXT_NODE, wxEmptyString, editionUID.ToString() );
		m_pEditionEntry->AddChild( pEditionUID );
	}

	m_pChapterAtom     = nullptr;
	m_pPrevChapterAtom = nullptr;

	wxASSERT( m_pEditionEntry != nullptr );
	add_comment_node( m_pEditionEntry, wxString::Format( wxS( "CUE file: \u201C%s\u201D" ), m_inputFile.ToString( false ) ) );

	m_pXmlTags.reset( create_xml_document( Xml::TAGS ) );
	m_pTags = m_pXmlTags->GetRoot();
	GetConfig().BuildXmlComments( m_tagsFile, m_pTags );
	m_artistTags.Empty();
	AddDiscTags( cueSheet, m_pTags, editionUID );

	add_comment_node( m_pTags, wxString::Format( wxS( "CUE file: \u201C%s\u201D" ), m_inputFile.ToString( false ) ) );
	add_comment_node( m_pTags, wxString::Format( "Number of tracks: %" wxSizeTFmtSpec "u", cueSheet.GetTracksCount() ) );

	return wxCueSheetRenderer::OnPreRenderDisc( cueSheet );
}

bool wxXmlCueSheetRenderer::OnPreRenderTrack( const wxCueSheet& cueSheet, const wxTrack& track )
{
	wxASSERT( m_pEditionEntry != nullptr );
	wxLogInfo( _( "Converting track %" wxSizeTFmtSpec "u" ), track.GetNumber() );

	m_pChapterAtom = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, Xml::CHAPTER_ATOM );

	if ( m_pFirstChapterAtom == nullptr ) m_pFirstChapterAtom = m_pChapterAtom;

	add_comment_node( m_pChapterAtom, wxString::Format( "Track %02" wxSizeTFmtSpec "u", track.GetNumber() ) );
	return wxCueSheetRenderer::OnPreRenderTrack( cueSheet, track );
}

bool wxXmlCueSheetRenderer::OnRenderTrack( const wxCueSheet& cueSheet, const wxTrack& track )
{
	const wxULongLong chapterUID( GenerateUID() );

	add_chapter_uid( m_pChapterAtom, chapterUID );
	AddTrackTags( track, chapterUID, m_pTags );
	return wxCueSheetRenderer::OnRenderTrack( cueSheet, track );
}

bool wxXmlCueSheetRenderer::OnPostRenderTrack( const wxCueSheet& cueSheet, const wxTrack& track )
{
	wxASSERT( m_pEditionEntry != nullptr );

	const size_t nTrackIdx = cueSheet.GetTrackIdxFromNumber( track.GetNumber() );

	wxString trackNameFmt = GetConfig().GetTrackNameFormat();
	(*m_pStringProcessor)( trackNameFmt );

	add_chapter_display( m_pChapterAtom, cueSheet.FormatTrack( GetConfig().GetTagSources(), nTrackIdx, trackNameFmt ), GetConfig().GetLang() );
	m_pEditionEntry->AddChild( m_pChapterAtom );
	m_pPrevChapterAtom = m_pChapterAtom;
	return wxCueSheetRenderer::OnPostRenderTrack( cueSheet, track );
}

bool wxXmlCueSheetRenderer::OnRenderPreGap( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& preGap )
{
	wxLogInfo( _( "Converting pre-gap of track %" wxSizeTFmtSpec "u" ), track.GetNumber() );

	if ( track.GetNumber() == 1u )
	{
		if ( !GetConfig().TrackOneIndexOne() )
			if ( AddChapterTimeStart( m_pChapterAtom, cueSheet, preGap ) == nullptr ) return false;
	}
	else if ( GetConfig().GetChapterTimeEnd() )
	{
		if ( m_pPrevChapterAtom != nullptr )
			if ( AddChapterTimeEnd( m_pPrevChapterAtom, cueSheet, preGap, wxString::Format( "from pre-gap of track %" wxSizeTFmtSpec "u", track.GetNumber() ) ) == nullptr ) return false;
	}

	return wxCueSheetRenderer::OnRenderPreGap( cueSheet, track, preGap );
}

bool wxXmlCueSheetRenderer::OnRenderPostGap( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& postGap )
{
	wxLogInfo( _( "Converting post-gap of track %" wxSizeTFmtSpec "u" ), track.GetNumber() );

	if ( GetConfig().GetChapterTimeEnd() )
		if ( AddChapterTimeEnd( m_pChapterAtom, cueSheet, postGap, wxString::Format( "from post-gap of track %" wxSizeTFmtSpec "d", track.GetNumber() ) ) == nullptr ) return false;

	return wxCueSheetRenderer::OnRenderPostGap( cueSheet, track, postGap );
}

bool wxXmlCueSheetRenderer::OnRenderIndex( const wxCueSheet& cueSheet, const wxTrack& track, const wxIndex& idx )
{
	wxLogInfo( _( "Converting index %" wxSizeTFmtSpec "d of track %" wxSizeTFmtSpec "d" ), idx.GetNumber(), track.GetNumber() );

	switch ( idx.GetNumber() )
	{
		case 0:	// pre-gap
		{
			if ( track.GetNumber() == 1u )
			{
				if ( !GetConfig().TrackOneIndexOne() )
					if ( AddChapterTimeStart( m_pChapterAtom, cueSheet, idx ) == nullptr ) return false;
			}
			else if ( GetConfig().GetChapterTimeEnd() )
			{
				if ( m_pPrevChapterAtom != nullptr )
					if ( AddChapterTimeEnd( m_pPrevChapterAtom, cueSheet, idx, wxString::Format( "from index 0 of track %" wxSizeTFmtSpec "d", track.GetNumber() ) ) == nullptr ) return false;
			}

			break;
		}

		case 1:	// start
		{
			if ( track.GetNumber() == 1u )
			{
				if ( GetConfig().TrackOneIndexOne() )
					if ( AddChapterTimeStart( m_pChapterAtom, cueSheet, idx ) == nullptr ) return false;
			}
			else if ( AddChapterTimeStart( m_pChapterAtom, cueSheet, idx ) == nullptr )
			{
				return false;
			}

			break;
		}

		default:
		{
			if ( AddIdxChapterAtom( m_pChapterAtom, cueSheet, idx ) == nullptr ) return false;

			break;
		}
	}

	return wxCueSheetRenderer::OnRenderIndex( cueSheet, track, idx );
}

bool wxXmlCueSheetRenderer::OnPostRenderDisc( const wxCueSheet& cueSheet )
{
	wxLogInfo( _( "Calculating chapter names and end time from data file(s)" ) );

	const size_t nTracksCount = cueSheet.GetTracksCount();

	SetTotalParts( nTracksCount, m_pTags );

	if ( GetConfig().GetChapterTimeEnd() )
	{
		wxASSERT( m_pFirstChapterAtom != nullptr );
		wxXmlNode* pChapterAtom = m_pFirstChapterAtom;

		for ( size_t i = 0; i < nTracksCount; ++i )
		{
			if ( !has_chapter_time_end( pChapterAtom ) )
			{
				if ( GetConfig().GetUseDataFiles() )
				{
					const size_t nDataFileIdx = cueSheet.GetDataFileIdxIfLastForTrack( i );

					if ( nDataFileIdx != wxIndex::UnknownDataFileIdx )
					{
						const wxDataFile& dataFile = cueSheet.GetDataFiles().Item( nDataFileIdx );
						const wxDuration  duration( cueSheet.GetDuration( nDataFileIdx + 1u ) );

						if ( !duration.IsValid() )
						{
							wxString s;
							s << _( "Cannot calulate duration for data file \u201C" ) <<
								dataFile.GetRealFileName().GetFullName() <<
								_( "\u201D (index " ) << nDataFileIdx << ')';
							wxLogError( s );
							return false;
						}

						wxString sComment;
						sComment << duration.GetCdFramesStr() << " : from media file(s) (" <<
							( nDataFileIdx + 1u ) << wxS( " \u201C" ) << dataFile.GetRealFileName().GetFullName() << wxS( "\u201D)" );

						if ( AddChapterTimeEnd( pChapterAtom, duration, sComment ) == nullptr ) return false;
					}
				}
				else if ( GetConfig().GetUnknownChapterTimeEndToNextChapter() )
				{
					if ( ( i + 1u ) < cueSheet.GetTracksCount() )
					{
						const wxTrack& nextTrack = cueSheet.GetTrack( i + 1u );

						if ( AddChapterTimeEnd( pChapterAtom, cueSheet, nextTrack ) == nullptr ) return false;
					}
				}
			}

			pChapterAtom = pChapterAtom->GetNext();
		}
	}

	wxLogInfo( _( "XML document created" ) );
	return wxCueSheetRenderer::OnPostRenderDisc( cueSheet );
}

