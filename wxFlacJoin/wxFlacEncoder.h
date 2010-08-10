/*
	wxFlacEncoder.h
*/

#ifndef _WX_FLAC_ENCODER_H_
#define _WX_FLAC_ENCODER_H_

class wxFlacEncoder :public FLAC::Encoder::Stream
{
public:

	wxFlacEncoder( wxOutputStream& );
	virtual ~wxFlacEncoder(void);

protected:

	wxOutputStream& m_outputStream;

protected:

	/// See FLAC__StreamEncoderReadCallback
	virtual ::FLAC__StreamEncoderReadStatus read_callback(FLAC__byte[], size_t*);

	/// See FLAC__StreamEncoderWriteCallback
	virtual ::FLAC__StreamEncoderWriteStatus write_callback(const FLAC__byte[], size_t, unsigned, unsigned);

	/// See FLAC__StreamEncoderSeekCallback
	virtual ::FLAC__StreamEncoderSeekStatus seek_callback(FLAC__uint64);

	/// See FLAC__StreamEncoderTellCallback
	virtual ::FLAC__StreamEncoderTellStatus tell_callback(FLAC__uint64*);

	/// See FLAC__StreamEncoderMetadataCallback
	virtual void metadata_callback(const ::FLAC__StreamMetadata*);
};

#endif
