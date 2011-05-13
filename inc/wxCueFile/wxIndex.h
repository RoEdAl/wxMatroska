/*
   wxIndex.h
 */

#ifndef _WX_INDEX_H_
#define _WX_INDEX_H_

class wxIndex:public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxIndex );
	friend class wxSamplingInfo;

protected:

	size_t m_number;
	wxULongLong m_offset;
	bool m_bCdFrames;

protected:

	void copy( const wxIndex& );
	const wxULongLong& GetOffset() const;
	wxIndex& operator-=( wxULongLong );
	wxIndex& operator+=( wxULongLong );

public:

	static int CompareFn( wxIndex**, wxIndex** );

	size_t GetNumber() const;
	bool HasCdFrames() const;

	wxIndex& SetNumber( unsigned int );

	wxIndex& SetOffset( wxULongLong );
	wxIndex& Assign( size_t, wxULongLong );
	wxIndex& Assign( size_t, unsigned long, unsigned long, unsigned long );

	bool IsValid( bool = false ) const;
	static void FixDecimalPoint( wxString& );

public:

	wxIndex( void );
	wxIndex( unsigned int, wxULongLong );
	wxIndex( const wxIndex& );
	wxIndex& operator=( const wxIndex& );

	static bool TimeSpanToMsf( wxTimeSpan, unsigned int &, unsigned int &, unsigned int & );
	static wxString GetTimeStr( unsigned int, unsigned int, double );
};

WX_DECLARE_OBJARRAY( wxIndex, wxArrayIndex );

#endif // _WX_INDEX_H_