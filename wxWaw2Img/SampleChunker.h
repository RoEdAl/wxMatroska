/*
        SampleChunker.h
 */

#ifndef _SAMPLE_CHUNKER_H_
#define _SAMPLE_CHUNKER_H_

class SampleChunker:
	public WaveDrawer
{
public:

	SampleChunker( wxUint64, wxUint32, bool, wxFloat32 );

protected:

	virtual void ProcessInitializer();
	virtual void ProcessSample( wxFloat32 fSample );
	virtual void NextColumn( wxFloat32 fValue, wxFloat32 fLogSample ) = 0;

	bool UseLogarithmicScale() const;
	const LogarithmicScale& GetLogarithmicScale() const;

protected:

	wxUint64 m_nNumberOfSamples;
	wxUint64 m_nCurrentSample;
	wxUint64 m_nSamplesInColumn;

	wxUint32 m_nWidth;
	wxUint32 m_nCurrentColumn;

private:

	wxUint64					  m_nLimit;
	wxFloat32					  m_fMaxSample;
	wxFloat32					  m_fAbsMaxSample;
	wxScopedPtr<LogarithmicScale> m_pLogarithmicScale;

private:

	wxUint64 get_next_sample_limit() const;
};

#endif

