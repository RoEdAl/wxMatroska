/*
	wxFlacDecoderAndEncoder.cpp
*/

#include "StdWx.h"
#include "wxFlacDecoder.h"
#include "wxFlacEncoder.h"
#include "wxFlacOutputStream.h"
#include "wxFlacDecoderAndEncoder.h"
#include "wxConfiguration.h"
#include "wxApp.h"

wxFlacDecoderAndEncoder::wxFlacDecoderAndEncoder( wxOutputStream& outputStream )
	:m_flacOutputStream( outputStream ),
	 m_pDecoder( (wxFlacDecoder*)NULL ),
	 m_pStreamInfo( (FLAC::Metadata::StreamInfo*)NULL ),
	 m_bValidStream( false )
{
}

wxFlacDecoderAndEncoder::~wxFlacDecoderAndEncoder(void)
{
	Finish();

	if ( m_pStreamInfo != (FLAC::Metadata::StreamInfo*)NULL )
	{
		delete m_pStreamInfo;
	}
}

bool wxFlacDecoderAndEncoder::HasDecoder() const
{
	return (m_pDecoder != (wxFlacDecoder*)NULL);
}

wxFlacDecoder& wxFlacDecoderAndEncoder::GetDecoder()
{
	wxASSERT(m_pDecoder != (wxFlacDecoder*)NULL);
	return *m_pDecoder;
}

wxFlacEncoder& wxFlacDecoderAndEncoder::GetEncoder()
{
	return m_flacOutputStream.GetEncoder();
}

bool wxFlacDecoderAndEncoder::Finish( bool bDecodingOnly )
{
	if ( HasDecoder() )
	{
		FLAC::Decoder::Stream::State state = m_pDecoder->get_state();
		if ( state != FLAC__STREAM_DECODER_UNINITIALIZED )
		{
			bool res = m_pDecoder->finish();
			if ( !res )
			{
				wxString sStateStr( state.as_cstring() );
				wxLogError( wxT("Fail to finish FLAC decoder - state %d - %s"), (int)(::FLAC__StreamDecoderState)state, sStateStr );
				return false;
			}
		}
		delete m_pDecoder;

		m_pDecoder = (wxFlacDecoder*)NULL;
	}

	if ( !bDecodingOnly )
	{
		wxFlacEncoder& encoder = m_flacOutputStream.GetEncoder();
		FLAC::Encoder::Stream::State state = encoder.get_state();
		if ( state != FLAC__STREAM_ENCODER_UNINITIALIZED )
		{
			bool res = encoder.finish();
			if ( !res )
			{
				wxString sStateStr( state.as_cstring() );
				wxLogError( wxT("Fail to finish FLAC encoder - state %d - %s"), (int)(::FLAC__StreamEncoderState)state, sStateStr );
				return false;
			}
		}
	}

	return true;
}

bool wxFlacDecoderAndEncoder::SetInputStream( wxInputStream& inputStream )
{
	if ( !Finish(true) )
	{
		return false;
	}

	m_pDecoder = new wxIntFlacDecoder( inputStream, m_flacOutputStream, *this );
	m_pDecoder->set_md5_checking( true );
	::FLAC__StreamDecoderInitStatus status = m_pDecoder->init();
	if ( status != FLAC__STREAM_DECODER_INIT_STATUS_OK )
	{
		wxString sStatusStr( FLAC__StreamDecoderInitStatusString[status] );
		wxLogError( wxT("Fail to initialize FLAC decoding - %d - %s"), (int)status, sStatusStr );
		m_pDecoder->finish();
		delete m_pDecoder;
		m_pDecoder = (wxFlacDecoder*)NULL;
		return false;
	}

	return true;
}

void wxFlacDecoderAndEncoder::metadata_callback(const ::FLAC__StreamMetadata* metadata )
{
	switch( metadata->type )
	{
		case FLAC__METADATA_TYPE_STREAMINFO:
		{
			FLAC::Metadata::StreamInfo si( (::FLAC__StreamMetadata*)metadata, true );
			OnStreamInfo( si );
		}
		break;
	}
}

void wxFlacDecoderAndEncoder::OnStreamInfo( const FLAC::Metadata::StreamInfo& si )
{
	wxLogInfo( wxT("STREAMINFO metadata:") );
	wxLogInfo( wxT("Sample rate: %dHz"), si.get_sample_rate() );
	wxLogInfo( wxT("Bits per sample: %d"), si.get_bits_per_sample() );
	wxLogInfo( wxT("Number of channels: %d"), si.get_channels() );
	wxULongLong samples( si.get_total_samples() );
	wxLogInfo( wxT("Number of samples: %s"), samples.ToString() );

	wxFlacEncoder& encoder = GetEncoder();
	FLAC::Encoder::Stream::State state = encoder.get_state();
	if ( state == FLAC__STREAM_ENCODER_UNINITIALIZED )
	{ // init
		encoder.set_verify( true );
		wxGetApp().GetConfiguration().ConfigureFlacEncoder( encoder );

		wxLogInfo( wxT("Initializing FLAC encoder") );
		wxLogInfo( wxT("Sample rate: %dHz"), si.get_sample_rate() );
		encoder.set_sample_rate( si.get_sample_rate() );
		wxLogInfo( wxT("Bits per sample: %d"), si.get_bits_per_sample() );
		encoder.set_bits_per_sample( si.get_bits_per_sample() );
		wxLogInfo( wxT("Number of channels: %d"), si.get_channels() );
		encoder.set_channels( si.get_channels() );

		::FLAC__StreamEncoderInitStatus status = encoder.init();
		if ( status != FLAC__STREAM_ENCODER_INIT_STATUS_OK )
		{
			wxString sStatusStr( FLAC__StreamEncoderInitStatusString[status] );
			wxLogError( wxT("Fail to initialize FLAC encoder - %s - %s"), (int)status, sStatusStr );
			m_bValidStream = false;
		}
		else
		{
			m_pStreamInfo = new FLAC::Metadata::StreamInfo( si );
			m_bValidStream = true;
		}
	}
	else
	{ // check
		wxASSERT( m_pStreamInfo != (FLAC::Metadata::StreamInfo*)NULL );

		m_bValidStream = true;
		if ( m_pStreamInfo->get_sample_rate() != si.get_sample_rate() )
		{
			wxLogWarning( wxT("Invalid sample rate %dHz - expected %dHz"), si.get_sample_rate(), m_pStreamInfo->get_sample_rate() );
			m_bValidStream = false;
		}

		if ( m_pStreamInfo->get_bits_per_sample() != si.get_bits_per_sample() )
		{
			wxLogWarning( wxT("Invalid bits per sample %d - expected %d"), si.get_bits_per_sample(), m_pStreamInfo->get_bits_per_sample() );
			m_bValidStream = false;
		}

		if ( m_pStreamInfo->get_channels() != si.get_channels() )
		{
			wxLogWarning( wxT("Invalid number of channels %d - expected %d"), si.get_channels(), m_pStreamInfo->get_channels() );
			m_bValidStream = false;
		}
	}
}

bool wxFlacDecoderAndEncoder::IsValidStream() const
{
	return m_bValidStream;
}
