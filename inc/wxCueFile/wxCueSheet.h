/*
   wxCueSheet.h
 */

#ifndef _WX_CUE_SHEET_H_
#define _WX_CUE_SHEET_H_

#ifndef _WX_DURATION_H_
#include "wxDuration.h"
#endif

#ifndef _WX_TRACK_H_
#include "wxTrack.h"
#endif

#ifndef _WX_CUE_COMPONENT_H
#include "wxCueComponent.h"
#endif

class wxCueSheet: public wxCueComponent, public wxAbstractDurationHolder
{
	wxDECLARE_DYNAMIC_CLASS( wxCueSheet );

protected:

	wxArrayCueTag m_catalog;
	wxArrayCueTag m_cdtextfile;
	wxArrayTrack m_tracks;

protected:

	void copy( const wxCueSheet& );
	wxCueSheet& Append( const wxCueSheet&, const wxDuration& );

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

public:

	const wxArrayCueTag& GetCatalog() const;
	const wxArrayCueTag& GetCdTextFile() const;
	const wxArrayTrack& GetTracks() const;

	virtual bool HasDuration() const;
	virtual wxDuration GetDuration() const;
	bool CalculateDuration( const wxString& = wxEmptyString );

	wxTrack& GetTrack( size_t );
	wxTrack& GetLastTrack();
	bool HasTrack( unsigned long ) const;
	wxTrack& GetTrackByNumber( unsigned long );

	const wxTrack& GetTrack( size_t ) const;
	const wxTrack& GetLastTrack() const;
	wxCueSheet& AddTrack( const wxTrack& );
	wxArrayTrack& SortTracks();

	wxCueSheet& AddCatalog( const wxString& );
	wxCueSheet& AddCdTextFile( const wxString& );

	void Clear( void );

	wxString Format( const wxString& ) const;

	wxString FormatTrack( size_t, const wxString & ) const;
};

#endif // _WX_CUE_SHEET_H_