/*
	wxDataFile.h
*/

#ifndef _WX_DATA_FILE_H_
#define _WX_DATA_FILE_H_

class wxDataFile :public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxDataFile)

public:

	typedef enum _FileType
    {
        BINARY, MOTOROLA, AIFF, WAVE, MP3
    } FileType;

	static const wxTimeSpan wxInvalidDuration;
	static const wxULongLong wxInvalidNumberOfSamples;

protected:

	typedef struct _FILE_TYPE_STR
	{
		FileType ftype;
		const wxChar* szName;
	} FILE_TYPE_STR;

	static FILE_TYPE_STR FileTypeString[];
	static size_t FileTypeStringSize;

protected:

	wxFileName m_fileName;
	FileType m_ftype;

protected:

	void copy(const wxDataFile&);

	static wxULongLong GetNumberOfSamplesFromBinary( const wxFileName& );
	static wxULongLong GetNumberOfSamplesFromMediaInfo( const wxFileName& );

public:

	wxString GetFileName() const;
	wxString GetFullPath() const;

	FileType GetFileType() const;
	wxString GetFileTypeAsString() const;
	static wxString GetSamplesStr( wxULongLong );

	bool IsEmpty() const;
	wxDataFile& Assign( const wxString&, FileType = BINARY);
	wxDataFile& Assign( const wxFileName&, FileType = BINARY);

public:

	wxDataFile(void);
	wxDataFile(const wxDataFile&);
	wxDataFile(const wxString&, FileType = BINARY);
	wxDataFile(const wxFileName&, FileType = BINARY);
	~wxDataFile(void);

	wxDataFile& operator=(const wxDataFile&);
	void Clear();

	bool FindFile( wxFileName&, const wxString& = wxEmptyString ) const;
	bool FileExists( const wxString& = wxEmptyString ) const;

	wxULongLong GetNumberOfSamples( const wxString& = wxEmptyString, bool = false ) const; // wxInvalidDuration if duration cannot be calculated
	static wxTimeSpan GetDuration( wxULongLong );
	static wxULongLong GetNumberOfFrames( wxULongLong );
	static void GetNumberOfFrames( wxULongLong, wxULongLong&, wxUint32& );
	wxTimeSpan GetDuration( const wxString& = wxEmptyString ) const; // wxInvalidDuration if duration cannot be calculated

	static wxString FileTypeToString( FileType );
	static bool StringToFileType( const wxString&, FileType& );
	static wxString GetFileTypeRegExp();
};

WX_DECLARE_OBJARRAY( wxDataFile, wxArrayDataFile );

#endif // _WX_DATA_FILE_H_
