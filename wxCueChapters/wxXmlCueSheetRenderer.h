/*
 * wxXmlCueSheetRenderer.h
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

		wxScopedPtr< wxXmlDocument > m_pXmlDoc;
		wxXmlNode*					 m_pChapterAtom;
		wxXmlNode*					 m_pFirstChapterAtom;
		wxXmlNode*					 m_pPrevChapterAtom;
		wxXmlNode*					 m_pEditionEntry;

		wxScopedPtr< wxXmlDocument > m_pXmlTags;
		wxXmlNode*					 m_pTags;

		const wxConfiguration* m_pCfg;
		wxInputFile			   m_inputFile;
		wxFileName			   m_outputFile;
		wxFileName			   m_tagsFile;

		wxTagSynonimsCollection m_discCdTextSynonims;
		wxTagSynonimsCollection m_discSynonims;
		wxTagSynonimsCollection m_trackCdTextSynonims;
		wxTagSynonimsCollection m_trackSynonims;

        wxArrayCueTag m_artistTags;

	public:

		static wxXmlCueSheetRenderer* const Null;

		static struct Tag
		{
			static const char ORIGINAL_MEDIA_TYPE[];
			static const char CATALOG_NUMBER[];
			static const char TOTAL_PARTS[];
			static const char PART_NUMBER[];
		};

		static struct Xml
		{
			static const char CHAPTER_UID[];
			static const char EDITION_ENTRY[];
			static const char EDITION_UID[];
			static const char CHAPTER_TIME_START[];
			static const char CHAPTER_TIME_END[];
			static const char CHAPTER_DISPLAY[];
			static const char CHAPTER_STRING[];
			static const char CHAPTER_LANGUAGE[];
			static const char CHAPTER_FLAG_HIDDEN[];
			static const char CHAPTER_ATOM[];
			static const char CHAPTERS[];
			static const char TARGETS[];
			static const char TAGS[];
			static const char TARGET_TYPE_VALUE[];
			static const char TARGET_TYPE[];
			static const char TAG[];
			static const char TAG_LANGUAGE[];
			static const char NAME[];
			static const char STRING[];
			static const char SIMPLE[];
		};

		static struct XmlValue
		{
			static const char ALBUM[];
			static const char TRACK[];
		};

	protected:

		void AddTags( const wxCueComponent&,
					  const wxTagSynonimsCollection&,
					  const wxTagSynonimsCollection&,
					  wxXmlNode* );
		void AddCdTextInfo( const wxCueComponent&, wxXmlNode* );
		wxXmlNode* AddDiscTags( const wxCueSheet&, wxXmlNode*, const wxULongLong&, int = 50 );
		wxXmlNode* AppendDiscTags( const wxCueSheet&, wxXmlNode*, long = 50 );

		wxXmlNode* SetTotalParts( size_t, wxXmlNode*, long = 50 );
		wxXmlNode* AddTrackTags( const wxTrack&, const wxULongLong&, wxXmlNode*, int = 30 );

		wxXmlNode* AddChapterTimeStart( wxXmlNode*, const wxCueSheet&, const wxIndex& ) const;

		wxXmlNode* AddChapterTimeEnd( wxXmlNode*, const wxCueSheet&, const wxTrack& ) const;
		wxXmlNode* AddChapterTimeEnd( wxXmlNode*, const wxCueSheet&, const wxIndex&, const wxString& ) const;
		wxXmlNode* AddChapterTimeEnd( wxXmlNode*, const wxDuration&, const wxString& ) const;

		wxXmlNode* AddIdxChapterAtom( wxXmlNode*, const wxCueSheet&, const wxIndex& ) const;

		void init_synonims();

		void SetConfiguration( const wxConfiguration& );
		const wxConfiguration& GetConfig() const;

		bool SaveXmlDoc( const wxScopedPtr< wxXmlDocument >&, const wxFileName& ) const;

	protected:

		static wxXmlNode* get_last_child( wxXmlNode* );
		static wxXmlNode* add_chapter_uid( wxXmlNode*, const wxULongLong& uid );
		static wxXmlNode* add_chapter_time_start( wxXmlNode*, const wxString&, const wxString& );
		static wxXmlNode* add_chapter_time_start( wxXmlNode*, const wxDuration&, const wxString& );
		static wxXmlNode* find_chapter_time_start( wxXmlNode* );
		static bool is_album_tag( wxXmlNode*, long );
		static wxXmlNode* find_disc_tag_node( wxXmlNode*, long );

		static bool set_total_parts( wxXmlNode*, size_t );
		static wxXmlNode* add_chapter_time_end( wxXmlNode*, const wxString&, const wxString& );
		static wxXmlNode* add_chapter_time_end( wxXmlNode*, const wxDuration&, const wxString& );
		static bool has_chapter_time_end( wxXmlNode* );
		static wxXmlNode* add_chapter_display( wxXmlNode*, const wxString&, const wxString& );
		static wxXmlNode* add_hidden_flag( wxXmlNode*, bool );

		static wxXmlNode* add_idx_chapter_atom( wxXmlNode*, const wxDuration &, size_t, const wxString &, const wxString &, bool );
		static wxXmlNode* create_simple_tag( const wxCueTag&, const wxString& );
		wxXmlDocument* create_xml_document( const wxString& );
		static bool is_simple( wxXmlNode*, const wxCueTag& );
		static wxXmlNode* find_simple_tag( wxXmlNode*, const wxCueTag& );
		static wxXmlNode* add_simple_tag( wxXmlNode*, const wxString&, const wxString&, const wxString& );

		static wxXmlNode* add_simple_tag( wxXmlNode*, const wxString &, size_t, const wxString & );
		static wxXmlNode* add_simple_tag( wxXmlNode*, const wxCueTag&, const wxString& );
		static void add_simple_tags( wxXmlNode*, const wxArrayCueTag&, const wxString& );
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

		const wxFileName& GetOutputFile() const;
		const wxFileName& GetTagsFile() const;

		bool SaveXmlDoc();
};
#endif

