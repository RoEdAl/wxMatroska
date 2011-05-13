/*
   wxFlacDecoder.cpp
 */

#include "StdWx.h"
#include "wxFlacDecoder.h"

wxFlacDecoder::wxFlacDecoder( wxInputStream& inputStream, wxOutputStream& outputStream )
	: m_inputStream( inputStream ), m_outputStream( outputStream )
{}

wxFlacDecoder::~wxFlacDecoder( void )
{}

::FLAC__StreamDecoderReadStatus wxFlacDecoder::read_callback( FLAC__byte buffer[], size_t* bytes )
{
	m_inputStream.Read( buffer, *bytes );
	*bytes = m_inputStream.LastRead();
	return m_inputStream.Eof()?FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM : FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

::FLAC__StreamDecoderSeekStatus wxFlacDecoder::seek_callback( FLAC__uint64 absolute_byte_offset )
{
	if ( !m_inputStream.IsSeekable() )
	{
		return FLAC__STREAM_DECODER_SEEK_STATUS_UNSUPPORTED;
	}

	wxFileOffset res = m_inputStream.SeekI( absolute_byte_offset );
	return ( res == wxInvalidOffset )?FLAC__STREAM_DECODER_SEEK_STATUS_ERROR : FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

::FLAC__StreamDecoderTellStatus wxFlacDecoder::tell_callback( FLAC__uint64* absolute_byte_offset )
{
	if ( !m_inputStream.IsSeekable() )
	{
		return FLAC__STREAM_DECODER_TELL_STATUS_UNSUPPORTED;
	}

	wxFileOffset pos = m_inputStream.TellI();
	if ( pos != wxInvalidOffset )
	{
		*absolute_byte_offset = pos;
		return FLAC__STREAM_DECODER_TELL_STATUS_OK;
	}
	else
	{
		return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
	}
}

::FLAC__StreamDecoderLengthStatus wxFlacDecoder::length_callback( FLAC__uint64* stream_length )
{
	wxFileOffset len = m_inputStream.GetLength();
	if ( len != wxInvalidOffset )
	{
		*stream_length = len;
	}

	return ( len == wxInvalidOffset )?FLAC__STREAM_DECODER_LENGTH_STATUS_UNSUPPORTED : FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

bool wxFlacDecoder::eof_callback()
{
	return m_inputStream.Eof();
}

::FLAC__StreamDecoderWriteStatus wxFlacDecoder::write_callback( const::FLAC__Frame* frame, const FLAC__int32* const buffer[] )
{
	m_outputStream.Write( buffer, frame->header.blocksize );
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void wxFlacDecoder::metadata_callback( const::FLAC__StreamMetadata* metadata )
{
	switch ( metadata->type )
	{
		case FLAC__METADATA_TYPE_STREAMINFO:
		{
			FLAC::Metadata::StreamInfo si( metadata );
			break;
		}

		case FLAC__METADATA_TYPE_CUESHEET:
		{
			FLAC::Metadata::CueSheet cs( metadata );
			break;
		}

		case FLAC__METADATA_TYPE_PICTURE:
		{
			FLAC::Metadata::Picture pi( metadata );
			break;
		}

		case FLAC__METADATA_TYPE_VORBIS_COMMENT:
		{
			FLAC::Metadata::VorbisComment vc( metadata );
			break;
		}

		case FLAC__METADATA_TYPE_APPLICATION:
		{
			FLAC::Metadata::Application ap( metadata );
			break;
		}
	}
}

void wxFlacDecoder::error_callback( ::FLAC__StreamDecoderErrorStatus status )
{
	wxString sFlacStatus( FLAC__StreamDecoderErrorStatusString[ status ] );
	wxLogError( _( "FLAC error %d: %s" ), status, sFlacStatus );
}