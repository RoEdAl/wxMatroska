/*
 *      AudioRenderer.cpp
 */
#include "StdWx.h"
#include "FloatArray.h"
#include "LogarithmicScale.h"
#include "SoundFile.h"
#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "AudioRenderer.h"

AudioColumn::AudioColumn()
{
	fAmplitude		 = 0.0f;
	nNumberOfSamples = 0;
}

AudioColumn::AudioColumn( wxFloat32 _fAmplitude, wxUint32 _nNumberOfSamples )
{
	fAmplitude		 = _fAmplitude;
	nNumberOfSamples = _nNumberOfSamples;
}

AudioColumn::AudioColumn( const AudioColumn& ac )
{
	fAmplitude		 = ac.fAmplitude;
	nNumberOfSamples = ac.nNumberOfSamples;
}

AudioColumn& AudioColumn::operator =( const AudioColumn& ac )
{
	fAmplitude		 = ac.fAmplitude;
	nNumberOfSamples = ac.nNumberOfSamples;

	return *this;
}

bool AudioColumn::operator ==( const AudioColumn& ac ) const
{
	return ( fAmplitude == ac.fAmplitude ) && ( nNumberOfSamples == ac.nNumberOfSamples );
}

AudioRenderer::AudioRenderer( wxUint64 nNumberOfSamples, wxUint32 nWidth, bool bUseLogarithmicScale, wxFloat32 fLogBase, wxUint32 nSourceSamplerate ):
	SampleChunker( nNumberOfSamples, nWidth, bUseLogarithmicScale, fLogBase ), m_bUseLogarithmicScale( bUseLogarithmicScale ), m_nSourceSamplerate( nSourceSamplerate )
{}

const AudioColumnArray& AudioRenderer::GetAudioColumns() const
{
	return m_ac;
}

void AudioRenderer::ProcessInitializer()
{
	__super::ProcessInitializer();
	m_ac.Clear();
}

void AudioRenderer::NextColumn( wxFloat32 fSample, wxFloat32 fLogSample )
{
	m_ac.Add( AudioColumn( m_bUseLogarithmicScale ? fLogSample : fSample, m_nSamplesInColumn ) );
}

bool AudioRenderer::GenerateAudio( const wxFileName& filename, wxUint32 nFrequency, wxFloat32 fBaseline ) const
{
	return GenerateAudio( filename, m_ac, m_nSourceSamplerate, nFrequency, fBaseline );
}

void AudioRenderer::ProcessFinalizer()
{
	// __super::ProcessFinalizer();

	// GenerateAudio( "C:/Users/Normal/Documents/Visual Studio 2010/Projects/wxMatroska/render_audio.ogg", 220 );
}

class QGen
{
	public:

		QGen( wxUint32 nLen, wxFloat32 fBaseline ):
			m_nLen( nLen ),
			m_fBaseline( fBaseline ),
			m_sign( true ), m_nPos( 0 ), m_nSamplesCounter( wxULL( 0 ) ),
			m_ar1( new wxFloat32[ nLen ] ), m_ar2( new wxFloat32[ nLen ] )
		{
			wxASSERT( fBaseline >= 0.0f && fBaseline <= 1.0f );
		}

		void SetAmplitude( wxFloat32 fAmplitude )
		{
			wxFloat32 fUpFactor	  = ( 1.0f - m_fBaseline ) / 0.5f;
			wxFloat32 fDownFactor = m_fBaseline / 0.5f;

			wxFloat32 fUpAmplitude	 = fAmplitude * fUpFactor;
			wxFloat32 fDownAmplitude = -fAmplitude * fDownFactor;

			for ( wxUint32 i = 0; i < m_nLen; i++ )
			{
				m_ar1[ i ] = fUpAmplitude;
				m_ar2[ i ] = fDownAmplitude;
			}
		}

		void Generate( SNDFILE* sf, wxUint32 nNumberOfSamples )
		{
			while ( nNumberOfSamples > 0 )
			{
				wxUint32 n			 = m_nLen - m_nPos;
				bool	 sign_change = true;

				if ( n > nNumberOfSamples )
				{
					n			= nNumberOfSamples;
					sign_change = false;
				}

				sf_writef_float( sf, ( m_sign ? m_ar1 : m_ar2 ).get(), n );

				if ( sign_change )
				{
					m_sign = !m_sign;
				}

				nNumberOfSamples  -= n;
				m_nPos			   = ( m_nPos + n ) % m_nLen;
				m_nSamplesCounter += n;
			}
		}

		wxUint64 GetNumberOfSavedSamples() const
		{
			return m_nSamplesCounter;
		}

	protected:

		wxUint32	 m_nLen;
		wxFloat32	 m_fBaseline;
		wxFloatArray m_ar1;
		wxFloatArray m_ar2;
		bool	 m_sign;
		wxUint32 m_nPos;
		wxUint64 m_nSamplesCounter;
};

bool AudioRenderer::GenerateAudio( const wxFileName& filename, const AudioColumnArray& ac, wxUint32 nSamplerate, wxUint32 nFrequency, wxFloat32 fBaseline )
{
	wxASSERT( nFrequency > 0 );
	wxASSERT( nSamplerate > 8000 );

	wxLogInfo( _( "Opening audio file \u201C%s\u201D for writing. Samplerate %d, frequency %d" ), filename.GetFullName(), nSamplerate, nFrequency );

	SF_INFO sf_info;
	memset( &sf_info, 0, sizeof ( SF_INFO ) );
	sf_info.channels   = 1;
	sf_info.samplerate = nSamplerate;
	sf_info.format	   = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

	SoundFile sf;

	if ( !sf.Open( filename.GetFullPath(), sf_info, wxFile::write ) )
	{
		wxLogInfo( _( "Fail to open destination audio file \u201C%s\u201D" ), filename.GetFullName() );
		return false;
	}

	wxLogInfo( _( "Generating audio" ) );
	QGen qgen( nSamplerate / ( nFrequency * 2 ), fBaseline );

	for ( size_t i = 0, nSize = ac.GetCount(); i < nSize; i++ )
	{
		qgen.SetAmplitude( abs( ac[ i ].fAmplitude ) );
		qgen.Generate( sf.GetHandle(), ac[ i ].nNumberOfSamples );
	}

	wxLogInfo( _T( "Number of saved samples: %ld" ), qgen.GetNumberOfSavedSamples() );

	wxLogInfo( _( "Closing audio file" ) );
	sf.Close();
	return true;
}

