/*
 * wxTextCueSheetRenderer.h
 */

#ifndef _WX_TEXT_CUE_SHEET_RENDERER_H_
#define _WX_TEXT_CUE_SHEET_RENDERER_H_

#ifndef _WX_CUE_SHEET_RENDERER_H_
#include "wxCueSheetRenderer.h"
#endif

#ifndef _WX_SAMPLING_INFO_H_
#include "wxSamplingInfo.h"
#endif

class wxTextCueSheetRenderer:
	public wxCueSheetRenderer
{
	wxDECLARE_DYNAMIC_CLASS( wxTextCueSheetRenderer );

	protected:

		wxTextOutputStream* m_pTextOutputStream;
		int m_nDumpFlags;
		wxSamplingInfo m_si;
		wxRegEx m_reSpace;

		size_t m_nDataFileIdx;

	protected:

		virtual bool OnPreRenderDisc( const wxCueSheet& );
		virtual bool OnRenderDisc( const wxCueSheet& );
		virtual bool OnRenderTrack( const wxCueSheet&, const wxTrack& );
		virtual bool OnRenderIndex( const wxCueSheet&, const wxTrack&, const wxIndex& );
		virtual bool OnRenderPreGap( const wxCueSheet&, const wxTrack&, const wxIndex& );
		virtual bool OnRenderPostGap( const wxCueSheet&, const wxTrack&, const wxIndex& );

	private:

		void DumpComponentString( const wxCueComponent&, const wxString&, const wxString& );
		void DumpComponentTag( const wxCueComponent&, const wxCueTag& );
		void InternalRenderComponent( const wxCueComponent& );
		void InternalRenderCueSheet( const wxCueSheet& );
		void InternalRenderTrack( const wxCueSheet&, const wxTrack& );

		void InternalRenderIndex( const wxCueSheet&, const wxTrack&, const wxIndex&, wxString );
		void InternalRenderIndex( const wxCueSheet&, const wxTrack&, const wxIndex& );

		void InternalRenderDataFile( const wxCueSheet&, size_t );

	public:

		enum
		{
			DUMP_COMMENTS    = 1,
			DUMP_GARBAGE     = 2,
			DUMP_EMPTY_LINES = 4,
			DUMP_TAGS        = 8
		};

		wxTextCueSheetRenderer( wxTextOutputStream* = (wxTextOutputStream*)NULL, int = DUMP_COMMENTS | DUMP_TAGS );

		void Assign( wxTextOutputStream*, int = DUMP_COMMENTS );

		wxTextOutputStream* GetOutputStream() const;
		int GetDumpFlags() const;

		static bool ToString( wxTextOutputStream&, const wxCueSheet&, int = DUMP_COMMENTS | DUMP_TAGS );
		static wxString ToString( const wxCueSheet&, int                  = DUMP_COMMENTS | DUMP_TAGS );
};

#endif

