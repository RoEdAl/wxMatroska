/*
	wxFlacDecoderAndEncoder.h
*/

#ifndef _WX_FLAC_ENCODER_AND_DECODER_H_
#define _WX_FLAC_ENCODER_AND_DECODER_H_

#ifndef _WX_FLAC_DECODER_H_
class wxFlacDecoder;
#endif

#ifndef _WX_FLAC_OUTPUT_STREAM_H_
class wxFlacOutputStream;
#endif

class wxFlacDecoderAndEncoder :public wxObject
{
private:

	class wxIntFlacDecoder :public wxFlacDecoder
	{
		public:

		wxIntFlacDecoder( wxInputStream& inputStream, wxOutputStream& outputStream, wxFlacDecoderAndEncoder& de )
			:wxFlacDecoder( inputStream, outputStream ), m_de( de )
		{}

		protected:

		/// see FLAC__StreamDecoderMetadataCallback
		virtual void metadata_callback(const ::FLAC__StreamMetadata* metadata )
		{
			m_de.metadata_callback( metadata );
		}

		private:

		wxFlacDecoderAndEncoder& m_de;
	};

protected:

	wxFlacDecoder* m_pDecoder;
	wxFlacOutputStream m_flacOutputStream;

	FLAC::Metadata::StreamInfo* m_pStreamInfo;
	bool m_bValidStream;

protected:

	void metadata_callback(const ::FLAC__StreamMetadata* );
	void OnStreamInfo( const FLAC::Metadata::StreamInfo& );

public:

	wxFlacDecoderAndEncoder(wxOutputStream&);
	virtual ~wxFlacDecoderAndEncoder(void);

	bool SetInputStream( wxInputStream& );
	bool Finish( bool = false );

	bool HasDecoder() const;
	bool IsValidStream() const;

	wxFlacDecoder& GetDecoder();
	wxFlacEncoder& GetEncoder();
};

#endif
