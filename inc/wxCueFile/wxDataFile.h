/*
 * wxDataFile.h
 */

#ifndef _WX_DATA_FILE_H_
#define _WX_DATA_FILE_H_

#ifndef _WX_DURATION_H_
#include "wxDuration.h"
#endif

class wxDataFile;

WX_DECLARE_OBJARRAY( wxFileName, wxArrayFileName );
WX_DECLARE_OBJARRAY( wxDataFile, wxArrayDataFile );

class wxDataFile:
	public wxObject, public wxDurationHolder
{
	wxDECLARE_DYNAMIC_CLASS( wxDataFile );

	public:

		typedef enum _FileType
		{
			BINARY, MOTOROLA, AIFF, WAVE, MP3
		} FileType;

		typedef enum _MediaType
		{
			MEDIA_TYPE_UNKNOWN,
			MEDIA_TYPE_FLAC,
			MEDIA_TYPE_WAVPACK
		} MediaType;

	protected:

		typedef struct _FILE_TYPE_STR
		{
			FileType ftype;
			const wxChar* szName;
		} FILE_TYPE_STR;

		static const FILE_TYPE_STR FileTypeString[];
		static const size_t		   FileTypeStringSize;

	protected:

		static const wxChar* const INFOS[];
		static const size_t		   INFOS_SIZE;
		static const wxChar* const AUDIO_INFOS[];
		static const size_t		   AUDIO_INFOS_SIZE;

	protected:

		wxFileName m_fileName;
		wxFileName m_realFileName;
		FileType   m_ftype;
		wxString   m_sMIFormat;
		wxString   m_sCueSheet;

	protected:

		void copy( const wxDataFile& );

		static wxULongLong GetNumberOfFramesFromBinary( const wxFileName&,
														const wxSamplingInfo& si );
		static bool GetFromMediaInfo( const wxFileName&, wxULongLong&,
									  wxSamplingInfo&, wxString&, wxString& );

	public:

		const wxFileName& GetFileName() const;
		bool HasRealFileName() const;
		const wxFileName& GetRealFileName() const;
		FileType GetFileType() const;
		wxString GetFileTypeAsString() const;
		const wxString& GetMIFormat() const;
		static MediaType GetMediaType( const wxString& );
		MediaType GetMediaType() const;
		bool HasCueSheet() const;
		const wxString& GetCueSheet() const;
		bool IsBinary() const;
		bool IsEmpty() const;

		wxDataFile& Assign( const wxString &, FileType = BINARY );
		wxDataFile& Assign( const wxFileName &, FileType = BINARY );

	public:

		wxDataFile( void );
		wxDataFile( const wxDataFile& );
		wxDataFile( const wxString&, FileType = BINARY );
		wxDataFile( const wxFileName&, FileType = BINARY );

		wxDataFile& operator =( const wxDataFile& );
		void Clear();

		bool FindFile( wxFileName&, const wxString& = wxEmptyString ) const;
		bool FindFile( const wxString& = wxEmptyString );

		bool GetInfo( const wxString& = wxEmptyString );

		static wxString FileTypeToString( FileType );
		static bool StringToFileType( const wxString&, FileType& );
		static wxString GetFileTypeRegExp();
};

#endif	// _WX_DATA_FILE_H_

