/*
	wxFlacOutputStream.cpp
*/

#include "StdWx.h"
#include "wxFlacEncoder.h"
#include "wxFlacOutputStream.h"

IMPLEMENT_CLASS( wxFlacOutputStream, wxFilterOutputStream )

wxFlacOutputStream::wxFlacOutputStream(void)
	:m_encoder( (wxOutputStream*)NULL, *this )
{
}

wxFlacOutputStream::wxFlacOutputStream(wxOutputStream& stream)
	:wxFilterOutputStream( stream ), m_encoder( &stream, *this )
{
}

wxFlacOutputStream::wxFlacOutputStream(wxOutputStream *stream)
	:wxFilterOutputStream( stream ), m_encoder( stream, *this )
{
}

wxFlacOutputStream::~wxFlacOutputStream(void)
{
	FLAC::Encoder::Stream::State state = m_encoder.get_state();
	if ( state == FLAC__STREAM_ENCODER_OK )
	{
		m_encoder.finish();
	}
}

size_t wxFlacOutputStream::OnSysWrite (const void * buffer, size_t blockSize )
{
	if ( !m_encoder.is_valid() )
	{
		m_lasterror = wxSTREAM_WRITE_ERROR;
		return wxInvalidSize.GetLo();
	}

	if ( !m_encoder.process( (const FLAC__int32 * const*)buffer, blockSize ) )
	{
		m_lasterror = wxSTREAM_WRITE_ERROR;
		return wxInvalidSize.GetLo();
	}
	else
	{
		return GetFilterOutputStream()->LastWrite();
	}
}

wxFlacEncoder& wxFlacOutputStream::GetEncoder()
{
	return m_encoder;
}

void wxFlacOutputStream::metadata_callback(const ::FLAC__StreamMetadata* metadata )
{
	switch( metadata->type )
	{
		case FLAC__METADATA_TYPE_STREAMINFO:
		{
			FLAC::Metadata::StreamInfo si( (::FLAC__StreamMetadata*)metadata, true );
			ProcessStreamInfo( si );
		}
		break;
	}
}

void wxFlacOutputStream::ProcessStreamInfo( const FLAC::Metadata::StreamInfo& si )
{
	wxLogInfo( wxT("STREAMINFO of output file:") );
	wxULongLong samples( si.get_total_samples() );
	wxLogInfo( wxT("Number of samples: %s"), samples.ToString() );
}

