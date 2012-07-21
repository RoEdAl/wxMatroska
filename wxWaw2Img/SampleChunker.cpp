/*
	SampleChunker.cpp
*/
#include "StdWx.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"

SampleChunker::SampleChunker(wxUint64 nNumberOfSamples, wxUint32 nWidth, bool bCalcLogarithmic, wxFloat32 fLogBase)
	:m_nNumberOfSamples(nNumberOfSamples),
	m_nCurrentSample(wxULL(0)),
	m_nSamplesInColumn(wxULL(0)),
	m_nLimit(0),
	m_nWidth(nWidth),
	m_nCurrentColumn(0),
	m_fMaxSample(0.0f),
	m_fAbsMaxSample(0.0f),
	m_bCalcLogarithmic( bCalcLogarithmic ),
	m_fLogBase(fLogBase),
	m_fLogLogBase(0.0f)
{
	wxASSERT( !bCalcLogarithmic || fLogBase > 1.0f );

	if ( bCalcLogarithmic )
	{
		m_fLogLogBase = log( m_fLogBase );
	}
}

wxUint64 SampleChunker::get_next_sample_limit() const
{
	wxULongLong ul( m_nNumberOfSamples );
	ul *= m_nCurrentColumn + 1;
	ul /= m_nWidth;
	return ul.GetValue();
}

void SampleChunker::ProcessInitializer()
{
	m_nCurrentSample = wxULL(0);
	m_nCurrentColumn = 0;
	m_fMaxSample = 0.0;
	m_fAbsMaxSample = 0;
	m_nLimit = get_next_sample_limit();
}

void SampleChunker::ProcessSample( wxFloat32 fSample )
{
	wxASSERT( fSample >= -1.0f && fSample <= 1.0f );

	if ( m_nCurrentColumn >= m_nWidth )
	{
		return;
	}

	wxFloat32 fAbsSample = abs( fSample );
	if ( fAbsSample > m_fAbsMaxSample )
	{
		m_fMaxSample = fSample;
		m_fAbsMaxSample = fAbsSample;
	}
	
	m_nCurrentSample += wxULL(1);
	m_nSamplesInColumn += wxULL(1);
		
	if ( m_nCurrentSample >= m_nLimit )
	{
		if ( m_bCalcLogarithmic )
		{
			wxFloat32 fLogSample = log( m_fAbsMaxSample * (m_fLogBase - 1.0f) + 1.0f ) / m_fLogLogBase;
			wxASSERT( fLogSample >= 0.0f && fLogSample <= 1.0f );
			NextColumn( m_fMaxSample, (m_fMaxSample >= 0.0f)? fLogSample : -fLogSample );
		}
		else
		{
			NextColumn( m_fMaxSample, m_fMaxSample );
		}
		
		m_nCurrentColumn += 1;
		m_nLimit = get_next_sample_limit();			
		m_fMaxSample = 0.0f;
		m_fAbsMaxSample = 0.0f;
		m_nSamplesInColumn = wxULL(0);
	}
}


