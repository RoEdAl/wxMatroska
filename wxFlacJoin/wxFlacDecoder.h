/*
	wxFlacDecoder.h
*/

#ifndef _WX_FLAC_DECODER
#define _WX_FLAC_DECODER

class wxFlacDecoder :public FLAC::Decoder::Stream
{
public:

	wxFlacDecoder(wxInputStream&, wxOutputStream&);
	virtual ~wxFlacDecoder(void);

protected:

	wxInputStream& m_inputStream;
	wxOutputStream& m_outputStream;

protected:

	/// see FLAC__StreamDecoderReadCallback
	virtual ::FLAC__StreamDecoderReadStatus read_callback(FLAC__byte[], size_t*);

	/// see FLAC__StreamDecoderSeekCallback
	virtual ::FLAC__StreamDecoderSeekStatus seek_callback(FLAC__uint64);

	/// see FLAC__StreamDecoderTellCallback
	virtual ::FLAC__StreamDecoderTellStatus tell_callback(FLAC__uint64*);

	/// see FLAC__StreamDecoderLengthCallback
	virtual ::FLAC__StreamDecoderLengthStatus length_callback(FLAC__uint64*);

	/// see FLAC__StreamDecoderEofCallback
	virtual bool eof_callback();

	/// see FLAC__StreamDecoderWriteCallback
	virtual ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame*, const FLAC__int32 * const[]);

	/// see FLAC__StreamDecoderMetadataCallback
	virtual void metadata_callback(const ::FLAC__StreamMetadata*);

	/// see FLAC__StreamDecoderErrorCallback
	virtual void error_callback(::FLAC__StreamDecoderErrorStatus);
};

#endif
