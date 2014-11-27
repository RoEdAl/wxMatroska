/*
 *  wxCoverFile.h
 */

#ifndef _WX_COVER_FILE_H_
#define _WX_COVER_FILE_H_

class wxCoverFile;

WX_DECLARE_OBJARRAY( wxCoverFile, wxArrayCoverFile );

class wxCoverFile
{
	public:

		enum Type
		{
			// ! A type not enumerated below
			Other = 0x00,

			// ! 32x32 PNG image that should be used as the file icon
			FileIcon = 0x01,

			// ! File icon of a different size or format
			OtherFileIcon = 0x02,

			// ! Front cover image of the album
			FrontCover = 0x03,

			// ! Back cover image of the album
			BackCover = 0x04,

			// ! Inside leaflet page of the album
			LeafletPage = 0x05,

			// ! Image from the album itself
			Media = 0x06,

			// ! Picture of the lead artist or soloist
			LeadArtist = 0x07,

			// ! Picture of the artist or performer
			Artist = 0x08,

			// ! Picture of the conductor
			Conductor = 0x09,

			// ! Picture of the band or orchestra
			Band = 0x0A,

			// ! Picture of the composer
			Composer = 0x0B,

			// ! Picture of the lyricist or text writer
			Lyricist = 0x0C,

			// ! Picture of the recording location or studio
			RecordingLocation = 0x0D,

			// ! Picture of the artists during recording
			DuringRecording = 0x0E,

			// ! Picture of the artists during performance
			DuringPerformance = 0x0F,

			// ! Picture from a movie or video related to the track
			MovieScreenCapture = 0x10,

			// ! Picture of a large, coloured fish
			ColouredFish = 0x11,

			// ! Illustration related to the track
			Illustration = 0x12,

			// ! Logo of the band or performer
			BandLogo = 0x13,

			// ! Logo of the publisher (record company)
			PublisherLogo = 0x14
		};

		static const char* const CoverNames[];
		static const size_t		 CoverNamesSize;

		static const char* const CoverExts[];
		static const size_t		 CoverExtsSize;

		static const wxULongLong MAX_FILE_SIZE;

		struct TypeName
		{
			Type type;
			const char* name;
		};

		static const TypeName TypeNames[];
		static const size_t	  TypeNamesSize;

		wxCoverFile();
		wxCoverFile( const wxCoverFile& );
		wxCoverFile( const wxFileName&, Type );
		wxCoverFile( const wxMemoryBuffer&, Type, const wxString&, const wxString& );

	public:

		const wxFileName&	  GetFileName() const;
		const wxMemoryBuffer& GetData() const;
		const wxString&		  GetMimeType() const;
		const wxString&		  GetDescription() const;
		const wxMemoryBuffer& GetChecksum() const;
		wxString GetExt() const;
		Type GetType() const;
		size_t GetSize() const;

		static bool GetMimeFromExt( const wxFileName&, wxString& );

		bool HasFileName() const
		{
			return m_fileName.IsOk();
		}

		bool HasData() const
		{
			return !m_data.IsEmpty();
		}

		bool HasMimeType() const
		{
			return !m_mimeType.IsEmpty();
		}

		bool HasDescription() const
		{
			return !m_description.IsEmpty();
		}

		static void Append( wxArrayCoverFile&, const wxCoverFile& );
		static void Append( wxArrayCoverFile&, const wxArrayCoverFile& );

		static bool IsCoverFile( const wxFileName& );
		static bool GetCoverFile( const wxDir&, const wxString&, wxFileName& );
		static bool GetCoverFile( const wxFileName&, wxFileName& );

		static void Extract( const wxFileName&, wxArrayCoverFile& );

		static Type GetTypeFromStr( const wxString& );

		static bool	  GetStrFromType( Type, wxString & );
		static size_t GetSortOrder( Type );

		static void Sort( wxArrayCoverFile& );

		bool Save( const wxFileName& );

	protected:

		wxFileName	   m_fileName;
		wxMemoryBuffer m_data;
		wxString	   m_mimeType;
		wxString	   m_description;
		wxMemoryBuffer m_checksum;
		Type		   m_type;
		wxULongLong	   m_fileSize;

	private:

		static int Cmp( wxCoverFile**, wxCoverFile** );
};
#endif

