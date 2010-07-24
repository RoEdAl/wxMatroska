/*
	wxTrack.h
*/

#ifndef _WX_TRACK_H_
#define _WX_TRACK_H_

#ifndef _WX_INDEX_H_
class wxIndex;
WX_DECLARE_OBJARRAY( wxIndex, wxArrayIndex );
#endif

#ifndef _WX_DATA_FILE_H_
#include "wxDataFile.h"
#endif

#ifndef _WX_CUE_COMPONENT_H
#include "wxCueComponent.h"
#endif

#ifndef _WX_DATA_FILE_H_
#include "wxDatafile.h"
#endif

class wxTrack :public wxCueComponent
{
	DECLARE_DYNAMIC_CLASS(wxTrack)

public:

    typedef enum _Flag
    {
        DCP, CH4, PRE, SCMS, DATA, NONE
    } Flag;

	WX_DEFINE_ARRAY_INT(Flag, wxArrayFlag);

    typedef enum _DataMode
    {
        AUDIO , CDG , MODE1_2048 , MODE1_2352 , MODE2_2336 , MODE2_2352 , CDI_2336 , CDI_2352 
    } DataMode;

protected:

	unsigned long m_number;
	DataMode m_dataMode;
	wxArrayIndex m_indexes;
	wxIndex* m_pPreGap;
	wxIndex* m_pPostGap;
	wxDataFile m_df;
	wxArrayFlag m_flags;

protected:

	typedef struct _FLAG_STR
	{
		Flag flag;
		const wxChar* szName;
	} FLAG_STR;

	static FLAG_STR FlagString[];
	static size_t FlagStringSize;

	typedef struct _DATA_MODE_STR
	{
		DataMode mode;
		const wxChar* szName;
	} DATA_MODE_STR;

	static DATA_MODE_STR DataModeString[];
	static size_t DataModeStringSize;

protected:

	void copy(const wxTrack&);

public:

	static int CompareFn( wxTrack**, wxTrack** );

	unsigned long GetNumber() const;

	DataMode GetMode() const;
	wxTrack& SetMode( DataMode );
	bool SetMode( const wxString& );

	const wxArrayIndex& GetIndexes() const;
	const wxIndex& GetFirstIndex() const;
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

	virtual void GetReplacements( wxHashString& ) const;

public:

	wxTrack(void);
	wxTrack( unsigned long );
	wxTrack(const wxTrack&);
	virtual ~wxTrack(void);
	wxTrack& operator=(const wxTrack&);

	virtual void ToStream(wxTextOutputStream&, int = DUMP_COMMENTS ) const;

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