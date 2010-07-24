/*
	wxIndex.h
*/

#ifndef _WX_INDEX_H_
#define _WX_INDEX_H_

class wxIndex :public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxIndex)

protected:

	unsigned int m_number;
	unsigned int m_minutes;
	unsigned int m_seconds;
	unsigned int m_frames;

protected:

	void copy(const wxIndex&);

public:

	static int CompareFn( wxIndex**, wxIndex** );

	unsigned int GetNumber() const;
	unsigned int GetMinutes() const;
	unsigned int GetSeconds() const;
	unsigned int GetFrames() const;

	wxTimeSpan GetTimeSpan() const;

	wxIndex& SetNumber( unsigned int );
	wxIndex& SetMsf( unsigned int, unsigned int, unsigned int );
	bool SetMsf( wxTimeSpan );
	wxIndex& SetMsf( wxULongLong );
	wxIndex& Assign( unsigned int, unsigned int, unsigned int, unsigned int );
	bool Assign( unsigned int, wxTimeSpan );

	bool IsValid(bool = false) const;
	void ToStream( wxTextOutputStream& ) const;
	void ToStream( wxTextOutputStream&, wxString ) const;
	wxString ToString() const;
	wxString GetTimeStr() const;
	static void FixDecimalPoint( wxString& );

	wxIndex& operator-=( unsigned long );

public:

	wxIndex(void);
	wxIndex( unsigned int, unsigned int, unsigned int, unsigned int);
	wxIndex( const wxIndex& );
	wxIndex& operator=(const wxIndex&);
	~wxIndex(void);

	static bool TimeSpanToMsf( wxTimeSpan, unsigned int&, unsigned int&, unsigned int& );
	static wxString GetTimeStr( unsigned int, unsigned int, double );
};

WX_DECLARE_OBJARRAY( wxIndex, wxArrayIndex );

#endif // _WX_INDEX_H_
