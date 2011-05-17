/*
   wxTrack.h
 */

#ifndef _WX_TRACK_H_
#define _WX_TRACK_H_

#ifndef _WX_DURATION_H_
class wxDuration;
#endif

#ifndef _WX_INDEX_H_
#include "wxIndex.h"
#endif

#ifndef _WX_DATA_FILE_H_
#include "wxDataFile.h"
#endif

#ifndef _WX_CUE_COMPONENT_H
#include "wxCueComponent.h"
#endif

#ifndef _WX_DATA_FILE_H_
#include "wxDataFile.h"
#endif

#ifndef _WX_SAMPLING_INFO_H_
class wxSamplingInfo;
#endif

class wxTrack:
	public wxCueComponent
{
	wxDECLARE_DYNAMIC_CLASS( wxTrack );

public:

	typedef enum _Flag
	{
		DCP, CH4, PRE, SCMS, DATA, NONE
	} Flag;

	WX_DEFINE_ARRAY_INT( Flag, wxArrayFlag );

	typedef enum _DataMode
	{
		AUDIO, CDG, MODE1_2048, MODE1_2352, MODE2_2336, MODE2_2352, CDI_2336,
		CDI_2352
	} DataMode;

protected:

	size_t				 m_number;
	DataMode			 m_dataMode;
	wxArrayIndex		 m_indexes;
	wxScopedPtr<wxIndex> m_pPreGap;
	wxScopedPtr<wxIndex> m_pPostGap;
	wxDataFile			 m_df;
	wxArrayFlag			 m_flags;

protected:

	typedef struct _FLAG_STR
	{
		Flag flag;
		const wxChar* szName;
	} FLAG_STR;

	static FLAG_STR FlagString[];
	static size_t	FlagStringSize;

	typedef struct _DATA_MODE_STR
	{
		DataMode mode;
		const wxChar* szName;
	} DATA_MODE_STR;

	static DATA_MODE_STR DataModeString[];
	static size_t DataModeStringSize;

protected:

	void copy( const wxTrack& );

public:

	static int CompareFn( wxTrack**, wxTrack** );

	size_t GetNumber() const;

	wxTrack& SetNumber( size_t );

	DataMode GetMode() const;
	wxString GetModeAsString() const;

	wxTrack& SetMode( DataMode );
	bool SetMode( const wxString& );

	const wxArrayIndex& GetIndexes() const;
	const wxIndex&		GetFirstIndex() const;
	void AddIndex( const wxIndex& );

	bool HasPreGap() const;
	bool HasPostGap() const;

	const wxIndex& GetPreGap() const;
	const wxIndex& GetPostGap() const;

	wxArrayIndex& SortIndicies();

	void ClearPreGap();
	void ClearPostGap();

	void SetPreGap( const wxIndex& );
	void SetPostGap( const wxIndex& );

	const wxArrayFlag& GetFlags() const;
	wxString GetFlagsAsString() const;

	wxTrack& AddFlag( Flag );
	bool AddFlag( const wxString& );
	void ClearFlags();
	bool HasFlags() const;

	bool HasFlag( Flag ) const;

	const wxDataFile& GetDataFile() const;
	bool HasDataFile() const;
	void SetDataFile( const wxDataFile& );
	void ClearDataFile();
	bool CalculateDuration( const wxString& = wxEmptyString );

	virtual void GetReplacements( wxHashString& ) const;

public:

	wxTrack( void );
	wxTrack( unsigned long );
	wxTrack( const wxTrack& );
	wxTrack& operator =( const wxTrack& );
	wxTrack& Shift( const wxDuration& );

	bool IsValid() const;

	static wxString FlagToString( Flag );
	static bool StringToFlag( const wxString&, Flag& );
	static wxString GetFlagRegExp();

	static wxString DataModeToString( DataMode );
	static bool StringToDataMode( const wxString&, DataMode& );
	static wxString GetDataModeRegExp();
};

WX_DECLARE_OBJARRAY( wxTrack, wxArrayTrack );

#endif

