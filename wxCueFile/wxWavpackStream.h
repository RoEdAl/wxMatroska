/*
	wxWavpackStream.h
*/

#ifndef _WX_WAVPACK_STREAM_H_
#define _WX_WAVPACK_STREAM_H_

class wxWavpackStream :public wxObject
{
public:

	wxWavpackStream(wxInputStream&, wxOutputStream&);
	virtual ~wxWavpackStream(void);

	static WavpackStreamReader* GetStream();

protected:

	wxInputStream& m_inputStream;
	wxOutputStream& m_outputStream;

protected:

	class WavpackStreamReaderWrapper
	{
		private:

		WavpackStreamReader m_wavpackStreamReader;

		public:

		WavpackStreamReaderWrapper()
		{
			wxWavpackStream::InitWavpackStreamReader( m_wavpackStreamReader );
		}

		WavpackStreamReader* GetStream() const
		{
			return const_cast<WavpackStreamReader*>(&m_wavpackStreamReader);
		}
	};

	static void InitWavpackStreamReader( WavpackStreamReader& );

protected:

	// static
	static int32_t read_bytes(void *id, void *data, int32_t bcount);
    static uint32_t get_pos(void *id);
    static int set_pos_abs(void *id, uint32_t pos);
    static int set_pos_rel(void *id, int32_t delta, int mode);
    static int push_back_byte(void *id, int c);
    static uint32_t get_length(void *id);
    static int can_seek(void *id);

    // this callback is for writing edited tags only
    static int32_t write_bytes(void *id, void *data, int32_t bcount);

protected:

	// non-static
	int32_t read_bytes(void *data, int32_t bcount);
    uint32_t get_pos() const;
    int set_pos_abs(uint32_t pos);
    int set_pos_rel(int32_t delta, int mode);
    int push_back_byte(int c);
    uint32_t get_length() const;
    int can_seek() const;

    // this callback is for writing edited tags only
    int32_t write_bytes(void *data, int32_t bcount);
};

#endif
