/*
 * wxCueComponent.h
 */

#ifndef _WX_CUE_COMPONENT_H
#define _WX_CUE_COMPONENT_H

#ifndef _WX_TAG_SYNONIMS_H_
class wxTagSynonimsCollection;
#endif

#ifndef _WX_CUE_TAG_H_
#include "wxCueTag.h"
#endif

class wxCueComponent:
	public wxObject
{
	wxDECLARE_ABSTRACT_CLASS( wxCueComponent );

	public:

		typedef enum _ENTRY_FORMAT
		{
			CHARACTER, BINARY
		} ENTRY_FORMAT;

		typedef enum _ENTRY_TYPE
		{
			ANY, TRACK, DISC
		} ENTRY_TYPE;

		WX_DECLARE_STRING_HASH_MAP( wxString, wxHashString );

	public:

		typedef struct _CDTEXT_ENTRY
		{
			const char* keyword;
			ENTRY_TYPE type;
			ENTRY_FORMAT format;
			char replacement;
		} CDTEXT_ENTRY;

		static const CDTEXT_ENTRY CdTextFields[];
		static const size_t		  CdTextFieldsSize;

		typedef struct _KEYWORD_ENTRY
		{
			const char* keyword;
			ENTRY_TYPE type;
		} KEYWORD_ENTRY;

		static const KEYWORD_ENTRY Keywords[];
		static const size_t		   KeywordsSize;

		static void GetSynonims( wxTagSynonimsCollection&, bool );

	protected:

		bool		  m_bTrack;
		wxArrayString m_comments;
		wxArrayString m_garbage;
		wxArrayCueTag m_cdTextTags;
		wxArrayCueTag m_tags;

	protected:

		void copy( const wxCueComponent& );
		static void remove_duplicates( const wxRegEx&, wxHashArrayCueTag& );
		static void remove_duplicates( const wxRegEx&, wxArrayCueTag& );

	public:

		const wxArrayString& GetComments() const;
		const wxArrayString& GetGarbage() const;
		virtual bool HasGarbage() const;
		const wxArrayCueTag& GetCdTextTags() const;
		const wxArrayCueTag& GetTags() const;
		size_t GetTags( const wxString&, wxArrayCueTag& ) const;
		size_t MoveTags( const wxString&, wxArrayCueTag& );
		void GetTags( wxCueTag::TagSources, const wxTagSynonimsCollection&, const wxTagSynonimsCollection&, wxArrayCueTag&, wxArrayCueTag& ) const;

		bool AddCdTextInfoTag( const wxString&, const wxString& );
		bool AddCdTextInfoTag( const wxCueTag& );
		void AddCdTextInfoTags( const wxArrayCueTag& );

		void RemoveCdTextInfoTag( const wxCueTag& );
		void RemoveCdTextInfoTags( const wxArrayCueTag& );

		void AddTag( wxCueTag::TAG_SOURCE, const wxString&, const wxString& );
		bool AddTag( const wxCueTag& );
		bool AddTagIf( const wxCueTag&, const wxCueTag& );
		bool AddTagIfAndRemove( const wxCueTag&, const wxCueTag& );
		void AddTags( const wxArrayCueTag& );

		size_t MoveCdTextInfoTags( const wxTagSynonimsCollection& );

		size_t RemoveTag( const wxCueTag& );
		size_t RemoveTag( const wxString& );
		size_t RemoveTags( const wxArrayCueTag& );

		bool CheckEntryType( ENTRY_TYPE ) const;

		void Clear( void );
		bool IsTrack() const;

		void AddComment( const wxString& );
		void AddGarbage( const wxString& );

		virtual void GetReplacements( wxHashString& ) const;

	public:

		wxCueComponent( bool = false );
		wxCueComponent( const wxCueComponent& );
		wxCueComponent& operator =( const wxCueComponent& );

		wxCueComponent& operator +=( const wxCueComponent& );
		wxCueComponent& Append( const wxCueComponent& );

		static wxString GetCdTextInfoRegExp();
		static wxString GetKeywordsRegExp();
		static bool GetCdTextInfoFormat( const wxString&, ENTRY_FORMAT& );
		static bool GetCdTextInfoType( const wxString&, ENTRY_TYPE& );
		static bool GetEntryType( const wxString&, ENTRY_TYPE& );

		static wxString FormatCdTextData( const wxString&, const wxString& );
};
#endif

