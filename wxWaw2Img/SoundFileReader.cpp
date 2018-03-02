/*
 *      SoundFileReader.cpp
 */
#include "StdWx.h"
#include "WaveDrawer.h"
#include "MultiChannelWaveDrawer.h"
#include "SoundFile.h"
#include "SoundFileReader.h"

// wxDECLARE_SCOPED_ARRAY(float, wxFloatArray)
wxDEFINE_SCOPED_ARRAY( float, wxFloatArray ) SoundFileReader::SoundFileReader()
{}

SoundFileReader::~SoundFileReader()
{
	m_soundFile.Close();
}

bool SoundFileReader::Open( const wxFileName& fn )
{
	if ( !m_soundFile.Open( fn.GetFullPath(), wxFile::read ) )
	{
		return false;
	}

	return true;
}

wxUint64 SoundFileReader::GetSize() const
{
	return m_soundFile.GetInfo().frames;
}

wxUint16 SoundFileReader::GetChannels() const
{
	return m_soundFile.GetInfo().channels;
}

void SoundFileReader::ReadSamples( MultiChannelWaveDrawer& waveDrawer )
{
	SNDFILE* sndfile = m_soundFile.GetHandle();

	/*
	 * double d;
	 *
	 * if ( !sf_command( m_soundFile.GetHandle(), SFC_CALC_NORM_SIGNAL_MAX, &d, sizeof(d) ) )
	 * {
	 *      wxLogMessage( _("Normalized value: %f"), d );
	 * }
	 * else
	 * {
	 *      d = 1;
	 * }
	 */

	int        nChannels   = m_soundFile.GetInfo().channels;
	int        nSamplerate = m_soundFile.GetInfo().samplerate;
	sf_count_t nBlock      = nChannels * nSamplerate;

	wxFloatArray block( new float[ nBlock ] );

	sf_count_t nCount = sf_readf_float( sndfile, block.get(), nSamplerate );

	while ( nCount > 0 )
	{
		float* p = block.get();
		for ( sf_count_t i = 0; i < nCount; i++ )
		{
			waveDrawer.NextSample( p );
			p += nChannels;
		}

		nCount = sf_readf_float( sndfile, block.get(), nSamplerate );
	}

	m_soundFile.Close();
}

