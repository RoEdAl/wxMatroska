/*
 * wxIndex.h
 */

#ifndef _WX_INDEX_H_
#define _WX_INDEX_H_

class wxIndex:
	public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxIndex );
	friend class wxSamplingInfo;

	protected:

		size_t m_number;
		wxULongLong m_offset;
		bool m_bCdFrames;
		size_t m_nDataFileIdx;

	protected:

		void copy( const wxIndex& );
		const wxULongLong& GetOffset() const;
		wxIndex& operator -=( wxULongLong );
		wxIndex& operator +=( wxULongLong );

	public:

		static const size_t UnknownDataFileIdx;
		static int CompareFn( wxIndex**, wxIndex** );

		size_t GetNumber() const;
		bool IsZero() const;
		bool HasCdFrames() const;
		bool HasDataFileIdx() const;
		size_t GetDataFileIdx() const;

		wxIndex& SetNumber( size_t );
		wxIndex& SetOffset( wxULongLong );
		wxIndex& SetDataFileIdx( size_t );
		bool ShiftDataFileIdx( size_t );

		wxIndex& Assign( size_t, wxULongLong );
		wxIndex& Assign( size_t, wxULongLong, size_t );
		wxIndex& Assign( size_t, unsigned long, unsigned long, unsigned long );
		wxIndex& Assign( size_t, unsigned long, unsigned long, unsigned long, size_t );

		bool IsValid( bool = false ) const;
		static void FixDecimalPoint( wxString& );

	public:

		wxIndex( void );
		wxIndex( unsigned int, wxULongLong );
		wxIndex( unsigned int, wxULongLong, size_t );
		wxIndex( const wxIndex& );
		wxIndex& operator =( const wxIndex& );

		static bool TimeSpanToMsf( wxTimeSpan, unsigned int&, unsigned int&, unsigned int& );
		static wxString GetTimeStr( unsigned int, unsigned int, double );
};

WX_DECLARE_OBJARRAY( wxIndex, wxArrayIndex );
#endif  // _WX_INDEX_H_

