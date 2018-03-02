/*
 *      Interval.h
 */

#ifndef _INTERVAL_H_
#define _INTERVAL_H_

enum INTERVAL_UNIT
{
	INTERVAL_UNIT_NONE,
	INTERVAL_UNIT_SECOND,
	INTERVAL_UNIT_MINUTE,
	INTERVAL_UNIT_HOUR,
	INTERVAL_UNIT_PERCENT
};

class Interval
{
	public:

		INTERVAL_UNIT m_eUnit;
		wxUint32 m_nValue;

	public:

		Interval();
		Interval( INTERVAL_UNIT, wxUint32 );
		Interval( const Interval& );

		Interval& operator =( const Interval& );

		operator bool() const;
		bool Get( const wxTimeSpan&, wxTimeSpan& ) const;
		static bool Parse( const wxString&, Interval& );
		wxString AsString() const;
};

#endif

