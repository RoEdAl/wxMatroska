/*
   wxXmlCueSheetRenderer.h
 */

#ifndef _WX_XML_CUE_SHEET_RENDERER_H_
#define _WX_XML_CUE_SHEET_RENDERER_H_

#ifndef _WX_CUE_SHEET_RENDERER_H_
#include <wxCueFile/wxCueSheetRenderer.h>
#endif

#ifndef _WX_INPUT_FILE_H_
#include "wxInputFile.h"
#endif

#ifndef _WX_CONFIGURATION_H_
class wxConfiguration;
#endif

#ifndef _WX_TAG_SYNONIMS_H_
#include <wxCueFile/wxTagSynonims.h>
#endif

#ifndef _WX_SAMPLING_INFO_H_
#include <wxCueFile/wxSamplingInfo.h>
#endif

class wxXmlCueSheetRenderer:
	public wxCueSheetRenderer
{
	wxDECLARE_DYNAMIC_CLASS( wxXmlCueSheetRenderer );

protected:

	virtual bool OnPreRenderDisc( const wxCueSheet& );
	virtual bool OnPreRenderTrack( const wxCueSheet&, const wxTrack& );
	virtual bool OnRenderTrack( const wxCueSheet&, const wxTrack& );
	virtual bool OnPostRenderTrack( const wxCueSheet&, const wxTrack& );
	virtual bool OnPostRenderDisc( const wxCueSheet& );

	virtual bool OnRenderPreGap( const wxCueSheet&, const wxTrack&, const wxIndex& );
	virtual bool OnRenderPostGap( const wxCueSheet&, const wxTrack&, const wxIndex& );
	virtual bool OnRenderIndex( const wxCueSheet&, const wxTrack&, const wxIndex& );

protected:

	wxScopedPtr<wxXmlDocument> m_pXmlDoc;
	wxXmlNode*				   m_pChapterAtom;
	wxXmlNode*				   m_pFirstChapterAtom;
	wxXmlNode*				   m_pPrevChapterAtom;
	wxXmlNode*				   m_pEditionEntry;

	wxScopedPtr<wxXmlDocument> m_pXmlTags;
	wxXmlNode*				   m_pTags;

	const wxConfiguration* m_pCfg;
	wxInputFile			   m_inputFile;
	wxString			   m_sOutputFile;
	wxString			   m_sTagsFile;

	wxSamplingInfo m_si;

	wxTagSynonimsCollection m_discCdTextSynonims;
	wxTagSynonimsCollection m_discSynonims;
	wxTagSynonimsCollection m_trackCdTextSynonims;
	wxTagSynonimsCollection m_trackSynonims;

public:

	static wxXmlCueSheetRenderer* const Null;

	static struct Tag
	{
		static const wxChar* const ORIGINAL_MEDIA_TYPE;
		static const wxChar* const CATALOG_NUMBER;
		static const wxChar* const TOTAL_PARTS;
		static const wxChar* const PART_NUMBER;
	};

	static struct Xml
	{
		static const wxChar* const CHAPTER_UID;
		static const wxChar* const EDITION_ENTRY;
		static const wxChar* const EDITION_UID;
		static const wxChar* const CHAPTER_TIME_START;
		static const wxChar* const CHAPTER_TIME_END;
		static const wxChar* const CHAPTER_DISPLAY;
		static const wxChar* const CHAPTER_STRING;
		static const wxChar* const CHAPTER_LANGUAGE;
		static const wxChar* const CHAPTER_FLAG_HIDDEN;
		static const wxChar* const CHAPTER_ATOM;
		static const wxChar* const CHAPTERS;
		static const wxChar* const TARGETS;
		static const wxChar* const TAGS;
		static const wxChar* const TARGET_TYPE_VALUE;
		static const wxChar* const TARGET_TYPE;
		static const wxChar* const TAG;
		static const wxChar* const TAG_LANGUAGE;
		static const wxChar* const NAME;
		static const wxChar* const STRING;
		static const wxChar* const SIMPLE;
	};

	static struct XmlValue
	{
		static const wxChar* const ALBUM;
		static const wxChar* const TRACK;
	};

protected:

	void AddTags( const wxCueComponent&,
				  const wxTagSynonimsCollection&,
				  const wxTagSynonimsCollection&,
				  wxXmlNode* );
	void AddCdTextInfo( const wxCueComponent&, wxXmlNode* );
	wxXmlNode* AddDiscTags( const wxCueSheet&,
							wxXmlNode*,
							const wxULongLong&,
							int = 50 );
	wxXmlNode* AppendDiscTags( const wxCueSheet&,
							   wxXmlNode*,
							   long = 50 );

	wxXmlNode* SetTotalParts(
		size_t,
		wxXmlNode *,
		long = 50 );
	wxXmlNode* AddTrackTags( const wxTrack&,
							 const wxULongLong&,
							 wxXmlNode*,
							 int = 30 );

	wxXmlNode* AddChapterTimeStart( wxXmlNode*, const wxIndex& ) const;

	wxXmlNode* AddChapterTimeEnd( wxXmlNode*, const wxTrack& ) const;
	wxXmlNode* AddChapterTimeEnd( wxXmlNode*, const wxIndex& ) const;
	wxXmlNode* AddChapterTimeEnd( wxXmlNode*, const wxULongLong& ) const;
	wxXmlNode* AddChapterTimeEnd( wxXmlNode*, const wxDuration& ) const;

	wxXmlNode* AddIdxChapterAtom( wxXmlNode*, const wxIndex& ) const;

	void init_synonims();

	void SetConfiguration( const wxConfiguration& );
	const wxConfiguration& GetConfig() const;

protected:

	static wxXmlNode* get_last_child( wxXmlNode* );
	static wxXmlNode* add_chapter_uid( wxXmlNode*, const wxULongLong& uid );
	static wxXmlNode* add_chapter_time_start( wxXmlNode*, const wxString& );

	static wxXmlNode* add_chapter_time_start( wxXmlNode *, const wxSamplingInfo &, wxULongLong );
	static wxXmlNode* find_chapter_time_start( wxXmlNode* );
	static bool is_album_tag( wxXmlNode*, long );
	static wxXmlNode* find_disc_tag_node( wxXmlNode*, long );

	static bool set_total_parts( wxXmlNode *, size_t );
	static wxXmlNode* add_chapter_time_end( wxXmlNode*, const wxString& );

	static wxXmlNode* add_chapter_time_end( wxXmlNode *, const wxSamplingInfo &, wxULongLong );
	static bool has_chapter_time_end( wxXmlNode* );
	static wxXmlNode* add_chapter_display( wxXmlNode*, const wxString&, const wxString& );
	static wxXmlNode* add_hidden_flag( wxXmlNode*, bool );
	static wxXmlNode* add_idx_chapter_atom( wxXmlNode*, const wxSamplingInfo&, const wxULongLong&, unsigned int, const wxString&, bool );
	static wxXmlNode* create_simple_tag( const wxCueTag&, const wxString& );
	static wxXmlDocument* create_xml_document( const wxString& );
	static bool is_simple( wxXmlNode*, const wxCueTag& );
	static wxXmlNode* find_simple_tag( wxXmlNode*, const wxCueTag& );
	static wxXmlNode* add_simple_tag( wxXmlNode*, const wxString&, const wxString&, const wxString& );
	static wxXmlNode* add_simple_tag( wxXmlNode*, const wxCueTag&, const wxString& );
	static wxXmlNode* create_comment_node( const wxString& );
	static void add_comment_node( wxXmlNode*, const wxString& );
	static wxULongLong GenerateUID();

protected:

	wxXmlCueSheetRenderer();

public:

	static wxXmlCueSheetRenderer* CreateObject( const wxConfiguration&, const wxInputFile& );

	void SetInputFile( const wxInputFile& );

	bool HasXmlDoc() const;
	bool HasXmlTags() const;

	wxXmlDocument& GetXmlDoc() const;
	wxXmlDocument& GetXmlTags() const;

	const wxString& GetOutputFile() const;
	const wxString& GetTagsFile() const;

	bool SaveXmlDoc();
};

#endif