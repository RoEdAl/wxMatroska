/*
   wxCueSheet.h
 */

#ifndef _WX_CUE_SHEET_H_
#define _WX_CUE_SHEET_H_

#ifndef _WX_DURATION_H_
#include "wxDuration.h"
#endif

#ifndef _WX_DATA_FILE_H_
#include "wxDataFile.h"
#endif

#ifndef _WX_TRACK_H_
#include "wxTrack.h"
#endif

#ifndef _WX_CUE_COMPONENT_H
#include "wxCueComponent.h"
#endif

#ifndef _WX_CUE_SHEET_CONTENT_H_
#include "wxCueSheetContent.h"
#endif

class wxCueSheet:
	public wxCueComponent, public wxAbstractDurationHolder
{
	wxDECLARE_DYNAMIC_CLASS( wxCueSheet );

public:

	static const wxChar* const CD_ALIASES[];
	static const size_t CD_ALIASES_SIZE;

	static wxString GetCdAliasesRegExp();

	static const wxChar ALBUM_REG_EX1[];
	static const wxChar ALBUM_REG_EX2[];

protected:

	wxArrayCueSheetContent m_content;
	wxArrayFileName m_log;
	wxArrayFileName m_cover;

	wxArrayCueTag m_catalog;
	wxArrayCueTag m_cdtextfile;
	wxArrayTrack m_tracks;

protected:

	void copy( const wxCueSheet& );
	void AddCdTextInfoTagToAllTracks( const wxCueTag& );
	void AddTagToAllTracks( const wxCueTag& );
	void PrepareToAppend();
	wxCueSheet& Append( const wxCueSheet&, const wxDuration& );
	static void AppendFileNames( wxArrayFileName&, const wxArrayFileName& );

public:

	wxCueSheet( void );
	wxCueSheet( const wxCueSheet& );
	wxCueSheet& operator =( const wxCueSheet& );
	bool Append( const wxCueSheet& );

	virtual bool HasGarbage() const;

	bool HasSingleDataFile() const;
	bool HasSingleDataFile( wxDataFile& ) const;
	wxCueSheet& SetSingleDataFile( const wxDataFile& );
	wxCueSheet& SetDataFiles( const wxArrayDataFile& );

	bool IsLastTrackForDataFile( size_t, wxDataFile & ) const;

	const wxArrayCueSheetContent& GetContent() const;
	const wxArrayFileName&		  GetLog() const;
	const wxArrayFileName&		  GetCover() const;
	const wxArrayCueTag&		  GetCatalog() const;
	const wxArrayCueTag&		  GetCdTextFile() const;
	const wxArrayTrack&			  GetTracks() const;

	void FindCommonTags( const wxTagSynonimsCollection&, const wxTagSynonimsCollection&, bool );

	wxCueSheet& AddCatalog( const wxString& );
	wxCueSheet& AddCdTextFile( const wxString& );
	wxCueSheet& AddContent( const wxCueSheetContent& );
	wxCueSheet& AddContent( const wxString& );
	wxCueSheet& AddLog( const wxFileName& );
	bool AddCover( const wxFileName& );

	virtual bool HasDuration() const;
	virtual wxDuration GetDuration() const;
	bool CalculateDuration( const wxString& = wxEmptyString );

	wxTrack& GetTrack( size_t );
	wxTrack& GetLastTrack();
	bool HasTrack( unsigned long ) const;
	wxTrack& GetTrackByNumber( unsigned long );

	const wxTrack& GetTrack( size_t ) const;
	const wxTrack& GetLastTrack() const;
	wxCueSheet&	   AddTrack( const wxTrack& );
	wxArrayTrack&  SortTracks();

	void Clear( void );

	wxString Format( const wxString& ) const;

	wxString FormatTrack( size_t, const wxString & ) const;
};

#endif // _WX_CUE_SHEET_H_

