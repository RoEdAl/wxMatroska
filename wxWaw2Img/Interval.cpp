/*
 *      Interval.cpp
 */

#include "StdWx.h"
#include "Interval.h"

Interval::Interval():
	m_eUnit( INTERVAL_UNIT_NONE ), m_nValue( 0 )
{}

Interval::Interval( INTERVAL_UNIT eUnit, wxUint32 nValue ):
	m_eUnit( eUnit ), m_nValue( nValue )
{}

Interval::Interval( const Interval& interval ):
	m_eUnit( interval.m_eUnit ), m_nValue( interval.m_nValue )
{}

Interval& Interval::operator =( const Interval& interval )
{
	m_eUnit	 = interval.m_eUnit;
	m_nValue = interval.m_nValue;
	return *this;
}

Interval::operator bool() const
{
	return m_eUnit != INTERVAL_UNIT_NONE;
}

bool Interval::Get( const wxTimeSpan& duration, wxTimeSpan& ts ) const
{
	switch ( m_eUnit )
	{
		case INTERVAL_UNIT_NONE:
		default:
		{
			return false;
		}

		case INTERVAL_UNIT_SECOND:
		{
			ts = wxTimeSpan::Seconds( m_nValue );
			return true;
		}

		case INTERVAL_UNIT_MINUTE:
		{
			ts = wxTimeSpan::Minutes( m_nValue );
			return true;
		}

		case INTERVAL_UNIT_HOUR:
		{
			ts = wxTimeSpan::Hours( m_nValue );
			return true;
		}

		case INTERVAL_UNIT_PERCENT:
		ts = wxTimeSpan::Milliseconds( duration.GetMilliseconds().GetValue() * m_nValue / 100 );
		return true;
	}
}

bool Interval::Parse( const wxString& _s, Interval& parsing )
{
	wxString s( _s );

	if ( s.IsEmpty() )
	{
		return false;
	}

	bool   bTruncate = true;
	wxChar c		 = s.Last();

	switch ( c )
	{
		case wxT( 's' ):
		{
			parsing.m_eUnit = INTERVAL_UNIT_SECOND;
			break;
		}

		case wxT( 'm' ):
		{
			parsing.m_eUnit = INTERVAL_UNIT_MINUTE;
			break;
		}

		case wxT( 'h' ):
		{
			parsing.m_eUnit = INTERVAL_UNIT_HOUR;
			break;
		}

		case wxT( '%' ):
		{
			parsing.m_eUnit = INTERVAL_UNIT_PERCENT;
			break;
		}

		default:
		{
			parsing.m_eUnit = INTERVAL_UNIT_MINUTE;
			bTruncate		= false;
			break;
		}
	}

	if ( bTruncate )
	{
		s.RemoveLast();

		if ( s.IsEmpty() )
		{
			return false;
		}
	}

	unsigned long v;

	if ( s.ToCULong( &v ) || s.ToULong( &v ) )
	{
		parsing.m_nValue = v;
		return true;
	}

	return false;
}

wxString Interval::AsString() const
{
	switch ( m_eUnit )
	{
		case INTERVAL_UNIT_NONE:
		default:
		{
			return _( "unknown" );
		}

		case INTERVAL_UNIT_SECOND:
		case INTERVAL_UNIT_MINUTE:
		case INTERVAL_UNIT_HOUR:
		{
			wxTimeSpan ts;
			Get( ts, ts );
			return ts.Format();
		}

		case INTERVAL_UNIT_PERCENT:
		return wxString::Format( _( "%u%%" ), m_nValue );
	}
}

