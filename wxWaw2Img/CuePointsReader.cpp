/*
 *      CuePointsReader.cpp
 */
#include "StdWx.h"
#include <wxEncodingDetection/wxEncodingDetection.h>
#include "FloatArray.h"
#include "CuePointsReader.h"

CuePointsReader::CuePointsReader():
	m_reMsf( "\\A(\\d{1,4}):(\\d{1,2}):(\\d{2})\\Z", wxRE_ADVANCED ),
	m_reMsms( "\\A(\\d{1,4}):(\\d{1,2}).(\\d{3})\\Z", wxRE_ADVANCED )
{
	wxASSERT( m_reMsf.IsValid() );
	wxASSERT( m_reMsms.IsValid() );
}

static bool parse_msf( const wxRegEx& reMsf, const wxString& s, wxTimeSpan& ts )
{
	bool		  res = true;
	unsigned long min, sec, frames;

	if ( reMsf.Matches( s ) )
	{
		if ( !reMsf.GetMatch( s, 1 ).ToULong( &min ) )
		{
			res = false;
		}

		if ( !reMsf.GetMatch( s, 2 ).ToULong( &sec ) )
		{
			res = false;
		}

		if ( !reMsf.GetMatch( s, 3 ).ToULong( &frames ) )
		{
			res = false;
		}

		if ( frames >= 75 )
		{
			res = false;
		}
	}
	else
	{
		res = false;
	}

	if ( res )
	{
		ts = wxTimeSpan( 0, min, sec, frames * 1000 / 75 );
	}

	return res;
}

static bool parse_msms( const wxRegEx& reMsms, const wxString& s, wxTimeSpan& ts )
{
	bool		  res = true;
	unsigned long min, sec, msec;

	if ( reMsms.Matches( s ) )
	{
		if ( !reMsms.GetMatch( s, 1 ).ToULong( &min ) )
		{
			res = false;
		}

		if ( !reMsms.GetMatch( s, 2 ).ToULong( &sec ) )
		{
			res = false;
		}

		if ( !reMsms.GetMatch( s, 3 ).ToULong( &msec ) )
		{
			res = false;
		}
	}
	else
	{
		res = false;
	}

	if ( res )
	{
		ts = wxTimeSpan( 0, min, sec, msec );
	}

	return res;
}

bool CuePointsReader::ParseCuePointPosition( const wxString& s, wxTimeSpan& ts )
{
	unsigned long sec;
	double		  dsec;

	if ( parse_msf( m_reMsf, s, ts ) || parse_msms( m_reMsms, s, ts ) )
	{
		return true;
	}
	else if ( s.ToCULong( &sec ) )
	{
		ts = wxTimeSpan::Seconds( sec );
		return true;
	}
	else if ( s.ToULong( &sec ) )
	{
		ts = wxTimeSpan::Seconds( sec );
		return true;
	}
	else if ( s.ToCDouble( &dsec ) && dsec > 0.0 )
	{
		ts = wxTimeSpan::Milliseconds( dsec * 1000 );
		return true;
	}
	else if ( s.ToDouble( &dsec ) && dsec > 0.0 )
	{
		ts = wxTimeSpan::Milliseconds( dsec * 1000 );
		return true;
	}
	else
	{
		wxLogWarning( _( "Unable to parse cue point: %s" ), s );
		return false;
	}
}

bool CuePointsReader::Read( wxTimeSpanArray& cuePoints, const wxFileName& inputFile, bool bUseMLang )
{
	wxLogInfo( _( "Opening cuesheet file \u201C%s\u201D" ), inputFile.GetFullName() );

	wxString							   sCPDescription;
	wxEncodingDetection::wxMBConvSharedPtr pConv( wxEncodingDetection::GetFileEncoding( inputFile.GetFullPath(), bUseMLang, sCPDescription ) );

	if ( pConv )
	{
		wxLogInfo( _( "Detected encoding of file \u201C%s\u201D file is \u201C%s\u201D" ), inputFile.GetFullName(), sCPDescription );
	}
	else
	{
		pConv = wxEncodingDetection::GetDefaultEncoding( bUseMLang, sCPDescription );
		wxLogInfo( _( "Using default file encoding \u201C%s\u201D" ), sCPDescription );
	}

	wxFileInputStream fis( inputFile.GetFullPath() );

	if ( !fis.IsOk() )
	{
		wxLogWarning( _( "Fail to open cuesheet file" ) );
		return false;
	}
	wxString   s;
	wxTimeSpan ts;

	wxTextInputStream tis( fis, wxT( '\t' ), *pConv );
	while ( !tis.GetInputStream().Eof() )
	{
		s = tis.ReadLine();
		s.Trim( false );
		s.Trim( true );

		if ( s.IsEmpty() || s.StartsWith( "#" ) )
		{
			continue;
		}

		wxStringTokenizer tokenizer( s );

		if ( !tokenizer.HasMoreTokens() )
		{
			continue;
		}

		// first only token
		if ( ParseCuePointPosition( tokenizer.GetNextToken(), ts ) )
		{
			wxLogInfo( _( "Cue point: %s" ), ts.Format() );
			cuePoints.Add( ts );
		}
	}

	wxLogInfo( _( "Closing cuesheet file" ) );
	return true;
}

