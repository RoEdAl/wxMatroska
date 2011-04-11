/*
	wxWavpackStream.cpp
*/

#include "StdWx.h"
#include <wavpack.h>
#include "wxWavpackStream.h"

wxWavpackStream::wxWavpackStream(wxInputStream& inputStream, wxOutputStream& outputStream)
	:m_inputStream(inputStream),m_outputStream(outputStream)
{
}

wxWavpackStream::~wxWavpackStream(void)
{
}

void wxWavpackStream::InitWavpackStreamReader( WavpackStreamReader& wavpackStreamReader )
{
	wavpackStreamReader.read_bytes = &read_bytes;
	wavpackStreamReader.get_pos = &get_pos;
	wavpackStreamReader.set_pos_abs = &set_pos_abs;
	wavpackStreamReader.set_pos_rel = &set_pos_rel;
	wavpackStreamReader.push_back_byte = &push_back_byte;
	wavpackStreamReader.get_length = &get_length;
	wavpackStreamReader.can_seek = &can_seek;
	wavpackStreamReader.write_bytes = &write_bytes;
}

WavpackStreamReader* wxWavpackStream::GetStream()
{
	static WavpackStreamReaderWrapper stream;
	return stream.GetStream();
}

int32_t wxWavpackStream::read_bytes(void *id, void *data, int32_t bcount)
{
	wxASSERT( id != NULL );
	wxWavpackStream* pThis = (wxWavpackStream*)id;
	return pThis->read_bytes( data, bcount );
}

int32_t wxWavpackStream::read_bytes(void *data, int32_t bcount)
{
	m_inputStream.Read( data, bcount );
	return (int32_t)m_inputStream.LastRead();
}

uint32_t wxWavpackStream::get_pos(void *id)
{
	wxASSERT( id != NULL );
	wxWavpackStream* pThis = (wxWavpackStream*)id;
	return pThis->get_pos();
}

uint32_t wxWavpackStream::get_pos() const
{
	return (uint32_t)m_inputStream.TellI();
}

int wxWavpackStream::set_pos_abs(void *id, uint32_t pos)
{
	wxASSERT( id != NULL );
	wxWavpackStream* pThis = (wxWavpackStream*)id;
	return pThis->set_pos_abs( pos );
}

int wxWavpackStream::set_pos_abs(uint32_t pos)
{
	wxASSERT( m_inputStream.IsSeekable() );

	wxFileOffset offset = m_inputStream.SeekI( pos );
	return (offset == wxInvalidOffset)? 1 : 0;
}

int wxWavpackStream::set_pos_rel(void *id, int32_t delta, int mode)
{
	wxASSERT( id != NULL );
	wxWavpackStream* pThis = (wxWavpackStream*)id;
	return pThis->set_pos_rel( delta, mode );
}

int wxWavpackStream::set_pos_rel(int32_t delta, int mode)
{
	wxASSERT( m_inputStream.IsSeekable() );

	wxSeekMode seekMode;
	switch( mode )
	{
		case SEEK_CUR:
		default:
		seekMode = wxFromCurrent;
		break;

		case SEEK_END:
		seekMode = wxFromEnd;
		break;

		case SEEK_SET:
		seekMode = wxFromStart;
		break;
	}

	wxFileOffset offset = m_inputStream.SeekI( delta, seekMode );
	return (offset == wxInvalidOffset)? 1 : 0;
}

int wxWavpackStream::push_back_byte(void *id, int c)
{
	wxASSERT( id != NULL );
	wxWavpackStream* pThis = (wxWavpackStream*)id;
	return pThis->push_back_byte( c );
}

int wxWavpackStream::push_back_byte(int c)
{
	m_outputStream.SeekO( 0, wxFromEnd );
	wxInt8 buf = c;
	m_outputStream.Write( &buf, 1 );
	return c;
}

uint32_t wxWavpackStream::get_length(void *id)
{
	wxASSERT( id != NULL );
	wxWavpackStream* pThis = (wxWavpackStream*)id;
	return pThis->get_length();
}

uint32_t wxWavpackStream::get_length() const
{
	return (uint32_t)m_inputStream.GetLength();
}

int wxWavpackStream::can_seek(void *id)
{
	wxASSERT( id != NULL );
	wxWavpackStream* pThis = (wxWavpackStream*)id;
	return pThis->can_seek();
}

int wxWavpackStream::can_seek() const
{
	return m_inputStream.IsSeekable()? 1 : 0;
}

int32_t wxWavpackStream::write_bytes(void *id, void *data, int32_t bcount)
{
	wxASSERT( id != NULL );
	wxWavpackStream* pThis = (wxWavpackStream*)id;
	return pThis->write_bytes( data, bcount );
}

int32_t wxWavpackStream::write_bytes(void *data, int32_t bcount)
{
	m_outputStream.Write( data, bcount );
	return (int32_t)m_outputStream.LastWrite();
}
