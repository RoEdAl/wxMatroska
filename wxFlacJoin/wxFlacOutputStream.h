/*
	wxFlacOutputStream.h
*/

#ifndef _WX_FLAC_OUTPUT_STREAM_H_
#define _WX_FLAC_OUTPUT_STREAM_H_

#ifndef _WX_FLAC_ENCODER_H_
class wxFlacEncoder;
#endif

class wxFlacOutputStream :public wxFilterOutputStream
{
	DECLARE_DYNAMIC_CLASS(wxFlacOutputStream)

private:

	class wxIntFlacEncoder :public wxFlacEncoder
	{
		public:
		wxIntFlacEncoder( wxOutputStream* pOutputStream, wxFlacOutputStream& fos )
			:wxFlacEncoder( pOutputStream ), m_fos(fos)
		{}

		protected:

		virtual void metadata_callback(const ::FLAC__StreamMetadata* metadata )
		{
			m_fos.metadata_callback( metadata );
		}

		private:

		wxFlacOutputStream& m_fos;
	};

protected:

	wxIntFlacEncoder m_encoder;

protected:

	size_t OnSysWrite (const void *, size_t);
	void metadata_callback(const ::FLAC__StreamMetadata*);
	void ProcessStreamInfo( const FLAC::Metadata::StreamInfo& );

public:

	wxFlacOutputStream(void);
    wxFlacOutputStream(wxOutputStream& stream);
    wxFlacOutputStream(wxOutputStream *stream);
	virtual ~wxFlacOutputStream(void);

	wxFlacEncoder& GetEncoder();
};

#endif
