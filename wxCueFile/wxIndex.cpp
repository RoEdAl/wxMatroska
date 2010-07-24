/*
	wxIndex.cpp
*/

#include "StdWx.h"
#include "wxIndex.h"

IMPLEMENT_DYNAMIC_CLASS( wxIndex, wxObject )

wxIndex::wxIndex(void)
	:m_number(0),m_minutes(0),m_seconds(0),m_frames(0)
{
}

wxIndex::wxIndex( unsigned int number, unsigned int minutes, unsigned int seconds, unsigned int frames)
	:m_number(number),m_minutes(minutes),m_seconds(seconds),m_frames(frames)
{
}

wxIndex::wxIndex(const wxIndex& idx)
{
	copy( idx );
}

wxIndex::~wxIndex(void)
{
}

wxIndex& wxIndex::operator=( const wxIndex& idx )
{
	copy( idx );
	return *this;
}

void wxIndex::copy(const wxIndex& idx)
{
	m_number = idx.m_number;
	m_minutes = idx.m_minutes;
	m_seconds = idx.m_seconds;
	m_frames = idx.m_frames;
}

unsigned int wxIndex::GetNumber() const
{
	return m_number;
}

unsigned int wxIndex::GetMinutes() const
{
	return m_minutes;
}

unsigned int wxIndex::GetSeconds() const
{
	return m_seconds;
}

unsigned int wxIndex::GetFrames() const
{
	return m_frames;
}

bool wxIndex::IsValid(bool bPrePost) const
{
	return
		(bPrePost? (m_number==0u) : true) &&
		(m_number<100u) &&
		(m_minutes<85u) &&
		(m_seconds<60u) &&
		(m_frames<75u);

}

wxIndex& wxIndex::SetNumber( unsigned int number )
{
	m_number = number;
	return *this;
}

wxIndex& wxIndex::SetMsf( unsigned int minutes, unsigned int seconds, unsigned int frames )
{
	m_minutes = minutes;
	m_seconds = seconds;
	m_frames = frames;
	return *this;
}

bool wxIndex::TimeSpanToMsf( wxTimeSpan ts, 
	unsigned int& minutes, unsigned int& seconds, unsigned int& frames )
{
	if ( ts.IsNegative() ) return false;
	if ( ts.IsLongerThan( wxTimeSpan::Hours(2) ) ) return false;

	wxLongLong f( ts.GetMilliseconds() );
	f *= 75;
	f /= 1000;

	frames = f.GetValue() % 75;
	f -= frames;

	seconds = f.GetValue() % (60 * 75);
	f -= seconds;
	seconds /= 75;

	minutes = f.GetValue() % (60 * 60 * 75);
	f -= minutes;
	minutes /= (60*75);

	unsigned int hours = f.GetValue() / (60 * 60 * 75);
	minutes += hours * 60;

	return true;
}

wxIndex& wxIndex::Assign( unsigned int number, unsigned int minutes, unsigned int seconds, unsigned int frames )
{
	m_number = number;
	m_minutes = minutes;
	m_seconds = seconds;
	m_frames = frames;
	return *this;
}

bool wxIndex::Assign( unsigned int number, wxTimeSpan ts )
{
	m_number = number;
	return TimeSpanToMsf( ts,
		m_minutes, m_seconds, m_frames );
}

bool wxIndex::SetMsf( wxTimeSpan ts )
{
	return TimeSpanToMsf( ts,
		m_minutes, m_seconds, m_frames );
}

wxTimeSpan wxIndex::GetTimeSpan() const
{
	wxLongLong frames( 0, m_frames );
	frames += (m_seconds * 75);
	frames += (m_minutes * 60 * 75);

	wxLongLong ms( frames );
	ms *= 1000;
	ms /= 75;

	return wxTimeSpan::Milliseconds( ms );
}

void wxIndex::ToStream( wxTextOutputStream& stream, wxString desc ) const
{
	wxString sLine;
	sLine.Printf( wxT("\t\t%s %02d:%02d:%02d\n"), desc.GetData(), m_minutes, m_seconds, m_frames );
	stream.WriteString( sLine );
}

void wxIndex::ToStream( wxTextOutputStream& stream ) const
{
	wxString sLine;
	sLine.Printf( wxT("\t\tINDEX %02d %02d:%02d:%02d\n"), m_number, m_minutes, m_seconds, m_frames );
	stream.WriteString( sLine );
}

wxString wxIndex::ToString() const
{
	wxString s;
	s.Printf( wxT("%02d:%02d:%02d"), m_minutes, m_seconds, m_frames );
	return s;
}

wxString wxIndex::GetTimeStr() const
{
	// 1.000 = 75
	// 0.040 = 3

	double seconds = m_frames;
	seconds *= 0.040;
	seconds /= 3;
	seconds += m_seconds;

	unsigned int minutes = m_minutes % 60;
	unsigned int hours = m_minutes / 60;

	return GetTimeStr( hours, minutes, seconds );
}

wxString wxIndex::GetTimeStr( unsigned int hours, unsigned int minutes, double seconds )
{
	wxString s;
	s.Printf( wxT("%02d:%02d:%012.9f"), hours, minutes, seconds );
	FixDecimalPoint( s );
	return s;
}

void wxIndex::FixDecimalPoint( wxString& s )
{
#if wxUSE_INTL
    wxString sep = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT,
                                     wxLOCALE_CAT_NUMBER);
#else // !wxUSE_INTL
    // As above, this is the most common alternative value. Notice that here it
    // doesn't matter if we guess wrongly and the current separator is already
    // ".": we'll just waste a call to Replace() in this case.
    wxString sep(",");
#endif // wxUSE_INTL/!wxUSE_INTL

    s.Replace(sep, ".");
}

wxIndex& wxIndex::operator -=(unsigned long frames)
{
	wxULongLong totalFrames( m_minutes * wxULL(60) * wxULL(75) );
	totalFrames += wxULL(75) * m_seconds;
	totalFrames += m_frames;

	totalFrames -= frames;

	wxULongLong nf = totalFrames % wxULL(75);
	totalFrames -= nf;
	totalFrames /= wxULL(75);
	wxULongLong ns = totalFrames % wxULL(60);
	totalFrames -= ns;
	totalFrames /= wxULL(60);

	m_minutes = totalFrames.GetLo();
	m_seconds = ns.GetLo();
	m_frames = nf.GetLo();

	return *this;
}

int wxIndex::CompareFn(wxIndex** i1, wxIndex** i2)
{
	if ( (*i1)->GetNumber() < (*i2)->GetNumber() )
	{
		return -1;
	}
	else if ( (*i1)->GetNumber() > (*i2)->GetNumber() )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY( wxArrayIndex );
