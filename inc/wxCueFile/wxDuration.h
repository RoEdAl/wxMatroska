/*
   wxDuration.h
 */

#ifndef _WX_DURATION_H_
#define _WX_DURATION_H_

#ifndef _WX_SAMPLING_INFO_H_
#include "wxSamplingInfo.h"
#endif

#ifndef _WX_INDEX_H_
#include "wxIndex.h"
#endif

class wxDuration:
	public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxCueSheet );

protected:

	wxSamplingInfo m_si;
	wxULongLong m_numberOfSamples;

public:

	wxDuration( void );
	wxDuration( const wxSamplingInfo&, wxULongLong );
	wxDuration( const wxDuration& );

	wxDuration& operator =( const wxDuration& );

	wxDuration& Assign( const wxSamplingInfo &, wxULongLong );

	const wxSamplingInfo& GetSamplingInfo() const;
	const wxULongLong&	  GetNumberOfSamples() const;

	bool IsValid() const;
	void Clear();
	void Invalidate();
	bool Add( const wxDuration& );

	wxIndex ConvertIndex( const wxIndex&, bool = true ) const;

protected:

	void copy( const wxDuration& );
};

class wxAbstractDurationHolder
{
public:

	virtual bool HasDuration() const	   = 0;
	virtual wxDuration GetDuration() const = 0;
};

class wxDurationHolder:
	public wxAbstractDurationHolder
{
protected:

	wxScopedPtr<wxDuration> m_pDuration;

public:

	wxDurationHolder();

	virtual bool HasDuration() const;
	virtual wxDuration GetDuration() const;

protected:

	void SetDuration( const wxDuration& );
	void ClearDuration();
};

#endif

