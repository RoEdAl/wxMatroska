/*
	wxCueSheet.h
*/

#ifndef _WX_CUE_SHEET_H_
#define _WX_CUE_SHEET_H_

#ifndef _WX_TRACK_H_
class wxTrack;
WX_DECLARE_OBJARRAY( wxTrack, wxArrayTrack );
#endif

#ifndef _WX_CUE_COMPONENT_H
#include "wxCueComponent.h"
#endif

class wxCueSheet :public wxCueComponent
{
	DECLARE_DYNAMIC_CLASS(wxCueSheet)

protected:

	wxString m_sCatalog;
	wxString m_sCdTextFile;
	wxArrayTrack m_tracks;

protected:

	void copy(const wxCueSheet&);

public:

	wxCueSheet(void);
	wxCueSheet(const wxCueSheet&);
	virtual ~wxCueSheet(void);
	wxCueSheet& operator=( const wxCueSheet& );

	virtual bool HasGarbage() const;

	bool HasSingleDataFile() const;
	bool HasSingleDataFile( wxDataFile& ) const;

	wxCueSheet& SetSingleDataFile( const wxDataFile& );
	wxCueSheet& SetDataFiles( const wxArrayDataFile& );

	bool IsLastTrackForDataFile( size_t, wxDataFile& ) const;

public:

	const wxString& GetCatalog() const;
	const wxString& GetCdTextFile() const;
	const wxArrayTrack& GetTracks() const;

	wxTrack& GetTrack( size_t );
	wxTrack& GetLastTrack();
	const wxTrack& GetTrackConst( size_t ) const;
	const wxTrack& GetLastTrackConst() const;
	wxCueSheet& AddTrack( const wxTrack& );
	wxArrayTrack& SortTracks();

	wxCueSheet& SetCatalog( const wxString& );
	wxCueSheet& SetCdTextFile( const wxString& );

	void Clear(void);

	wxString FormatTrack( size_t, const wxString& ) const;
};

#endif // _WX_CUE_SHEET_H_
