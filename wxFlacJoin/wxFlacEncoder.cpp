/*
	wxFlacEncoder.cpp
*/

#include "StdWx.h"
#include "wxFlacEncoder.h"

wxFlacEncoder::wxFlacEncoder(wxOutputStream* outputStream)
	:m_outputStream(outputStream)
{
}

wxFlacEncoder::~wxFlacEncoder(void)
{
}

::FLAC__StreamEncoderReadStatus wxFlacEncoder::read_callback(FLAC__byte WXUNUSED(buffer)[], size_t *WXUNUSED(bytes))
{
	return FLAC__STREAM_ENCODER_READ_STATUS_UNSUPPORTED;
}

::FLAC__StreamEncoderWriteStatus wxFlacEncoder::write_callback(const FLAC__byte buffer[], size_t bytes, unsigned WXUNUSED(samples), unsigned WXUNUSED(current_frame) )
{
	wxASSERT( m_outputStream != (wxOutputStream*)NULL );
	return m_outputStream->Write( buffer, bytes ).IsOk()? FLAC__STREAM_ENCODER_WRITE_STATUS_OK : FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
}

::FLAC__StreamEncoderSeekStatus wxFlacEncoder::seek_callback(FLAC__uint64 absolute_byte_offset)
{
	wxASSERT( m_outputStream != (wxOutputStream*)NULL );
	if ( !m_outputStream->IsSeekable() )
	{
		return FLAC__STREAM_ENCODER_SEEK_STATUS_UNSUPPORTED;
	}
	wxFileOffset res = m_outputStream->SeekO( absolute_byte_offset );
	return ( res == wxInvalidOffset)? FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR : FLAC__STREAM_ENCODER_SEEK_STATUS_OK;
}

::FLAC__StreamEncoderTellStatus wxFlacEncoder::tell_callback(FLAC__uint64 *absolute_byte_offset)
{
	wxASSERT( m_outputStream != (wxOutputStream*)NULL );
	if ( !m_outputStream->IsSeekable() )
	{
		return FLAC__STREAM_ENCODER_TELL_STATUS_UNSUPPORTED;
	}

	wxFileOffset res = m_outputStream->TellO();
	if ( res != wxInvalidOffset )
	{
		*absolute_byte_offset = res;
	}
	return ( res == wxInvalidOffset )? FLAC__STREAM_ENCODER_TELL_STATUS_ERROR : FLAC__STREAM_ENCODER_TELL_STATUS_OK;
}

void wxFlacEncoder::metadata_callback(const ::FLAC__StreamMetadata *WXUNUSED(metadata) )
{
}


