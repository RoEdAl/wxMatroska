/*
	wxFlacMetaDataReader.h
*/

#ifndef _WX_FLAC_META_DATA_READER_H_
#define _WX_FLAC_META_DATA_READER_H_

#ifndef _WX_FLAC_DECODER_H_
#include <wxFlacDecoder.h>
#endif

class wxFlacMetaDataReader :public wxObject
{

protected:

	class FlacDecoder :public wxFlacDecoder
	{
		public:
		FlacDecoder(wxInputStream& inputStream, wxOutputStream& outputStream, wxFlacMetaDataReader& fmtr )
			:wxFlacDecoder( inputStream, outputStream ), m_fmtr(fmtr)
		{}
				
		protected:

		virtual void metadata_callback(const ::FLAC__StreamMetadata* metadata )
		{
			m_fmtr.metadata_callback( metadata );
		}

		protected:

		wxFlacMetaDataReader& m_fmtr;

	};

	class wxNullOutputStream :public wxOutputStream
	{
		public:

		wxNullOutputStream() {}

		protected:

		virtual size_t OnSysWrite(const void *WXUNUSED(buffer), size_t bufsize ) { return bufsize; }
	};

protected:

	void metadata_callback(const ::FLAC__StreamMetadata* );
	FLAC::Metadata::VorbisComment* m_pVorbisComment;
	FLAC::Metadata::CueSheet* m_pCueSheet;
	wxString m_sFlacFile;

public:

	wxFlacMetaDataReader(void);
	~wxFlacMetaDataReader(void);

	bool ReadMetadata( const wxString& );

	const wxString& GetFlacFile() const;

	bool HasVorbisComment() const;
	bool HasCueSheet() const;

	WX_DECLARE_STRING_HASH_MAP( wxString, wxHashString );

	const FLAC::Metadata::VorbisComment& GetVorbisComment() const;
	wxString GetCueSheetFromVorbisComment() const;
	void ReadVorbisComments( wxHashString& ) const;

	const FLAC::Metadata::CueSheet& GetCueSheet() const;

};

#endif
