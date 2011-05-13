/*
   wxSamplingInfo.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxSamplingInfo.h>
#include <wxCueFile/wxIndex.h>

wxIMPLEMENT_DYNAMIC_CLASS( wxSamplingInfo, wxObject )

const wxTimeSpan wxSamplingInfo::wxInvalidDuration = wxTimeSpan::Hours( -1 );
const wxUint64 wxSamplingInfo::wxInvalidNumberOfFrames = wxULL( 0xFFFFFFFFFFFFFFFF );

wxSamplingInfo::wxSamplingInfo( void ):
	m_nSamplingRate( 44100 ), m_nNumChannels( 2 ), m_nBitsPerSample( 16 )
{}

wxSamplingInfo::wxSamplingInfo( const wxSamplingInfo& si )
{
	copy( si );
}

wxSamplingInfo::wxSamplingInfo( unsigned long nSamplingRate, unsigned short nNumChannels, unsigned short nBitsPerSample ):
	m_nSamplingRate( nSamplingRate ), m_nNumChannels( nNumChannels ), m_nBitsPerSample( nBitsPerSample )
{}

wxSamplingInfo::wxSamplingInfo( const FLAC::Metadata::StreamInfo& si )
{
	m_nSamplingRate	 = si.get_sample_rate();
	m_nNumChannels	 = si.get_channels();
	m_nBitsPerSample = si.get_bits_per_sample();
}

wxSamplingInfo& wxSamplingInfo::Assign( unsigned long nSamplingRate, unsigned short nNumChannels, unsigned short nBitsPerSample )
{
	m_nSamplingRate	 = nSamplingRate;
	m_nNumChannels	 = nNumChannels;
	m_nBitsPerSample = nBitsPerSample;
	return *this;
}

wxSamplingInfo& wxSamplingInfo::Assign( const FLAC::Metadata::StreamInfo& si )
{
	m_nSamplingRate	 = si.get_sample_rate();
	m_nNumChannels	 = si.get_channels();
	m_nBitsPerSample = si.get_bits_per_sample();
	return *this;
}

wxSamplingInfo& wxSamplingInfo::SetDefault()
{
	m_nSamplingRate	 = 44100;
	m_nNumChannels	 = 2;
	m_nBitsPerSample = 16;
	return *this;
}

wxSamplingInfo& wxSamplingInfo::operator =( const wxSamplingInfo& si )
{
	copy( si );
	return *this;
}

void wxSamplingInfo::copy( const wxSamplingInfo& si )
{
	m_nSamplingRate	 = si.m_nSamplingRate;
	m_nNumChannels	 = si.m_nNumChannels;
	m_nBitsPerSample = si.m_nBitsPerSample;
}

unsigned long wxSamplingInfo::GetSamplingRate() const
{
	return m_nSamplingRate;
}

unsigned short wxSamplingInfo::GetNumberOfChannels() const
{
	return m_nNumChannels;
}

unsigned short wxSamplingInfo::GetBitsPerSample() const
{
	return m_nBitsPerSample;
}

wxSamplingInfo& wxSamplingInfo::SetSamplingRate( unsigned long nSamplingRate )
{
	m_nSamplingRate = nSamplingRate;
	return *this;
}

wxSamplingInfo& wxSamplingInfo::SetNumberOfChannels( unsigned short nNumChannels )
{
	m_nNumChannels = nNumChannels;
	return *this;
}

wxSamplingInfo& wxSamplingInfo::SetBitsPerSample( unsigned short nBitsPerSample )
{
	m_nBitsPerSample = nBitsPerSample;
	return *this;
}

bool wxSamplingInfo::IsOK( bool bIgnoreBitsPerSample ) const
{
	return
		( m_nSamplingRate > 0 ) &&
		( m_nNumChannels > 0 ) &&
		( bIgnoreBitsPerSample?true : ( m_nBitsPerSample > 0 ) ) &&
		( ( m_nBitsPerSample % 8 ) == 0 );
}

bool wxSamplingInfo::Equals( const wxSamplingInfo& si, bool bIgnoreBitsPerSample ) const
{
	if ( !( IsOK( bIgnoreBitsPerSample ) || si.IsOK( bIgnoreBitsPerSample ) ) )
	{
		return true;
	}
	else if ( IsOK( bIgnoreBitsPerSample ) && si.IsOK( bIgnoreBitsPerSample ) )
	{
		return
			( m_nSamplingRate == si.m_nSamplingRate ) &&
			( m_nNumChannels == si.m_nNumChannels ) &&
			( bIgnoreBitsPerSample?true : ( m_nBitsPerSample == si.m_nBitsPerSample ) );
	}
	else
	{
		return false;
	}
}

wxULongLong wxSamplingInfo::GetNumberOfFramesFromBytes( const wxULongLong& bytes ) const
{
	wxASSERT( IsOK() );
	wxULongLong bytesPerFrame( 0, m_nBitsPerSample * m_nNumChannels / 8 );

	wxULongLong frames( bytes );
	frames /= bytesPerFrame;
	return frames;
}

wxTimeSpan wxSamplingInfo::GetDuration( wxULongLong frames )
{
	wxASSERT( IsOK( true ) );

	if ( frames.GetValue() == wxInvalidNumberOfFrames )
	{
		return wxInvalidDuration;
	}

	// samples -> duration
	// 441(00) = 10(00) ms
	wxULongLong duration( frames );
	duration *= wxULL( 1000 );
	wxULongLong longSamplesRate( 0, m_nSamplingRate );
	duration /= longSamplesRate;
	return wxTimeSpan::Milliseconds( duration.GetValue() );
}

void wxSamplingInfo::GetNumberOfCdFrames(
	wxULongLong frames,
	wxULongLong& cdFrames, wxUint32& rest ) const
{
	cdFrames  = frames;
	cdFrames *= wxULL( 75 );
	wxULongLong samplingRate( 0, m_nSamplingRate );
	wxULongLong urest( cdFrames % samplingRate );
	cdFrames /= samplingRate;
	rest	  = urest.GetLo();
}

wxULongLong wxSamplingInfo::GetNumberOfCdFrames( wxULongLong frames ) const
{
	wxULongLong cdFrames;
	wxUint32	rest;

	GetNumberOfCdFrames( frames, cdFrames, rest );
	return cdFrames;
}

wxString wxSamplingInfo::GetSamplesStr( wxULongLong frames ) const
{
	// 1.0 = sampling rate
	wxULongLong s( frames );
	wxULongLong samplingRate( 0, m_nSamplingRate );
	wxULongLong sr( frames % samplingRate );
	double		rest = sr.ToDouble() / m_nSamplingRate;

	s -= sr;
	s /= samplingRate;

	// seconds
	wxULongLong ss( s % 60 );
	s	 -= ss;
	s	 /= wxULL( 60 );
	rest += ss.ToDouble();

	// minutes
	wxULongLong mm( s % 60 );
	s -= mm;
	s /= wxULL( 60 );

	// hours

	return wxIndex::GetTimeStr( s.GetLo(), mm.GetLo(), rest );
}

wxString wxSamplingInfo::GetCdFramesStr( wxULongLong frames ) const
{
	wxULongLong cdFrames( GetNumberOfCdFrames( frames ) );

	wxULongLong nf( cdFrames % wxULL( 75 ) );

	cdFrames -= nf;
	cdFrames /= wxULL( 75 );
	wxULongLong ns = cdFrames % wxULL( 60 );
	cdFrames -= ns;
	cdFrames /= wxULL( 60 );

	unsigned long r_minutes = cdFrames.GetLo();
	unsigned long r_seconds = ns.GetLo();
	unsigned long r_frames	= nf.GetLo();

	return wxString::Format( wxT( "%d:%02d:%02d" ), r_minutes, r_seconds, r_frames );
}

wxULongLong wxSamplingInfo::GetFramesFromCdFrames( wxULongLong cdFrames ) const
{
	wxULongLong samplingRate( 0, m_nSamplingRate );
	wxULongLong res( cdFrames );

	res *= samplingRate;
	res /= wxULL( 75 );
	return res;
}

wxULongLong wxSamplingInfo::GetIndexOffset( const wxIndex& idx ) const
{
	if ( idx.HasCdFrames() )
	{
		return GetFramesFromCdFrames( idx.GetOffset() );
	}
	else
	{
		return idx.GetOffset();
	}
}

wxIndex wxSamplingInfo::ConvertIndex( const wxIndex& idx ) const
{
	wxIndex res;

	if ( idx.HasCdFrames() )
	{
		res.SetNumber( idx.GetNumber() ).SetOffset( GetFramesFromCdFrames( idx.GetOffset() ) );
	}
	else
	{
		res = idx;
	}

	return res;
}

wxIndex wxSamplingInfo::ConvertIndex( const wxIndex& idx, wxULongLong offset, bool bAdd ) const
{
	wxIndex res;

	if ( idx.HasCdFrames() )
	{
		res.SetNumber( idx.GetNumber() ).SetOffset( GetFramesFromCdFrames( idx.GetOffset() ) );
	}
	else
	{
		res = idx;
	}

	if ( bAdd )
	{
		res += offset;
	}
	else
	{
		res -= offset;
	}

	return res;
}

wxString wxSamplingInfo::GetIndexOffsetStr( const wxIndex& idx ) const
{
	return GetSamplesStr( GetIndexOffset( idx ) );
}

wxString wxSamplingInfo::GetIndexOffsetFramesStr( const wxIndex& idx ) const
{
	return GetCdFramesStr( GetIndexOffset( idx ) );
}